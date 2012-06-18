/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** dpv1.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Source file for the Profibus DPV1 ABCC Object
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
#include "dpv1.h"
#include "abp_dpv1.h"

#include "string.h"

/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

const dpv1_ObjectType dpv1_sObject =
{
   "Profibus",                /* Name                                         */
   1,                         /* Revision                                     */
   1,                         /* Number of instances                          */
   1                          /* Highest instance number                      */
};

dpv1_InstanceType dpv1_sInstance =
{
   0x1811,                    /* PNO ID number                                */
   { 0 },                     /* Parameter data                               */
   { 0 },                     /* Expected Cfg data                            */
   TRUE,                      /* SSA Enabled                                  */
   DPV1_SIZE_OF_ID_REL_DIAG,  /* Size of identifier related diag.             */
   5,                         /* Buffer mode                                  */
};


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** DPV1_ProcessCommand()
**------------------------------------------------------------------------------
*/

void DPV1_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the Profibus Object and it's Instance
   */

   if( psNewMessage->sHeader.iInstance == ABP_INST_OBJ )
   {
      /*
      ** Profibus object Command
      */

      dpv1_ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Profibus instance Command
      */

      dpv1_InstanceCommand( psNewMessage );
   }

   ABCC_SendMessage( psNewMessage );
}


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** void dpv1_InstanceCommand()
**------------------------------------------------------------------------------
*/

void dpv1_InstanceCommand( ABP_MsgType* psNewMessage )
{
   /*
   ** This function processes commands to the Profibus Instance
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
      case ABP_DPV1_IA_IDENT_NUMBER:

         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( dpv1_sInstance.iIdentNumber );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      case ABP_DPV1_IA_PRM_DATA:

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_GETABLE );
         break;

      case ABP_DPV1_IA_EXPECTED_CFG_DATA:

         /*
         ** Not implemented in this example
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;

      case ABP_DPV1_IA_SSA_ENABLED:

         psNewMessage->abData[ 0 ] = dpv1_sInstance.fSsaEnabled;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      case ABP_DPV1_IA_SIZEOF_ID_REL_DIAG:

         psNewMessage->abData[ 0 ] = dpv1_sInstance.bSizeOfIdRelDiag;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      case ABP_DPV1_IA_BUFFER_MODE:

         psNewMessage->abData[ 0 ] = dpv1_sInstance.bBufferMode;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      default:

         /*
         ** Unsupported attribute
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;

   case ABP_CMD_SET_ATTR:

      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_DPV1_IA_PRM_DATA:

         /*
         ** Verify data size
         */

         if( psNewMessage->sHeader.bDataSize > ( DPV1_PRM_STANDARD_BYTES +
               DPV1_PRM_DPV1_STATUS_BYTES + DPV1_PRM_APPL_SPECIFIC_BYTES ) )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_TOO_MUCH_DATA );
         }
         else if( psNewMessage->sHeader.bDataSize < DPV1_PRM_STANDARD_BYTES )
         {
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_NOT_ENOUGH_DATA );
         }
         else
         {
            /*
            ** If the application has defined user specific parameter data,
            ** this is the place to check the data (from byte 10 and forth)
            ** In this example we only update the attribute
            */

            SYS_CopyMemBytes( dpv1_sInstance.abPrmData,
                              psNewMessage->abData,
                              10 );
            ABP_SetMsgResponse( psNewMessage, 0 );
         }
         break;

      case ABP_DPV1_IA_IDENT_NUMBER:
      case ABP_DPV1_IA_EXPECTED_CFG_DATA:
      case ABP_DPV1_IA_SSA_ENABLED:
      case ABP_DPV1_IA_SIZEOF_ID_REL_DIAG:
      case ABP_DPV1_IA_BUFFER_MODE:

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_SETABLE );
         break;

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
}


/*------------------------------------------------------------------------------
** dpv1_ObjectCommand()
**------------------------------------------------------------------------------
*/

void dpv1_ObjectCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iStrLength;


   /*
   ** This function processes commands to the Profibus Object (Instance 0)
   */

   switch ( ( psNewMessage->sHeader.bCmd ) & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:

      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_OA_NAME:

         iStrLength = (UINT16)strlen( dpv1_sObject.pcName );
         SYS_CopyMemBytes( psNewMessage->abData,
                           dpv1_sObject.pcName,
                           iStrLength );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:

         psNewMessage->abData[ 0 ] = dpv1_sObject.bRevision;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      case ABP_OA_NUM_INST:

         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( dpv1_sObject.iNumberOfInstances );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      case ABP_OA_HIGHEST_INST:

         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( dpv1_sObject.iHighestInstanceNo );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      default:

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;

   default:

      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;
   }
}


/****************************************************************************
**
** End of dpv1.c
**
*****************************************************************************
*/