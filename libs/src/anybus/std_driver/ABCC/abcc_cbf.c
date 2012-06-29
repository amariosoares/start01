/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** abcc_cbf.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This file contains the functions that are called BY THE DRIVER and are meant
** for the application. The application is free to fill the functions of this
** file with application specific code.
**
********************************************************************************
********************************************************************************
**
** Porting instructions:
** ---------------------
**
** Porting required.
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

#include "stdio.h"
#include "includes.h"
#include "utils.h"
#include "abcc_td.h"
#include "abcc_com.h"
#include "sys_adpt.h"
#include "abcc.h"

#include "appd.h"
#include "app.h"
//#include "dev.h"
#include "dpv1.h"
#include "appc.h"

/*******************************************************************************
**
** Public globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_eAbccState
**------------------------------------------------------------------------------
*/
#define ABCC_CBF_DBG 1
ABP_AnbStateType        ABCC_eAbccState;


/*******************************************************************************
**
** Private globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** abcc_fWdTimeout
**------------------------------------------------------------------------------
*/

BOOL8                   abcc_fWdTimeout = FALSE;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_CbfAutoCfgRequest()
**------------------------------------------------------------------------------
*/

void ABCC_CbfAutoCfgRequest( UINT16 iModuleType,
                             UINT16 iNetworkType,
                             BOOL8 fParameterSupport,
                             ABCC_PdMapType** ppsReadMappings,
                             ABCC_PdMapType** ppsWriteMappings )
{
   /*
   ** This function is called when the driver is about to start the automatic
   ** process data mapping. If no automatic configuration is desired the pointers
   ** are set to NULL, otherwise the are set to point at the structures containing
   ** mapping information.
   **
   ** Refer to document "SDS-7004-003 ID and type number.xls" for a complete
   ** list of module types. The ABCC module is identified as 0x0401. If the
   ** connected module is not an ABCC-module it is recommended that the stack
   ** is shutdown. Read from the ABCC object, instance 1 attribute 1.
   **
   ** If the user wants the possibility to set different mappings depending on
   ** fieldbus/module type the iNetworkType variable can be used as a switch.
   ** Read from the Network object, Instance 1, Attribute 1.
   **
   ** Via the fParameterSupport flag it is possible to determine if the
   ** connected network module supports parameter data.
   ** Read from the Network object, Instance 1, Attribute 4.
   **
   ** For more information please refer to the Anybus-CompactCom Software Design Guide.
   */

   /*
   ** APPD handles the ADI to process data mapping
   */
  printf("modue_type=%d,iNetworkType=%d,fParameterSupport=%d\n",iModuleType,iNetworkType,fParameterSupport);
   APPD_MapAdiToPd( ppsReadMappings, ppsWriteMappings );

} /* End of ABCC_CbfAutoCfgRequest() */


/*------------------------------------------------------------------------------
** ABCC_CbfCfgCompleted()
**------------------------------------------------------------------------------
*/

void ABCC_CbfCfgCompleted( void )
{
   /*
   ** The driver has now completed the process data configuration. If the
   ** application is content with the configuration it is now time for the
   ** application to set the “Setup complete” attribute.
   ** After that the application can start to communicate with the ABCC module.
   */

   /*
   ** We start by initializing outgoing process data.
   */

   APPD_ProcessDataChanged();

   /*
   ** Then we let the client finalise the setup procedure.
   */

   APPC_Init();
   APPC_RunStateMachine();

} /* End of ABCC_CbfCfgCompleted() */


#ifdef ABCC_DRIVER_DEBUG
/*------------------------------------------------------------------------------
** ABCC_CbfDebugInfo()
**------------------------------------------------------------------------------
*/

void ABCC_CbfDebugInfo( UINT8* pabText )
{
   /*
   ** Function enabling an application specific DEBUG channel.
   */

   /*
   ** Make sure the line is cleared.
   */

   printf( "                                                                      \r" );

   /*
   ** Print debug text.
   */

   printf( "%s\n", pabText );

} /* End of ABCC_CbfDebugInfo() */

#endif


/*------------------------------------------------------------------------------
** ABCC_CbfDriverError()
**------------------------------------------------------------------------------
*/

