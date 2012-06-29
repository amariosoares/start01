/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** aplink.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This module contains the public interface for the Application driver Data
** Link services.
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
**
** Services List
** -------------
**
** Functions called by higher layer (ABCC Driver):
**    APLINK_Init()                 - Initialises the APLINK resources.
**    APLINK_ChangeDriverState()    - Change the state of the driver.
**    APLINK_RunStateMachine()      - Runs the driver state-machine.
**
**
** Functions called by higher layer (ABCC):
**    APLINK_WriteProcessData()     - Write process data to the ABCC module.
**    APLINK_ReadProcessData()      - Read process data from the ABCC module.
**    APLINK_SendMessage()          - Send a message to the ABCC module.
**
**  Functions called by lower level (APDR):
**    APLINK_MessageReceived()      - A new message has been received (from ABCC).
**    APLINK_PutProcessData()       - New process data received from ABCC.
**    APLINK_GetNewProcessData()    - Get new process data from the appl.
**    APLINK_FreeCmdSlots()         - Check if the APP can handle more ABCC Cmds.
**    APLINK_GetNextMessage()       - Get next message from queue.
**    APLINK_CompleteMsgTrans()     - Complete transmission of a Msg.
**    APLINK_UseNewWritePdMap()     - Complete use of a new write PD map.
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

#ifndef APLINK_H
#define APLINK_H


/*******************************************************************************
**
** Defines
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Defines for selecting queue in function APLINK_GetNextMessage().
**
**------------------------------------------------------------------------------
*/

#define APLINK_COMMAND_QUEUE           0
#define APLINK_RESPONSE_QUEUE          1


/*------------------------------------------------------------------------------
**
** Define for the maximum number of commands.
**
**------------------------------------------------------------------------------
*/

#define APLINK_MAX_NBR_COMMANDS_IN_USE ( ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS + \
                                         ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS )



/*******************************************************************************
**
** Data Types
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** APLINK_DriverStateType
**
**------------------------------------------------------------------------------
*/

typedef enum APLINK_DriverStateType
{
   APLINK_SM_IN_RESET               = 0x0000,
   APLINK_SM_SETUP,
   APLINK_SM_IN_CONFIGURATION,
   APLINK_SM_IN_RUN_NO_PD,
   APLINK_SM_IN_RUN,
   APLINK_SM_IN_SHUTDOWN,

   /*
   ** Force the compiler to use a 16-bit variable as enumeration.
   */

   APLINK_SM_SET_ENUM_ANSI_SIZE     = 0x7FFF

}
APLINK_DriverStateType;


/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/

extern UTIL_BufMsgType  APLINK_asRspToAbccBuf[ ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS ];
extern UTIL_BufMsgType  APLINK_asCmdToAbccBuf[ ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS ];
extern UTIL_BufMsgType  APLINK_asMsgToApplBuf[ APLINK_MAX_NBR_COMMANDS_IN_USE ];


/*******************************************************************************
**
** Private Services Definitions
**
********************************************************************************
*/


/*******************************************************************************
**
** Public Services Definitions
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** APLINK_Init()
**
** Initialises the APLINK structure.
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
**    APLINK_Init();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_Init( void );


/*------------------------------------------------------------------------------
**
** APLINK_ChangeDriverState()
**
** Forces a state change of the ABCC driver.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    eRequestedState      - The requested state.
**
** Outputs:
**                         -
**
** Usage:
**    APLINK_ChangeDriverState( APLINK_SM_IN_RESET );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_ChangeDriverState( APLINK_DriverStateType eRequestedState );


/*------------------------------------------------------------------------------
**
** APLINK_RunStateMachine()
**
** This function is responsible for running the entire ABCC driver state-machine.
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
**    APLINK_RunStateMachine();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_RunStateMachine( void );


/*------------------------------------------------------------------------------
**
** APLINK_WriteProcessData()
**
** This function will copy data from the application to the ABCC module. The
** length of the data is set during configuration.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbData         - Pointer to start of data.
**
** Outputs:
**                   -
**
** Usage:
**    APLINK_WriteProcessData( &abMyWriteProcessData[ 0 ] );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_WriteProcessData( UINT8* pbData );


#ifdef ABCC_ENABLE_READ_PD_FUNCTION
/*------------------------------------------------------------------------------
**
** APLINK_ReadProcessData()
** This function will copy data from the ABCC module to the application. The
** length of the data is set during configuration.
**
**------------------------------------------------------------------------------
**
** Inputs:
**   pbData          - Pointer to the start of data.
**
** Outputs:
**   None            -
**
** Usage:
**    APLINK_ReadProcessData( &pbMyReadData[ 0 ] );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_ReadProcessData( UINT8* pbData );
#endif


/*------------------------------------------------------------------------------
**
** APLINK_SendMessage()
**
** This function will send a message to the ABCC module.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psMessage   - Pointer to message to send.
**
** Outputs:
**    Returns     - Status of the request:
**                  ABCC_STATUS_OK                       - Request sent OK.
**                  ABCC_STATUS_NOT_ALLOWED_DRIVER_STATE - Module is in
**                                                         configuration mode,
**                                                         appl cannot send
**                                                         messages.
**                  ABCC_STATUS_OUT_OF_RESOURCES         - Command buffer full.
**
** Usage:
**    APLINK_SendMessage( &sMessage );
**
**------------------------------------------------------------------------------
*/

