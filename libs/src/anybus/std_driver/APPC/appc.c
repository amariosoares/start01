/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** appc.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Source file for the APPC (Application Client)
**
** Handles mainly initial configuration of the ABCC module
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
#include "abcc.h"
#include "util.h"
#include "appc.h"

#include "stdio.h"

/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

UINT8 appc_bSwitch1;
UINT8 appc_bSwitch2;
BOOL8 appc_fSwitch1Set = FALSE;
BOOL8 appc_fSwitch2Set = FALSE;
APPC_StateType appc_eState = APPC_NW_TYPE;

/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** APPC_Init()
**------------------------------------------------------------------------------
*/

void APPC_Init( void )
{
   appc_eState = APPC_NW_TYPE;
}

/*------------------------------------------------------------------------------
** APPC_ProcessRespMsg()
**------------------------------------------------------------------------------
*/

void APPC_ProcessRespMsg( ABP_MsgType* psNewMessage )
{
   /* 
   ** A response to a command sent by APPC has arrived.
   ** We can identify the response by its source ID.
   */

   switch ( psNewMessage->sHeader.bSourceId )
   {
   case 0:
   
      if( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_E_BIT )
      {
         /* 
         ** For some reason we got an error response to the 
         ** 'get network type string' command.
         ** This should not happen.
         ** Since it is a response, we just drop it.
         */
      }
      else
      {
         /* 
         ** We got a valid response to the 'Get network type string' command.
         **
         ** We need to add null termination to the string before we print it.
         */
         
         psNewMessage->abData[ appc_Min( ABP_MAX_MSG_DATA_BYTES-1, psNewMessage->sHeader.bDataSize ) ] = 0;

         /*
         ** Make sure the line is cleared, then print it.
         */

         printf( "                                                                      \r" );
         printf( "Network type:     %s\n", psNewMessage->abData );

      }
      break;

   case 1:
   
      if( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_E_BIT )
      {
         /* 
         ** We got an error response to the 'Get firmware version' command
         ** This should not happen.
         ** Since it is a response, we just drop it.
         */
      }
      else
      {
         /*
         ** We got a valid response to the 'Get firmware version' command.
         **
         ** Make sure the line is cleared, then print the version number.
         */
         
         printf( "                                                                      \r" );
         printf( "Firmware version: %d.%.2d build %d\n", 
                 psNewMessage->abData[ 0 ] ,
                 psNewMessage->abData[ 1 ] ,
                 psNewMessage->abData[ 2 ] );
      }
      break;

   case 2:
   
      if( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_E_BIT )
      {
         /* 
         ** We got an error response to the 'Get serial number' command
         ** This should not happen.
         ** Since it is a response, we just drop it.
         */
      }
      else
      {
         /* 
         ** We got a valid response to the 'Get serial number' command.
         **
         ** Make sure the line is cleared, then print the serial number.
         */
         
         printf( "                                                                      \r" );
         printf( "Serial number:    %X:%X:%X:%X\n\n",
                 psNewMessage->abData[ 3 ], 
                 psNewMessage->abData[ 2 ], 
                 psNewMessage->abData[ 1 ], 
                 psNewMessage->abData[ 0 ] );
      }
      break;

   case 3:
      
      /*
      ** Response to the 'Set DIP switch1' command.
      ** This response is normally a ack to the command, but it could be a
      ** error response in case the current network type have no use for the
      ** DIP switch1 value.
      ** Either way, the response is not of interest to us in this application.
      */

      break;

   case 4:

      /*
      ** Response to the 'Set DIP switch2' command.
      ** This response is normally a ack to the command, but it could be a
      ** error response in case the current network type have no use for the 
      ** DIP switch2 value (Profibus DPV1 for example).
      ** Either way, the response is not of interest to us in this application.
      */

      break;

   case 5:

      if( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_E_BIT )
      {
         /*
         ** Error response to the 'Setup Complete' command.
         ** Something is wrong with the entered configuration.
         */

#ifdef ABCC_DRIVER_DEBUG
         ABCC_CbfDebugInfo( "APPC: Setup complete failed." );
#endif
      }
      else
      {
         /*
         ** Ack to the 'Setup complete' command.
         */
      }
      break;

   default:
   
      /*
      ** A response message with a invalid source ID.
      ** Should never happen unless there is something wrong with the hardware
      ** or software.
      ** Only way to handle invalid responses is to simply ignore them.
      */
      
      break;
   }

   /*
   ** After we have recieved a response to the previous command, we run the 
   ** state machine to send the next command.
   */

   APPC_RunStateMachine();
   
} /* end of APPC_ProcessRespMsg( ABP_MsgType* psNewMessage ) */


