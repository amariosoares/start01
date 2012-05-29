/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** apdr.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** Public interface for the application driver (low level).
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
** Public Services:
**
** Called by higher layer (APLINK)
**    APDR_RunDataLinkStateMachine()   - Handles the state-machine.
**    APDR_ParallelPollMode()          - Poll the "Status Register" of the ABCC.
**    APDR_Init()                      - Initialise the Application Driver.
**    APDR_SetPdSize()                 - Set the process data lengths
**    APDR_PrepareNewWritePdMap()      - Prepare use of new write PD map.
**
** Called by the Application;
**    APDR_ServiceInterruptParallel()  - Called by the application when a
**                                       parallel interrupt is received.
**    APDR_SerialTelegramReceived()    - A serial telegram is received.
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

#ifndef APDR_H
#define APDR_H


/*******************************************************************************
**
** Defines
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Defines for Parallel mode.
**
**------------------------------------------------------------------------------
*/

#define APDR_MESSAGE_LENGTH            ( ABP_MAX_MSG_DATA_BYTES + sizeof( ABP_MsgHeaderType ) )


/*------------------------------------------------------------------------------
**
** Serial fragmentation defines.
**
**------------------------------------------------------------------------------
*/

#define APDR_SERIAL_MESSAGE_FRAG_LENGTH      16
#define APDR_SERIAL_MESSAGE_MAX_FRAGS        17
#define APDR_SERIAL_MESSAGE_LAST_FRAG_LEN    7


/*******************************************************************************
**
** Public Services Definitions
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** APDR_RunDataLinkStateMachine()
**
** This function is responsible for running the state-machine that is needed
** for handling the communication with the ABCC.
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
**    APDR_RunDataLinkStateMachine();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_RunDataLinkStateMachine( void );


/*------------------------------------------------------------------------------
**
** APDR_ServiceInterruptParallel()
**
** This function will serve the parallel interrupt. When a parallel interrupt is
** received this function shall be called, unless this function is re-written
** to be the systems parallel-interrupt routine.
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
**    APDR_ServiceInterruptParallel();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_ServiceInterruptParallel( void );


/*------------------------------------------------------------------------------
**
** APDR_ParallelPollMode()
**
** When using POLL mode (parallel interrupt is not used) the ABCC-driver needs to
** poll the "ABCC status" register in order to determine when data is available.
** This function is called by the ABCC-driver Timer system and does not need to be
** called by the application.
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
**    APDR_ParallelPollMode();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_ParallelPollMode( void );


/*------------------------------------------------------------------------------
**
** APDR_SerialTelegramReceived()
**
** This function is called BY THE APPLICATION when a new telegram is
** received from the serial channel. The application does not need to process
** the data in any way (that is, CRC etc does not need to be checked).
**
** NOTE! THE BUFFER MUST NOT BE DELETED OR CLEARED (it can be used to receive
** next telegram).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbNewData         - Start-pointer to the new data.
**
** Outputs:
**          -
**
** Usage:
**    APDR_SerialTelegramReceived( &abDMARecBuffer[ 0 ] );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_SerialTelegramReceived( UINT8* pbNewData );


/*------------------------------------------------------------------------------
**
** APDR_Init()
**
** Initialise the Application Driver.
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
** APDR_Init();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_Init( void );


/*------------------------------------------------------------------------------
**
** APDR_SetPdSize()
**
** This function is called by APLINK when the process data lengths have been
** determined (or changed).
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iPdReadLength        - Number of process data bytes sent from the ABCC
**                           to the application.
**    iPdWriteLength       - Number of process data bytes sent from the
**                           application to the ABCC.
**
** Outputs:
**          -
**
** Usage:
**    APDR_SetPdSize( 11, 16 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_SetPdSize( UINT16 iPdReadLength, UINT16 iPdWriteLength );


/*------------------------------------------------------------------------------
**
** APDR_PrepareNewWritePdMap()
**
** This function is called by APLINK when it's time to prepare use of a new
** write process data map (serial mode only).
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
**    APDR_PrepareNewWritePdMap();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APDR_PrepareNewWritePdMap( void );


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of apdr.h
**
********************************************************************************
*/
