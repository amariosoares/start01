/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** app.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public header file for the APP object.
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    APP_ProcessCmdMsg()        - Processes commands sent to the APP object
**
** Private Services:
**
**    app_InstanceCommand()      - Processes commands to the instance
**    app_ObjectCommand()        - Processes commands to the object
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

#ifndef APP_H
#define APP_H


/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** app_ObjectType
**
** Structure describing an Application Object.
**
**------------------------------------------------------------------------------
*/

typedef struct app_ObjectType 
{
  UINT8* acName;
  UINT8 bRevision;
  UINT16 iNumberOfInstances;
  UINT16 iHighestInstanceNo;
} 
app_ObjectType;


/*------------------------------------------------------------------------------
**
** app_InstanceType
**
** Structure describing an Application Data Instance.
**
**------------------------------------------------------------------------------
*/

typedef struct app_InstanceType 
{
  BOOL8 fConfigured;
  UINT8 bLanguage;
  UINT8 bSupportedLanguages[ 1 ];
  UINT8 bNumberOfSupportedLanguages;
} 
app_InstanceType;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** app_aacLanguages
**
**------------------------------------------------------------------------------
**
** Array of language strings. One string for each supported language.
**
**------------------------------------------------------------------------------
*/

extern UINT8* app_aacLanguages[];


/*------------------------------------------------------------------------------
**
** app_sObject
**
**------------------------------------------------------------------------------
**
** Structure representing the APP object (instance 0).
**
**------------------------------------------------------------------------------
*/

extern const app_ObjectType app_sObject;


/*------------------------------------------------------------------------------
**
** app_sInstance
**
**------------------------------------------------------------------------------
**
** Structure representing the APP instance.
**
**------------------------------------------------------------------------------
*/

extern app_InstanceType app_sInstance;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** APP_ProcessCmdMsg( )
**
** Processes commands sent to the APP object
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    APP_ProcessCmdMsg( &sMessage );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APP_ProcessCmdMsg( ABP_MsgType* psNewMessage );


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** void app_InstanceCommand( )
**
** Processes commands to Application Instance
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    app_InstanceCommand( &sMessage );
**
**------------------------------------------------------------------------------
*/

void app_InstanceCommand( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** app_ObjectCommand( )
**
** Processes commands to the Application Object (Instance 0)
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    app_ObjectCommand( &sMessage );
**
**------------------------------------------------------------------------------
*/

void app_ObjectCommand( ABP_MsgType* psNewMessage );


#endif  /* inclusion lock */


/*******************************************************************************
**
** End of app.h
**
********************************************************************************
*/