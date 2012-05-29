/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** apdr.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This file contains the public interface for the APDR.
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
#include "sys_adpt.h"
#include "util.h"
#include "abcc.h"
#include "apdr.h"
#include "aplink.h"
#include "abp.h"
#include "apdr_priv.h"


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

apdr_DataType      apdr_s;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** APDR_Init()
**------------------------------------------------------------------------------
*/

void APDR_Init( void )
{
#ifdef ABCC_ACCESS_MODE_SERIAL
   /*
   ** Assign the timeout timer for the serial telegram reception.
   */

   if( ( ABCC_GET_OPERATION_MODE_OM0 == 1 ) &&
       ( ABCC_GET_OPERATION_MODE_OM1 == 0 ) &&
       ( ABCC_GET_OPERATION_MODE_OM2 == 0 ) )
   {
      /*
      ** 19200.
      */

      apdr_s.iTelegramToTime = APDR_TELEGRAM_TIMEOUT_19200;
   }
   else if( ( ABCC_GET_OPERATION_MODE_OM0 == 0 ) &&
            ( ABCC_GET_OPERATION_MODE_OM1 == 1 ) &&
            ( ABCC_GET_OPERATION_MODE_OM2 == 0 ) )
   {
      /*
      ** 57600.
      */

      apdr_s.iTelegramToTime = APDR_TELEGRAM_TIMEOUT_57600;
   }
   else if( ( ABCC_GET_OPERATION_MODE_OM0 == 1 ) &&
            ( ABCC_GET_OPERATION_MODE_OM1 == 1 ) &&
            ( ABCC_GET_OPERATION_MODE_OM2 == 0 ) )
   {
      /*
      ** 115200.
      */

      apdr_s.iTelegramToTime = APDR_TELEGRAM_TIMEOUT_115200;
   }
   else if( ( ABCC_GET_OPERATION_MODE_OM0 == 0 ) &&
            ( ABCC_GET_OPERATION_MODE_OM1 == 0 ) &&
            ( ABCC_GET_OPERATION_MODE_OM2 == 1 ) )
   {
      /*
      ** 625000
      */

      apdr_s.iTelegramToTime = APDR_TELEGRAM_TIMEOUT_625000;
   }
   else
   {
      /*
      ** Unknown/reserved baud rate setting.
      */

      ABCC_CbfDriverError( ABCC_SEV_FATAL, ABCC_EC_INCORRECT_OPERATING_MODE );

   } /* End if( Serial mode ) */



   /*
   ** Create the Telegram timeout timer.
   */

   apdr_s.xTelegramToTimer = UTIL_CreateTimer( (void*)apdr_TelegramTimeout,
                                               apdr_s.iTelegramToTime );
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


#ifdef ABCC_ACCESS_MODE_PARALLEL
   /*
   ** Setup pointers to the DPRAM. We only need this if we can directly access
   ** the dpram without special DPRAM functions.
   */

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   apdr_s.sDpram.pabWritePd   = (UINT8*)( ABCC_PARALLEL_BASE_ADDRESS + ABCC_PARALLEL_OFFSET + APDR_DPRAM_PD_WRITE_LOC );
   apdr_s.sDpram.pabReadPd    = (UINT8*)( ABCC_PARALLEL_BASE_ADDRESS + ABCC_PARALLEL_OFFSET + APDR_DPRAM_PD_READ_LOC );
   apdr_s.sDpram.psWriteMsg   = (ABP_MsgType*)( ABCC_PARALLEL_BASE_ADDRESS + ABCC_PARALLEL_OFFSET + APDR_DPRAM_MESSAGE_WRITE_LOC );
   apdr_s.sDpram.psReadMsg    = (ABP_MsgType*)( ABCC_PARALLEL_BASE_ADDRESS + ABCC_PARALLEL_OFFSET + APDR_DPRAM_MESSAGE_READ_LOC );
#endif


#ifdef ABCC_PARALLEL_POLL_MODE
   /*
   ** Disable polling initially.
   */

   apdr_s.fParallelPollEnabled = FALSE;
#endif /* end #ifdef ABCC_PARALLEL_POLL_MODE */

#endif /* End #ifdef ABCC_ACCESS_MODE_PARALLEL */


   /*
   ** Create the ABCC watchdog timeout timer.
   */

   apdr_s.xAbccWdTimer = UTIL_CreateTimer( (void*)apdr_AbccWatchdogTimeout,
                                           (UINT16)ABCC_WATCHDOG_TIMEOUT_TIME_MS );


   /*
   ** Create the initil handshake timer.
   */

   apdr_s.xInitHsTimer = UTIL_CreateTimer( (void*)apdr_InitHsTimerExpired,
                                           (UINT16)ABCC_INTIAL_HANDSHAKE_DELAY_TIME_MS );


   /*
   ** Set the state of the state-machine to INIT.
   */

   apdr_s.eState = APDR_SM_INIT;

} /* End of APDR_Init() */


