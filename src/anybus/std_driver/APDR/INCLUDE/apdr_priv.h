/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** apdr_priv.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Private interface for the application driver (low level).
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
**    apdr_TelegramTimeout()           - This function signals telegram timeout.
**    apdr_AbccWatchdogTimeout()       - This function signals ABCC watchdog
**                                       timeout.
**    apdr_InitHsTimerExpired()        - Initial handshake timer expired.
**
**    apdr_GetAbccStateAndSup()        - Reads the ABCC state & Sup-bit.
**    apdr_BuildSerialTelegram()       - Build the serial transmission buffer.
**
** Tasks:
**
**        -
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

#ifndef APDR_PRIV_H
#define APDR_PRIV_H


/*******************************************************************************
**
** Defines
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Serial limitiations.
**
**------------------------------------------------------------------------------
*/

#define APDR_SERIAL_CTRL_STAT_LENGTH         1
#define APDR_SERIAL_IO_LENGTH                ABP_MAX_PROCESS_DATA
#define APDR_SERIAL_CRC_LENGTH               2
#define APDR_SERIAL_TELEGRAM_LENGTH          ( APDR_SERIAL_CTRL_STAT_LENGTH +    \
                                               APDR_SERIAL_MESSAGE_FRAG_LENGTH + \
                                               APDR_SERIAL_IO_LENGTH +           \
                                               APDR_SERIAL_CRC_LENGTH )


/*------------------------------------------------------------------------------
**
** Parallel mode offset definitions.
**
**------------------------------------------------------------------------------
*/

#define APDR_DPRAM_CTRL_REG_OFFSET        ABCC_PARALLEL_OFFSET + 0x7FE
#define APDR_DPRAM_STAT_REG_OFFSET        ABCC_PARALLEL_OFFSET + 0x7FF

#define APDR_DPRAM_PD_WRITE_LOC           0x000
#define APDR_DPRAM_PD_READ_LOC            0x100
#define APDR_DPRAM_MESSAGE_WRITE_LOC      0x300
#define APDR_DPRAM_MESSAGE_READ_LOC       0x500

#define APDR_DPRAM_MESSAGE_READ_OFFSET    ABCC_PARALLEL_OFFSET + APDR_DPRAM_MESSAGE_READ_LOC
#define APDR_DPRAM_MESSAGE_WRITE_OFFSET   ABCC_PARALLEL_OFFSET + APDR_DPRAM_MESSAGE_WRITE_LOC
#define APDR_DPRAM_PD_READ_OFFSET         ABCC_PARALLEL_OFFSET + APDR_DPRAM_PD_READ_LOC
#define APDR_DPRAM_PD_WRITE_OFFSET        ABCC_PARALLEL_OFFSET + APDR_DPRAM_PD_WRITE_LOC


/*------------------------------------------------------------------------------
**
** Defines for the telegram timeout timer.
**
**------------------------------------------------------------------------------
*/

#define APDR_TELEGRAM_TIMEOUT_19200       350
#define APDR_TELEGRAM_TIMEOUT_57600       120
#define APDR_TELEGRAM_TIMEOUT_115200      60
#define APDR_TELEGRAM_TIMEOUT_625000      20


/*------------------------------------------------------------------------------
**
** Defines for keeping track of what type of message that we sent in last tele-
** gram.
**
**------------------------------------------------------------------------------
*/

#define APDR_MESSAGE_COMMAND_TYPE         0x40
#define APDR_MESSAGE_RESPONSE_TYPE        0x00


/*******************************************************************************
**
** Data Types
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** apdr_DpramType
**
**------------------------------------------------------------------------------
*/

typedef struct apdr_DpramType
{
   UINT8*               pabWritePd;
   UINT8*               pabReadPd;
   ABP_MsgType*         psWriteMsg;
   ABP_MsgType*         psReadMsg;
}
apdr_DpramType;


/*------------------------------------------------------------------------------
**
** State machine for the main APDR state-machine that is running the
** communication with the ABCC-driver.
**
**------------------------------------------------------------------------------
*/