void ABCC_CbfDriverError( ABCC_SeverityType eSeverity,
                          ABCC_ErrorCodeType eErrorCode )
{
   /*
   ** An error has occured in the driver.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfDriverError() */


#ifdef ABCC_ACCESS_MODE_PARALLEL
#ifndef ABCC_PARALLEL_POLL_MODE
/*------------------------------------------------------------------------------
** ABCC_CbfEnableInterruptParallel()
**------------------------------------------------------------------------------
*/

void ABCC_CbfEnableInterruptParallel( void )
{
   /*
   ** Clear any pending parallel interrupts. When cleared the interrupts shall
   ** be turned on. Until this function is called no (parallel) interrupts shall
   ** be enabled.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfEnableInterruptParallel() */
#endif /* End #ifdef ABCC_PARALLEL_POLL_MODE */
#endif /* End #ifndef ABCC_ACCESS_MODE_PARALLEL  */


/*------------------------------------------------------------------------------
** ABCC_CbfNetworkDataFormat()
**------------------------------------------------------------------------------
*/

void ABCC_CbfNetworkDataFormat( ABCC_DataFormatType eFormat )
{
   /*
   ** This function is called by the driver during start-up. The passed variable
   ** (bFormat) indicates the data-format of the ADI Value/Max value/Min value
   ** and default value.
   ** If bFormat is set to "0" theses values are sent with LSB first (INTEL).
   ** If bFormat is set to "1" theses values are sent with MSB first (Motorola).
   ** The data-format is controlled by the network, the data is
   ** transparently through the ABCC module to/from the network.
   ** If the application has the same data-format as indicated with bFormat, no
   ** swapping is needed. However, if the formats differ the above mentioned
   ** data needs to be swapped. Ad this is also true for the process data.
   */

   /*
   ** Report the network byte order to APPD
   */

   APPD_SetNetworkByteOrder( eFormat );

} /* End of ABCC_CbfNetworkDataFormat() */


/*------------------------------------------------------------------------------
** ABCC_CbfNewMsgFromAbcc()
**------------------------------------------------------------------------------
*/

void ABCC_CbfNewMsgFromAbcc( ABP_MsgType* psNewMessage )
{
   /*
   ** A new message is received from the ABCC module.
   ** Please note that pointer is only valid within this function.
   */

   /*
   ** Example code of how these messages can be handled.
   */
   int type = psNewMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ;
   
   DEBUG_LOG(ABCC_CBF_DBG,("type=%s\n", type?"cmd":"response"));
   DEBUG_LOG(ABCC_CBF_DBG,("destobj=%d\n",psNewMessage->sHeader.bDestObj ));
   DEBUG_LOG(ABCC_CBF_DBG,("inst=%d\n",psNewMessage->sHeader.iInstance));
   DEBUG_LOG(ABCC_CBF_DBG,("cmd=%d\n",psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS));
   if( ( psNewMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) &&
       ( psNewMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_E_BIT ) )
   {
      /*
      ** Invalid message format ( E and C bits set )
      */

      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_MSG_FORMAT );
      ABCC_SendMessage( psNewMessage );
   }
   else if( ( psNewMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) != (UINT8)0 )
   {

      /*
      ** This is a new command - route to a server by Destination object.
      */

      switch( psNewMessage->sHeader.bDestObj )
      {
      case ABP_OBJ_NUM_APP:

         /*
         ** Destination: Application object.
         */
#ifdef ABCC_DRIVER_DEBUG
         ABCC_CbfDebugInfo( "ABCC_CBF: Command msg to APP." );
#endif
         APP_ProcessCmdMsg( psNewMessage );

         break;

      case ABP_OBJ_NUM_APPD:

         /*
         ** Destination: Application data object.
         */
#ifdef ABCC_DRIVER_DEBUG
         ABCC_CbfDebugInfo( "ABCC_CBF: Command msg to APPD." );
#endif
         APPD_ProcessCmdMsg( psNewMessage );

         break;

      case ABP_OBJ_NUM_DEV:

         /*
         ** Destination: DeviceNet object.
         */
#ifdef ABCC_DRIVER_DEBUG
         ABCC_CbfDebugInfo( "ABCC_CBF: Command msg to DEV." );
#endif
         DEV_ProcessCmdMsg( psNewMessage );
         break;

      case ABP_OBJ_NUM_DPV1:

         /*
         ** Destination: Profibus DPV1 object.
         */
#ifdef ABCC_DRIVER_DEBUG
         ABCC_CbfDebugInfo( "ABCC_CBF: Command msg to DPV1." );
#endif
         DPV1_ProcessCmdMsg( psNewMessage );
         break;

      default:

         /*
         ** Couldn't find a server object to route the command to.
         ** Return an unsupported object error response.
         */
#ifdef ABCC_DRIVER_DEBUG
            ABCC_CbfDebugInfo( "ABCC_CBF: Command msg to unsupported object." );
#endif
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_OBJ );

         ABCC_SendMessage( psNewMessage );

         break;

      } /* end switch( Destination object ) */
   }
   else
   {

      /*
      ** It's a response - route to the client that uses this Source ID.
      **
      ** A suggested approach is to let every client reserve and use a
      ** fixed number of the available Source ID's.
      ** Each client is then responsible for keeping track of individual
      ** messages.
      **
      ** However, in this example, there is only one client (the APPC), so
      ** all responses are routed there.
      */

      APPC_ProcessRespMsg( psNewMessage );

   } /* End if( Control Error and Command bits ) */

} /* End of ABCC_CbfNewMsgFromAbcc() */


