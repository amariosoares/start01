/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** aplink.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This module contains the public interface for the ABCC driver Data Link
** services.
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
#include "apdr.h"
#include "abcc.h"
#include "util.h"
#include "aplink.h"
#include "aplink_priv.h"


/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/

UTIL_BufMsgType      APLINK_asRspToAbccBuf[ ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS ];
UTIL_BufMsgType      APLINK_asCmdToAbccBuf[ ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS ];
UTIL_BufMsgType      APLINK_asMsgToApplBuf[ APLINK_MAX_NBR_COMMANDS_IN_USE ];


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

aplink_DataType         aplink_s;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** APLINK_Init()
**------------------------------------------------------------------------------
*/

void APLINK_Init( void )
{
   /*
   ** This function initialises all structures/resources used by APLINK.
   */

   /*
   ** The following resources are needed:
   ** - Resource lock for process data from application to ABCC.
   ** - Resource lock for process data from ABCC to application.
   **
   ** - Queue for responses sent from the application to the ABCC.
   ** - Queue for commands sent from the application to the ABCC.
   ** - Queue for responses/commands sent from the ABCC to the application.
   ** - Resource lock for protection of the Queues that are used for
   **   command/response exchange.
   */

   /*
   ** Create the queue systems.
   */

   aplink_s.xRspsToAbccQueue = UTIL_CreateMessageQueue( aplink_s.pasRspToAbcc,
                                                        ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS );


   aplink_s.xCmdsToAbccQueue = UTIL_CreateMessageQueue( aplink_s.pasCmdToAbcc,
                                                        ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );


   aplink_s.xMsgsFromAbccQueue = UTIL_CreateMessageQueue( aplink_s.pasMsgToAppl,
                                                          ( ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS +
                                                            ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS ) );


   /*
   ** Initialise variables.
   */

   aplink_s.bNbrOfCmdsActiveFromAbcc   = (UINT8)0;
   aplink_s.bNbrOfCmdsActiveToAbcc     = (UINT8)0;
   aplink_s.bLastAbccState             = (UINT8)0xFF;

#ifdef ABCC_ACCESS_MODE_SERIAL
   aplink_s.iNewWritePdLength          = (UINT16)0;
#endif

   aplink_s.fRemappingReadPd           = FALSE;
   aplink_s.fRemappingWritePd          = FALSE;

   /*
   ** Clear the process data buffers.
   */

#if defined( ABCC_COMPARE_READ_PD ) || defined( ABCC_ENABLE_READ_PD_FUNCTION )
   SYS_ClearMemBytes( aplink_s.abReadDataBuffer, ABCC_READ_PD_BUFFER_LENGTH );
#endif
   SYS_ClearMemBytes( aplink_s.abWriteDataBuffer, ABCC_WRITE_PD_BUFFER_LENGTH );


} /* End of APLINK_Init() */


/*------------------------------------------------------------------------------
** APLINK_ChangeDriverState()
**------------------------------------------------------------------------------
*/

void APLINK_ChangeDriverState( APLINK_DriverStateType eRequestedState )
{
   /*
   ** Set the new state of the driver.
   */


   /*
   ** First we check if the requested state is "Shutdown", since it is allowed
   ** in all states.
   */

   if( eRequestedState == APLINK_SM_IN_SHUTDOWN )
   {
      aplink_s.eDriverState = APLINK_SM_IN_SHUTDOWN;

      /*
      ** Shutdown all services...
      ** We cannot do any shutdown services in the state-machine itself since
      ** it is not certain that the application runs the state-machine any more.
      */

      UTIL_ClearTimerSystem();
      UTIL_ClearMessageQueueSystem();

      UTIL_ClearMsgBuffer( &APLINK_asRspToAbccBuf[ 0 ],
                           ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS );

      UTIL_ClearMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                           ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      UTIL_ClearMsgBuffer( &APLINK_asMsgToApplBuf[ 0 ],
                           APLINK_MAX_NBR_COMMANDS_IN_USE );

      ABCC_CbfDebugInfo( "APLINK: Change to APLINK_SM_IN_SHUTDOWN." );
   }
   else
   {
      /*
      ** Check that it is allowed to proceed to this state.
      */

      switch( aplink_s.eDriverState )
      {
      case APLINK_SM_IN_RESET:
         /*
         ** In this state we only allow one additional transition:
         ** - To "APLINK_SM_SETUP"
         */

         if( eRequestedState == APLINK_SM_SETUP )
         {
            aplink_s.eDriverState = eRequestedState;

            ABCC_CbfDebugInfo( "APLINK: Change to APLINK_SM_SETUP." );
         }
         else
         {
            ABCC_CbfDriverError( ABCC_SEV_WARNING,
                                 ABCC_EC_STATE_TRANSITION_NOT_ALLOWED );
         } /* End if( eRequestedState == Allowed? ) */

         break; /* End case APLINK_SM_IN_RESET: */


      case APLINK_SM_SETUP:
         /*
         ** In this state we only allow one additional transition:
         ** - To "APLINK_SM_IN_CONFIGURATION"
         */

         if( eRequestedState == APLINK_SM_IN_CONFIGURATION )
         {
            aplink_s.eDriverState = eRequestedState;

            ABCC_CbfDebugInfo( "APLINK: Change to APLINK_SM_IN_CONFIGURATION." );
         }
         else
         {
            ABCC_CbfDriverError( ABCC_SEV_WARNING,
                                 ABCC_EC_STATE_TRANSITION_NOT_ALLOWED );
         } /* End if( eRequestedState == Allowed? ) */

         break; /* End case APLINK_SM_SETUP: */


      case APLINK_SM_IN_CONFIGURATION:
         /*
         ** In this state we only allow one additional transition:
         ** - To "APLINK_SM_IN_RUN_NO_PD"
         */

         if( eRequestedState == APLINK_SM_IN_RUN_NO_PD )
         {
            aplink_s.eDriverState = eRequestedState;

            ABCC_CbfDebugInfo( "APLINK: Change to APLINK_SM_IN_RUN_NO_PD." );
         }
         else
         {
            ABCC_CbfDriverError( ABCC_SEV_WARNING,
                                 ABCC_EC_STATE_TRANSITION_NOT_ALLOWED );
         } /* End if( eRequestedState == Allowed? ) */

         break; /* End case APLINK_SM_IN_CONFIGURATION: */


      case APLINK_SM_IN_RUN_NO_PD:
         /*
         ** In this state we only allow one additional transition:
         ** - To "APLINK_SM_IN_RUN"
         */

         if( eRequestedState == APLINK_SM_IN_RUN )
         {
            aplink_s.eDriverState = eRequestedState;

            ABCC_CbfDebugInfo( "APLINK: Change to APLINK_SM_IN_RUN." );
         }
         else
         {
            ABCC_CbfDriverError( ABCC_SEV_WARNING,
                                 ABCC_EC_STATE_TRANSITION_NOT_ALLOWED );
         } /* End if( eRequestedState == Allowed? ) */

         break; /* End case APLINK_SM_IN_RUN_NO_PD: */


      case APLINK_SM_IN_RUN:
         /*
         ** In this state we don't allow any additional transitions. The only
         ** way to get out of this state is through the "Shutdown" command.
         */

         ABCC_CbfDriverError( ABCC_SEV_WARNING, ABCC_EC_STATE_TRANSITION_NOT_ALLOWED );

         break; /* End case APLINK_SM_IN_RUN: */


      case APLINK_SM_IN_SHUTDOWN:
         /*
         ** In this state we only allow one additional transition:
         ** - To "APLINK_SM_IN_RESET"
         */

         if( eRequestedState == APLINK_SM_IN_RESET )
         {
            aplink_s.eDriverState = eRequestedState;
            ABCC_CbfDebugInfo( "APLINK: Change to APLINK_SM_IN_RESET." );

         }
         else
         {
            ABCC_CbfDriverError( ABCC_SEV_WARNING,
                                 ABCC_EC_STATE_TRANSITION_NOT_ALLOWED );
         } /* End if( eRequestedState == Allowed? ) */

         break; /* End case APLINK_SM_IN_SHUTDOWN: */


      default:
         /*
         ** This state is illegal.
         */

         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_UNKNOWN_STATE_ENTERED );

         break; /* End default: */
      } /* End of switch( aplink_s.eDriverState ) */

   } /* End if( eRequestedState == APLINK_SM_IN_SHUTDOWN ) */

} /* End of APLINK_ChangeDriverState() */