typedef enum apdr_StateMachineType
{
   /*
   ** Initil state. Initialises timers etc.
   */

   APDR_SM_INIT,


   /*
   ** State where we are waiting for permission to start communicating with the
   ** ABCC module.
   */

   APDR_SM_INITIAL_HANDSHAKE,


   /*
   ** Idle - We are able to initialise new commands.
   */

   APDR_SM_IDLE,


   /*
   ** We are sending a new telegram, containing a message.
   */

   APDR_SM_SEND_TELEGRAM_WITH_MESSAGE,


   /*
   ** We are sending a new telegram containing only process data.
   */

   APDR_SM_SEND_TELEGRAM_WITHOUT_MESSAGE,


   /*
   ** We are waiting for a telegram from the ABCC module.
   */

   APDR_SM_WAIT_FOR_TELEGRAM,


   /*
   ** Process a telegram received from the ABCC module.
   */

   APDR_SM_PROCESS_RECEIVED_TELEGRAM,


   /*
   ** ABCC watchdog timeout has occured.
   */

   APDR_SM_ABCC_COM_LOST,

   /*
   ** Force the compiler to use a 16-bit variable as enumeration.
   */

   APDR_SM_ENUM_ANSI_SIZE = 0x7FFF

}
apdr_StateMachineType;


/*------------------------------------------------------------------------------
**
** apdr_ReadFragInfoType
**
**------------------------------------------------------------------------------
*/

typedef struct apdr_ReadFragInfoType
{
   UINT16         iNbrRecFrags;
   UINT8          abMessage[ APDR_MESSAGE_LENGTH ];
}
apdr_ReadFragInfoType;


/*------------------------------------------------------------------------------
**
** apdr_TempPdDataType
**
**------------------------------------------------------------------------------
*/

typedef struct apdr_TempPdDataType
{
   union
   {
      /*
      ** This data must be word-aligned.
      */

      UINT8    ab[ ABP_MAX_PROCESS_DATA ];
      UINT16   ai[ ABP_MAX_PROCESS_DATA / 2 ];
   }
   uData;
}
apdr_TempPdDataType;


/*------------------------------------------------------------------------------
**
** apdr_DataType
**
**------------------------------------------------------------------------------
*/

