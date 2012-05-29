/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** util.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This module contains the public interface for the ABCC driver utilities:
**
** - Timer handling system
** - Handling of big/little-endian
** - Queue/Message handling system
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
**    UTIL_ClearTimerSystem()          - Clears the timer system to initial state.
**    UTIL_CreateTimer()               - This function will create a unique timer.
**    UTIL_GetTimer()                  - Reads the time until timer expires.
**    UTIL_ReloadTimer()               - Set a new time for an allocated timer.
**    UTIL_RunTimerSystem()            - Updates the entire timer system.
**    UTIL_StartTimer()                - Start an allocated timer with a time > 0.
**    UTIL_StopTimer()                 - Stops an allocated timer.
**
**    UTIL_WordToNative()              - Macro that can be used to swap a var to native.
**
**    UTIL_AddToMessageQueue()         - Inserts a message to the queue.
**    UTIL_ClearMessageQueueSystem()   - Empties all message queues.
**    UTIL_CreateMessageQueue()        - This function will create a message queue.
**    UTIL_EntriesLeftInMessageQueue() - Returns the number of entries left.
**    UTIL_GetFirstMessageFromQueue()  - Read and removes a message from queue.
**
**    UTIL_AllocateMsgBuffer()         - Allocates memory for a message.
**    UTIL_ClearMsgBuffer()            - Clear all entries in Buffer-array.
**    UTIL_FreeMsgBuffer()             - Free allocated message buffer.
**
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

#ifndef UTIL_H
#define UTIL_H


/*******************************************************************************
**
** Defines
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Define for an invalid timer.
**
**------------------------------------------------------------------------------
*/

#define UTIL_INVALID_TIMER_ID    0xFFFF


/*------------------------------------------------------------------------------
**
** Define for an invalid message queue Id.
**
**------------------------------------------------------------------------------
*/

#define UTIL_INVALID_MSG_Q_ID    0xFFFF


/*******************************************************************************
**
** Data Types
**
********************************************************************************
*/


/*------------------------------------------------------------------------------
**
** UTIL_MsgFragInfoType
**
**------------------------------------------------------------------------------
*/

typedef struct UTIL_MsgFragInfoType
{
   UINT8    bNbrOfFragments;
   UINT8    bFragCounter;
}
UTIL_MsgFragInfoType;


/*------------------------------------------------------------------------------
**
** UTIL_BufMsgType
**
**------------------------------------------------------------------------------
*/

typedef struct UTIL_BufMsgType
{
   ABP_MsgType             sMsg;
   UTIL_MsgFragInfoType    sFragInfo;
   BOOL8                   fEntryUsed;
}
UTIL_BufMsgType;




/*------------------------------------------------------------------------------
**
**  UTIL_StatusType
**
**------------------------------------------------------------------------------
*/

typedef enum UTIL_StatusType
{
   /*
   ** General status codes.
   */

   UTIL_OK = 0,
   UTIL_INVALID_ID,


   /*
   ** Queue specific status codes.
   */

   UTIL_QUEUE_EMPTY,
   UTIL_QUEUE_FULL,
   UTIL_QUEUE_MESSAGE_TOO_LONG,


   /*
   ** Resource specific status codes.
   */

   UTIL_RESOURCE_NOT_CREATED,
   UTIL_RESOURCE_BUSY,


   /*
   ** Force the compiler to use a 16-bit variable as enumeration.
   */

   UTIL_SET_ENUM_ANSI_SIZE = 0x7FFF

} UTIL_StatusType;


/*------------------------------------------------------------------------------
**
** UTIL_TimerType
**
**------------------------------------------------------------------------------
*/

typedef UINT16 UTIL_TimerType;


/*------------------------------------------------------------------------------
**
** UTIL_QueueType
**
**------------------------------------------------------------------------------
*/

typedef UINT16 UTIL_QueueType;


/*******************************************************************************
**
** Public Services Definitions
**
********************************************************************************
*/


