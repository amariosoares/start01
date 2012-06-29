/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** util.c
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
#include "abcc.h"
#include "sys_adpt.h"
#include "util.h"
#include "util_priv.h"


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

util_DataType  util_s;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** UTIL_ClearTimerSystem()
**------------------------------------------------------------------------------
*/

void UTIL_ClearTimerSystem( void )
{
   util_OneTimerEntryType*    psTimerEntry;
   UINT16                     i;


   /*
   ** Clear entire timer system.
   */

   util_s.sTimerSystem.lCurrentTimeInMs = 0;
   util_s.sTimerSystem.iAllocatedTimers = 0;
   psTimerEntry = &util_s.sTimerSystem.asTimerEntries[ 0 ];

   for( i = 0; i < UTIL_NUMBER_OF_TIMERS; i++ )
   {
      psTimerEntry->pFunctionPtr = NULL;
      psTimerEntry->iTimeInMs    = 0;
      psTimerEntry->fEnabled     = FALSE;

      psTimerEntry++;

   } /* End for( all timers ) */

} /* End of UTIL_ClearTimerSystem() */


/*------------------------------------------------------------------------------
** UTIL_CreateTimer()
**------------------------------------------------------------------------------
*/

UTIL_TimerType UTIL_CreateTimer( void* pFunctionPtr, UINT16 iTime )
{
   UTIL_TimerType   xTimerId;


   /*
   ** Creates a new timer.
   ** Check that we have timers available.
   */

   if( util_s.sTimerSystem.iAllocatedTimers >= UTIL_NUMBER_OF_TIMERS )
   {
      /*
      ** We are out of timers. Generate a FATAL error to the application.
      */

      ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_TIMERS );

      return( UTIL_INVALID_TIMER_ID );
   }

   /*
   ** Check if the pointer is OK
   */

   if( pFunctionPtr == NULL )
   {
      /*
      ** The pointer is probably not correct. Generate a FATAL error to the application.
      */

      ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_TIMER_SYSTEM_MALFUNCTION );

      return( UTIL_INVALID_TIMER_ID );
   }

   /*
   ** Increase the number of allocated timers and fill in the timer information.
   */

   xTimerId = (UTIL_TimerType)util_s.sTimerSystem.iAllocatedTimers;
   util_s.sTimerSystem.asTimerEntries[ xTimerId ].fEnabled       = FALSE;
   util_s.sTimerSystem.asTimerEntries[ xTimerId ].iTimeInMs      = iTime;
   util_s.sTimerSystem.asTimerEntries[ xTimerId ].pFunctionPtr   = (ABCC_TD_pFuncType)pFunctionPtr;
   util_s.sTimerSystem.iAllocatedTimers++;

   /*
   ** Return the index of the allocated timer as the TimerId.
   */

   return( xTimerId );

} /* End of UTIL_CreateTimer() */


/*------------------------------------------------------------------------------
** UTIL_GetTimer()
**------------------------------------------------------------------------------
*/

UINT16 UTIL_GetTimer( UTIL_TimerType xTimerId )
{
   /*
   ** Return the current time of the requested timer.
   */

   if( xTimerId <= util_s.sTimerSystem.iAllocatedTimers )
   {
      return( util_s.sTimerSystem.asTimerEntries[ xTimerId ].iTimeInMs );
   }
   else
   {
      /*
      ** This is an invalid timer. Return 0 ms.
      */

      return( 0 );

   } /* End if( Valid timerId? ) */

} /* End of UTIL_GetTimer() */


/*------------------------------------------------------------------------------
** UTIL_ReloadTimer()
**------------------------------------------------------------------------------
*/