typedef struct apdr_DataType
{
   /*---------------------------------------------------------------------------
   **
   ** psWriteMsgBuffer
   **
   **---------------------------------------------------------------------------
   **
   ** Pointer to structure that contains a message that shall be sent.
   **
   **---------------------------------------------------------------------------
   */

   UTIL_BufMsgType*              psWriteMsgBuffer;


#ifdef ABCC_ACCESS_MODE_SERIAL
   /*---------------------------------------------------------------------------
   **
   ** sMsgRead
   **
   **---------------------------------------------------------------------------
   **
   ** Structure holding a complete message during (read) de-fragmentation.
   **
   **---------------------------------------------------------------------------
   */

   apdr_ReadFragInfoType            sMsgRead;

#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


#ifdef ABCC_ACCESS_MODE_PARALLEL

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   /*---------------------------------------------------------------------------
   **
   ** sDpram
   **
   **---------------------------------------------------------------------------
   **
   ** Structure containing pointer to the DPRAM areas.
   **
   **---------------------------------------------------------------------------
   */

   apdr_DpramType         sDpram;
#endif

#ifndef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   /*---------------------------------------------------------------------------
   **
   ** sTempPd
   **
   **---------------------------------------------------------------------------
   **
   ** Since we cannot directly access the DPRAM memory we will need to read
   ** (and write) to/from the DPRAM through buffers.
   ** One buffer for messages and one for process data.
   ** This buffer is put into a structure so that word alignment can be
   ** guaranteed.
   **
   **---------------------------------------------------------------------------
   */

   apdr_TempPdDataType        sTempPd;

#endif /* End #ifndef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR */

#endif /* End #ifdef ABCC_ACCESS_MODE_PARALLEL */


   /*---------------------------------------------------------------------------
   **
   ** eState
   **
   **---------------------------------------------------------------------------
   **
   ** The state of the APDR state-machine.
   **
   **---------------------------------------------------------------------------
   */

   apdr_StateMachineType      eState;


#ifdef ABCC_ACCESS_MODE_SERIAL
   /*---------------------------------------------------------------------------
   **
   ** xTelegramToTimer
   **
   **---------------------------------------------------------------------------
   **
   ** Timer for timeout for each telegram.
   **
   **---------------------------------------------------------------------------
   */

   UTIL_TimerType             xTelegramToTimer;


   /*---------------------------------------------------------------------------
   **
   ** iTelegramToTime
   **
   **---------------------------------------------------------------------------
   **
   ** Timeout timer for each serial telegram. This variable is baud rate
   ** dependent.
   **
   **---------------------------------------------------------------------------
   */

   UINT16                     iTelegramToTime;
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


   /*---------------------------------------------------------------------------
   **
   ** xAbccWdTimer
   **
   **---------------------------------------------------------------------------
   **
   ** Timer for the ABCC watchdog timer.
   **
   **---------------------------------------------------------------------------
   */

   UTIL_TimerType             xAbccWdTimer;


   /*---------------------------------------------------------------------------
   **
   ** xInitHsTimer
   **
   **---------------------------------------------------------------------------
   **
   ** Timer that is used during initialisation for the initial handshake.
   **
   **---------------------------------------------------------------------------
   */

   UTIL_TimerType             xInitHsTimer;


   /*---------------------------------------------------------------------------
   **
   ** bLastStatusReg
   ** bCurrentStatusReg
   **
   **---------------------------------------------------------------------------
   **
   ** Last value and current value of the received status register.
   **
   **---------------------------------------------------------------------------
   */

   UINT8                      bLastStatusReg;
   UINT8                      bCurrentStatusReg;


   /*---------------------------------------------------------------------------
   **
   ** bWorkingControlReg
   **
   **---------------------------------------------------------------------------
   **
   ** The working copy of the application control register.
   **
   **---------------------------------------------------------------------------
   */

   UINT8                      bWorkingControlReg;


   /*---------------------------------------------------------------------------
   **
   ** bLastSentMsgType
   **
   **---------------------------------------------------------------------------
   **
   ** Variable that holds the type of the last message.
   **
   **---------------------------------------------------------------------------
   */

   UINT8                      bLastSentMsgType;


#ifdef ABCC_ACCESS_MODE_SERIAL
   /*---------------------------------------------------------------------------
   **
   ** fAbccTelegramTo
   **
   **---------------------------------------------------------------------------
   **
   ** Flag that signals that telegram timeout has occured.
   **
   **---------------------------------------------------------------------------
   */

   BOOL8                      fAbccTelegramTo;
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


   /*---------------------------------------------------------------------------
   **
   ** fAbccWdTo
   **
   **---------------------------------------------------------------------------
   **
   ** Flag that signals that ABCC watchdog timeout has occured.
   **
   **---------------------------------------------------------------------------
   */

   BOOL8                      fAbccWdTo;


   /*---------------------------------------------------------------------------
   **
   ** fTelegramReceived
   **
   **---------------------------------------------------------------------------
   **
   ** Flag that signals that a new telegram has been received.
   **
   **---------------------------------------------------------------------------
   */

   BOOL8                       fTelegramReceived;


   /*---------------------------------------------------------------------------
   **
   ** fInitHsTo
   **
   **---------------------------------------------------------------------------
   **
   ** Flag that is used to signal initial handshake timeout.
   **
   **---------------------------------------------------------------------------
   */

   BOOL8                       fInitHsTo;


#ifdef ABCC_ACCESS_MODE_SERIAL
   /*---------------------------------------------------------------------------
   **
   ** pbReceptionBuffer
   **
   **---------------------------------------------------------------------------
   **
   ** Pointer to the telegram that were received.
   **
   **---------------------------------------------------------------------------
   */

   UINT8*                  pbReceptionBuffer;


   /*---------------------------------------------------------------------------
   **
   ** abTransmissionBuffer[]
   **
   **---------------------------------------------------------------------------
   **
   ** Buffer for holding telegrams (only in serial mode).
   **
   **---------------------------------------------------------------------------
   */

   UINT8                   abTransmissionBuffer[ APDR_SERIAL_TELEGRAM_LENGTH ];


   /*---------------------------------------------------------------------------
   **
   ** iSerialLengthWrite
   **
   **---------------------------------------------------------------------------
   **
   ** The length in bytes of the message that is present in the transmission
   ** buffer.
   **
   **---------------------------------------------------------------------------
   */

   UINT16                  iSerialLengthWrite;


   /*---------------------------------------------------------------------------
   **
   ** iSerialLengthRead
   **
   **---------------------------------------------------------------------------
   **
   ** The length in bytes that we have received.
   **
   **---------------------------------------------------------------------------
   */

   UINT16                  iSerialLengthRead;

#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


   /*---------------------------------------------------------------------------
   **
   ** iPdReadLength
   ** iPdWriteLength
   **
   **---------------------------------------------------------------------------
   **
   ** Length indicators for process data.
   **
   **---------------------------------------------------------------------------
   */

   UINT16                  iPdReadLength;
   UINT16                  iPdWriteLength;


   /*------------------------------------------------------------------------------
   **
   ** fAbccWdToOccured
   **
   **------------------------------------------------------------------------------
   **
   ** Flag that indicates if we have previously encountered a ABCC WD timeout.
   **
   **------------------------------------------------------------------------------
   */

   BOOL8                    fAbccWdToOccured;


#ifdef ABCC_ACCESS_MODE_PARALLEL
#ifdef ABCC_PARALLEL_POLL_MODE
   /*---------------------------------------------------------------------------
   **
   ** fParallelPollEnabled
   **
   **---------------------------------------------------------------------------
   **
   ** Flag to indicate that Polling is allowed.
   **
   **---------------------------------------------------------------------------
   */

   BOOL8                   fParallelPollEnabled;
#endif /* end #ifdef ABCC_PARALLEL_POLL_MODE */
#endif /* end #ifdef ABCC_ACCESS_MODE_PARALLEL */


#ifdef ABCC_ACCESS_MODE_SERIAL
   /*---------------------------------------------------------------------------
   **
   ** fCompleteWrPdRemap
   **
   **---------------------------------------------------------------------------
   **
   ** Flag, indicating that a switch to new write process data map
   ** shall be done at a time appropriate for the serial protocoll.
   **
   **---------------------------------------------------------------------------
   */

   BOOL8                   fCompleteWrPdRemap;
#endif /* ABCC_ACCESS_MODE_SERIAL */

}
apdr_DataType;



