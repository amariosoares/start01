/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** apdr_ser.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Contain implementation of the serial part of the data link layer.
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
#include "crc.h"
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

#ifdef ABCC_ACCESS_MODE_SERIAL
/*------------------------------------------------------------------------------
** apdr_BuildSerialTelegram()
**------------------------------------------------------------------------------
*/

UINT16 apdr_BuildSerialTelegram( BOOL8 fMessageData )
{
   UINT8*      pbTempPtr;
   UINT32      lCrcCalcLength;
   UINT16      iCrc;


   /*
   ** This function will build a telegram in the serial transmission buffer so
   ** that it is ready for transmission.
   */


   /*
   ** Add the Control/Status information.
   */

   apdr_s.abTransmissionBuffer[ 0 ] = apdr_s.bWorkingControlReg;



   if( fMessageData )
   {
      /*
      ** Copy the message (fragment of a message) to the transmission buffer.
      */

      if( ( apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments != 0 ) &&
          ( apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter <
            apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments ) )
      {

         pbTempPtr = (UINT8*)&apdr_s.psWriteMsgBuffer->sMsg;
         pbTempPtr += apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter * APDR_SERIAL_MESSAGE_FRAG_LENGTH;

         /*
         ** Copy the fragment.
         */

         SYS_CopyMemBytes( (UINT8*)&apdr_s.abTransmissionBuffer[ 1 ],
                           (UINT8*)pbTempPtr,
                           APDR_SERIAL_MESSAGE_FRAG_LENGTH );

         /*
         ** Increase the fragcounter.
         */

         apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter++;

      }
      else
      {
         /*
         ** Some error with the fragmentation protocol.
         */

         ABCC_CbfDriverError( ABCC_SEV_WARNING, ABCC_EC_SERIAL_FRAG_ERROR );


         /*
         ** Signal end of fragmentation.
         */

         apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter = apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments;

      } /* End if( Fragmentation information correct? ) */

   } /* End if( fMessageData ) */



   /*
   ** Copy the process data to the transmission buffer and prepare for
   ** next fragment (if needed).
   */

   APLINK_GetNewProcessData( (UINT8*)&apdr_s.abTransmissionBuffer[ 17 ],
                             apdr_s.iPdWriteLength );


   lCrcCalcLength = (UINT32)apdr_s.iPdWriteLength +
                    APDR_SERIAL_CTRL_STAT_LENGTH +
                    APDR_SERIAL_MESSAGE_FRAG_LENGTH;

   /*
   ** Calculate the CRC.
   */

   iCrc = CRC_Crc16( (UINT8*)&apdr_s.abTransmissionBuffer[ 0 ],
                     (UINT16)lCrcCalcLength );


   /*
   ** Write the CRC to the buffer.
   */

   apdr_s.abTransmissionBuffer[ lCrcCalcLength ]      = (UINT8)( ( iCrc >> 8 ) & 0x00FF );
   apdr_s.abTransmissionBuffer[ lCrcCalcLength + 1 ]  = (UINT8)( iCrc & 0x00FF );


   /*
   ** Return the length of the transmission buffer.
   */

   return( (UINT16)lCrcCalcLength + APDR_SERIAL_CRC_LENGTH );

} /* End of apdr_BuildSerialTelegram() */


/*------------------------------------------------------------------------------
** APDR_RunDataLinkStateMachine()
**------------------------------------------------------------------------------
*/