UTIL_StatusType UTIL_ReloadTimer( UTIL_TimerType xTimerId, UINT16 iTime )
{
   util_OneTimerEntryType* psTimerEntry;


   /*
   ** Reload the timer-value of a timer.
   ** First we need to check if this is a valid timer.
   */

   if( ( xTimerId <= util_s.sTimerSystem.iAllocatedTimers ) )
   {
      /*
      ** Timer is valid. Stop the timer and assign a new timeout time.
      */

      psTimerEntry = &util_s.sTimerSystem.asTimerEntries[ xTimerId ];
      psTimerEntry->fEnabled  = FALSE;
      psTimerEntry->iTimeInMs = iTime;


      /*
      ** Time is updated, start the timer.
      */

      psTimerEntry->fEnabled  = TRUE;

      return( UTIL_OK );
   }
   else
   {
      /*
      ** This is not a valid TimerId
      */

      return( UTIL_INVALID_ID );

   } /* End if( Valid timer? ) */

} /* End of UTIL_ReloadTimer() */


/*------------------------------------------------------------------------------
** UTIL_RunTimerSystem()
**------------------------------------------------------------------------------
*/

void UTIL_RunTimerSystem( UINT16 iTickInMs )
{
   util_OneTimerEntryType*    psTimerEntry;
   UINT16                     i;


   /*
   ** Run the timer sub system. Since this function were called last time
   ** iTickInMs has passed.
   */

   util_s.sTimerSystem.lCurrentTimeInMs += iTickInMs;


   /*
   ** Update all the allocated timers.
   */

   psTimerEntry = &util_s.sTimerSystem.asTimerEntries[ 0 ];

   for( i = 0; i < util_s.sTimerSystem.iAllocatedTimers; i++ )
   {
      if( ( psTimerEntry->fEnabled ) &&
          ( psTimerEntry->iTimeInMs != 0 ) )
      {
         /*
         ** Timer is enabled. Decrease the timeout time.
         */

         if( psTimerEntry->iTimeInMs <= iTickInMs )
         {
            /*
            ** This timers remaining time is less to our equal to the time of
            ** this tick. This means that the remaining time is zero.
            */

            psTimerEntry->iTimeInMs = 0;


            if( psTimerEntry->pFunctionPtr != NULL )
            {
               /*
               ** Execute the call-back function of this timer.
               */

               psTimerEntry->pFunctionPtr();
            }

         }
         else
         {
            /*
            ** Decrease the time left on the timer with the number of ms that
            ** has passed.
            */

            psTimerEntry->iTimeInMs -= iTickInMs;
         }

      } /* End if( Timer Enabled ) */


      /*
      ** Set the pointer to point at next entry in the timer list.
      */

      psTimerEntry++;

   } /* End for( all timers ) */

} /* End of UTIL_RunTimerSystem() */


/*------------------------------------------------------------------------------
** UTIL_StartTimer()
**------------------------------------------------------------------------------
*/

UTIL_StatusType UTIL_StartTimer( UTIL_TimerType xTimerId )
{
   /*
   ** Start the timer with the current time.
   */

   if( ( xTimerId <= util_s.sTimerSystem.iAllocatedTimers ) )
   {
      /*
      ** Timer is valid, start the timer.
      */

      util_s.sTimerSystem.asTimerEntries[ xTimerId ].fEnabled  = TRUE;

      return( UTIL_OK );
   }
   else
   {
      /*
      ** This is not a valid TimerId
      */

      return( UTIL_INVALID_ID );

   } /* End if( Valid timer? ) */

} /* End of UTIL_StartTimer() */


/*------------------------------------------------------------------------------
** UTIL_StopTimer()
**------------------------------------------------------------------------------
*/

UTIL_StatusType UTIL_StopTimer( UTIL_TimerType xTimerId )
{
   /*
   ** Stop the timer.
   */

   if( ( xTimerId <= util_s.sTimerSystem.iAllocatedTimers ) )
   {
      /*
      ** Timer is valid, stop the timer.
      */

      util_s.sTimerSystem.asTimerEntries[ xTimerId ].fEnabled  = FALSE;

      return( UTIL_OK );
   }
   else
   {
      /*
      ** This is not a valid TimerId
      */

      return( UTIL_INVALID_ID );

   } /* End if( Valid timer? ) */

} /* End of UTIL_StopTimer() */


