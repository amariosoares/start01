/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** aplink_priv.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This module contains the private interface for the Application driver Data
** Link services.
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
**
** Services List
** -------------
**
** Private Services:
**
**    aplink_RunCfgStateMachine()      - Runs the configuration state-machine.
**    aplink_HandleQueuedMsgsToAppl()  - Scans the incoming ABCC messages.
**    aplink_AddFragAndPostMsg()       - Send message to ABCC.
**    aplink_GetSizeOfDataType()       - Returns the number of bytes for a DT.
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

#ifndef APLINK_PRIV_H
#define APLINK_PRIV_H


/*******************************************************************************
**
** Data Types
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** aplink_CfgStateType
**
**------------------------------------------------------------------------------
*/

typedef enum aplink_CfgStateType
{
   APLINK_CFG_SM_INIT                     = 0x0000,
   APLINK_CFG_SM_READ_MODULE_TYPE,
   APLINK_CFG_SM_WAIT_MODULE_TYPE_RSP,


   APLINK_CFG_SM_READ_NW_TYPE,
   APLINK_CFG_SM_READ_NW_TYPE_RSP,

   APLINK_CFG_SM_READ_NW_PARAM_SUPPORT,
   APLINK_CFG_SM_READ_NW_PARAM_SUPPORT_RSP,

   APLINK_CFG_SM_READ_NW_DATA_FORMAT,
   APLINK_CFG_SM_WAIT_NW_DATA_FORMAT_RSP,

   APLINK_CFG_SM_SEND_READ_MAPPING,
   APLINK_CFG_SM_WAIT_READ_MAPPING_RSP,
   APLINK_CFG_SM_NEXT_READ,

   APLINK_CFG_SM_SEND_WRITE_MAPPING,
   APLINK_CFG_SM_WAIT_WRITE_MAPPING_RSP,
   APLINK_CFG_SM_NEXT_WRITE,


   /*
   ** Force the compiler to use a 16-bit variable as enumeration.
   */

   APLINK_CFG_SM_SET_ENUM_ANSI_SIZE       = 0x7FFF

}
aplink_CfgStateType;


/*------------------------------------------------------------------------------
**
** aplink_DataType
**
**------------------------------------------------------------------------------
*/

