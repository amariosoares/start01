/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** dpv1.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public header file for the DPV1 (Profibus) object.
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    DPV1_ProcessCmdMsg()      - Processes commands sent to the DPV1 object
**
** Private Services:
**
**    dpv1_InstanceCommand()    - Processes commands to DPV1 Instances
**    dpv1_ObjectCommand()      - Processes commands to the DPV1 Object
**                                (Instance 0)
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

#ifndef DPV1_H
#define DPV1_H

/*******************************************************************************
**
** Constants
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** DPV1_SIZE_OF_ID_REL_DIAG
**
**------------------------------------------------------------------------------
**
** Size of identifier related diagnostics (attribute 5, instance 1, DPV1 object)
**
**------------------------------------------------------------------------------
*/

#define DPV1_SIZE_OF_ID_REL_DIAG 0


/*------------------------------------------------------------------------------
**
** DPV1_PRM_STANDARD_BYTES
**
**------------------------------------------------------------------------------
**
** Size of Profibus DPV0 standard Prm data, according to IEC-61158
**
**------------------------------------------------------------------------------
*/

#define DPV1_PRM_STANDARD_BYTES  7


/*------------------------------------------------------------------------------
**
** DPV1_PRM_DPV1_STATUS_BYTES
**
**------------------------------------------------------------------------------
**
** Size of Profibus DPV1 Status bytes in User Prm data, according to IEC-61158
**
**------------------------------------------------------------------------------
*/

#define DPV1_PRM_DPV1_STATUS_BYTES  3


/*------------------------------------------------------------------------------
**
** DPV1_PRM_APPL_SPECIFIC_BYTES
**
**------------------------------------------------------------------------------
**
** Size of application specific user parameter data
**
**------------------------------------------------------------------------------
*/

#define DPV1_PRM_APPL_SPECIFIC_BYTES   0


/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** dpv1_ObjectType
**
** Structure describing an Profibus Object.
**
**------------------------------------------------------------------------------
*/

typedef struct dpv1_ObjectType
{
   const UINT8*  pcName;
   UINT8  bRevision;
   UINT16 iNumberOfInstances;
   UINT16 iHighestInstanceNo;
}
dpv1_ObjectType;


/*------------------------------------------------------------------------------
**
** dpv1_InstanceType
**
** Structure describing an Profibus Instance.
**
**------------------------------------------------------------------------------
*/

typedef struct dpv1_InstanceType
{
   UINT16 iIdentNumber;
   UINT8  abPrmData[ DPV1_PRM_STANDARD_BYTES +
                     DPV1_PRM_DPV1_STATUS_BYTES +
                     DPV1_PRM_APPL_SPECIFIC_BYTES ];
   UINT8  abExpectedCfgData[ 8 ];      /* Size depends on application   */
   BOOL8  fSsaEnabled;
   UINT8  bSizeOfIdRelDiag;
   UINT8  bBufferMode;
}
dpv1_InstanceType;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** dpv1_sObject
**
**------------------------------------------------------------------------------
**
** Structure representing the DPV1 object (instance 0).
**
**------------------------------------------------------------------------------
*/

extern const dpv1_ObjectType dpv1_sObject;


/*------------------------------------------------------------------------------
**
** dpv1_sInstance
**
**------------------------------------------------------------------------------
**
** Structure representing the DPV1 instance.
**
**------------------------------------------------------------------------------
*/

extern dpv1_InstanceType dpv1_sInstance;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** void DPV1_ProcessCmdMsg( )
**
** Processes commands sent to the DPV1 object
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
**    DPV1_ProcessCmdMsg( &sMessage );
**
**------------------------------------------------------------------------------
*/

void DPV1_ProcessCmdMsg( ABP_MsgType* psNewMessage );


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** void dpv1_InstanceCommand( )
**
** Processes commands to DPV1 Instances
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
**    dpv1_InstanceCommand( &sMessage );
**
**------------------------------------------------------------------------------
*/

void dpv1_InstanceCommand( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** void dpv1_ObjectCommand( )
**
** Processes commands to the DPV1 Object (Instance 0)
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
**    dpv1_ObjectCommand( &sMessage );
**
**------------------------------------------------------------------------------
*/

void dpv1_ObjectCommand( ABP_MsgType* psNewMessage );


#endif  /* inclusion lock */


/*******************************************************************************
**
** End of dpv1.h
**
********************************************************************************
*/