/*------------------------------------------------------------------------------
** APPC_RunStateMachine()
**------------------------------------------------------------------------------
*/

void APPC_RunStateMachine( void )
{
   /*
   ** Run the APPC state machine
   */
   
   switch ( appc_eState )
   {
   case APPC_NW_TYPE:
      
      /*
      ** When sending requests to the ABCC, each message need to have a 
      ** unique source ID so that we can identify the response message.
      ** For clients who need to send lots of requests to the ABCC, some kind of 
      ** dynamic source ID allocation is rcommended.
      **
      ** However, this simple client will only send a couple of requests at 
      ** startup, so we simply assign fixed source IDs to each request.
      */
      
      /*
      ** Try to send a request (witch source ID 0) to the ABCC
      */
      
      if( appc_GetAttribute( ABP_OBJ_NUM_NW, 1, ABP_NW_IA_NW_TYPE_STR, 0 ) == ABCC_STATUS_OK )
      {
         /* 
         ** Get 'network type string' request sent,
         ** Next request to send is 'Get firmware version'.
         */
         
         appc_eState = APPC_FIRMWARE;
      }
      break;

   case APPC_FIRMWARE:
   
      if( appc_GetAttribute( ABP_OBJ_NUM_ANB, 1, ABP_ANB_IA_FW_VERSION, 1 ) == ABCC_STATUS_OK )
      {
         /* 
         ** Get 'firmware version' request sent,
         ** Next request to send is 'Get serial number'.
         */
         
         appc_eState = APPC_SERIAL_NO;
      }
      break;

   case APPC_SERIAL_NO:
   
      if( appc_GetAttribute( ABP_OBJ_NUM_ANB, 1, ABP_ANB_IA_SERIAL_NUM, 2 ) == ABCC_STATUS_OK )
      {
         /* 
         ** Get 'serial number' request sent,
         ** Next we send the configuration DIP switch values, if available.
         ** Otherwise we need to send 'setup complete'
         */

         if( appc_fSwitch1Set )
         {
            appc_eState = APPC_DIP_SW1;
         }
         else
         {
            appc_eState = APPC_SETUP_COMPLETE;
         }
      }
      break;

   case APPC_DIP_SW1:

      /*
      ** The configuration DIP switches are available,
      ** we start by sending the value of the first switch.
      */

      if( appc_SetByteAttribute( ABP_OBJ_NUM_NC, 1, ABP_NC_VAR_IA_VALUE, appc_bSwitch1, 3 ) == ABCC_STATUS_OK )
      {
         /*
         ** DIP SW1 settings sent
         */
         
         if( appc_fSwitch2Set )
         {
            appc_eState = APPC_DIP_SW2;
         }
         else
         {
            appc_eState = APPC_SETUP_COMPLETE;
         }
      }
      break;

   case APPC_DIP_SW2:
   
      /* 
      ** Time to send DIP SW2 settings
      */
      
      if( appc_SetByteAttribute( ABP_OBJ_NUM_NC, 2, ABP_NC_VAR_IA_VALUE, appc_bSwitch2, 4 ) == ABCC_STATUS_OK )
      {
         /*
         ** DIP SW2 settings sent,
         ** Time to send 'setup complete'
         */
         
         appc_eState = APPC_SETUP_COMPLETE;
      }
      break;

   case APPC_SETUP_COMPLETE:

      /*
      ** We finish the configuration by sending 'setup complete'.
      */

      if( appc_SetByteAttribute( ABP_OBJ_NUM_ANB, 1, ABP_ANB_IA_SETUP_COMPLETE, TRUE, 5 ) == ABCC_STATUS_OK )
      {
         /*
         ** Setup complete command sent, we are done here
         */

         appc_eState = APPC_DONE;
      }
      break;

   case APPC_DONE:

      /*
      ** Nothing more to do
      */

      break;
   }

} /* end of APPC_RunStateMachine( ) */