/*------------------------------------------------------------------------------
** UTIL_AddToMessageQueue()
**------------------------------------------------------------------------------
*/

UTIL_StatusType UTIL_AddToMessageQueue( UTIL_QueueType xQueueId,
                                        UTIL_BufMsgType* psBuffer )
{
   UTIL_BufMsgType**    ppsVeryLast;
   util_OneQueueEntry*  psQEntry;


   SYS_UseCritical();


   /*
   ** Add the specified message to the message queue.
   */

   if( (UINT16)xQueueId <= util_s.sQueueSystem.iAllocatedQueues )
   {
      /*
      ** Enter a critical section to guarantee consistency.
      */

      SYS_EnterCritical();

      /*
      ** Check so that the queue is not full.
      */

      psQEntry = &util_s.sQueueSystem.asQueueEntries[ xQueueId ];

      if( psQEntry->iCurrentNbrOfEntries < psQEntry->iMaxNbrOfEntries )
      {
         /*
         ** There is at least one free entry. Check so that we are not trying
         ** to add messages that are longer than what is allocated.
         */

         *psQEntry->ppsInsertPtr = psBuffer;


         /*
         ** Increase the number of entries that are present in the queue.
         */

         psQEntry->iCurrentNbrOfEntries++;


         /*
         ** Increase the "Insert"-pointer.
         */

         psQEntry->ppsInsertPtr++;


         /*
         ** Check so that we haven't exceeded the length of the buffer.
         ** Setup the temporary pointer to point at the very last entry in our
         ** queue array. If the new "Insert"-pointer points at a position
         ** after our temporary pointer we will set the "Insert"-pointer to
         ** the first position (works like a ring-buffer).
         */

         ppsVeryLast = psQEntry->ppsStartPtr;
         ppsVeryLast += psQEntry->iMaxNbrOfEntries;

         if( psQEntry->ppsInsertPtr >= ppsVeryLast )
         {
            /*
            ** Set the "Insert"-ptr to the very first entry in the
            ** array/ring-buffer.
            */

            psQEntry->ppsInsertPtr = psQEntry->ppsStartPtr;
         }


         /*
         ** We are done, new item is inserted and pointers are updated.
         */

         SYS_ExitCritical();

         return( UTIL_OK );
      }
      else
      {
         /*
         ** The queue is full! Exit critical and return.
         */

         SYS_ExitCritical();


         return( UTIL_QUEUE_FULL );

      } /* End if( Free entries in queue? ) */

   }
   else
   {
      /*
      ** The QueueId is not valid.
      */

      return( UTIL_INVALID_ID );

   } /* End if( Valid QueueId? ) */

} /* End of UTIL_AddToMessageQueue() */


/*------------------------------------------------------------------------------
** UTIL_ClearMessageQueueSystem()
**------------------------------------------------------------------------------
*/

void UTIL_ClearMessageQueueSystem( void )
{
   util_OneQueueEntry*     psQueueEntry;
   UINT16                  i;


   /*
   ** Clear all message queues.
   */

   util_s.sQueueSystem.iAllocatedQueues   = 0;
   psQueueEntry                           = &util_s.sQueueSystem.asQueueEntries[ 0 ];

   for( i = 0; i < UTIL_NUMBER_OF_QUEUES; i++ )
   {
      psQueueEntry->ppsStartPtr            = NULL;
      psQueueEntry->ppsInsertPtr           = NULL;
      psQueueEntry->ppsRemovePtr           = NULL;
      psQueueEntry->iCurrentNbrOfEntries   = 0;
      psQueueEntry->iMaxNbrOfEntries       = 0;

      psQueueEntry++;
   }

} /* End of UTIL_ClearMessageQueueSystem() */


/*------------------------------------------------------------------------------
** UTIL_CreateMessageQueue()
**------------------------------------------------------------------------------
*/