#ifdef ABCC_ACCESS_MODE_PARALLEL
/*------------------------------------------------------------------------------
** APDR_ServiceInterruptParallel()
**------------------------------------------------------------------------------
*/
#ifndef ABCC_PARALLEL_POLL_MODE
void APDR_ServiceInterruptParallel( void )
{
   UINT8       bNewAbccStatus;


   /*
   ** Parallel interrupt received, read the ABCC status register.
   */

   SYS_ReadParallel( &bNewAbccStatus, APDR_DPRAM_STAT_REG_OFFSET, 1 );


   /*
   ** Check if toggle bit is changed.
   */

   if( ( bNewAbccStatus & ABP_STAT_T_BIT ) !=
       ( apdr_s.bCurrentStatusReg & ABP_CTRL_T_BIT ) )
   {
      /*
      ** The toggle bit is toggled, that is, a new telegram is posted by the
      ** ABCC.
      */

      apdr_s.bCurrentStatusReg = bNewAbccStatus;

      /*
      ** Signal reception of a new telegram.
      */

      apdr_s.fTelegramReceived = TRUE;

      /*
      ** Signal to application that we have received a new telegram.
      */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
      ABCC_CbfTelegramReceived();
#endif
   }
   else
   {
      /*
      ** Nothing new, do nothing...
      */

      ABCC_CbfDriverError( ABCC_SEV_INFORMATION, ABCC_EC_FALSE_PARALLEL_INT_GENERATED );

   } /* End if( Toggle bit changed? ) */

} /* End of APDR_ServiceInterruptParallel() */


#else /* Either INTERRUPT or POLL mode is used. */
/*------------------------------------------------------------------------------
** APDR_ParallelPollMode()
**------------------------------------------------------------------------------
*/

void APDR_ParallelPollMode( void )
{
   UINT8       bNewAbccStatus;


   /*
   ** Poll status register to see if new telegram available.
   */

   bNewAbccStatus = SYS_VerifyReadParallel( APDR_DPRAM_STAT_REG_OFFSET );


   /*
   ** Check if toggle bit is changed.
   */

   if( ( bNewAbccStatus & ABP_STAT_T_BIT ) !=
       ( apdr_s.bCurrentStatusReg & ABP_CTRL_T_BIT ) )
   {
      /*
      ** The toggle bit is toggled, that is, a new telegram is posted by the
      ** ABCC.
      */

      apdr_s.bCurrentStatusReg = bNewAbccStatus;

      /*
      ** Signal reception of a new telegram.
      */

      apdr_s.fTelegramReceived = TRUE;

      /*
      ** Signal to application that we have received a new telegram.
      */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
      ABCC_CbfTelegramReceived();
#endif
   }
   else
   {
      /*
      ** Nothing new, do nothing...
      */

   } /* End if( Toggle bit changed? ) */

} /* End of APDR_ParallelPollMode() */
#endif /* End #ifdef ABCC_PARALLEL_POLL_MODE */
#endif /* End #ifdef ABCC_ACCESS_MODE_PARALLEL */


#ifdef ABCC_ACCESS_MODE_SERIAL
/*------------------------------------------------------------------------------
** APDR_SerialTelegramReceived()
**------------------------------------------------------------------------------
*/

void APDR_SerialTelegramReceived( UINT8* pbNewData )
{
   /*
   ** Our nice application has received some data for us.
   */

   apdr_s.pbReceptionBuffer   = pbNewData;


   /*
   ** Signal reception of a new telegram.
   */

   apdr_s.fTelegramReceived = TRUE;


   /*
   ** Signal to application that we have received a new telegram.
   */

#ifdef ABCC_TX_RX_CALLBACK_ENABLE
   ABCC_CbfTelegramReceived();
#endif

} /* End of APDR_SerialTelegramReceived() */
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


/*------------------------------------------------------------------------------
** APDR_SetPdSize()
**------------------------------------------------------------------------------
*/

void APDR_SetPdSize( UINT16 iPdReadLength, UINT16 iPdWriteLength )
{
   /*
   ** This function is called by APLINK when the process
   ** data lengths have been determined (or changed).
   */

#ifdef ABCC_ACCESS_MODE_SERIAL
   apdr_s.iSerialLengthRead = iPdReadLength +  ( APDR_SERIAL_CTRL_STAT_LENGTH +
                                                 APDR_SERIAL_MESSAGE_FRAG_LENGTH +
                                                 APDR_SERIAL_CRC_LENGTH );

   SYS_ReStartSerialComm( apdr_s.iSerialLengthRead );
#endif

   /*
   ** Store the length information.
   */

   apdr_s.iPdReadLength    = iPdReadLength;
   apdr_s.iPdWriteLength   = iPdWriteLength;

} /* End of APDR_SetPdSize() */



/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

#ifdef ABCC_ACCESS_MODE_SERIAL
/*------------------------------------------------------------------------------
** apdr_TelegramTimeout()
**------------------------------------------------------------------------------
*/

void apdr_TelegramTimeout( void )
{
   /*
   ** Signal that the telegram timeout timer has expired.
   */

   apdr_s.fAbccTelegramTo = TRUE;

} /* End of apdr_TelegramTimeout() */
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


/*------------------------------------------------------------------------------
** apdr_AbccWatchdogTimeout()
**------------------------------------------------------------------------------
*/

void apdr_AbccWatchdogTimeout( void )
{
   /*
   ** Signal that the ABCC watchdog has expired.
   */

   apdr_s.fAbccWdTo = TRUE;

} /* End of apdr_AbccWatchdogTimeout() */


/*------------------------------------------------------------------------------
** apdr_InitHsTimerExpired()
**------------------------------------------------------------------------------
*/

void apdr_InitHsTimerExpired( void )
{
   /*
   ** Signal that the Initial handshake timer has expired.
   */

   apdr_s.fInitHsTo = TRUE;

} /* End of apdr_InitHsTimerExpired() */


/*******************************************************************************
**
** End of apdr.c
**
********************************************************************************
*/