/*------------------------------------------------------------------------------
**
** Timer handling system services
**
**------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
**
** UTIL_ClearTimerSystem()
**
** This function will clear the timer system. That is, all timers will be
** deleted.
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
**    UTIL_ClearTimerSystem();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void UTIL_ClearTimerSystem( void );


/*------------------------------------------------------------------------------
**
** UTIL_CreateTimer()
**
** This function will create a unique timer.
** The timer will not be started when created.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pxFunctionPtr     - Pointer to function that will be called if/when timer
**                        expire.
**    iTime             - Time until expiry.
**
** Outputs:
**    Return            - Returns an identification used for the timer
**                        (UTIL_TimerType). If timer cannot be allocated
**                        UTIL_TIMER_NOT_VALID will be returned as xTimerId.
**
** Usage:
**    xTimerId = UTIL_CreateTimer( pFunctionPtr, iTime );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_TimerType UTIL_CreateTimer( void* pFunctionPtr, UINT16 iTime );


/*------------------------------------------------------------------------------
**
** UTIL_GetTimer()
**
** Reads the time until expired of an allocated timer
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xTimerId          - Timer identification (UTIL_TimerType).
**
** Outputs:
**    Return            - The time until expired in ms.
**
** Usage:
**    iTimeToExpire = UTIL_GetTimer( xTimerId );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT16 UTIL_GetTimer( UTIL_TimerType xTimerId );


/*------------------------------------------------------------------------------
**
** UTIL_ReloadTimer()
**
** Set a new time for an allocated timer. Timer is started if it is stopped.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xTimerId          - Timer identification (UTIL_TimerType).
**    iTime             - New expiry time.
**
** Outputs:
**    return            - Status of the request.
**
** Usage:
**    eStatus = UTIL_ReloadTimer( xTimerId, iTime );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_StatusType UTIL_ReloadTimer( UTIL_TimerType xTimerId, UINT16 iTime );


/*------------------------------------------------------------------------------
**
** UTIL_RunTimerSystem()
**
** This function will run all timers and increase the system clock of the ABCC-
** Driver.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iTickInMs         - The number of ms since this function were called last
**                        time.
**
** Outputs:
**          -
**
** Usage:
**    UTIL_RunTimerSystem( 1 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void UTIL_RunTimerSystem( UINT16 iTickInMs );


/*------------------------------------------------------------------------------
**
** UTIL_StartTimer()
**
** Start an allocated timer with a timer-time larger than 0.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xTimerId          - Timer identification (UTIL_TimerType).
**
** Outputs:
**    return            - Status of the request.
**
** Usage:
**    eStatus = UTIL_StartTimer( xTimerId );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_StatusType UTIL_StartTimer( UTIL_TimerType xTimerId );


/*------------------------------------------------------------------------------
**
** UTIL_StopTimer()
**
** Stops an allocated timer.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xTimerId          - Timer identification (UTIL_TimerType).
**
** Outputs:
**    return            - Status of the request.
**
** Usage:
**    eStatus = UTIL_StopTimer( xTimerId );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_StatusType UTIL_StopTimer( UTIL_TimerType xTimerId );


/*------------------------------------------------------------------------------
**
** Handling of big/little-endian services
**
**------------------------------------------------------------------------------
*/


/*------------------------------------------------------------------------------
**
** UTIL_WordToNative()
**
** This macro will swap one word of data to the native format.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iX             - Word to swap.
**
** Outputs:
**    returns        - The word in native format.
**
** Usage:
**    iMyVar = UTIL_WordToNative( iMyVar );
**
**------------------------------------------------------------------------------
*/

#ifdef ABCC_SYS_BIG_ENDIAN
   #define UTIL_WordToNative( iX ) \
           ( ( ( (iX) & 0xFF00 ) >> 8 ) | ( ( (iX) & 0x00FF ) << 8 ) )
#else
   #define UTIL_WordToNative( iX ) ( iX )
#endif