UTIL_QueueType UTIL_CreateMessageQueue( UTIL_BufMsgType** ppasStartOfBuffer,
                                        UINT16 iNumberOfElements )
{
   util_OneQueueEntry*  psQEntry;
   UTIL_QueueType       xQueueId;


   SYS_UseCritical();


   /*
   ** Create a new message queue abstraction.
   ** Check that there are any queues available.
   */

   if( util_s.sQueueSystem.iAllocatedQueues >= UTIL_NUMBER_OF_QUEUES )
   {
      /*
      ** We are out of message queues. Generate a FATAL error to the application.
      */

      ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_OUT_OF_QUEUES );

      return( UTIL_INVALID_MSG_Q_ID );

   } /* End if( Out of queues? ) */


   /*
   ** Set-up the Message queue structure.
   */

   xQueueId = util_s.sQueueSystem.iAllocatedQueues;
   util_s.sQueueSystem.iAllocatedQueues++;


   /*
   ** Enter a critical section.
   */

   SYS_EnterCritical();

   psQEntry = &util_s.sQueueSystem.asQueueEntries[ xQueueId ];
   psQEntry->ppsStartPtr           = ppasStartOfBuffer;
   psQEntry->ppsInsertPtr          = ppasStartOfBuffer;
   psQEntry->ppsRemovePtr          = ppasStartOfBuffer;
   psQEntry->iCurrentNbrOfEntries  = 0;
   psQEntry->iMaxNbrOfEntries      = iNumberOfElements;

   /*
   ** ...and exit the critical section.
   */

   SYS_ExitCritical();


   return( xQueueId );

} /* End of UTIL_CreateMessageQueue() */


/*------------------------------------------------------------------------------
** UTIL_EntriesLeftInMessageQueue()
**------------------------------------------------------------------------------
*/

UINT16 UTIL_EntriesLeftInMessageQueue( UTIL_QueueType xQueueId )
{
   util_OneQueueEntry*  psQEntry;
   UINT16               iEntriesRemaining;


   SYS_UseCritical();


   /*
   ** Return the number of entries that are free in the queue. That is, if the
   ** queue can contain 10 messages and there are 7 messages stored this function
   ** will return 3.
   */

   if( (UINT16)xQueueId <= util_s.sQueueSystem.iAllocatedQueues )
   {
      /*
      ** Since this resource might be used by several users we need to use the
      ** "SYS_EnterCritical()" to guarantee that we are the only one accessing
      ** the resource.
      */

      SYS_EnterCritical();


      /*
      ** Check the number of entries left.
      */

      psQEntry = &util_s.sQueueSystem.asQueueEntries[ xQueueId ];

      iEntriesRemaining = ( psQEntry->iMaxNbrOfEntries -
                            psQEntry->iCurrentNbrOfEntries );

      /*
      ** We are done, release the critical section.
      */

      SYS_ExitCritical();


      return( iEntriesRemaining );
   }
   else
   {
      /*
      ** Not valid QueueId.
      */

      return( 0 );

   } /* End if( Valid QueueId? ) */

} /* End of UTIL_EntriesLeftInMessageQueue() */


/*------------------------------------------------------------------------------
** UTIL_GetFirstMessageFromQueue()
**------------------------------------------------------------------------------
*/