/*------------------------------------------------------------------------------
** APLINK_RunStateMachine()
**------------------------------------------------------------------------------
*/

void APLINK_RunStateMachine( void )
{
   /*
   ** Run the driver state-machine.
   */

   switch( aplink_s.eDriverState )
   {
   case APLINK_SM_IN_RESET:
      /*
      ** In this state we will do nothing. We will wait for someone to put us
      ** into "SETUP" state.
      */
      break; /* End case APLINK_SM_IN_RESET: */


   case APLINK_SM_SETUP:
      /*
      ** OK, the application has finally ordered us to start doing something.
      ** We will initialise all the internal resources. After that we will
      ** proceed to the "Configuration" state.
      */

      APDR_Init();
      APLINK_Init();


      /*
      ** Proceed to configuration state.
      */

      aplink_s.eCfgState   = APLINK_CFG_SM_INIT;
      aplink_s.eDriverState = APLINK_SM_IN_CONFIGURATION;

      break; /* End case APLINK_SM_SETUP: */


   case APLINK_SM_IN_CONFIGURATION:
      /*
      ** The application has the opportunity to let us setup the process data
      ** mapping for him. We will do that for him in this state and when finished
      ** we will wait for the application to send the configuration completed
      ** flag to be set...
      */

      APDR_RunDataLinkStateMachine();
      aplink_RunCfgStateMachine();

      break; /* End case APLINK_SM_IN_CONFIGURATION: */


   case APLINK_SM_IN_RUN_NO_PD:
      /*
      ** In this state the application has got the opportunity to do additional
      ** configuration. We will just wait for the application to send the
      ** "Setup complete" command, thus letting us send process data too.
      */

      APDR_RunDataLinkStateMachine();
      aplink_HandleQueuedMsgsToAppl();

      /*
      ** Initially we always signal that we have new data.
      */

      aplink_s.fNewWritePdAvailable = TRUE;

      break; /* End case APLINK_SM_IN_RUN_NO_PD: */


   case APLINK_SM_IN_RUN:
      /*
      ** OK, we have now entered the "final" state, the application is now able
      ** both send messages and exchange process data.
      */

      APDR_RunDataLinkStateMachine();
      aplink_HandleQueuedMsgsToAppl();

      break; /* End case APLINK_SM_IN_RUN: */


   case APLINK_SM_IN_SHUTDOWN:
      /*
      ** The application has requested that the driver shall be shutdown. This
      ** means that we will release all resources that are allocated.
      ** Furthermore, internal variables will be cleared.
      */


      /*
      ** We will now proceed to the "Reset" state.
      */

      aplink_s.eDriverState = APLINK_SM_IN_RESET;

      break; /* End case APLINK_SM_IN_SHUTDOWN: */



   default:
      /*
      ** This state is illegal.
      */

      ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_UNKNOWN_STATE_ENTERED );

      break; /* End default: */

   } /* End switch( aplink_s.eDriverState ) */


} /* End of APLINK_RunStateMachine() */


/*------------------------------------------------------------------------------
** APLINK_WriteProcessData()
**------------------------------------------------------------------------------
*/

void APLINK_WriteProcessData( UINT8* pbData )
{


   SYS_UseCritical();


   /*
   ** The Application wants to write new WRITE data to the ABCC module.
   */

   SYS_EnterCritical();


#ifdef ABCC_COMPARE_WRITE_PD

   /*
   ** Copy the ProcessData from the Application, but first: compare to see
   ** if data is new.
   */

   if( SYS_CompareMemBytes( (UINT8*)&aplink_s.abWriteDataBuffer[ 0 ],
                            (UINT8*)pbData,
                             aplink_s.iWritePdLength ) != NULL )
   {
      /*
      ** New data.
      */

      SYS_CopyMemBytes( (UINT8*)&aplink_s.abWriteDataBuffer[ 0 ],
                         pbData,
                         aplink_s.iWritePdLength );

      /*
      ** Signal that the WRITE data is updated.
      */

      aplink_s.fNewWritePdAvailable = TRUE;

   } /* End if( New data? ) */

#else

   /*
   ** Copy the ProcessData from the Application.
   ** No comparison necessary.
   */

   SYS_CopyMemBytes( (UINT8*)&aplink_s.abWriteDataBuffer[ 0 ],
                      pbData,
                      aplink_s.iWritePdLength );

   /*
   ** Signal that the WRITE data is updated.
   */

   aplink_s.fNewWritePdAvailable = TRUE;

#endif /* end #ifdef ABCC_COMPARE_WRITE_PD */

   SYS_ExitCritical();

} /* End of APLINK_WriteProcessData() */


/*------------------------------------------------------------------------------
** APLINK_ReadProcessData()
**------------------------------------------------------------------------------
*/
#ifdef ABCC_ENABLE_READ_PD_FUNCTION
void APLINK_ReadProcessData( UINT8* pbData )
{


   SYS_UseCritical();


   /*
   ** The application wants to read process data from the ABCC module.
   */

   SYS_EnterCritical();

   /*
   ** Copy the data.
   */

   SYS_CopyMemBytes( pbData,
                     (UINT8*)&aplink_s.abReadDataBuffer[ 0 ],
                     aplink_s.iReadPdLength );


   SYS_ExitCritical();


} /* End of APLINK_ReadProcessData() */
#endif  /* end #ifdef ABCC_ENABLE_READ_PD_FUNCTION */

/*------------------------------------------------------------------------------
** APLINK_MessageReceived()
**------------------------------------------------------------------------------
*/

void APLINK_MessageReceived( UTIL_BufMsgType* psQEntry )
{
   /*
   ** A new message from the ABCC is received (this function is called by APDR).
   ** The following must be controlled:
   ** - Is it a response or command?
   ** - If response:
   **    - Is it successfull response to write ABCC 1/5?
   **      Yes - Setup process data sizes, go to "APLINK_SM_IN_RUN"-state.
   ** - If command:
   **    - Increase counter for commands beeing processed.
   */

   /*
   ** Swap the Instance to native format.
   */

   psQEntry->sMsg.sHeader.iInstance = UTIL_WordToNative( psQEntry->sMsg.sHeader.iInstance );



   if( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) != (UINT8)0 )
   {
      /*
      ** This is a command.
      */

      /*
      ** Increase the number of commands that we are currently dealing with.
      */

      aplink_s.bNbrOfCmdsActiveFromAbcc++;
   }
   else
   {
      /*
      ** This is a response. Check if this response is a response to
      ** set Attribute of Anybus object, instance 1, attribute 5.
      */

      if( ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_ANB ) &&
          ( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) == (UINT8)0 ) &&
          ( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_CMD_BITS ) == (UINT8)ABP_CMD_SET_ATTR ) &&
          ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
          ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)ABP_ANB_IA_SETUP_COMPLETE ) &&
          ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)0x00 ) &&
          ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)0x00 ) )
      {
         /*
         ** This is a positive response to write to Setup completed flag.
         */

         ABCC_CbfDebugInfo( "APLINK: Recived ACK to setup completed." );

         /*
         ** Request the lengths of the process data from the application.
         */

         ABCC_CbfRequestPdSize( &aplink_s.iReadPdLength, &aplink_s.iWritePdLength );

         /*
         ** Set the process data lengths used by the application driver.
         */

         APDR_SetPdSize( aplink_s.iReadPdLength, aplink_s.iWritePdLength );

         /*
         ** Since the process data is likely changed we signal that there's new
         ** data to process.
         */

         aplink_s.fNewWritePdAvailable = TRUE;

         /*
         ** Change the state of the driver to "RUN" so that we are able to
         ** exchange process data.
         */

         APLINK_ChangeDriverState( APLINK_SM_IN_RUN );

      } /* End if( Write "Setup complete"? ) */

   } /* End if( psMessage->sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) */


   /*
   ** Add the message to the queue system.
   */

   if( UTIL_AddToMessageQueue( aplink_s.xMsgsFromAbccQueue, psQEntry ) != UTIL_OK )
   {
      /*
      ** Failed to add the buffer to the queue system. Delte the buffer.
      */

      UTIL_FreeMsgBuffer( psQEntry );

   }

} /* End of APLINK_MessageReceived() */


