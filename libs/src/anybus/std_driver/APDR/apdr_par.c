/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** apdr_par.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Contain implementation of the parallel part of the data link layer.
**
**
********************************************************************************
********************************************************************************
**
** Porting instructions:
** ---------------------
**
** No porting required.
**
********************************************************************************
********************************************************************************
**                                                                            **
** COPYRIGHT NOTIFICATION (c) 2008 HMS Industrial Networks AB                 **
**                                                                            **
** This code is the property of HMS Industrial Networks AB.                   **
** The source code may not be reproduced, distributed, or used without        **
** permission. When used together with a product from HMS, this code can be   **
** modified, reproduced and distributed in binary form without any            **
** restrictions.                                                              **
**                                                                            **
** THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT    **
** WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR     **
** THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR     **
** THAT DEFECTS IN IT CAN BE CORRECTED.                                       **
**                                                                            **
********************************************************************************
********************************************************************************
**
** Company: HMS Industrial Networks AB
**          Stationsgatan 37
**          SE-302 45  Halmstad
**          SWEDEN
**          Tel:     +46 (0)35 - 17 29 00
**          Fax:     +46 (0)35 - 17 29 09
**          e-mail:  info@hms.se
**
********************************************************************************
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc_com.h"
#include "sys_adpt.h"
#include "util.h"
#include "apdr.h"
#include "aplink.h"
#include "abcc.h"
#include "abp.h"
#include "apdr_priv.h"


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/


#ifdef ABCC_ACCESS_MODE_PARALLEL
/*------------------------------------------------------------------------------
** APDR_RunDataLinkStateMachine()
**------------------------------------------------------------------------------
*/