/*------------------------------------------------------------------------------
**
** Queue/Message handling system services
**
**------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
**
** UTIL_AddToMessageQueue()
**
** Inserts a message to the queue that is specified by the queue Id.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xQueueId          - Queue Id of the queue in which the message shall be
**                        inserted (type is "UTIL_QueueType").
**    psBuffer          - Pointer to the buffer to insert.
**
** Outputs:
**    Return            - Status of the request: UTIL_OK, UTIL_QUEUE_FULL.
**
** Usage:
**    eStatus = UTIL_AddToMessageQueue( xQueueId, psBuffer );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_StatusType UTIL_AddToMessageQueue( UTIL_QueueType xQueueId,
                                                UTIL_BufMsgType* psBuffer );


/*------------------------------------------------------------------------------
**
** UTIL_ClearMessageQueueSystem()
**
** Clears all message queues.
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
**    UTIL_ClearMessageQueueSystem();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void UTIL_ClearMessageQueueSystem( void );


/*------------------------------------------------------------------------------
**
** UTIL_CreateMessageQueue()
**
** This function will create a message queue.
** The length of the queue (number of entries) is statically assigned.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    ppasStartOfBuffer    - Pointer-pointer to the start of the message queue.
**    iNumberOfElements    - The number of array-elements.
**
** Outputs:
**    Return               - Returns the Queue Id ("UTIL_QueueType").
**
** Usage:
**    xQueueId = UTIL_CreateMessageQueue( paxStartOfBuffer,
**                                        iNumberOfElements );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_QueueType UTIL_CreateMessageQueue( UTIL_BufMsgType** ppasStartOfBuffer,
                                                UINT16 iNumberOfElements );


/*------------------------------------------------------------------------------
**
** UTIL_EntriesLeftInMessageQueue()
**
** Returns the number of entries that are left until the message queue is full.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xQueueId             - Queue Id.
**
** Outputs:
**    Return               - Number of entries left (UINT16).
**
** Usage:
**    iNoEntriesLeft = UTIL_EntriesLeftInMessageQueue( xQueueId );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT16 UTIL_EntriesLeftInMessageQueue( UTIL_QueueType xQueueId );


/*------------------------------------------------------------------------------
**
** UTIL_GetFirstMessageFromQueue()
**
** Read and removes a message from a queue that is specified by the queue Id
**
**------------------------------------------------------------------------------
**
** Inputs:
**    xQueueId          - Queue Id.
**    ppsDestMsg        - Pointer to where the pointer will be copied.
**
** Outputs:
**    Return            - Status of the request: UTIL_OK, UTIL_QUEUE_EMPTY.
**
** Usage:
**    eStatus = UTIL_CreateMessageQueue( xQueueId, pbDestinationMessage );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_StatusType UTIL_GetFirstMessageFromQueue( UTIL_QueueType xQueueId,
                                                       UTIL_BufMsgType** ppsDestMsg );


/*------------------------------------------------------------------------------
**
** UTIL_AllocateMsgBuffer()
**
** Allocates a memory block that shall be used for storing a message.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psFirstBufEntry   - Pointer to first entry in buffer-array.
**    iBufLength        - The number of entries of the buffer-array.
**
** Outputs:
**    Returns        - Pointer to allocated buffer. Returns NULL if no buffer
**                     available.
**
** Usage:
**    psMyBuffer = UTIL_AllocateMsgBuffer( psFirstBufEntry, iBufLength );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UTIL_BufMsgType* UTIL_AllocateMsgBuffer( UTIL_BufMsgType* psFirstBufEntry,
                                                 UINT16 iBufLength );


/*------------------------------------------------------------------------------
**
** UTIL_ClearMsgBuffer()
**
** Free all buffers of the "Buffer-array".
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psFirstBufEntry   - Pointer to first entry in buffer-array.
**    iBufLength        - The number of entries of the buffer-array.
**
** Outputs:
**          -
**
** Usage:
**    UTIL_ClearMsgBuffer( psFirstBufEntry, iBufLength );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void UTIL_ClearMsgBuffer( UTIL_BufMsgType* psFirstBufEntry,
                                  UINT16 iBufLength );


/*------------------------------------------------------------------------------
**
** UTIL_FreeMsgBuffer()
**
** Free the allocated memory so that it can be re-used.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psBuf          - Pointer to the allocated memory.
**
** Outputs:
**    None
**
** Usage:
**    UTIL_FreeMsgBuffer( psMyBuffer );
**
**------------------------------------------------------------------------------
*/

#define UTIL_FreeMsgBuffer( psBuf )    psBuf->fEntryUsed = FALSE

#endif  /* inclusion lock */

/*******************************************************************************
**
** End of util.h
**
********************************************************************************
*/