/*------------------------------------------------------------------------------
** APLINK_PutProcessData()
**------------------------------------------------------------------------------
*/

void APLINK_PutProcessData( UINT8 bAbccStatus,
                            UINT8* pbSourceData,
                            UINT16 iLengthOfProcessData )
{
   UINT8*   pbDataPtr = NULL;
   BOOL8    fReportToAppl = FALSE;
   BOOL8    fNewData = FALSE;


   SYS_UseCritical();


   /*
   ** Check if the state or SUP-bit is changed.
   */

   if( bAbccStatus != aplink_s.bLastAbccState )
   {
      /*
      ** State is changed. Report this to the application.
      */

      fReportToAppl = TRUE;


      /*
      ** Assign the new state to our State variable.
      */

      aplink_s.bLastAbccState = bAbccStatus;

   } /* End if( bAbccStatus != aplink_s.bLastAbccState ) */


   if( aplink_s.fRemappingReadPd )
   {
      /*
      ** Do not update process data while a REMAP_ADI_READ_AREA
      ** command is being processed since there is no way to synchronize
      ** the low level switch of process data with the application.
      **
      ** Set the pointer to our read process data buffer (containing old data).
      */

      pbDataPtr = (UINT8*)&aplink_s.abReadDataBuffer[ 0 ];
   }
   else if( aplink_s.eDriverState == APLINK_SM_IN_RUN )
   {
#ifdef ABCC_COMPARE_READ_PD
      /*
      ** Compare the new data.
      */

      if( SYS_CompareMemBytes( pbSourceData,
                               (UINT8*)&aplink_s.abReadDataBuffer[ 0 ],
                               iLengthOfProcessData ) == NULL )
      {
         /*
         ** No new data, do nothing.
         */
      }
      else
      {
         /*
         ** Copy the new data.
         */

         SYS_EnterCritical();

         SYS_CopyMemBytes( (UINT8*)&aplink_s.abReadDataBuffer[ 0 ],
                           pbSourceData,
                           iLengthOfProcessData );

         SYS_ExitCritical();

         fNewData       = TRUE;
         fReportToAppl  = TRUE;

      } /* End if( New data? ) */


      /*
      ** Set the pointer to our read process data buffer.
      */

      pbDataPtr = (UINT8*)&aplink_s.abReadDataBuffer[ 0 ];

#else
      /*
      ** We will not compare the data at all, we consider the data to be new
      ** all the time.
      **
      ** If the function ABCC_ReadProcessData() is disabled we will not need to
      ** copy the process data to our buffers.
      */

#ifdef ABCC_ENABLE_READ_PD_FUNCTION
      SYS_EnterCritical();

      SYS_CopyMemBytes( (UINT8*)&aplink_s.abReadDataBuffer[ 0 ],
                        pbSourceData,
                        iLengthOfProcessData );

      SYS_ExitCritical();

      /*
      ** Set the pointer to our read process data buffer.
      */

      pbDataPtr = (UINT8*)&aplink_s.abReadDataBuffer[ 0 ];
#else

      /*
      ** Set the pointer to the buffer from lower-layer.
      */

      pbDataPtr = pbSourceData;
#endif /* end #ifdef ABCC_ENABLE_READ_PD_FUNCTION */

      /*
      ** Always signal that the data is new.
      */

      fNewData       = TRUE;
      fReportToAppl  = TRUE;


#endif /* end #ifdef ABCC_COMPARE_READ_PD */

   } /* End if( aplink_s.eDriverState != APLINK_SM_IN_RUN ) */


   if( fReportToAppl )
   {
      ABCC_CbfNewReadPd( aplink_s.bLastAbccState,
                         fNewData,
                         iLengthOfProcessData,
                         pbDataPtr );

   } /* End if( fReportToAppl )  */

} /* End of APLINK_PutProcessData() */


/*------------------------------------------------------------------------------
** APLINK_GetNewProcessData()
**------------------------------------------------------------------------------
*/

BOOL8 APLINK_GetNewProcessData( UINT8* pbDestData, UINT16 iLengthOfProcessData )
{
   BOOL8             fStatus = FALSE;


   SYS_UseCritical();


   /*
   ** The APDR wants to read new WRITE data from the application
   */

   if( aplink_s.fRemappingWritePd )
   {
      /*
      ** Do not update process data while a REMAP_ADI_WRITE_AREA
      ** command is being processed since there is no way to synchronize
      ** the low level switch of process data with the application.
      */

      return( FALSE );
   }

   /*
   ** Check that we are in "APLINK_SM_IN_RUN" state.
   */

   if( aplink_s.eDriverState != APLINK_SM_IN_RUN )
   {
      /*
      ** The application driver doesn't need new process data at this point.
      */

      return( FALSE );
   }


   if( iLengthOfProcessData == 0 )
   {
      /*
      ** The lower layer driver wants to copy zero bytes of data.
      ** That is actually fine, just make sure that we dont't clear the
      ** flag indicating that there is no longer any new data.
      */

      return( TRUE );

   } /* end if( iLengthOfProcessData == 0 ) */


   SYS_EnterCritical();

   /*
   ** Check if thers any NEW data. If not we wont care about copying the
   ** data to the drivers buffer.
   */

   if( aplink_s.fNewWritePdAvailable == TRUE )
   {
      aplink_s.fNewWritePdAvailable = FALSE;

      /*
      ** Copy the ProcessData from the Application "Write Area"
      */

      SYS_CopyMemBytes( pbDestData,
                        (UINT8*)&aplink_s.abWriteDataBuffer[ 0 ],
                        iLengthOfProcessData );

      fStatus = TRUE;
   }


   SYS_ExitCritical();


   return( fStatus );

} /* End of APLINK_GetNewProcessData() */


/*------------------------------------------------------------------------------
** APLINK_FreeCmdSlots()
**------------------------------------------------------------------------------
*/

BOOL8 APLINK_FreeCmdSlots( void )
{
   /*
   ** Check if we are able to store more commands.
   ** Commands that are sent from the ABCC module to us.
   ** Function will return TRUE if there are free slots and FALSE if there are
   ** no free slots.
   */

   if( ( aplink_s.bNbrOfCmdsActiveFromAbcc < (UINT8)ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS ) &&
       ( UTIL_EntriesLeftInMessageQueue( aplink_s.xMsgsFromAbccQueue ) > 0 ) )
   {
      /*
      ** We are able to accept more commands.
      */

      return( TRUE );
   }
   else
   {
      /*
      ** We cannot accept more commands for the moment.
      */

      return( FALSE );
   } /* End if( aplink_s.bNbrOfCmdsActiveFromAbcc; >= ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS ) */

} /* End of APLINK_FreeCmdSlots() */


/*------------------------------------------------------------------------------
** APLINK_GetNextMessage()
**------------------------------------------------------------------------------
*/

BOOL8 APLINK_GetNextMessage( UINT8 bQueueSelected, UTIL_BufMsgType** ppsQEntry )
{
   /*
   ** This function will get a message from the selected queue. If there are
   ** no messages available it will return FALSE, otherwise TRUE.
   */

   if( bQueueSelected == (UINT8)APLINK_COMMAND_QUEUE )
   {
      /*
      ** Try to read a command.
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xCmdsToAbccQueue, ppsQEntry ) == UTIL_QUEUE_EMPTY )
      {
         /*
         ** No message in this queue.
         */

         return( FALSE );
      }
      else
      {
         /*
         ** New command available.
         */

         return( TRUE );

      } /* End if( New command available? ) */

   }
   else if( bQueueSelected == (UINT8)APLINK_RESPONSE_QUEUE )
   {
      /*
      ** Try to read a response.
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xRspsToAbccQueue, ppsQEntry ) == UTIL_QUEUE_EMPTY )
      {
         /*
         ** No message in this queue.
         */

         return( FALSE );
      }
      else
      {
         /*
         ** New response available.
         **
         ** The counter which holds the number of active commands from
         ** ABCC module is decremented when the APDR object has transmitted the
         ** response (signaled with function "APLINK_CompleteMsgTrans(...)").
         */

         return( TRUE );

      } /* End if( New response available? ) */

   }
   else
   {
      /*
      ** Oops, illegal queue selected.
      */

      ABCC_CbfDriverError( ABCC_SEV_WARNING, ABCC_EC_QUEUE_SELECTION_ERROR );

      return( FALSE );

   } /* End if( bQueueSelected == APLINK_COMMAND_QUEUE ) */

} /* End of APLINK_GetNextMessage() */