typedef struct aplink_DataType
{
   /*------------------------------------------------------------------------------
   **
   ** asRspToAbcc[]
   **
   **------------------------------------------------------------------------------
   **
   ** Array of structures containing the responses to the applications' commands.
   **
   **------------------------------------------------------------------------------
   */

   UTIL_BufMsgType*     pasRspToAbcc[ ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS ];


   /*------------------------------------------------------------------------------
   **
   ** asCmdToAbcc[]
   **
   **------------------------------------------------------------------------------
   **
   ** Array of structures containing commands that shall be sent to the ABCC.
   **
   **------------------------------------------------------------------------------
   */

   UTIL_BufMsgType*     pasCmdToAbcc[ ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS ];


   /*------------------------------------------------------------------------------
   **
   ** asMsgToAppl[]
   **
   **------------------------------------------------------------------------------
   **
   ** Array of structures containing messages (commands and responses) to APPL.
   **
   **------------------------------------------------------------------------------
   */

   UTIL_BufMsgType*     pasMsgToAppl[ ( ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS +
                                        ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS ) ];


   /*------------------------------------------------------------------------------
   **
   ** xRspsToAbccQueue
   **
   **------------------------------------------------------------------------------
   **
   ** Queue for response messages to the ABCC from the application.
   **
   **------------------------------------------------------------------------------
   */

   UTIL_QueueType             xRspsToAbccQueue;


   /*------------------------------------------------------------------------------
   **
   ** xCmdsToAbccQueue
   **
   **------------------------------------------------------------------------------
   **
   ** Queue for commands to the ABCC from the application.
   **
   **------------------------------------------------------------------------------
   */

   UTIL_QueueType             xCmdsToAbccQueue;


   /*------------------------------------------------------------------------------
   **
   ** xMsgsFromAbccQueue
   **
   **------------------------------------------------------------------------------
   **
   ** Queue for messages from the ABCC to the application.
   **
   **------------------------------------------------------------------------------
   */

   UTIL_QueueType             xMsgsFromAbccQueue;


   /*------------------------------------------------------------------------------
   **
   ** iModuleType
   **
   **------------------------------------------------------------------------------
   **
   ** The kind of module (ABCC, other).
   **
   **------------------------------------------------------------------------------
   */

   UINT16                     iModuleType;


   /*------------------------------------------------------------------------------
   **
   ** iNetworkType
   **
   **------------------------------------------------------------------------------
   **
   ** The kind of network (Profibus, DeviceNet, CANopen etc.).
   **
   **------------------------------------------------------------------------------
   */

   UINT16                     iNetworkType;


   /*------------------------------------------------------------------------------
   **
   ** bParamSupport
   **
   **------------------------------------------------------------------------------
   **
   ** If the network have "Parameter support" (that is, if the network has the
   ** ability to issue acyclic requests).
   **
   **------------------------------------------------------------------------------
   */

   UINT8                      bParamSupport;


   /*------------------------------------------------------------------------------
   **
   ** bLastAbccState
   **
   **------------------------------------------------------------------------------
   **
   ** Variable that holds the current ABCC module state.
   **
   **------------------------------------------------------------------------------
   */

   UINT8                      bLastAbccState;


   /*------------------------------------------------------------------------------
   **
   ** eDriverState
   **
   **------------------------------------------------------------------------------
   **
   ** Variable holding the ABCC driver state.
   **
   **------------------------------------------------------------------------------
   */

   APLINK_DriverStateType      eDriverState;


   /*------------------------------------------------------------------------------
   **
   ** abWriteDataBuffer
   **
   **------------------------------------------------------------------------------
   **
   ** Buffer for holding the data that the application wants to write to the
   ** ABCC module.
   **
   **------------------------------------------------------------------------------
   */

   UINT8                      abWriteDataBuffer[ ABCC_WRITE_PD_BUFFER_LENGTH ];


#if defined( ABCC_COMPARE_READ_PD ) || defined( ABCC_ENABLE_READ_PD_FUNCTION )
   /*------------------------------------------------------------------------------
   **
   ** abReadDataBuffer
   **
   **------------------------------------------------------------------------------
   **
   ** Buffer for holding the data that the ABCC module wants to write to the
   ** application.
   **
   **------------------------------------------------------------------------------
   */

   UINT8                      abReadDataBuffer[ ABCC_READ_PD_BUFFER_LENGTH ];
#endif

   /*------------------------------------------------------------------------------
   **
   ** bNbrOfCmdsActiveFromAbcc;
   **
   **------------------------------------------------------------------------------
   **
   ** Variable holding the number of commands (sent from the ABCC module) that
   ** the application currently is processing.
   **
   **------------------------------------------------------------------------------
   */

   UINT8                      bNbrOfCmdsActiveFromAbcc;


   /*------------------------------------------------------------------------------
   **
   ** bNbrOfCmdsActiveToAbcc
   **
   **------------------------------------------------------------------------------
   **
   ** Variable holding the number of commands, sent from the application to the
   ** ABCC, not responded to.
   **
   **------------------------------------------------------------------------------
   */

   UINT8                      bNbrOfCmdsActiveToAbcc;


   /*------------------------------------------------------------------------------
   **
   ** iReadPdLength
   ** iWritePdLength
   **
   **------------------------------------------------------------------------------
   **
   ** Lengths of the process data.
   **
   **------------------------------------------------------------------------------
   */

   UINT16                     iReadPdLength;
   UINT16                     iWritePdLength;


   /*------------------------------------------------------------------------------
   **
   ** eCfgState
   **
   **------------------------------------------------------------------------------
   **
   ** State of the configuration process.
   **
   **------------------------------------------------------------------------------
   */

   aplink_CfgStateType        eCfgState;


   /*------------------------------------------------------------------------------
   **
   ** psReadCfg
   ** psWriteCfg
   **
   **------------------------------------------------------------------------------
   **
   ** Pointers to the application configuration structure.
   **
   **------------------------------------------------------------------------------
   */

   ABCC_PdMapType*              psReadCfg;
   ABCC_PdMapType*              psWriteCfg;


   /*------------------------------------------------------------------------------
   **
   ** fNewWritePdAvailable
   **
   **------------------------------------------------------------------------------
   **
   ** Flag to signal that there is new process data available from the application.
   **
   **------------------------------------------------------------------------------
   */

   BOOL8                      fNewWritePdAvailable;


#ifdef ABCC_ACCESS_MODE_SERIAL
   /*------------------------------------------------------------------------------
   **
   ** iNewWritePdLength
   **
   **------------------------------------------------------------------------------
   **
   ** The new write process data length to use after a successful remap.
   **
   **------------------------------------------------------------------------------
   */

   UINT16                     iNewWritePdLength;
#endif /* ABCC_ACCESS_MODE_SERIAL */


   /*------------------------------------------------------------------------------
   **
   ** fRemappingReadPd
   ** fRemappingWritePd
   **
   **------------------------------------------------------------------------------
   **
   ** Flags, each indicating that a switch of its respective process data map
   ** is being proccessed as the result of an ACK to a REMAP_ADI_READ_AREA or
   ** REMAP_ADI_WRITE_AREA command.
   **
   **------------------------------------------------------------------------------
   */

   BOOL8                      fRemappingReadPd;
   BOOL8                      fRemappingWritePd;

}
aplink_DataType;