/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** apdr_s
**
**------------------------------------------------------------------------------
**
** Structure containing all variables used by the APDR object.
**
**------------------------------------------------------------------------------
*/

extern apdr_DataType     apdr_s;


/*******************************************************************************
**
** Private Services Definitions
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** apdr_TelegramTimeout()
**
** Signals that the telegram reception has timed out.
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
**    Called by timer.
**
**------------------------------------------------------------------------------
*/

EXTFUNC void apdr_TelegramTimeout( void );


/*------------------------------------------------------------------------------
**
** apdr_AbccWatchdogTimeout()
**
** Signals that the watchdog timer has expired.
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
**    Called by timer.
**
**------------------------------------------------------------------------------
*/

EXTFUNC void apdr_AbccWatchdogTimeout( void );


/*------------------------------------------------------------------------------
**
** apdr_InitHsTimerExpired()
**
** Function that signals that the intial-handshake timer has expired.
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
**    apdr_InitHsTimerExpired();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void apdr_InitHsTimerExpired( void );


/*------------------------------------------------------------------------------
**
** apdr_GetAbccStateAndSup()
**
** Reads the state and supervision bit of the ABCC module from the status
** register.
**
**------------------------------------------------------------------------------
**
** Inputs:
**                   - None.
**
** Outputs:
**    Returns        - The state bits and supervision bit.
**
** Usage:
**    bStateAndSup = apdr_GetAbccStateAndSup();
**
**------------------------------------------------------------------------------
*/


#define apdr_GetAbccStateAndSup()  ( apdr_s.bCurrentStatusReg & \
                                   ( ABP_STAT_S_BITS | ABP_STAT_SUP_BIT ) )


/*------------------------------------------------------------------------------
**
** apdr_BuildSerialTelegram()
**
** This function builds the serial transmission buffer and returns the length of
** the assembled transmission buffer in bytes.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    fMessageData         - Whether message data shall be sent or not.
**
** Outputs:
**    Returns              - The length of the transmission buffer in bytes.
**
** Usage:
**    iLength = apdr_BuildSerialTelegram( TRUE );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT16 apdr_BuildSerialTelegram( BOOL8 fMessageData );

#endif  /* inclusion lock */

/*******************************************************************************
**
** End of apdr_priv.h
**
********************************************************************************
*/