/*------------------------------------------------------------------------------
** APLINK_CompleteMsgTrans()
**------------------------------------------------------------------------------
*/

void APLINK_CompleteMsgTrans( UTIL_BufMsgType* psQEntry )
{

   SYS_UseCritical();


   /*
   ** The APDR has sent the message received from APLINK. Now, we can
   ** free the allocated buffer and decrease our
   ** aplink_s.bNbrOfCmdsActiveFromAbcc counter if the message were a response
   ** to a command.
   */

   SYS_EnterCritical();

   if( !( psQEntry->sMsg.sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) )
   {
      /*
      ** Response!
      **
      ** Decrease the counter which holds the number of active commands from
      ** ABCC module since we have now responded to his last command.
      */

      if( aplink_s.bNbrOfCmdsActiveFromAbcc > (UINT8)0 )
      {
         aplink_s.bNbrOfCmdsActiveFromAbcc--;
      }

      if( ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_APPD ) &&
          ( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) == (UINT8)0 ) &&
          ( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_CMD_BITS ) == (UINT8)ABP_APPD_REMAP_ADI_WRITE_AREA ) &&
          ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)0x02 ) )
      {
         /*
         ** This is an ACK to a REMAP_ADI_WRITE_AREA command.
         */

#ifdef ABCC_ACCESS_MODE_PARALLEL
         /*
         ** Change the write process data length according to what's stated
         ** in the ACK (psQEntry->sMsg.abData[ 0 & 1 ]).
         */

         aplink_s.iWritePdLength = UTIL_WordToNative( *((UINT16*) &psQEntry->sMsg.abData[ 0 ] ) );

         /*
         ** Set the process data lengths used by the application driver and
         ** indicate that the remapping process is completed.
         */

         APDR_SetPdSize( aplink_s.iReadPdLength, aplink_s.iWritePdLength );
         aplink_s.fRemappingWritePd = FALSE;
#else
         /*
         ** The write process data length shall be changed at a point later
         ** indicated by APDR.
         ** Note the new write process data length according to what's stated
         ** in the ACK (psQEntry->sMsg.abData[ 0 & 1 ]) and signal the event
         ** to APDR.
         */

         aplink_s.iNewWritePdLength = UTIL_WordToNative( *( (UINT16*)&psQEntry->sMsg.abData[ 0 ] ) );
         APDR_PrepareNewWritePdMap();
#endif /* ABCC_ACCESS_MODE_PARALLEL */

      } /* end if( ACK to REMAP_ADI_WRITE_AREA ) */

      if( ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_APPD ) &&
          ( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) == (UINT8)0 ) &&
          ( ( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_CMD_BITS ) == (UINT8)ABP_APPD_REMAP_ADI_READ_AREA ) &&
          ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)0x02 ) )
      {
         /*
         ** This is an ACK to a REMAP_ADI_READ_AREA command.
         ** Change the read process data length according to what's stated
         ** in the ACK (psQEntry->sMsg.abData[ 0 & 1 ]).
         */

         aplink_s.iReadPdLength = UTIL_WordToNative( *( (UINT16*)&psQEntry->sMsg.abData[ 0 ] ) );

         /*
         ** Set the process data lengths used by the application driver and
         ** indicate that the remapping process is completed.
         */

         APDR_SetPdSize( aplink_s.iReadPdLength, aplink_s.iWritePdLength );
         aplink_s.fRemappingReadPd = FALSE;

      } /* end if( ACK to a REMAP_ADI_READ_AREA ) */

   } /* End if( !( psQEntry->sMsg.sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) ) */


   /*
   ** Free the buffer.
   */

   UTIL_FreeMsgBuffer( psQEntry );


   SYS_ExitCritical();

} /* End of APLINK_CompleteMsgTrans() */


/*------------------------------------------------------------------------------
** APLINK_SendMessage()
**------------------------------------------------------------------------------
*/

ABCC_StatusType APLINK_SendMessage( ABP_MsgType* psMessage )
{
   UTIL_BufMsgType*     psQEntry;


   /*
   ** Send a message to one of our ABCC queues.
   ** First we will check that we are in a state where the application is
   ** allowed to send messages.
   */

   if( ( aplink_s.eDriverState == APLINK_SM_IN_RUN_NO_PD ) ||
       ( aplink_s.eDriverState == APLINK_SM_IN_RUN ) )
   {
      /*
      ** Allocate a buffer for the message.
      */

      if( psMessage->sHeader.bCmd & ABP_MSG_HEADER_C_BIT )
      {
         psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                            ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );
      }
      else
      {
         psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asRspToAbccBuf[ 0 ],
                                            ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS );
      }


      if( psQEntry == NULL )
      {
         return( ABCC_STATUS_OUT_OF_RESOURCES );
      }

      if( ( psMessage->sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_APPD ) &&
          ( ( psMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) == (UINT8)0 ) &&
          ( ( psMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_CMD_BITS ) == (UINT8)ABP_APPD_REMAP_ADI_WRITE_AREA ) &&
          ( psMessage->sHeader.bDataSize == (UINT8)0x02 ) )
      {
         /*
         ** This is an ACK to a REMAP_ADI_WRITE_AREA command.
         ** Indicate that a switch of the write process data map is being
         ** proccessed.
         */

         aplink_s.fRemappingWritePd = TRUE;

      } /* end if( ACK to REMAP_ADI_WRITE_AREA ) */

      if( ( psMessage->sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_APPD ) &&
          ( ( psMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) == (UINT8)0 ) &&
          ( ( psMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_CMD_BITS ) == (UINT8)ABP_APPD_REMAP_ADI_READ_AREA ) &&
          ( psMessage->sHeader.bDataSize == (UINT8)0x02 ) )
      {
         /*
         ** This is an ACK to a REMAP_ADI_READ_AREA command.
         ** Indicate that a switch of the read process data map is being
         ** proccessed.
         */

         aplink_s.fRemappingReadPd = TRUE;

      } /* end if( ACK to a REMAP_ADI_READ_AREA ) */

      /*
      ** Copy the message to the queue info structure.
      */

      SYS_CopyMemBytes( (UINT8*)&psQEntry->sMsg,
                        (UINT8*)psMessage,
                        (UINT16)( sizeof( ABP_MsgHeaderType ) +
                                  psMessage->sHeader.bDataSize ) );

#ifdef ABCC_ACCESS_MODE_SERIAL
      psQEntry->sFragInfo.bNbrOfFragments = (UINT8)0;
      psQEntry->sFragInfo.bFragCounter    = (UINT8)0;
#endif

      return( aplink_AddFragAndPostMsg( psQEntry ) );
   }
   else
   {
      /*
      ** The application is not allowed to issue request to the ABCC module
      ** right now.
      */

      return( ABCC_STATUS_NOT_ALLOWED_DRIVER_STATE );
   } /* End if( Allowed state? ) */

} /* End of APLINK_SendMessage() */


#ifdef ABCC_ACCESS_MODE_SERIAL
/*------------------------------------------------------------------------------
** APLINK_UseNewWritePdMap()
**------------------------------------------------------------------------------
*/

void APLINK_UseNewWritePdMap( void )
{
   /*
   ** It's now time to use the write process data map that was the result of
   ** a previous REMAP_ADI_WRITE_AREA command.
   */

   aplink_s.iWritePdLength = aplink_s.iNewWritePdLength;

   /*
   ** Set the process data lengths used by the application driver and
   ** indicate that the remapping process is completed.
   */

   APDR_SetPdSize( aplink_s.iReadPdLength, aplink_s.iWritePdLength );
   aplink_s.fRemappingWritePd = FALSE;

} /* end of APLINK_UseNewWritePdMap() */
#endif /* ABCC_ACCESS_MODE_SERIAL */


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** aplink_RunCfgStateMachine()
**------------------------------------------------------------------------------
*/

