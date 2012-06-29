/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** dev.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public header file for the DEV (DeviceNet) object.
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    DEV_ProcessCmdMsg()    - Processes commands sent to the DEV object
**
** Private Services:
**
**    dev_InstanceCommand()  - Processes commands to DEV Instances
**    dev_ObjectCommand()    - Processes commands to the DEV Object (Instance 0)
**
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

#ifndef DEV_H
#define DEV_H

/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** dev_ObjectType
**
** Structure describing an DeviceNet Object.
**
**------------------------------------------------------------------------------
*/

typedef struct dev_ObjectType 
{
  const UINT8* pcName;
  UINT8 bRevision;
  UINT16 iNumberOfInstances;
  UINT16 iHighestInstanceNo;
} 
dev_ObjectType;


/*------------------------------------------------------------------------------
**
** dev_InstanceType
**
** Structure describing an DeviceNet Instance.
**
**------------------------------------------------------------------------------
*/

typedef struct dev_InstanceType 
{
  UINT16 iVendorId;
  UINT16 iDeviceType;
  UINT16 iProductCode;
  UINT8  bMajorRevision;
  UINT8  bMinorRevision;
  UINT32 lSerialNumber;
  const UINT8* pcProductName;
  UINT16 iProducingInstanceNumber;
  UINT16 iConsumingInstanceNumber;
  BOOL8   fEnableAddressFromNet;
  BOOL8   fEnableBaudRateFromNet;
  BOOL8   fEnableCipRequestForwarding;
  BOOL8   fEnableParameterObject;
} 
dev_InstanceType;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** dev_sObject
**
**------------------------------------------------------------------------------
**
** Structure representing the DEV object (instance 0).
**
**------------------------------------------------------------------------------
*/

extern const dev_ObjectType dev_sObject;


/*------------------------------------------------------------------------------
**
** dev_sInstance
**
**------------------------------------------------------------------------------
**
** Structure representing the DEV instance.
**
**------------------------------------------------------------------------------
*/

extern const dev_InstanceType dev_sInstance;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** void DEV_ProcessCmdMsg( )
**
** Processes commands sent to the DEV object
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage   - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    DEV_ProcessCmdMsg( &sMessage );
**
**------------------------------------------------------------------------------
*/

void DEV_ProcessCmdMsg( ABP_MsgType* psNewMessage );


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** void dev_InstanceCommand( )
**
** Processes commands to DEV Instances
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
**    dev_InstanceCommand( &sMessage );
**
**------------------------------------------------------------------------------
*/

void dev_InstanceCommand( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** void dev_ObjectCommand( )
**
** Processes commands to the DEV Object (Instance 0)
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
**    dev_ObjectCommand( &sMessage );
**
**------------------------------------------------------------------------------
*/

void dev_ObjectCommand( ABP_MsgType* psNewMessage );


#endif  /* inclusion lock */


/*******************************************************************************
**
** End of dev.h
**
********************************************************************************
*/