/*------------------------------------------------------------------------------
** APPC_SetSwitch1()
**------------------------------------------------------------------------------
*/

void APPC_SetSwitch1( UINT8 bSw1 )
{
   appc_bSwitch1 = bSw1;
   appc_fSwitch1Set = TRUE;

} /* End of APPC_SetSwitchSettings() */

/*------------------------------------------------------------------------------
** APPC_SetSwitch2()
**------------------------------------------------------------------------------
*/

void APPC_SetSwitch2( UINT8 bSw2 )
{
   appc_bSwitch2 = bSw2;
   appc_fSwitch2Set = TRUE;

} /* End of APPC_SetSwitchSettings() */


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** appc_GetAttribute()
**------------------------------------------------------------------------------
*/

ABCC_StatusType appc_GetAttribute( UINT8 bObject, 
                                   UINT16 iInstance, 
                                   UINT8 bAttribute, 
                                   UINT8 bSourceId )
{
   ABP_MsgType sMsg;
   
   sMsg.sHeader.bSourceId = bSourceId;    /* SourceId                    */
   sMsg.sHeader.bDestObj = bObject;       /* Object                      */
   sMsg.sHeader.iInstance = iInstance;    /* Instance                    */
   sMsg.sHeader.bCmd = ABP_MSG_HEADER_C_BIT | ABP_CMD_GET_ATTR;  /* Command, Get attribute      */
   sMsg.sHeader.bDataSize = 0;            /* Data size                   */
   sMsg.sHeader.bCmdExt0 = bAttribute;    /* CmdExt0 (Attribute)         */
   sMsg.sHeader.bCmdExt1 = 0;             /* CmdExt1 (reserved)          */

   return ( ABCC_SendMessage( &sMsg ) );
   
} /* End of appc_GetAttribute() */


/*------------------------------------------------------------------------------
** appc_SetByteAttribute()
**------------------------------------------------------------------------------
*/

ABCC_StatusType appc_SetByteAttribute( UINT8 bObject, 
                                       UINT16 iInstance, 
                                       UINT8 bAttribute, 
                                       UINT8 bVal, 
                                       UINT8 bSourceId )
{
   ABP_MsgType sMsg;
   
   sMsg.sHeader.bSourceId = bSourceId;    /* SourceId                    */
   sMsg.sHeader.bDestObj = bObject;       /* Object                      */
   sMsg.sHeader.iInstance = iInstance;    /* Instance                    */
   sMsg.sHeader.bCmd = ABP_MSG_HEADER_C_BIT | ABP_CMD_SET_ATTR;  /* Command, Set attribute      */
   sMsg.sHeader.bDataSize = 1;            /* Data size                   */
   sMsg.sHeader.bCmdExt0 = bAttribute;    /* CmdExt0 (Attribute)         */
   sMsg.sHeader.bCmdExt1 = 0;             /* CmdExt1 (reserved)          */
   sMsg.abData[0] = bVal;                 /* Data                        */

   return ( ABCC_SendMessage( &sMsg ) );
   
} /* end of appc_SetByteAttribute() */


/*******************************************************************************
**
** End of APPC.C
**
********************************************************************************
*/