void aplink_RunCfgStateMachine( void )
{
   UTIL_BufMsgType*  psQEntry;
   static UINT16     iMapCntr;
   static UINT16     iSingleEntrySize;
   UINT16*           piTemp;


   switch( aplink_s.eCfgState )
   {
   case APLINK_CFG_SM_INIT:
      /*
      ** Initialise variables etc.
      */

      aplink_s.psReadCfg   = NULL;
      aplink_s.psWriteCfg  = NULL;

      aplink_s.iReadPdLength  = 0;
      aplink_s.iWritePdLength = 0;

      aplink_s.eCfgState = APLINK_CFG_SM_READ_MODULE_TYPE;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_INIT." );
      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_READ_MODULE_TYPE." );


      break; /* End case APLINK_CFG_SM_INIT: */


   case APLINK_CFG_SM_READ_MODULE_TYPE:
      /*
      ** Allocate a buffer.
      */

      psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      if( psQEntry == NULL )
      {
         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );
      }


      /*
      ** Read the module type of the ABCC module.
      */

      psQEntry->sMsg.sHeader.bSourceId = (UINT8)0x01;
      psQEntry->sMsg.sHeader.bDestObj  = (UINT8)ABP_OBJ_NUM_ANB;
      psQEntry->sMsg.sHeader.iInstance = 0x0001;
      psQEntry->sMsg.sHeader.bCmd      = (UINT8)( ABP_CMD_GET_ATTR | ABP_MSG_HEADER_C_BIT );
      psQEntry->sMsg.sHeader.bDataSize = (UINT8)0x00;
      psQEntry->sMsg.sHeader.bCmdExt0  = (UINT8)ABP_ANB_IA_MODULE_TYPE;
      psQEntry->sMsg.sHeader.bCmdExt1  = (UINT8)0x00;

      /*
      ** Post the message to the ABCC.
      */

      (void)aplink_AddFragAndPostMsg( psQEntry );


      /*
      ** Wait for the response.
      */

      aplink_s.eCfgState = APLINK_CFG_SM_WAIT_MODULE_TYPE_RSP;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_WAIT_MODULE_TYPE_RSP." );

      break; /* End case APLINK_CFG_SM_READ_MODULE_TYPE */


   case APLINK_CFG_SM_WAIT_MODULE_TYPE_RSP:
      /*
      ** Check if theres a new response for us.
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
      {
         /*
         ** Decrease the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc--;


         /*
         ** We have received a new message.
         */

         if( ( psQEntry->sMsg.sHeader.bSourceId == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_ANB ) &&
             ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) ) ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) ) ) &&
             ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)ABP_ANB_IA_MODULE_TYPE_DS ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)ABP_ANB_IA_MODULE_TYPE ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)0x00 ) )
         {
            /*
            ** Response is OK. Read the data attached.
            */

            piTemp = (UINT16*)&psQEntry->sMsg.abData[ 0 ];
            aplink_s.iModuleType = UTIL_WordToNative( *piTemp );

            ABCC_CbfDebugInfo( "APLINK/Configuration: Module type response received." );

            /*
            ** Read the network data-type.
            */

            aplink_s.eCfgState = APLINK_CFG_SM_READ_NW_TYPE;

            ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_READ_NW_TYPE." );
         }
         else
         {
            /*
            ** There is some error in the response.
            */

            ABCC_CbfDebugInfo( "APLINK/Configuration: Module type error response." );

            /*
            ** Signal a fatal error to the application
            */

            ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_INVALID_MODULE_TYPE_RECEIVED );

            /*
            ** Change driver state, so the applicaton is able to send a RESET
            */

            APLINK_ChangeDriverState( APLINK_SM_IN_RUN_NO_PD );

         } /* End ( Response is not OK? ) */

         /*
         ** Free the allocated buffer.
         */

         UTIL_FreeMsgBuffer( psQEntry );

      } /* End if( New message? ) */

      break; /* End case APLINK_CFG_SM_WAIT_MODULE_TYPE_RSP: */


   case APLINK_CFG_SM_READ_NW_TYPE:
      /*
      ** Allocate a buffer.
      */

      psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      if( psQEntry == NULL )
      {
         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );
      }


      /*
      ** Read the network type of the ABCC module.
      */

      psQEntry->sMsg.sHeader.bSourceId  = (UINT8)0x01;
      psQEntry->sMsg.sHeader.bDestObj   = (UINT8)ABP_OBJ_NUM_NW;
      psQEntry->sMsg.sHeader.iInstance  = 0x0001;
      psQEntry->sMsg.sHeader.bCmd       = (UINT8)( ABP_CMD_GET_ATTR | ABP_MSG_HEADER_C_BIT );
      psQEntry->sMsg.sHeader.bDataSize  = (UINT8)0x00;
      psQEntry->sMsg.sHeader.bCmdExt0   = (UINT8)ABP_NW_IA_NW_TYPE;
      psQEntry->sMsg.sHeader.bCmdExt1   = (UINT8)0x00;


      /*
      ** Post the message to the ABCC module.
      */

      (void)aplink_AddFragAndPostMsg( psQEntry );


      /*
      ** Fetch the response.
      */

      aplink_s.eCfgState = APLINK_CFG_SM_READ_NW_TYPE_RSP;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_READ_NW_TYPE_RSP." );

      break; /* End case APLINK_CFG_SM_READ_NW_TYPE: */


   case APLINK_CFG_SM_READ_NW_TYPE_RSP:
      /*
      ** Check if theres a new response for us.
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
      {
         /*
         ** Decrease the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc--;


         /*
         ** We have received a new message.
         */

         if( ( psQEntry->sMsg.sHeader.bSourceId == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_NW ) &&
             ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) ) ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) ) ) &&
             ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)ABP_NW_IA_NW_TYPE_DS ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)ABP_NW_IA_NW_TYPE ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)0x00 ) )
         {
            /*
            ** Response is OK. Read the data attached.
            */

            piTemp = (UINT16*)&psQEntry->sMsg.abData[ 0 ];
            aplink_s.iNetworkType = UTIL_WordToNative( *piTemp );

            ABCC_CbfDebugInfo( "APLINK/Configuration: Network type response received." );


            /*
            ** Read the network parameter support.
            */

            aplink_s.eCfgState = APLINK_CFG_SM_READ_NW_PARAM_SUPPORT;

            ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_READ_NW_PARAM_SUPPORT." );

         }
         else
         {
            /*
            ** There is some error in the response.
            */

            ABCC_CbfDebugInfo( "APLINK/Configuration: Network Type error response." );

            /*
            ** Signal a fatal error to the application
            */

            ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_INVALID_NETWORK_TYPE_RECEIVED );

            /*
            ** Change driver state, so the applicaton is able to send a RESET
            */

            APLINK_ChangeDriverState( APLINK_SM_IN_RUN_NO_PD );

         } /* End if( Response OK? ) */

         /*
         ** Free the allocated buffer.
         */

         UTIL_FreeMsgBuffer( psQEntry );

      } /* End if( New message? ) */

      break; /* End case APLINK_CFG_SM_READ_NW_TYPE_RSP: */


   case APLINK_CFG_SM_READ_NW_PARAM_SUPPORT:
      /*
      ** Allocate a buffer.
      */

      psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      if( psQEntry == NULL )
      {
         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );
      }


      /*
      ** Read the network parameter support of the ABCC module.
      */

      psQEntry->sMsg.sHeader.bSourceId  = (UINT8)0x01;
      psQEntry->sMsg.sHeader.bDestObj   = (UINT8)ABP_OBJ_NUM_NW;
      psQEntry->sMsg.sHeader.iInstance  = 0x0001;
      psQEntry->sMsg.sHeader.bCmd       = (UINT8)( ABP_CMD_GET_ATTR | ABP_MSG_HEADER_C_BIT );
      psQEntry->sMsg.sHeader.bDataSize  = (UINT8)0x00;
      psQEntry->sMsg.sHeader.bCmdExt0   = (UINT8)ABP_NW_IA_PARAM_SUPPORT;
      psQEntry->sMsg.sHeader.bCmdExt1   = (UINT8)0x00;


      /*
      ** Post the message to the ABCC module.
      */

      (void)aplink_AddFragAndPostMsg( psQEntry );


      /*
      ** Fetch the response.
      */

      aplink_s.eCfgState = APLINK_CFG_SM_READ_NW_PARAM_SUPPORT_RSP;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_READ_NW_PARAM_SUPPORT_RSP." );


      break; /* End case APLINK_CFG_SM_READ_NW_PARAM_SUPPORT: */


   case APLINK_CFG_SM_READ_NW_PARAM_SUPPORT_RSP:
      /*
      ** Check if theres a new response for us.
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
      {
         /*
         ** Decrease the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc--;


         /*
         ** We have received a new message.
         */

         if( ( psQEntry->sMsg.sHeader.bSourceId == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_NW ) &&
             ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) ) ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) ) ) &&
             ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)ABP_NW_IA_PARAM_SUPPORT_DS ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)ABP_NW_IA_PARAM_SUPPORT ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)0x00 ) )
         {
            /*
            ** Response is OK. Read the data attached.
            */

            aplink_s.bParamSupport = psQEntry->sMsg.abData[ 0 ];

            ABCC_CbfDebugInfo( "APLINK/Configuration: Parameter support response received." );


            /*
            ** Request pointers for the configuration.
            */

            ABCC_CbfAutoCfgRequest( aplink_s.iModuleType,
                                    aplink_s.iNetworkType,
                                    aplink_s.bParamSupport,
                                    &aplink_s.psReadCfg,
                                    &aplink_s.psWriteCfg );


            /*
            ** Read the network data format support.
            */

            aplink_s.eCfgState = APLINK_CFG_SM_READ_NW_DATA_FORMAT;

            ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_READ_NW_DATA_FORMAT." );
         }
         else
         {
            /*
            ** There is some error in the response.
            */

            ABCC_CbfDebugInfo( "APLINK/Configuration: Parameter support error response." );

            /*
            ** Signal a fatal error to the application
            */

            ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_INVALID_PARAM_SUPPORT_RECEIVED );

            /*
            ** Change driver state, so the applicaton is able to send a RESET
            */

            APLINK_ChangeDriverState( APLINK_SM_IN_RUN_NO_PD );

         } /* End if( Response OK? ) */

         /*
         ** Free the allocated buffer.
         */

         UTIL_FreeMsgBuffer( psQEntry );

      } /* End if( New message? ) */

      break; /* End case APLINK_CFG_SM_READ_NW_PARAM_SUPPORT_RSP: */


   case APLINK_CFG_SM_READ_NW_DATA_FORMAT:
      /*
      ** Allocate a buffer.
      */

      psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      if( psQEntry == NULL )
      {
         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );
      }


      /*
      ** Read the Network data format of the module.
      */

      psQEntry->sMsg.sHeader.bSourceId  = (UINT8)0x01;
      psQEntry->sMsg.sHeader.bDestObj   = (UINT8)ABP_OBJ_NUM_NW;
      psQEntry->sMsg.sHeader.iInstance  = 0x0001;
      psQEntry->sMsg.sHeader.bCmd       = (UINT8)( ABP_CMD_GET_ATTR | ABP_MSG_HEADER_C_BIT );
      psQEntry->sMsg.sHeader.bDataSize  = (UINT8)0x00;
      psQEntry->sMsg.sHeader.bCmdExt0   = (UINT8)ABP_NW_IA_DATA_FORMAT;
      psQEntry->sMsg.sHeader.bCmdExt1   = (UINT8)0x00;

      /*
      ** Post the message to the ABCC.
      */

      (void)aplink_AddFragAndPostMsg( psQEntry );


      /*
      ** Wait for the response.
      */

      aplink_s.eCfgState = APLINK_CFG_SM_WAIT_NW_DATA_FORMAT_RSP;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_WAIT_NW_DATA_FORMAT_RSP." );

      break; /* End case APLINK_CFG_SM_READ_NW_DATA_FORMAT: */


   case APLINK_CFG_SM_WAIT_NW_DATA_FORMAT_RSP:
      /*
      ** Check if theres a new response for us.
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
      {
         /*
         ** Decrease the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc--;


         /*
         ** We have received a new message.
         */

         if( ( psQEntry->sMsg.sHeader.bSourceId == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_NW ) &&
             ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) ) ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) ) ) &&
             ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)ABP_NW_IA_DATA_FORMAT_DS ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)ABP_NW_IA_DATA_FORMAT ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)0x00 ) )
         {
            /*
            ** Signal the network data format to the application.
            */

            ABCC_CbfNetworkDataFormat( (ABCC_DataFormatType)psQEntry->sMsg.abData[ 0 ] );

            ABCC_CbfDebugInfo( "APLINK/Configuration: Network data-type response received." );

            iMapCntr = 0;

            /*
            ** Change state
            */

            aplink_s.eCfgState = APLINK_CFG_SM_NEXT_READ;

         }
         else
         {
            /*
            ** There is some error in the response.
            */

            ABCC_CbfDebugInfo( "APLINK/Configuration: Network data-format error response." );

            /*
            ** Signal a fatal error to the application
            */

            ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_INVALID_DATA_FORMAT_RECEIVED );

            /*
            ** Change driver state, so the applicaton is able to send a RESET
            */

            APLINK_ChangeDriverState( APLINK_SM_IN_RUN_NO_PD );

         } /* End if( New Message OK? ) */

         /*
         ** Free the allocated buffer.
         */

         UTIL_FreeMsgBuffer( psQEntry );

      } /* End if( New message? ) */

      break; /* End case APLINK_CFG_SM_WAIT_NW_DATA_FORMAT_RSP: */


   case APLINK_CFG_SM_SEND_READ_MAPPING:
      /*
      ** Allocate a buffer.
      */

      psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      if( psQEntry == NULL )
      {
         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );
      }


      /*
      ** Send a Read mapping message
      */

      psQEntry->sMsg.sHeader.bSourceId = (UINT8)0x01;
      psQEntry->sMsg.sHeader.bDestObj  = (UINT8)ABP_OBJ_NUM_NW;
      psQEntry->sMsg.sHeader.iInstance = 0x0001;
      psQEntry->sMsg.sHeader.bCmd      = (UINT8)( ABP_NW_CMD_MAP_ADI_READ_AREA | ABP_MSG_HEADER_C_BIT );
      psQEntry->sMsg.sHeader.bDataSize = (UINT8)0x04;
      psQEntry->sMsg.sHeader.bCmdExt0  = (UINT8)( aplink_s.psReadCfg->psMaps[ iMapCntr ].iAdiNbr & 0x00FF );
      psQEntry->sMsg.sHeader.bCmdExt1  = (UINT8)( ( aplink_s.psReadCfg->psMaps[ iMapCntr ].iAdiNbr >> 8 ) & 0x00FF );

      /*
      ** Assign the data type and the number of elements.
      */

      psQEntry->sMsg.abData[ 0 ] = aplink_s.psReadCfg->psMaps[ iMapCntr ].bDataType;
      psQEntry->sMsg.abData[ 1 ] = aplink_s.psReadCfg->psMaps[ iMapCntr ].bNbrElements;

      /*
      ** Assign the order number. Low byte first.
      */

      psQEntry->sMsg.abData[ 2 ]  = (UINT8)( aplink_s.psReadCfg->psMaps[ iMapCntr ].iOrderNumber & 0x00FF );
      psQEntry->sMsg.abData[ 3 ]  = (UINT8)( ( aplink_s.psReadCfg->psMaps[ iMapCntr ].iOrderNumber >> 8 ) & 0x00FF );

      /*
      ** Store the number of bytes of this mapping.
      */

      iSingleEntrySize = ( aplink_GetSizeOfDataType( aplink_s.psReadCfg->psMaps[ iMapCntr ].bDataType ) *
                           aplink_s.psReadCfg->psMaps[ iMapCntr ].bNbrElements );


      /*
      ** Post the message to the ABCC.
      */

      (void)aplink_AddFragAndPostMsg( psQEntry );


      /*
      ** Change state
      */

      aplink_s.eCfgState = APLINK_CFG_SM_WAIT_READ_MAPPING_RSP;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_WAIT_READ_MAPPING_RSP." );

      break; /* End case APLINK_CFG_SM_SEND_READ_MAPPING: */


   case APLINK_CFG_SM_WAIT_READ_MAPPING_RSP:
      /*
      ** Waite for a response
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
      {
         /*
         ** Decrease the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc--;


         /*
         ** We have received a new message.
         */

         if( ( psQEntry->sMsg.sHeader.bSourceId == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_NW ) &&
             ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) ) ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) ) ) &&
             ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)( aplink_s.psReadCfg->psMaps[ iMapCntr ].iAdiNbr & 0x00FF ) ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)( ( aplink_s.psReadCfg->psMaps[ iMapCntr ].iAdiNbr >> 8 ) & 0x00FF ) ) )
         {
            ABCC_CbfDebugInfo( "APLINK/Configuration: Read map added OK." );

            /*
            ** Response is OK. Read the data attached.
            */

            aplink_s.psReadCfg->psMaps[ iMapCntr ].bAreaOffset = psQEntry->sMsg.abData[ 0 ];
            aplink_s.psReadCfg->psMaps[ iMapCntr ].bMapStatus  = (UINT8)1;

            /*
            ** Add the mapped data size.
            */

            aplink_s.iReadPdLength += iSingleEntrySize;

         }
         else
         {
            /*
            ** Error in message
            */

            ABCC_CbfDebugInfo( "APLINK/Configuration: Read map error." );

            /*
            ** Response is NOT OK. Mark the mapping as not mapped.
            */

            aplink_s.psReadCfg->psMaps[ iMapCntr ].bAreaOffset = (UINT8)0;
            aplink_s.psReadCfg->psMaps[ iMapCntr ].bMapStatus  = (UINT8)0;

         } /* End if( correct message? ) */


         /*
         ** Increase the number of mappings.
         */

         iMapCntr++;


         /*
         ** Check for more mappings.
         */

         aplink_s.eCfgState = APLINK_CFG_SM_NEXT_READ;


         /*
         ** Free the allocated buffer.
         */

         UTIL_FreeMsgBuffer( psQEntry );

      } /* End if( New Response? ) */

      break; /* End case APLINK_CFG_SM_WAIT_READ_MAPPING_RSP: */


   case APLINK_CFG_SM_NEXT_READ:
      /*
      ** Check if there are more mappings.
      */

      if( ( aplink_s.psReadCfg != NULL ) &&
          ( aplink_s.psReadCfg->iNbrMaps != 0 ) &&
          ( aplink_s.psReadCfg->iNbrMaps <= 256 ) &&
          ( iMapCntr < aplink_s.psReadCfg->iNbrMaps ) )
      {
         /*
         ** There are more "READ"-mappings left. Change state.
         */

         aplink_s.eCfgState = APLINK_CFG_SM_SEND_READ_MAPPING;
      }
      else
      {
         /*
         ** Check if there is a write mapping available
         */

         if( ( aplink_s.psWriteCfg != NULL ) &&
             ( aplink_s.psWriteCfg->iNbrMaps != 0 ) &&
             ( aplink_s.psWriteCfg->iNbrMaps <= 256 ) )
         {
            /*
            ** There are at least one write mapping available
            */

            iMapCntr = 0;

            /*
            ** Change state
            */

            aplink_s.eCfgState = APLINK_CFG_SM_SEND_WRITE_MAPPING;

            ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_SEND_WRITE_MAPPING." );
         }
         else
         {
            /*
            ** There are NO "WRITE"-mappings. Change driver state.
            */

            APLINK_ChangeDriverState( APLINK_SM_IN_RUN_NO_PD );


            /*
            ** No read or write mapping available
            */

            ABCC_CbfCfgCompleted();

         } /* End if write mapping OK? ) */

      } /* End if( More Read mappings? ) */

      break; /* End case APLINK_CFG_SM_NEXT_READ: */


   case APLINK_CFG_SM_SEND_WRITE_MAPPING:
      /*
      ** Allocate a buffer.
      */

      psQEntry = UTIL_AllocateMsgBuffer( &APLINK_asCmdToAbccBuf[ 0 ],
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS );

      if( psQEntry == NULL )
      {
         ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_BUFFERS );
      }


      /*
      ** Send a Write mapping meassage
      */

      psQEntry->sMsg.sHeader.bSourceId  = (UINT8)0x01;
      psQEntry->sMsg.sHeader.bDestObj   = (UINT8)ABP_OBJ_NUM_NW;
      psQEntry->sMsg.sHeader.iInstance  = 0x0001;
      psQEntry->sMsg.sHeader.bCmd       = (UINT8)( ABP_NW_CMD_MAP_ADI_WRITE_AREA | ABP_MSG_HEADER_C_BIT );
      psQEntry->sMsg.sHeader.bDataSize  = (UINT8)0x04;
      psQEntry->sMsg.sHeader.bCmdExt0   = (UINT8)( aplink_s.psWriteCfg->psMaps[ iMapCntr ].iAdiNbr & 0x00FF );
      psQEntry->sMsg.sHeader.bCmdExt1   = (UINT8)( ( aplink_s.psWriteCfg->psMaps[ iMapCntr ].iAdiNbr >> 8 ) & 0x00FF );

      /*
      ** Assign the data type and number of elements.
      */

      psQEntry->sMsg.abData[ 0 ] = aplink_s.psWriteCfg->psMaps[ iMapCntr ].bDataType;
      psQEntry->sMsg.abData[ 1 ] = aplink_s.psWriteCfg->psMaps[ iMapCntr ].bNbrElements;

      /*
      ** Assign the order number. Low byte first.
      */

      psQEntry->sMsg.abData[ 2 ]  = (UINT8)( aplink_s.psWriteCfg->psMaps[ iMapCntr ].iOrderNumber & 0x00FF );
      psQEntry->sMsg.abData[ 3 ]  = (UINT8)( ( aplink_s.psWriteCfg->psMaps[ iMapCntr ].iOrderNumber >> 8 ) & 0x00FF );


      /*
      ** Store the number of bytes of this mapping.
      */

      iSingleEntrySize = ( aplink_GetSizeOfDataType( aplink_s.psWriteCfg->psMaps[ iMapCntr ].bDataType ) *
                           aplink_s.psWriteCfg->psMaps[ iMapCntr ].bNbrElements );

      /*
      ** Post the message to the ABCC.
      */

      (void)aplink_AddFragAndPostMsg( psQEntry );


      /*
      ** Change state
      */

      aplink_s.eCfgState = APLINK_CFG_SM_WAIT_WRITE_MAPPING_RSP;

      ABCC_CbfDebugInfo( "APLINK/Configuration: In APLINK_CFG_SM_WAIT_WRITE_MAPPING_RSP." );

      break; /* End case APLINK_CFG_SM_SEND_WRITE_MAPPING: */


   case APLINK_CFG_SM_WAIT_WRITE_MAPPING_RSP:
      /*
      ** Waite for a response
      */

      if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
      {
         /*
         ** Decrease the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc--;


         /*
         ** We have received a new message.
         */

         if( ( psQEntry->sMsg.sHeader.bSourceId == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bDestObj == (UINT8)ABP_OBJ_NUM_NW ) &&
             ( psQEntry->sMsg.sHeader.iInstance == 0x0001 ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) ) ) &&
             ( !( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) ) ) &&
             ( psQEntry->sMsg.sHeader.bDataSize == (UINT8)0x01 ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt0 == (UINT8)( aplink_s.psWriteCfg->psMaps[ iMapCntr ].iAdiNbr & 0x00FF ) ) &&
             ( psQEntry->sMsg.sHeader.bCmdExt1 == (UINT8)( ( aplink_s.psWriteCfg->psMaps[ iMapCntr ].iAdiNbr >> 8 ) & 0x00FF ) ) )
         {
            /*
            ** Response is OK. Read the data attached.
            */

            aplink_s.psWriteCfg->psMaps[ iMapCntr ].bAreaOffset = psQEntry->sMsg.abData[ 0 ];
            aplink_s.psWriteCfg->psMaps[ iMapCntr ].bMapStatus  = (UINT8)1;

            /*
            ** Add the mapped data size.
            */

            aplink_s.iWritePdLength += iSingleEntrySize;


            ABCC_CbfDebugInfo( "APLINK/Configuration: Write map OK." );
         }
         else
         {
            /*
            ** Error in message
            */

            ABCC_CbfDebugInfo( "APLINK/Configuration: Write map error." );

            /*
            ** Response is NOT OK. Mark the map.
            */

            aplink_s.psWriteCfg->psMaps[ iMapCntr ].bAreaOffset = (UINT8)0;
            aplink_s.psWriteCfg->psMaps[ iMapCntr ].bMapStatus  = (UINT8)0;

         } /* End if( correct message? ) */


         /*
         ** Increase the number of mappings.
         */

         iMapCntr++;


         /*
         ** Check if more mappings...
         */

         aplink_s.eCfgState = APLINK_CFG_SM_NEXT_WRITE;


         /*
         ** Free the allocated buffer.
         */

         UTIL_FreeMsgBuffer( psQEntry );

      } /* End if( New Response? ) */

      break; /* End case APLINK_CFG_SM_WAIT_WRITE_MAPPING_RSP: */


   case APLINK_CFG_SM_NEXT_WRITE:
      /*
      ** Check if there are more "WRITE"-mappings to process.
      */

      if( iMapCntr < aplink_s.psWriteCfg->iNbrMaps )
      {
         /*
         ** Change state
         */

         aplink_s.eCfgState = APLINK_CFG_SM_SEND_WRITE_MAPPING;
      }
      else
      {
         /*
         ** No more mappings to process.
         ** Change driver state
         */

         APLINK_ChangeDriverState( APLINK_SM_IN_RUN_NO_PD );

         /*
         ** No more mapping available
         */

         ABCC_CbfCfgCompleted();

      } /* End if( More Write mappings? ) */

      break; /* End case APLINK_CFG_SM_NEXT_WRITE: */


   default:

      /*
      ** Signal a fatal error to the application
      */

      ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_ILLEGAL_CFG_STATE );

      break;

   } /* End switch( aplink_s.eCfgState ) */

} /* End of aplink_RunCfgStateMachine() */


