/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** util_priv.h
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
** Private Services:
**
**        -
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

#ifndef UTIL_PRIV_H
#define UTIL_PRIV_H



/*******************************************************************************
**
** Constants
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Define for how many queues that we can handle.
**
**------------------------------------------------------------------------------
*/

#define UTIL_NUMBER_OF_QUEUES             3


/*------------------------------------------------------------------------------
**
** Define for how many timers that can be created an run simultaneously.
**
**------------------------------------------------------------------------------
*/

#define UTIL_NUMBER_OF_TIMERS             3


/*******************************************************************************
**
** Data Types
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** util_OneQueueEntry - Structure describing ONE queue.
**
**------------------------------------------------------------------------------
*/

typedef struct util_OneQueueEntry
{
   UTIL_BufMsgType**    ppsStartPtr;
   UTIL_BufMsgType**    ppsInsertPtr;
   UTIL_BufMsgType**    ppsRemovePtr;

   UINT16               iCurrentNbrOfEntries;
   UINT16               iMaxNbrOfEntries;
}
util_OneQueueEntry;


/*------------------------------------------------------------------------------
**
** util_QueueSystemType - Structure describing the queue system.
**
**------------------------------------------------------------------------------
*/

typedef struct util_QueueSystemType
{
   util_OneQueueEntry   asQueueEntries[ UTIL_NUMBER_OF_QUEUES ];
   UINT16               iAllocatedQueues;
}
util_QueueSystemType;


/*------------------------------------------------------------------------------
**
** util_OneTimerEntryType
**
**------------------------------------------------------------------------------
*/

typedef struct util_OneTimerEntryType
{
   ABCC_TD_pFuncType    pFunctionPtr;
   UINT16               iTimeInMs;
   BOOL8                fEnabled;
}
util_OneTimerEntryType;


/*------------------------------------------------------------------------------
**
** util_TimerSystemType
**
**------------------------------------------------------------------------------
*/

typedef struct util_TimerSystemType
{
   volatile UINT32         lCurrentTimeInMs;
   util_OneTimerEntryType  asTimerEntries[ UTIL_NUMBER_OF_TIMERS ];
   UINT16                  iAllocatedTimers;
}
util_TimerSystemType;


/*------------------------------------------------------------------------------
**
** util_DataType
**
**------------------------------------------------------------------------------
**
** Data type containing all UTIL private variables and structures.
**
**------------------------------------------------------------------------------
*/

typedef struct util_DataType
{
   /*---------------------------------------------------------------------------
   **
   ** sQueueSystem
   **
   **---------------------------------------------------------------------------
   **
   ** Structure encapsulating the Queue handling system.
   **
   **---------------------------------------------------------------------------
   */

   util_QueueSystemType    sQueueSystem;


   /*---------------------------------------------------------------------------
   **
   ** sTimerSystem
   **
   **---------------------------------------------------------------------------
   **
   ** Structure encapsulating the timer handling system.
   **
   **---------------------------------------------------------------------------
   */

   util_TimerSystemType    sTimerSystem;

}
util_DataType;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** util_s
**
**------------------------------------------------------------------------------
**
** Structure encapsulating all util specific variables.
**
**------------------------------------------------------------------------------
*/

extern util_DataType  util_s;


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of util_priv.h
**
********************************************************************************
*/