void APDR_RunDataLinkStateMachine( void )
{
   UTIL_BufMsgType*  psQEntry;
   ABP_MsgType*      psMsg;
   UINT16            iGeneratedCrc;
   UINT16            iReceivedCrc;
   UINT16            iCopyLength;


   /*
   ** This function will run the lowest layer state-machine in a serial
   ** implementation.
   */

   while( 1 )
   {
      switch( apdr_s.eState )
      {
      case APDR_SM_INIT:
         /*
         ** Initilise variables that are used by the system.
         */

         apdr_s.iPdReadLength          = 0;
         apdr_s.iPdWriteLength         = 0;
         apdr_s.sMsgRead.iNbrRecFrags  = 0;

         apdr_s.fAbccWdToOccured    = FALSE;
         apdr_s.bLastSentMsgType    = APDR_MESSAGE_RESPONSE_TYPE;
         apdr_s.fCompleteWrPdRemap  = FALSE;

         apdr_s.psWriteMsgBuffer = NULL;

         apdr_s.iSerialLengthRead = ( APDR_SERIAL_CTRL_STAT_LENGTH +
                                      APDR_SERIAL_MESSAGE_FRAG_LENGTH +
                                      APDR_SERIAL_CRC_LENGTH );

         SYS_ReStartSerialComm( apdr_s.iSerialLengthRead );


         /*
         ** Set initial values to Control bits (such as T-bit, R-bit, M-bit etc.)
         */

         apdr_s.bWorkingControlReg  = 0;
         apdr_s.bLastStatusReg      = 0;
         apdr_s.fInitHsTo           = FALSE;
         apdr_s.fTelegramReceived   = FALSE;

         /*
         ** Reload and start the initial handshake timer.
         */

         UTIL_ReloadTimer( apdr_s.xInitHsTimer, (UINT16)ABCC_INTIAL_HANDSHAKE_DELAY_TIME_MS );
         UTIL_StartTimer( apdr_s.xInitHsTimer );


         /*
         ** Switch state to intial handshake.
         */

         apdr_s.eState = APDR_SM_INITIAL_HANDSHAKE;

         break; /* End case APDR_SM_INIT: */


      case APDR_SM_INITIAL_HANDSHAKE:
         /*
         ** In case of serial driver the initial handshake procedure means that
         ** we need to wait a certain time before we are allowed to communicate
         ** with the ABCC.
         */

         if( apdr_s.fInitHsTo )
         {
            /*
            ** The initial handshake timer has expired. This means that we are
            ** allowed to proceed to next state.
            */

            UTIL_StopTimer( apdr_s.xInitHsTimer );

            apdr_s.eState = APDR_SM_IDLE;
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
         ** We shall send a telegram containing a message (or part
         ** of) and the process data.
         */

         /*
         ** Set the application M-bit in the transmission buffer.
         */

         apdr_s.bWorkingControlReg |= ABP_CTRL_M_BIT;


         /*
         ** Check if we are ready to accept more commands.
         ** Check with higher layer.
         */

         if( APLINK_FreeCmdSlots() == TRUE )
         {
            /*
            ** Set the application R-bit in the transmission buffer.
            */

            apdr_s.bWorkingControlReg |= ABP_CTRL_R_BIT;
         }
         else
         {
            /*
            ** Clear the application R-bit in the transmission buffer.
            */

            apdr_s.bWorkingControlReg &= ~ABP_CTRL_R_BIT;
         } /* End if( Accept more commands? ) */


         /*
         ** Toggle the application T-bit in the transmission buffer.
         */

         apdr_s.bWorkingControlReg ^= ABP_CTRL_T_BIT;


         /*
         ** Build the serial telegram.
         */

         apdr_s.iSerialLengthWrite = apdr_BuildSerialTelegram( TRUE );


         /*
         ** Post the telegram to the ABCC.
         */

         SYS_SendSerialTelegram( apdr_s.abTransmissionBuffer, apdr_s.iSerialLengthWrite );


         /*
         ** Signal to application that we have transmitted a new telegram.
         */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
         ABCC_CbfTelegramTransmitted( FALSE );
#endif

         /*
         ** Start ABCC watchdog timeout-timer.
         */

         UTIL_StartTimer( apdr_s.xAbccWdTimer );



         /*
         ** Reload and start telegram timeout-timer.
         */

         UTIL_ReloadTimer( apdr_s.xTelegramToTimer, apdr_s.iTelegramToTime );
         UTIL_StartTimer( apdr_s.xTelegramToTimer );


         /*
         ** Goto next state.
         */

         apdr_s.eState = APDR_SM_WAIT_FOR_TELEGRAM;

         /*
         ** Leave the state machine and let the ABCC generate a response
         */

         return;

         break; /* End case APDR_SM_SEND_TELEGRAM_WITH_MESSAGE: */


      case APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE:
         /*
         ** We shall send a telegram containing process data, but no
         ** message.
         **
         ** Clear the message field.
         */

         SYS_ClearMemBytes( (UINT8*)&apdr_s.abTransmissionBuffer[ 1 ],
                            APDR_SERIAL_MESSAGE_FRAG_LENGTH );


         /*
         ** Clear the application M-bit in the transmission buffer.
         */

         apdr_s.bWorkingControlReg &= ~ABP_CTRL_M_BIT;

         /*
         ** Check if we are ready to accept more commands.
         ** Check with higher layer.
         */

         if( APLINK_FreeCmdSlots() == TRUE )
         {
            /*
            ** Set the application R-bit in the transmission buffer.
            */

            apdr_s.bWorkingControlReg |= ABP_CTRL_R_BIT;
         }
         else
         {
            /*
            ** Clear the application R-bit in the transmission buffer.
            */

            apdr_s.bWorkingControlReg &= ~ABP_CTRL_R_BIT;
         } /* End if( Accept more commands? ) */


         /*
         ** Toggle the application T-bit in the transmission buffer.
         */

         apdr_s.bWorkingControlReg ^= ABP_CTRL_T_BIT;


         /*
         ** Build the serial telegram.
         */

         apdr_s.iSerialLengthWrite = apdr_BuildSerialTelegram( FALSE );


         /*
         ** Post the telegram to the ABCC.
         */

         SYS_SendSerialTelegram( apdr_s.abTransmissionBuffer, apdr_s.iSerialLengthWrite );


         /*
         ** Signal to application that we have transmitted a new telegram.
         */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
         ABCC_CbfTelegramTransmitted( FALSE );
#endif


         /*
         ** Start ABCC watchdog timeout-timer.
         */

         UTIL_StartTimer( apdr_s.xAbccWdTimer );


         /*
         ** Reload and start telegram timeout-timer.
         */

         UTIL_ReloadTimer( apdr_s.xTelegramToTimer, apdr_s.iTelegramToTime );
         UTIL_StartTimer( apdr_s.xTelegramToTimer );


         /*
         ** Goto next state.
         */

         apdr_s.eState = APDR_SM_WAIT_FOR_TELEGRAM;

         return;

         break; /* End case APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE: */


      case APDR_SM_WAIT_FOR_TELEGRAM:
         /*
         ** In this state we will check if there are any new events that has
         ** occured.
         */

         if( apdr_s.fTelegramReceived )
         {
            /*
            ** We have received a new telegram!
            */

            apdr_s.fTelegramReceived = FALSE;


            /*
            ** Calculate the CRC of the received data.
            */

            iGeneratedCrc = CRC_Crc16( (UINT8*)apdr_s.pbReceptionBuffer,
                                       (UINT16)( apdr_s.iSerialLengthRead - APDR_SERIAL_CRC_LENGTH ) );

            /*
            ** Read the CRC that is sent with the telegram.
            */

            iReceivedCrc  = (UINT16)apdr_s.pbReceptionBuffer[ apdr_s.iSerialLengthRead - 2 ] << 8;
            iReceivedCrc |= apdr_s.pbReceptionBuffer[ apdr_s.iSerialLengthRead - 1 ];


            if( ( ( apdr_s.bLastStatusReg & ABP_CTRL_T_BIT ) !=
                  ( apdr_s.pbReceptionBuffer[ 0 ] & ABP_CTRL_T_BIT ) ) &&
                ( iGeneratedCrc == iReceivedCrc ) )
            {
               /*
               ** The T-bit and CRC indicate that we have received a telegram.
               ** Copy the ABCC status register from the reception buffer to our
               ** internal structure.
               */

               apdr_s.bCurrentStatusReg = apdr_s.pbReceptionBuffer[ 0 ];

               /*
               ** Stop the ABCC watchdog timeout timer.
               */

               UTIL_StopTimer( apdr_s.xAbccWdTimer );


               /*
               ** Stop the telegram time-out timer
               */

               UTIL_StopTimer( apdr_s.xTelegramToTimer );


               /*
               ** Clear the timeout timer flags.
               */

               apdr_s.fAbccWdTo        = FALSE;
               apdr_s.fAbccTelegramTo  = FALSE;


               /*
               ** Signal that we no longer have a WD timeout situation.
               */

               if( apdr_s.fAbccWdToOccured )
               {
                  ABCC_CbfWdTimeoutRecovered();

                  apdr_s.fAbccWdToOccured = FALSE;
               }

               if( apdr_s.fCompleteWrPdRemap )
               {
                  /*
                  ** Reception of a telegram at this point shows that the ABCC
                  ** has received an ACK to a REMAP_ADI_WRITE_AREA command.
                  ** Now is the time to use this new write process data map.
                  */

                  APLINK_UseNewWritePdMap();

                  apdr_s.fCompleteWrPdRemap = FALSE;
               }

               /*
               ** Process the new data telegram in next state.
               */

               apdr_s.eState = APDR_SM_PROCESS_RECEIVED_TELEGRAM;
            }
            else
            {
               /*
               ** We have received a telegram, but it turned out that the CRC is
               ** incorrect, or the T-bit is not toggled. Thus, we will simply
               ** ignore it.
               */

            } /* end if( T-bit correct and iGeneratedCrc == iReceivedCrc ) */

         }
         else
         {
            /*
            ** No new telegram for us.
            */

            if( apdr_s.fAbccTelegramTo )
            {
               /*
               ** A telegram Time-out has occured
               */

               apdr_s.fAbccTelegramTo = FALSE;


               /*
               ** Re-start the communication so that we are certain that the
               ** reception buffer does not contain any rubbish data.
               */

               SYS_ReStartSerialComm( apdr_s.iSerialLengthRead );


               /*
               ** A Retransmission is required, Transmit the data again.
               */

               SYS_SendSerialTelegram( apdr_s.abTransmissionBuffer, apdr_s.iSerialLengthWrite );


               /*
               ** Signal to application that we have transmitted a new telegram.
               */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
               ABCC_CbfTelegramTransmitted( TRUE );
#endif

               /*
               ** Reload & Start the telegram Time-out timer
               */

               UTIL_ReloadTimer( apdr_s.xTelegramToTimer, apdr_s.iTelegramToTime );
               UTIL_StartTimer( apdr_s.xTelegramToTimer );


               return;

            }
            else
            {
               if( apdr_s.fAbccWdTo )
               {
                  /*
                  ** Stop the ABCC watchdog timeout timer.
                  */

                  UTIL_StopTimer( apdr_s.xAbccWdTimer );

                  /*
                  ** An ABCC timeout has occured.
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

            } /* End if( Telegram Timeout? ) */

         } /* End if( new telegram received ) */

         break; /* End case APDR_SM_WAIT_FOR_TELEGRAM: */


      case APDR_SM_PROCESS_RECEIVED_TELEGRAM:
         /*
         ** A funtion call from lower level have indicated a new telegram from
         ** the serial channel
         */


         /*
         ** Check if M-Bit is set
         */

         if( apdr_s.bCurrentStatusReg & ABP_CTRL_M_BIT )
         {
            /*
            ** This was a message
            **
            ** Store incomming message-fragment for later use.
            */

            if( apdr_s.sMsgRead.iNbrRecFrags < ( APDR_SERIAL_MESSAGE_MAX_FRAGS - 1 ) )
            {

               SYS_CopyMemBytes( (UINT8*)&apdr_s.sMsgRead.abMessage[ apdr_s.sMsgRead.iNbrRecFrags * APDR_SERIAL_MESSAGE_FRAG_LENGTH ],
                                 (UINT8*)&apdr_s.pbReceptionBuffer[ 1 ],
                                 APDR_SERIAL_MESSAGE_FRAG_LENGTH );
            }
            else if( apdr_s.sMsgRead.iNbrRecFrags == ( APDR_SERIAL_MESSAGE_MAX_FRAGS - 1 ) )
            {
               SYS_CopyMemBytes( (UINT8*)&apdr_s.sMsgRead.abMessage[ apdr_s.sMsgRead.iNbrRecFrags * APDR_SERIAL_MESSAGE_FRAG_LENGTH ],
                                 (UINT8*)&apdr_s.pbReceptionBuffer[ 1 ],
                                 APDR_SERIAL_MESSAGE_LAST_FRAG_LEN );
            }
            else
            {
               /*
               ** More message-fragments than we can handle. Alert the user.
               */

               ABCC_CbfDriverError( ABCC_SEV_WARNING, ABCC_EC_SERIAL_FRAG_ERROR );

            } /* End if( apdr_s.sMsgRead.iNbrRecFrags < ( APDR_SERIAL_MESSAGE_MAX_FRAGS - 1 ) ) */


            /*
            ** Increase the number of message-fragments received.
            */

            apdr_s.sMsgRead.iNbrRecFrags++;
         }
         else
         {
            /*
            ** ABCC M-Bit is zero
            */

            /*
            ** Check the previous M-Bit
            */

            if( !( apdr_s.bLastStatusReg & ABP_CTRL_M_BIT ) )
            {
               /*
               ** No action.
               ** Last telegram we received contained a M-bit that was zero.
               */

            }
            else
            {
               /*
               ** Send message to higer level.
               */

               psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asMsgToApplBuf[ 0 ], APLINK_MAX_NBR_COMMANDS_IN_USE );

               if( psQEntry == NULL )
               {
                  ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );

                  return;
               }


               psMsg       = (ABP_MsgType*)&apdr_s.sMsgRead.abMessage[ 0 ];
               iCopyLength = psMsg->sHeader.bDataSize + sizeof( ABP_MsgHeaderType );

               if( iCopyLength > ( ABP_MAX_MSG_DATA_BYTES + sizeof( ABP_MsgHeaderType ) ) )
               {
                  iCopyLength = ( ABP_MAX_MSG_DATA_BYTES + sizeof( ABP_MsgHeaderType ) );
               }


               SYS_CopyMemBytes( (UINT8*)&psQEntry->sMsg,
                                 (UINT8*)&apdr_s.sMsgRead.abMessage[ 0 ],
                                 iCopyLength );


               APLINK_MessageReceived( psQEntry );


               /*
               ** Clear message-fragmentation information
               */

               apdr_s.sMsgRead.iNbrRecFrags = 0;

            } /* End of if( Prev M-Bit == 0 ) */

         } /* End of if( ABCC M-bit == 1 ) */


         /*
         ** Send the process data up to higer level
         */

         APLINK_PutProcessData( (UINT8)apdr_GetAbccStateAndSup(),
                                (UINT8*)&apdr_s.pbReceptionBuffer[ 17 ],
                                apdr_s.iPdReadLength );


         /*
         ** Check if we need to send more message-fragments
         */

         if( apdr_s.psWriteMsgBuffer == NULL )
         {
            /*
            ** Change state
            */

            apdr_s.eState = APDR_SM_IDLE;
         }
         else if( ( apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments != 0 ) &&
                  ( apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter < apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments ) )
         {
            /*
            ** Change state
            */

            apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;
         }
         else
         {
            /*
            ** Check if last message-fragment sent was the last
            */

            if( ( apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments != 0 ) &&
                ( apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter == apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments ) )
            {
               /*
               ** Clear all send message-fragmentation infomation
               */

               apdr_s.psWriteMsgBuffer->sFragInfo.bFragCounter     = 0;
               apdr_s.psWriteMsgBuffer->sFragInfo.bNbrOfFragments  = 0;

               /*
               ** Change state
               */

               apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE;



               /*
               ** Tell Aplink that the message has been sent. Aplink will
               ** release the buffer used to hold the write-message.
               */

               APLINK_CompleteMsgTrans( apdr_s.psWriteMsgBuffer );


               /*
               ** Set the buffer pointer to NULL indicating that we have no
               ** buffer available.
               */

               apdr_s.psWriteMsgBuffer = NULL;

            }
            else
            {
               /*
               ** Change state
               */

               apdr_s.eState = APDR_SM_IDLE;

            } /* End of if( More Frags To Send ) */

         } /* End of if( More Message-fragments To Send ) */


         /*
         ** Copy this new ABCC status register to "Last".
         */

         apdr_s.bLastStatusReg = apdr_s.bCurrentStatusReg;

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
         ** Clear the ABCC watchdog timeout flag.
         */

         apdr_s.fAbccWdTo        = FALSE;
         apdr_s.fAbccTelegramTo  = FALSE;


         /*
         ** A Retransmission is required, transmit the data again.
         */

         SYS_SendSerialTelegram( apdr_s.abTransmissionBuffer, apdr_s.iSerialLengthWrite );


         /*
         ** Signal to application that we have transmitted a new telegram.
         */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
         ABCC_CbfTelegramTransmitted( TRUE );
#endif

         /*
         ** Reload & Start the telegram Time-out timer
         */

         UTIL_ReloadTimer( apdr_s.xTelegramToTimer, apdr_s.iTelegramToTime );
         UTIL_StartTimer( apdr_s.xTelegramToTimer );


         /*
         ** Move to next state
         */

         apdr_s.eState = APDR_SM_WAIT_FOR_TELEGRAM;

         return;

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

            } /* End if( Any responses available? ) */

         }
         else
         {
            /*
            ** ABCC is ready to accept new command(s).
            */

            if( apdr_s.bLastSentMsgType == APDR_MESSAGE_COMMAND_TYPE )
            {
               /*
               ** The last message we sent was a command.
               */

               if( APLINK_GetNextMessage( APLINK_RESPONSE_QUEUE, &apdr_s.psWriteMsgBuffer ) )
               {
                  /*
                  ** Send a response message.
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

                  if( APLINK_GetNextMessage( APLINK_COMMAND_QUEUE, &apdr_s.psWriteMsgBuffer ) )
                  {
                     /*
                     ** We have a command to send!
                     */

                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITH_MESSAGE;

                     /*
                     ** Set the last sent message to command.
                     */

                     apdr_s.bLastSentMsgType = APDR_MESSAGE_COMMAND_TYPE;
                  }
                  else
                  {
                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE;
                  } /* End if( Command available? ) */

               } /* End if( Response available ) */

            }
            else
            {
               /*
               ** The last message transmitted was a response.
               */

               if( APLINK_GetNextMessage( APLINK_COMMAND_QUEUE, &apdr_s.psWriteMsgBuffer ) )
               {
                  /*
                  ** Check if message needs fragmentation.
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
                  ** Check if there's a response available instead.
                  */

                  if( APLINK_GetNextMessage( APLINK_RESPONSE_QUEUE, &apdr_s.psWriteMsgBuffer ) )
                  {
                     /*
                     ** Send a response message.
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
                     ** Neither response nor command available.
                     */

                     apdr_s.eState = APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE;

                  } /* End if( Response available? ) */

               } /* End if( Command available? ) */

            } /* End if( Our Last Message == Cmd ) */

         } /* End if( ABCC R-bit == 0 ) */

         break; /* End case APDR_SM_IDLE: */


      } /* End switch( apdr_s.eState ) */

   } /* End while( 1 ) */

} /* End of APDR_RunDataLinkStateMachine() */


/*------------------------------------------------------------------------------
** APDR_PrepareNewWritePdMap()
**------------------------------------------------------------------------------
*/

void APDR_PrepareNewWritePdMap( void )
{
   /*
   ** Prepare a switch to new write process data map.
   */

   apdr_s.fCompleteWrPdRemap = TRUE;

} /* end of APDR_PrepareNewWritePdMap() */

#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */

/*******************************************************************************
**
** End of apdr_ser.c
**
********************************************************************************
*/