/*------------------------------------------------------------------------------
** aplink_HandleQueuedMsgsToAppl()
**------------------------------------------------------------------------------
*/

void aplink_HandleQueuedMsgsToAppl( void )
{
   UTIL_BufMsgType*     psQEntry;


   /*
   ** This function will check the queue for incomining requests from the
   ** ABCC module.
   */

   if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK )
   {
      /*
      ** A short note about the order of doing these things.
      ** Normally you would expect that "aplink_s.bNbrOfCmdsActiveToAbcc"
      ** should not be decremented until the buffer is released. However,
      ** since the application might very well send a new message as soon as
      ** the callback is executed we have to decrement the counter before the
      ** callback is executed (otherwise the application might get an out-of-
      ** resource error message). We can allow this because no other event
      ** can affect this buffer, thus this function will always complete before
      ** any other part of the code (APDR) is able to access this buffer-system
      ** again.
      */

      /*
      ** Check if this is a response message or command. If it is a response
      ** decrease the number of messages that are processing...
      */

      if( !( psQEntry->sMsg.sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) )
      {
         aplink_s.bNbrOfCmdsActiveToAbcc--;
      }


      /*
      ** A new message is copied to our temporary buffer, send this to the
      ** application.
      */

      ABCC_CbfNewMsgFromAbcc( &psQEntry->sMsg );


      /*
      ** Message handled by the application, release the allocated buffer.
      */

      UTIL_FreeMsgBuffer( psQEntry );


   } /* End if( UTIL_GetFirstMessageFromQueue( aplink_s.xMsgsFromAbccQueue, &psQEntry ) == UTIL_OK ) */

} /* End of aplink_HandleQueuedMsgsToAppl() */