EXTFUNC ABCC_StatusType APLINK_SendMessage( ABP_MsgType* psMessage );


/*------------------------------------------------------------------------------
**
** APLINK_MessageReceived()
**
** A complete message received from the ABCC module
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psQEntry             - Pointer to allocated buffer containg message.
**
** Outputs:
**    None                 -
**
** Usage:
**    APLINK_MessageReceived( psQEntry );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_MessageReceived( UTIL_BufMsgType* psQEntry );


/*------------------------------------------------------------------------------
**
** APLINK_PutProcessData()
**
** New ProcessData data received from ABCC.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    bAbccStatus          - Abcc status and SUP-bit.
**    pbSourceData         - Pointer to the start of process data that is
**                           received from ABCC module.
**    iLengthOfProcessData - Length of the received process data.
**
** Outputs:
**
**
** Usage:
**    APLINK_PutIOData( bAbccStatus, &abProcessData, iLength );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_PutProcessData( UINT8 bAbccStatus,
                                    UINT8* pbSourceData,
                                    UINT16 iLengthOfProcessData );


/*------------------------------------------------------------------------------
**
** APLINK_GetNewProcessData()
**
** Get new process data from the appl.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbDestData              - Pointer were to put the process data from the
**                              Application.
**    iLengthOfProcessData    - Length of the process data to copy.
**
** Outputs:
**    return                  - TRUE if any new data is copied to the supplied
**                              buffer.
**
** Usage:
**    APLINK_GetNewProcessData( &abProcessData, iLengthOfProcessData );
**
**------------------------------------------------------------------------------
*/

EXTFUNC BOOL8 APLINK_GetNewProcessData( UINT8* pbDestData,
                                        UINT16 iLengthOfProcessData );


/*------------------------------------------------------------------------------
**
**  APLINK_FreeCmdSlots()
**
**  Check if the APP can handle more ABCC commands.
**
**------------------------------------------------------------------------------
**
** Inputs:
**   None
**
** Outputs:
**   Return       - TRUE   APP can handle more ABCC commands.
**                  FALSE  APP can't handle more ABCC commands.
** Usage:
**   if ( APLINK_FreeCmdSlots() )
**
**------------------------------------------------------------------------------
*/

EXTFUNC BOOL8 APLINK_FreeCmdSlots( void );


/*------------------------------------------------------------------------------
**
** APLINK_GetNextMessage()
**
** Get next message from queue
**
**------------------------------------------------------------------------------
**
** Inputs:
**   bQueueSelected     - Select Cmd. ( APLINK_COMMAND_QUEUE ) or Rsp.
**                        ( APLINK_RESPONSE_QUEUE ) queue.
**   ppsQEntry          - Storage pointer.
**
** Outputs:
**    Return            - TRUE   There is a valid message at the pointer
**                               location.
**                        FALSE  Queue was empty.
**
** Usage:
**   if(  APLINK_GetNextMessage( bQueueSelected, ppsQEntry ) )
**
**------------------------------------------------------------------------------
*/

EXTFUNC BOOL8 APLINK_GetNextMessage( UINT8 bQueueSelected,
                                     UTIL_BufMsgType** ppsQEntry );


/*------------------------------------------------------------------------------
**
** APLINK_CompleteMsgTrans()
**
** The APDR has sent a message received from APLINK. Now, we will free the
** allocated buffer and decrement our "aplink_s.bNbrOfCmdsActiveFromAbcc"
** counter if the message were a response to a command.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psQEntry       - Pointer to the message buffer sent.
**
** Outputs:
**          -
**
** Usage:
**    APLINK_CompleteMsgTrans( psQEntry );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_CompleteMsgTrans( UTIL_BufMsgType* psQEntry );


/*------------------------------------------------------------------------------
**
** APLINK_UseNewWritePdMap()
**
** The APDR has come to the point where it's time to make use of a new
** write process data map previously defined by a REMAP_ADI_WRITE_AREA
** command (serial mode only).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    None
**
** Outputs:
**    None
**
** Usage:
**    APLINK_UseNewWritePdMap( psQEntry );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APLINK_UseNewWritePdMap( void );


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of aplink.h
**
********************************************************************************
*/