/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/


/*------------------------------------------------------------------------------
**
** aplink_s
**
**------------------------------------------------------------------------------
**
** Structure containing data that is private to the APLINK object.
**
**------------------------------------------------------------------------------
*/

extern aplink_DataType         aplink_s;


/*******************************************************************************
**
** Private Services Definitions
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** aplink_RunCfgStateMachine()
**
** Runs the configuration of the process data mapping.
**
**------------------------------------------------------------------------------
**
** Inputs:
**          -
**
** Outputs:
**          -
**
** Usage:
**    aplink_RunCfgStateMachine();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void aplink_RunCfgStateMachine( void );


/*------------------------------------------------------------------------------
**
** aplink_HandleQueuedMsgsToAppl()
**
** This function will scan the queue for incoming messages from the ABCC. If
** there are any messages it will send one of them to the application through
** a call-back function.
**
**------------------------------------------------------------------------------
**
** Inputs:
**          -
**
** Outputs:
**          -
**
** Usage:
**    aplink_HandleQueuedMsgsToAppl();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void aplink_HandleQueuedMsgsToAppl( void );


/*------------------------------------------------------------------------------
**
** aplink_AddFragAndPostMsg()
**
** Function for sending messages to the ABCC.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psQEntry          - Pointer to message to send.
**
** Outputs:
**    Returns           - Status of request.
**
** Usage:
**    aplink_AddFragAndPostMsg( psQEntry );
**
**------------------------------------------------------------------------------
*/

EXTFUNC ABCC_StatusType aplink_AddFragAndPostMsg( UTIL_BufMsgType* psQEntry );


/*------------------------------------------------------------------------------
**
** aplink_GetSizeOfDataType()
**
** Returns the number of bytes used by a certain data type.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    bDataType         - Data type.
**
** Outputs:
**    returns           - The number of bytes used for this data type.
**
** Usage:
**    bNbrBytes = aplink_GetSizeOfDataType( ABP_SINT16 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT16 aplink_GetSizeOfDataType( UINT8 bDataType );

#endif  /* inclusion lock */

/*******************************************************************************
**
** End of aplink_priv.h
**
********************************************************************************
*/