/*------------------------------------------------------------------------------
** aplink_AddFragAndPostMsg()
**------------------------------------------------------------------------------
*/

ABCC_StatusType aplink_AddFragAndPostMsg( UTIL_BufMsgType* psQEntry )
{
#ifdef ABCC_ACCESS_MODE_SERIAL
   UINT16   iTotalSize;
#endif


   /*
   ** Send a message to one of our ABCC queues.
   **
   ** Assign the fragmentation information.
   */

#ifdef ABCC_ACCESS_MODE_SERIAL

   iTotalSize = (UINT16)( (UINT16)psQEntry->sMsg.sHeader.bDataSize +
                          sizeof( ABP_MsgHeaderType ) );

   /*
   ** Calculate the number of fragments needed to transmitt the message in
   ** serial mode. Please note that the "empty" fragment is not included (added
   ** in lower layer).
   ** The algorithm calculates how many fragments needed to transfer the
   ** message.
   ** 0  - 16 bytes shall generate 1 fragment.
   ** 17 - 32 bytes shall generate 2 fragments.
   ** etc.
   */

   psQEntry->sFragInfo.bFragCounter      = (UINT8)0;
   psQEntry->sFragInfo.bNbrOfFragments   = (UINT8)( ( iTotalSize / APDR_SERIAL_MESSAGE_FRAG_LENGTH ) );

   if( ( iTotalSize % APDR_SERIAL_MESSAGE_FRAG_LENGTH ) != 0 )
   {
      psQEntry->sFragInfo.bNbrOfFragments++;
   }

#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


   /*
   ** Swap the Instance to native format.
   */

   psQEntry->sMsg.sHeader.iInstance = UTIL_WordToNative( psQEntry->sMsg.sHeader.iInstance );


   /*
   ** ...and post the message.
   */

   if( (BOOL8)( psQEntry->sMsg.sHeader.bCmd & ABP_MSG_HEADER_C_BIT ) )
   {
      /*
      ** Check so that we are not posting more commands than we can accept
      ** responses to.
      */

      if( aplink_s.bNbrOfCmdsActiveToAbcc >= ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS )
      {
         /*
         ** Free the allocated memory and return.
         */

         UTIL_FreeMsgBuffer( psQEntry );


         return( ABCC_STATUS_OUT_OF_RESOURCES );
      }


      /*
      ** Put message in Command queue.
      */

      if( UTIL_AddToMessageQueue( aplink_s.xCmdsToAbccQueue, psQEntry ) == UTIL_OK )
      {
         /*
         ** Increase the number of messages that are processing...
         */

         aplink_s.bNbrOfCmdsActiveToAbcc++;

         return( ABCC_STATUS_OK );
      }
      else
      {
         /*
         ** Free the allocated memory and return.
         */

         UTIL_FreeMsgBuffer( psQEntry );


         return( ABCC_STATUS_OUT_OF_RESOURCES );

      } /* End if( Message posted? ) */

   }
   else
   {
      /*
      ** Put message in response queue.
      */

      if( UTIL_AddToMessageQueue( aplink_s.xRspsToAbccQueue, psQEntry ) == UTIL_OK )
      {
         return( ABCC_STATUS_OK );
      }
      else
      {
         /*
         ** Free the allocated memory and return.
         */

         UTIL_FreeMsgBuffer( psQEntry );


         return( ABCC_STATUS_OUT_OF_RESOURCES );

      } /* End if( Message posted? ) */

   } /* End if( Command or Response? ) */

} /* End of aplink_AddFragAndPostMsg() */