/*------------------------------------------------------------------------------
** ABCC_CbfNewReadPd()
**------------------------------------------------------------------------------
*/

void ABCC_CbfNewReadPd( UINT8 bAbccStatus,
                        BOOL8 fNewData,
                        UINT16 iPdReadLength,
                        UINT8* pabData )
{
   static char   aabStates[8][16] = { { "SETUP" }, { "NW_INIT" }, { "WAIT_PROCESS" }, { "IDLE" }, { "PROCESS_ACTIVE" }, { "ERROR" }, { "" }, { "EXCEPTION" } };
   static ABP_AnbStateType eAbccPrevState;
   BOOL8                   fAbccSupervised;
   

   /*
   ** This function indicates that the driver has received new data from the
   ** ABCC module and/or that the ABCC status is changed.
   */

   /*
   ** PORTING ALERT!
   */

   /*
   ** Get the ABCC state and supvervised bit.
   */

   ABCC_eAbccState   = (ABP_AnbStateType)( bAbccStatus & ABP_STAT_S_BITS );
   fAbccSupervised   = (ABP_AnbStateType)( bAbccStatus & ABP_STAT_SUP_BIT );

   if( ABCC_eAbccState != eAbccPrevState )
   {
      /*
      ** Make sure the line is cleared.
      */

      printf( "                                                                      \r" );
      printf("ABCC state: %s\n", aabStates[ ABCC_eAbccState ] );
   }

   if( !abcc_fWdTimeout )
   {
      switch( ABCC_eAbccState )
      {
      case ABP_ANB_STATE_PROCESS_ACTIVE:
   
         /*
         ** The network process data channel is active and error free.
         ** The application should perform normal data handling.
         */
   
         if( fNewData || ( eAbccPrevState != ABP_ANB_STATE_PROCESS_ACTIVE ) )
         {
            /*
            ** The read process data is both valid and changed.
            */
   
            APPD_CbfNewReadProcessData();
         }
   
         break;
   
      case ABP_ANB_STATE_SETUP:
      case ABP_ANB_STATE_NW_INIT:
   
         /*
         ** Initialisation states.
         ** Read process data is not yet valid.
         **
         ** The application should keep write process data updated in state
         ** NW_INIT (initial data) since the data is buffered by the ABCC
         ** and may be sent to the network after a state switch.
         ** We make sure of this by calling APPD_ProcessDataChanged() in
         ** ABCC_CbfCfgCompleted().
         */
   
         break;
   
      case ABP_ANB_STATE_WAIT_PROCESS:
   
         /*
         ** The network process data channel is not active.
         ** The application should regard read process data as not valid.
         **
         ** The application should keep write process data updated in this
         ** state since the data is buffered by the ABCC and may be sent
         ** to the network after a state switch.
         */
   
         break;
   
      case ABP_ANB_STATE_IDLE:
   
         /*
         ** The network device is in idle mode (the definition of this mode is
         ** network specific).
         ** Depending on the network type, the read process data may be either
         ** updated or static (unchanged)
         **
         ** The application should keep write process data updated in this
         ** state since the data is buffered by the ABCC and may be sent
         ** to the network after a state switch.
         */
   
         break;
   
      case ABP_ANB_STATE_ERROR:
   
         /*
         ** There is at least one serious network error.
         **
         ** The application should regard read process data as not valid
         ** and may take optional network specific actions.
         */
   
         break;
   
      case ABP_ANB_STATE_EXCEPTION:
   
         /*
         ** Further network communication has been stopped by the ABCC due
         ** to an application error (invalid network configuration parameter,
         ** timeout etc.) or because the ABCC is waiting for a reset to be
         ** executed. Details can be read from the Anybus object.
         **
         ** Suggested actions: correct any errors and reset the ABCC.
         */
   
         break;
   
      default:
   
         /*
         ** Unknown ABCC state.
         */
   
         break;
   
      } /* End switch( ABCC state ) */

      eAbccPrevState = ABCC_eAbccState;
      
   } /* End if( !abcc_fWdTimeout ) */

} /* End of ABCC_CbfNewReadPd() */