UTIL_StatusType UTIL_GetFirstMessageFromQueue( UTIL_QueueType xQueueId,
                                               UTIL_BufMsgType** ppsDestMsg )
{
   util_OneQueueEntry*  psQEntry;
   UTIL_BufMsgType**    ppsVeryLast;


   SYS_UseCritical();


   /*
   ** This function will copy and delete the pointer to the OLDEST message from
   ** the specified queue.
   */

   if( (UINT16)xQueueId <= util_s.sQueueSystem.iAllocatedQueues )
   {
      /*
      ** Use critical sections to guarantee that we are the only one accessing
      ** the resource.
      */

      SYS_EnterCritical();

      /*
      ** Check that there are any messages available to read.
      */

      if( util_s.sQueueSystem.asQueueEntries[ xQueueId ].iCurrentNbrOfEntries > 0 )
      {
         /*
         ** Copy the pointer to the message from the queue to which we are
         ** now granted access.
         */

         psQEntry = &util_s.sQueueSystem.asQueueEntries[ xQueueId ];

         *ppsDestMsg = *psQEntry->ppsRemovePtr;


         /*
         ** Clear the queue entry.
         */

         *psQEntry->ppsRemovePtr = NULL;


         /*
         ** Now we need to remove this entry since we have copied the data to
         ** the application.
         */

         psQEntry->iCurrentNbrOfEntries--;
         psQEntry->ppsRemovePtr++;

         /*
         ** Since we are simulating a ring-buffer with an array we need to check
         ** that we are not running over the edge. If we are we need to set the
         ** "remove"-pointer to the very first position of the array.
         */

         ppsVeryLast = psQEntry->ppsStartPtr;
         ppsVeryLast += psQEntry->iMaxNbrOfEntries;

         if( psQEntry->ppsRemovePtr >= ppsVeryLast )
         {
            /*
            ** We have wrapped around in the array.
            */

            psQEntry->ppsRemovePtr = psQEntry->ppsStartPtr;
         }


         /*
         ** We are done, release the critical section.
         */

         SYS_ExitCritical();


         return( UTIL_OK );
      }
      else
      {
         /*
         ** Queue is empty. Release the critical section and return.
         */

         SYS_ExitCritical();


         return( UTIL_QUEUE_EMPTY );

      } /* End if( Any messages? ) */

   }
   else
   {
      /*
      ** Not valid QueueId.
      */

      return( UTIL_INVALID_ID );

   } /* End if( Valid QueueId? ) */

} /* End of UTIL_GetFirstMessageFromQueue() */


/*------------------------------------------------------------------------------
** UTIL_AllocateMsgBuffer()
**------------------------------------------------------------------------------
*/

UTIL_BufMsgType* UTIL_AllocateMsgBuffer( UTIL_BufMsgType* psFirstBufEntry, UINT16 iBufLength )
{
   UINT16               iEntry;


   SYS_UseCritical();


   /*
   ** Scan the buffer structure to find an entry that is not in use and return
   ** a pointer to it.
   */

   for( iEntry = 0; iEntry < iBufLength; iEntry++ )
   {
      SYS_EnterCritical();

      if( psFirstBufEntry->fEntryUsed == TRUE )
      {
         /*
         ** Entry occupied, try next.
         */

         SYS_ExitCritical();

         psFirstBufEntry++;
      }
      else
      {
         /*
         ** Entry is not occupied. Use this entry.
         */

         psFirstBufEntry->fEntryUsed = TRUE;

         SYS_ExitCritical();


         return( psFirstBufEntry );

      } /* End if( psFirstBufEntry->fEntryUsed == TRUE ) */

   } /* End for( iEntry = 0; iEntry < iBufLength; iEntry++ ) */


   /*
   ** No free entry found!
   */

   return( NULL );

} /* End of UTIL_AllocateMsgBuffer() */


/*------------------------------------------------------------------------------
** UTIL_ClearMsgBuffer()
**------------------------------------------------------------------------------
*/

void UTIL_ClearMsgBuffer( UTIL_BufMsgType* psFirstBufEntry, UINT16 iBufLength )
{
   UINT16               iEntry;


   SYS_UseCritical();


   /*
   ** Free all buffers of the "Buffer-array".
   */

   SYS_EnterCritical();

   for( iEntry = 0; iEntry < iBufLength; iEntry++ )
   {
      /*
      ** Free the entry.
      */

      UTIL_FreeMsgBuffer( psFirstBufEntry );

      psFirstBufEntry++;
   } /* End for( iEntry = 0; iEntry < iBufLength; iEntry++ ) */

   SYS_ExitCritical();

} /* End of UTIL_ClearMsgBuffer() */


/*******************************************************************************
**
** End of util.c
**
********************************************************************************
*/
