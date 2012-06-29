/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** dev.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Source file for the DeviceNet ABCC Object
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
#include "abcc.h"
#include "util.h"
#include "dev.h"
#include "abp_dev.h"
#include "string.h"


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

const dev_ObjectType dev_sObject = 
{
   "DeviceNet",                  /* Name                                      */
   1,                            /* Revision                                  */
   1,                            /* Number of instances                       */
   1                             /* Highest instance number                   */
};

const dev_InstanceType dev_sInstance = 
{
   0x005A,                       /* Vendor ID                                 */
   0x0000,                       /* Device type                               */
   0x0062,                       /* Product code                              */
   1,                            /* Major revision                            */
   1,                            /* Minor revision                            */
   0,                            /* Serial number                             */
   "Anybus CompactCom example",  /* Product name                              */
   0x0064,                       /* Producing instance number                 */
   0x0096,                       /* Consuming instance number                 */
   TRUE,                         /* Enable address from net                   */
   TRUE,                         /* Enable baud rate from net                 */
   FALSE,                        /* Enable CIP request forwarding             */
   TRUE                          /* Enable parameter object                   */
};


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** DEV_ProcessCommand()
**------------------------------------------------------------------------------
*/

void DEV_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /* 
   ** This function processes commands to the DeviceNet Object and it's Instance 
   */

   if( psNewMessage->sHeader.iInstance == ABP_INST_OBJ )
   {
      /* 
      ** Devicenet object Command 
      */
      
      dev_ObjectCommand( psNewMessage );
   }
   else
   {
      /* 
      ** Devicenet instance Command 
      */
      
      dev_InstanceCommand( psNewMessage );
   }
   ABCC_SendMessage( psNewMessage );
   
} /* End of DEV_ProcessCommand() */


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** dev_InstanceCommand()
**------------------------------------------------------------------------------
*/

void dev_InstanceCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iStrLength;
   

   /* 
   ** This function processes commands to the DeviceNet Instance 
   */

   if( psNewMessage->sHeader.iInstance != 1 )
   {
      /* 
      ** Instance does not exist 
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_INST );
      
      return;
   }

   switch ( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_DEV_IA_PRODUCT_NAME:
      
         /* 
         ** Copy attribute to message 
         */
         
         iStrLength = (UINT16)strlen( dev_sInstance.pcProductName );
         SYS_CopyMemBytes( psNewMessage->abData,
                           dev_sInstance.pcProductName,
                           iStrLength );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_DEV_IA_REVISION:
      
         /* 
         ** Copy attribute to message 
         */
         
         psNewMessage->abData[ 0 ] = dev_sInstance.bMajorRevision;
         psNewMessage->abData[ 1 ] = dev_sInstance.bMinorRevision;
         ABP_SetMsgResponse( psNewMessage, ABP_DEV_IA_REVISION_DS );
         break;

      /* 
      ** In this example, the default values of these
      ** attributes are desired, so we can simply respond
      ** with a error when they are requested 
      */

      case ABP_DEV_IA_VENDOR_ID:
      case ABP_DEV_IA_DEVICE_TYPE:
      case ABP_DEV_IA_PRODUCT_CODE:
      case ABP_DEV_IA_SERIAL_NUMBER:
      case ABP_DEV_IA_PROD_INSTANCE:
      case ABP_DEV_IA_CONS_INSTANCE:
      case ABP_DEV_IA_ADDRESS_FROM_NET:
      case ABP_DEV_IA_BAUD_RATE_FROM_NET:
      case ABP_DEV_IA_ENABLE_APP_CIP_OBJECTS:
      case ABP_DEV_IA_ENABLE_PARAM_OBJECT:
      default:
      
         /* 
         ** Unsupported attribute 
         */
         
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;

   default:
   
      /*
      ** Unsupported command
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;
   }
} /* End of dev_InstanceCommand() */


/*------------------------------------------------------------------------------
** dev_ObjectCommand()
**------------------------------------------------------------------------------
*/

void dev_ObjectCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iStrLength;
   
   
   /* 
   ** This function processes commands to the DeviceNet Object (Instance 0) 
   */

   switch ( ( psNewMessage->sHeader.bCmd ) & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_OA_NAME:
      
         /* 
         ** Copy attribute to message 
         */
         
         iStrLength = (UINT16)strlen( dev_sObject.pcName );
         SYS_CopyMemBytes( psNewMessage->abData,
                           dev_sObject.pcName,
                           iStrLength );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:
      
         /* 
         ** Copy attribute to message 
         */
         
         psNewMessage->abData[ 0 ] = dev_sObject.bRevision;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );                  
         break;

      case ABP_OA_NUM_INST:
      
         /* 
         ** Copy attribute to message 
         */
         
         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( dev_sObject.iNumberOfInstances );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      case ABP_OA_HIGHEST_INST:
      
         /* 
         ** Copy attribute to message 
         */
         
         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( dev_sObject.iHighestInstanceNo );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      default:
      
         /* 
         ** Error: Unsupported attribute 
         */
         
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
         
      } /* End of switch( Attribute number ) */
      break;
   
   default:
   
      /* 
      ** Unsupported command 
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;
      
   } /* End of switch( Command number ) */
} /* End of dev_ObjectCommand() */

/*******************************************************************************
**
** End of dev.c
**
********************************************************************************
*/