/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** app.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Source file for the Application Object and instance
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
#include "app.h"
#include "string.h"

/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

UINT8* app_aacLanguages[ 1 ] = { "English" };

const app_ObjectType app_sObject = 
{
   "Application",    /* Name                          */
   0x01,             /* Revision                      */
   1,                /* Number of instances           */
   1                 /* Highest instance number       */
};

app_InstanceType app_sInstance = 
{
   0,                /* Configured                    */
   ABP_LANG_ENG,     /* Language                      */
   { ABP_LANG_ENG }, /* Supported languages           */
   1                 /* Number of supported languages */
};

/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** APP_ProcessCmdMsg()
**------------------------------------------------------------------------------
*/

void APP_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   if( psNewMessage->sHeader.iInstance == ABP_INST_OBJ )
   {
      /*
      ** Application data object Command
      */
      
      app_ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Application data instance Command
      */
      
      app_InstanceCommand( psNewMessage );
   }

   ABCC_SendMessage(psNewMessage);
   
} /* End of APP_ProcessCmdMsg() */


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** void app_InstanceCommand()
**------------------------------------------------------------------------------
*/

void app_InstanceCommand( ABP_MsgType* psNewMessage )
{
   UINT16 i;
   

   if( psNewMessage->sHeader.iInstance != 1 )
   {
      /*
      ** The requested instance does not exist
      ** Respond with a error.
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_INST );
      
      return;
   }

   switch ( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_APP_IA_CONFIGURED:
      
         /* 
         ** The 'configured' attribute is requested.
         ** Copy attribute to a response message.
         */
         
         psNewMessage->abData[ 0 ] = app_sInstance.fConfigured;
         ABP_SetMsgResponse( psNewMessage, ABP_BOOL_SIZEOF );
         break;

      case ABP_APP_IA_SUP_LANG:
      
         /* 
         ** The 'supported languages' attribute is requested.
         ** Copy attribute to a response message.
         */
         
         SYS_CopyMemBytes( psNewMessage->abData,
                           app_sInstance.bSupportedLanguages,
                           app_sInstance.bNumberOfSupportedLanguages );
         ABP_SetMsgResponse( psNewMessage, app_sInstance.bNumberOfSupportedLanguages );
         break;

      default:
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;

   case ABP_CMD_GET_ENUM_STR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_APP_IA_SUP_LANG:
      
         if( psNewMessage->sHeader.bCmdExt1 <= ABP_LANG_ENG )
         {
            /*
            ** Copy ENUM STR to message
            */
            
            i = (UINT16)strlen( app_aacLanguages[ psNewMessage->sHeader.bCmdExt1 ] );
            SYS_CopyMemBytes( psNewMessage->abData,
                              app_aacLanguages[ psNewMessage->sHeader.bCmdExt1 ],
                              i );
            ABP_SetMsgResponse( psNewMessage, (UINT8)i );
         }
         else
         {
            /*
            ** ENUM value out of range
            */
            
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_1 );
         }
         break;

      default:
      
         /* 
         ** Attribute does not exist, or attribute is not ENUM
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
      
   }/* End switch( command number ) */
} /* End of app_InstanceCommand() */


/*------------------------------------------------------------------------------
** app_ObjectCommand()
**------------------------------------------------------------------------------
*/

void app_ObjectCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iStrLength;
   

   switch ( psNewMessage->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_OA_NAME:
      
         /* 
         ** The 'name' attribute is requested.
         ** Copy attribute to a response message.
         */
         
         iStrLength = (UINT16)strlen( app_sObject.acName );
         SYS_CopyMemBytes( psNewMessage->abData,
                           app_sObject.acName,
                           iStrLength );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:
      
         /* 
         ** The 'revision' attribute is requested.
         ** Copy attribute to a response message.
         */
         
         psNewMessage->abData[ 0 ] = app_sObject.bRevision;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );                  
         break;

      case ABP_OA_NUM_INST:
      
         /* 
         ** The 'Number of Instances' attribute is requested.
         ** Copy attribute to a response message.
         */
         
         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( app_sObject.iNumberOfInstances );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      case ABP_OA_HIGHEST_INST:
      
         /* 
         ** The 'Highest Instance Number' attribute is requested.
         ** Copy attribute to a response message.
         */
         
         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( app_sObject.iHighestInstanceNo );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      default:
      
         /*
         ** Error: Unsupported attribute
         */
         
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;
   
   case ABP_APP_CMD_RESET_REQUEST:
   
      /* 
      ** Reset request.
      ** This feature is not implemented in this example.
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   case ABP_CMD_RESET:
   
      /* 
      ** Reset command.
      ** This feature is not implemented in this example.
      */
      
      break;

   default:
   
      /*
      ** Some unsupported command.
      ** Respond with a error.
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;
      
   }/* End switch( command number ) */
} /* End of app_ObjectCommand() */


/*******************************************************************************
**
** End of app.c
**
********************************************************************************
*/