void APDR_RunDataLinkStateMachine( void )
{
   UTIL_BufMsgType* psQEntry;


   /*
   ** This function will run the lowest layer state-machine in a parallel
   ** implementation.
   */

#ifdef ABCC_PARALLEL_POLL_MODE
      /*
      ** Call the parallel Poll routine to see if we have a new telegram
      ** indication in the Status register
      */

      if( apdr_s.fParallelPollEnabled )
      {
         APDR_ParallelPollMode();
      }
#endif

   while( 1 )
   {

      switch( apdr_s.eState )
      {
      case APDR_SM_INIT:
         /*
         ** Initilise variables and queues that are used by the system.
         */

         apdr_s.iPdReadLength    = 0;
         apdr_s.iPdWriteLength   = 0;

         apdr_s.fAbccWdToOccured    = FALSE;
         apdr_s.bLastSentMsgType    = APDR_MESSAGE_RESPONSE_TYPE;


         /*
         ** Set initial values to Control bits (such as T-bit, R-bit, M-bit etc.)
         */

         apdr_s.bWorkingControlReg  = 0;
         apdr_s.bLastStatusReg      = 0;
         apdr_s.fInitHsTo           = FALSE;
         apdr_s.fTelegramReceived   = FALSE;


         /*
         ** Set the initial status register to zero, this means that the first
         ** telegram that the ABCC module sends shall have the Toggle-bit set.
         */

         apdr_s.bCurrentStatusReg = 0x00;


         /*
         ** Reload and start the initial handshake timer.
         */

         UTIL_ReloadTimer( apdr_s.xInitHsTimer, (UINT16)ABCC_INTIAL_HANDSHAKE_DELAY_TIME_MS );
         UTIL_StartTimer( apdr_s.xInitHsTimer );


         /*
         ** Switch state to initial handshake.
         */

         apdr_s.eState = APDR_SM_INITIAL_HANDSHAKE;

         break; /* End case APDR_SM_INIT: */


      case APDR_SM_INITIAL_HANDSHAKE:
         /*
         ** In case of parallel driver the initial handshake procedure means that
         ** we need to wait a certain time before we are allowed to communicate
         ** with the ABCC.
         */

         if( apdr_s.fInitHsTo )
         {
            /*
            ** The initial handshake timer has expired.
            ** Check that the ABCC status register signals SETUP state.
            ** This means that we are
            ** allowed to proceed to next state.
            */

            UTIL_StopTimer( apdr_s.xInitHsTimer );


            /*
            ** Read initial value from the DPRAM.
            */

            apdr_s.bCurrentStatusReg = SYS_VerifyReadParallel( APDR_DPRAM_STAT_REG_OFFSET );


#ifndef ABCC_PARALLEL_POLL_MODE
            /*
            ** Signal to the application that he/she now shall clear any pending
            ** DPRAM interrupts and enable the interrupt.
            */

            ABCC_CbfEnableInterruptParallel();
#else
            /*
            ** Signal that we are now allowed to start polling for new telegrams.
            */

            apdr_s.fParallelPollEnabled = TRUE;

#endif /* End #ifndef ABCC_PARALLEL_POLL_MODE */


            if( ( apdr_s.bCurrentStatusReg & ABP_STAT_S_BITS ) == ABP_ANB_STATE_SETUP )
            {
               /*
               ** We are now able to start the communication.
               */

               apdr_s.eState = APDR_SM_IDLE;
            }
            else
            {
               /*
               ** The ABCC module's state is not the expected, this is a fatal
               ** error since we cannot proceed.
               */

               ABCC_CbfDriverError( ABCC_SEV_FATAL,
                                    ABCC_EC_INCORRECT_INIT_HS_STATE );

               /*
               ** Return control to the user so that we avoid ending up in an
               ** endless loop.
               */

               return;

            } /* End if( Current ABCC state == AB_STATE_SETUP ) */

         }
         else
         {
            /*
            ** We are still not allowed to communicate with the ABCC.
            */

            return;

         } /* End if( apdr_s.fInitHsTo ) */

         break; /* End case APDR_SM_INITIAL_HANDSHAKE: */


      case APDR_SM_SEND_TELEGRAM_WITH_MESSAGE:
         /*
         ** This state will send a complete telegram (including process data) to the
         ** ABCC-module.
         */

         /*
         ** Set the application M-bit.
         */

         apdr_s.bWorkingControlReg |= ABP_CTRL_M_BIT;


         /*
         ** Check with higher layer if it can handle more incoming commands.
         ** If no slots available clear the Application R-bit! Otherwise set it.
         */

         if( APLINK_FreeCmdSlots() == TRUE )
         {
            /*
            ** Set the R-bit.
            */

            apdr_s.bWorkingControlReg |= ABP_CTRL_R_BIT;
         }
         else
         {
            /*
            ** Clear the R-bit.
            */

            apdr_s.bWorkingControlReg &= ~ABP_CTRL_R_BIT;

         } /* End if( APLINK_FreeCmdSlots() == TRUE ) */


         /*
         ** Toggle the Appl T-bit.
         */

         apdr_s.bWorkingControlReg ^= ABP_CTRL_T_BIT;


         /*
         ** Copy message data to DPRAM.
         */

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
         SYS_CopyMemBytes( (UINT8*)apdr_s.sDpram.psWriteMsg,
                           (UINT8*)&apdr_s.psWriteMsgBuffer->sMsg,
                           (UINT16)( sizeof( ABP_MsgHeaderType ) +
                                     apdr_s.psWriteMsgBuffer->sMsg.sHeader.bDataSize ) );
#else
         SYS_WriteParallel( (UINT8*)&apdr_s.psWriteMsgBuffer->sMsg,
                            APDR_DPRAM_MESSAGE_WRITE_OFFSET,
                            (UINT16)( sizeof( ABP_MsgHeaderType ) +
                             apdr_s.psWriteMsgBuffer->sMsg.sHeader.bDataSize ) );
#endif

         /*
         ** Copy process data to DPRAM.
         */

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
         /*
         ** Copy the data directly from APLINK to the DPRAM.
         */

         APLINK_GetNewProcessData( apdr_s.sDpram.pabWritePd,
                                   apdr_s.iPdWriteLength );
#else
         /*
         ** Copy data from APLINK to temporary buffer, then to the DPRAM.
         */

         if( APLINK_GetNewProcessData( (UINT8*)&apdr_s.sTempPd.uData.ai[ 0 ],
                                       apdr_s.iPdWriteLength ) )
         {
            /*
            ** New data available.
            */

            SYS_WriteParallel( (UINT8*)&apdr_s.sTempPd.uData.ai[ 0 ],
                               APDR_DPRAM_PD_WRITE_OFFSET,
                               apdr_s.iPdWriteLength );
         }
#endif

         /*
         ** Post our copy of the APPL Control register.
         */

         SYS_WriteParallel( &apdr_s.bWorkingControlReg, APDR_DPRAM_CTRL_REG_OFFSET, 1 );

         /*
         ** Tell Aplink that the message has been sent. Aplink will
         ** release the buffer used to hold the write-message.
         */

         APLINK_CompleteMsgTrans( apdr_s.psWriteMsgBuffer );

         /*
         ** Signal to application that we have transmitted a new telegram.
         */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
         ABCC_CbfTelegramTransmitted( FALSE );
#endif


         /*
         ** Start the ABCC timeout timer.
         */

         UTIL_StartTimer( apdr_s.xAbccWdTimer );


         /*
         ** Change state to wait for telegram response.
         */

         apdr_s.eState = APDR_SM_WAIT_FOR_TELEGRAM;


         /*
         ** Return control to user.
         */

         return;

         /* break; */ /* End case APDR_SM_SEND_TELEGRAM_WITH_MESSAGE: */


      case APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE:
         /*
         ** This state will send a telegram (including only process data) to the
         ** ABCC-module. No message data is sent!
         */

         /*
         ** Clear the application M-bit.
         */

         apdr_s.bWorkingControlReg &= ~ABP_CTRL_M_BIT;


         /*
         ** Check with higher layer if it can handle more incoming commands.
         ** If no slots available clear the Application R-bit! Otherwise set it.
         */

         if( APLINK_FreeCmdSlots() == TRUE )
         {
            /*
            ** Set the R-bit.
            */

            apdr_s.bWorkingControlReg |= ABP_CTRL_R_BIT;
         }
         else
         {
            /*
            ** Clear the R-bit.
            */

            apdr_s.bWorkingControlReg &= ~ABP_CTRL_R_BIT;
         } /* End if( APLINK_FreeCmdSlots() == TRUE ) */


         /*
         ** Toggle the Appl T-bit.
         */

         apdr_s.bWorkingControlReg ^= ABP_CTRL_T_BIT;


         /*
         ** Copy process data to DPRAM.
         */

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
         /*
         ** Copy the data directly from APLINK to the DPRAM.
         */

         APLINK_GetNewProcessData( apdr_s.sDpram.pabWritePd,
                                   apdr_s.iPdWriteLength );
#else
         /*
         ** Copy data from APLINK to temporary buffer, then to the DPRAM.
         */

         if( APLINK_GetNewProcessData( (UINT8*)&apdr_s.sTempPd.uData.ai[ 0 ],
                                       apdr_s.iPdWriteLength ) )
         {

            SYS_WriteParallel( (UINT8*)&apdr_s.sTempPd.uData.ai[ 0 ],
                               APDR_DPRAM_PD_WRITE_OFFSET,
                               apdr_s.iPdWriteLength );
         }
#endif


         /*
         ** Post our copy of the APPL Control register.
         */

         SYS_WriteParallel( &apdr_s.bWorkingControlReg, APDR_DPRAM_CTRL_REG_OFFSET, 1 );


         /*
         ** Signal to application that we have transmitted a new telegram.
         */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
         ABCC_CbfTelegramTransmitted( FALSE );
#endif


         /*
         ** Start the ABCC timeout timer.
         */

         UTIL_StartTimer( apdr_s.xAbccWdTimer );


         /*
         ** Change state to wait for telegram response.
         */

         apdr_s.eState = APDR_SM_WAIT_FOR_TELEGRAM;


         /*
         ** Return control to user.
         */

         return;

         /* break; */ /* End case APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE: */


      case APDR_SM_WAIT_FOR_TELEGRAM:
         /*
         ** In this state we will check if there are any new events that has
         ** occured.
         */

         if( apdr_s.fTelegramReceived )
         {
            /*
            ** We have received a new telegram!
            ** Clear the new data flag.
            */

            apdr_s.fTelegramReceived = FALSE;


            /*
            ** Stop the ABCC timeout timer.
            */

            UTIL_StopTimer( apdr_s.xAbccWdTimer );

            /*
            ** Clear timeout timer flag (just in case).
            */

            apdr_s.fAbccWdTo = FALSE;


            /*
            ** Signal that we no longer have a WD timeout situation.
            */

            if( apdr_s.fAbccWdToOccured )
            {
               ABCC_CbfWdTimeoutRecovered();

               apdr_s.fAbccWdToOccured = FALSE;
            }


            /*
            ** Process the new telegram in next state.
            */

            apdr_s.eState = APDR_SM_PROCESS_RECEIVED_TELEGRAM;
         }
         else
         {
            /*
            ** No new telegram for us.
            */

            if( apdr_s.fAbccWdTo )
            {
               /*
               ** Stop the ABCC watchdog timeout timer.
               */

               UTIL_StopTimer( apdr_s.xAbccWdTimer );


               /*
               ** Change state to Communication lost.
               */

               apdr_s.eState = APDR_SM_ABCC_COM_LOST;
            }
            else
            {
               /*
               ** No timeout.
               ** Return to the user and continue with these checks next time.
               */

               return;
            } /* End if( ABCC timeout? ) */

         } /* End if( new telegram received ) */

         break; /* End case APDR_SM_WAIT_FOR_TELEGRAM: */


      case APDR_SM_PROCESS_RECEIVED_TELEGRAM:
         /*
         ** In this state we have received access to DPRAM and shall copy the
         ** data to the driver.
         */

         if( apdr_s.bCurrentStatusReg & ABP_CTRL_M_BIT )
         {
            /*
            ** There's a new message for us.
            */

            psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asMsgToApplBuf[ 0 ], APLINK_MAX_NBR_COMMANDS_IN_USE );

            if( psQEntry == NULL )
            {
               ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );

               return;
            }


#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
            /*
            ** Copy the message directly from the DPRAM to the allocated buffer.
            */

            SYS_CopyMemBytes( (UINT8*)&psQEntry->sMsg,
                              (UINT8*)apdr_s.sDpram.psReadMsg,
                              (UINT16)( sizeof( ABP_MsgHeaderType ) +
                                        apdr_s.sDpram.psReadMsg->sHeader.bDataSize ) );
#else
            /*
            ** Copy the message to the allocated buffer
            */

            SYS_ReadParallel( (UINT8*)&psQEntry->sMsg,
                              APDR_DPRAM_MESSAGE_READ_OFFSET,
                              APDR_MESSAGE_LENGTH );

#endif

            /*
            ** Send this message to a higher level.
            */

            APLINK_MessageReceived( psQEntry );

         }
         else
         {
            /*
            ** This telegram contains no new message for us....
            */

         } /* End if( ABCC M-bit == 1 ) */



#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
         /*
         ** Handle the IO data that is present, send the data to higher level.
         */

         APLINK_PutProcessData( (UINT8)apdr_GetAbccStateAndSup(),
                                apdr_s.sDpram.pabReadPd,
                                apdr_s.iPdReadLength );

#else

         /*
         ** Handle the IO data that is present, send the data to higher level.
         */

         SYS_ReadParallel( (UINT8*)&apdr_s.sTempPd.uData.ai[ 0 ],
                           APDR_DPRAM_PD_READ_OFFSET,
                           apdr_s.iPdReadLength );


         APLINK_PutProcessData( (UINT8)apdr_GetAbccStateAndSup(),
                               (UINT8*)&apdr_s.sTempPd.uData.ai[ 0 ],
                                apdr_s.iPdReadLength );
#endif

         /*
         ** Copy this new ABCC status register to "Last".
         */

         apdr_s.bLastStatusReg = apdr_s.bCurrentStatusReg;


         /*
         ** Continue in state-machine to IDLE-state.
         */

         apdr_s.eState = APDR_SM_IDLE;

         break; /* End case APDR_SM_PROCESS_RECEIVED_TELEGRAM: */


      case APDR_SM_ABCC_COM_LOST:
         /*
         ** The communication with the ABCC is lost.
         ** We will signal to our host application that the ABCC module is
         ** dead like a brick. Then go back to the reception state.
         ** Call call-back function stating that ABCC whatchdog timeout has
         ** occured.
         */

         ABCC_CbfWdTimeout();

         /*
         ** Signal that there has been a WD timeout.
         */

         apdr_s.fAbccWdToOccured = TRUE;


         /*
         ** Clear the ABCC timeout timer flag.
         */

         apdr_s.fAbccWdTo = FALSE;


         /*
         ** Go to next state, wait again...
         */

         apdr_s.eState = APDR_SM_WAIT_FOR_TELEGRAM;

         break; /* End case APDR_SM_ABCC_COM_LOST: */


      case APDR_SM_IDLE:
      default:
         /*
         ** Reload the ABCC timeout timer.
         */

         UTIL_ReloadTimer( apdr_s.xAbccWdTimer, ABCC_WATCHDOG_TIMEOUT_TIME_MS );


         if( !( apdr_s.bLastStatusReg & ABP_CTRL_R_BIT ) )
         {
            /*
            ** The ABCC is not ready to accept more commands.
            */

            if( APLINK_GetNextMessage( APLINK_RESPONSE_QUEUE, &apdr_s.psWriteMsgBuffer ) )
            {
               /*
               ** We have a response to send.
               */

               apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;


               /*
               ** Set the last sent message to response.
               */

               apdr_s.bLastSentMsgType = APDR_MESSAGE_RESPONSE_TYPE;
            }
            else
            {
               /*
               ** We have no responses to send.
               */

               apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE;

            } /* End if( APLINK_GetNextMessage( APLINK_RESPONSE_QUEUE, &apdr_s.psWriteMsgBuffer ) ) */

         }
         else
         {
            /*
            ** The ABCC is ready to accept more commands.
            */

            if( apdr_s.bLastSentMsgType == APDR_MESSAGE_COMMAND_TYPE )
            {
               /*
               ** The last message we sent was a Command, thus we shall try to
               ** send a response this time.
               */

               if( APLINK_GetNextMessage( APLINK_RESPONSE_QUEUE, &apdr_s.psWriteMsgBuffer ) )
               {
                  /*
                  ** We have a response to send.
                  */

                  apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;

                  /*
                  ** Set the last sent message to response.
                  */

                  apdr_s.bLastSentMsgType = APDR_MESSAGE_RESPONSE_TYPE;
               }
               else
               {
                  if( APLINK_GetNextMessage( APLINK_COMMAND_QUEUE, &apdr_s.psWriteMsgBuffer ) )
                  {
                     /*
                     ** We have a command to send.
                     */

                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;

                     /*
                     ** Set the last sent message to command.
                     */

                     apdr_s.bLastSentMsgType = APDR_MESSAGE_COMMAND_TYPE;
                  }
                  else
                  {
                     /*
                     ** We have no command to send.
                     */

                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE;

                  } /* End if( APLINK_GetNextMessage( APLINK_COMMAND_QUEUE, &apdr_s.psWriteMsgBuffer ) ) */

               } /* End if( Response available? ) */

            }
            else
            {
               /*
               ** The last message we sent was a response, thus this time we
               ** shall try first to send a command.
               */

               if( APLINK_GetNextMessage( APLINK_COMMAND_QUEUE, &apdr_s.psWriteMsgBuffer ) )
               {
                  /*
                  ** We have a command to send.
                  */

                  apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;

                  /*
                  ** Set the last sent message to command.
                  */

                  apdr_s.bLastSentMsgType = APDR_MESSAGE_COMMAND_TYPE;
               }
               else
               {
                  if( APLINK_GetNextMessage( APLINK_RESPONSE_QUEUE, &apdr_s.psWriteMsgBuffer ) )
                  {
                     /*
                     ** We have a response to send.
                     */

                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;

                     /*
                     ** Set the last sent message to response.
                     */

                     apdr_s.bLastSentMsgType = APDR_MESSAGE_RESPONSE_TYPE;
                  }
                  else
                  {
                     /*
                     ** We have no response to send.
                     */

                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE;
                  }

               } /* End if( Command available? ) */

            } /* End if( Last message == Command ) */

         } /* End if( ABCC R bit == 0 ) */


         break; /* End case APDR_SM_IDLE: */


      } /* End switch( apdr_s.eState ) */

   } /* End while( 1 ) */

} /* End of APDR_RunDataLinkStateMachine() */

#endif /* End of #ifdef ABCC_ACCESS_MODE_PARALLEL */

/*******************************************************************************
**
** End of apdr_par.c
**
********************************************************************************
*/