/*------------------------------------------------------------------------------
** aplink_GetSizeOfDataType()
**------------------------------------------------------------------------------
*/

UINT16 aplink_GetSizeOfDataType( UINT8 bDataType )
{
   /*
   ** This function will return the number of bytes a certain data-type occupies.
   */

   switch( bDataType )
   {
   case ABP_BOOL:
      return( ABP_BOOL_SIZEOF );

   case ABP_SINT8:
      return( ABP_SINT8_SIZEOF );

   case ABP_UINT8:
      return( ABP_UINT8_SIZEOF );

   case ABP_SINT16:
      return( ABP_SINT16_SIZEOF );

   case ABP_SINT32:
      return( ABP_SINT32_SIZEOF );

   case ABP_UINT16:
      return( ABP_UINT16_SIZEOF );

   case ABP_UINT32:
      return( ABP_UINT32_SIZEOF );

   case ABP_CHAR:
      return( ABP_CHAR_SIZEOF );

   case ABP_ENUM:
      return( ABP_ENUM_SIZEOF );

   case ABP_SINT64:
      return( ABP_SINT64_SIZEOF );

   case ABP_UINT64:
      return( ABP_UINT64_SIZEOF );

   case ABP_FLOAT:
      return( ABP_FLOAT_SIZEOF );

   default:
      return( 0 );

    } /* end switch( bDataType ) */

} /* end of aplink_GetSizeOfDataType() */


/*******************************************************************************
**
** End of aplink.c
**
********************************************************************************
*/