/*------------------------------------------------------------------------------
** ABCC_CbfRequestPdSize()
**------------------------------------------------------------------------------
*/

void ABCC_CbfRequestPdSize( UINT16* piReadLength, UINT16* piWriteLength )
{
   /*
   ** This function will request the process data lengths from the application.
   */

   APPD_CbfRequestPdSize( piReadLength, piWriteLength );

} /* End of ABCC_CbfRequestPdSize() */


/*------------------------------------------------------------------------------
** ABCC_CbfTelegramReceived()
**------------------------------------------------------------------------------
*/

void ABCC_CbfTelegramReceived( void )
{
   /*
   ** A telegram (serial or parallel) is received from the ABCC module.
   ** This function is provided as status information. No action from the
   ** application is required.
   ** This callback function is called if "ABCC_TX_RX_CALLBACK_ENABLE" is defined.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfTelegramReceived() */


/*------------------------------------------------------------------------------
** ABCC_CbfTelegramTransmitted()
**------------------------------------------------------------------------------
*/

void ABCC_CbfTelegramTransmitted( BOOL8 fReTransmission )
{
   /*
   ** A telegram (serial or parallel) is transmitted to the ABCC module.
   ** This function is provided as status information. No action from the
   ** application is required.
   ** This callback function is called if "ABCC_TX_RX_CALLBACK_ENABLE" is defined.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfTelegramTransmitted() */


/*------------------------------------------------------------------------------
** ABCC_CbfWdTimeout()
**------------------------------------------------------------------------------
*/

void ABCC_CbfWdTimeout( void )
{
   /*
   ** We have (temporary?) lost the connection with the ABCC module. It is
   ** possible that the communication can be restored, but not likely.
   */

   /*
   ** PORTING ALERT!
   */

   abcc_fWdTimeout = TRUE;

   /*
   ** Make sure the line is cleared.
   */

   printf( "                                                                      \r" );
   printf( "Connection to ABCC lost!\n" );

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfWdTimeout() */


/*------------------------------------------------------------------------------
** ABCC_CbfWdTimeoutRecovered()
**------------------------------------------------------------------------------
*/

void ABCC_CbfWdTimeoutRecovered( void )
{
   /*
   ** This function signals that we have previously had a ABCC watchdog timeout,
   ** but now the communication works again.
   */

   abcc_fWdTimeout = FALSE;

   /*
   ** Make sure the line is cleared.
   */

   printf( "                                                                      \r" );
   printf( "Connection to ABCC Recovered.\n" );

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfWdTimeoutRecovered() */


/*******************************************************************************
**
** End of abcc_cbf.c
**
********************************************************************************
*/
