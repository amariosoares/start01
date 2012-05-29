/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** sys_adpt.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Header file for system adaptation files for ABCC-driver.
**
********************************************************************************
********************************************************************************
**
** Porting instructions:
** ---------------------
**
** PORTING ALERT! Choose/set the defines to fit system.
** Adapt critical section macros to local host system:
**    - SYS_EnterCritical()
**    - SYS_ExitCritical()
**    - SYS_UseCritical()
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    SYS_ClearMemBytes()        - Clear a number of bytes.
**    SYS_CopyMemBytes()         - Copy a number of bytes.
**    SYS_CompareMemBytes()      - Compare a number of bytes.
**
**    SYS_SendSerialTelegram()   - Send a serial telegram on the async channel.
**    SYS_ReStartSerialComm()    - The serial communication shall be re-started.
**    SYS_ReadParallel()         - Read the DPRAM.
**    SYS_WriteParallel()        - Write data to the DPRAM.
**    SYS_VerifyReadParallel()   - "Read & verify" one byte of the DPRAM.
**
**    SYS_EnterCritical()        - Enter a critical section.
**    SYS_ExitCritical()         - Exit a critical section.
**    SYS_UseCritical()          - Prep for critical section use in function.
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

#ifndef SYS_ADPT_H
#define SYS_ADPT_H


/*******************************************************************************
**
** Defines
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Define if ABCC debug mode shall be used.
** This define is provided for debuging only. In a "release" this define should
** not be defined. Function "ABCC_CbfDebugInfo()" will be called if this define
** is set.
**
**------------------------------------------------------------------------------
*/

#define ABCC_DRIVER_DEBUG


/*------------------------------------------------------------------------------
**
** ABCC_TX_RX_CALLBACK_ENABLE
** Each time a telegram is received/transmitted a call-back function is
** called ( ABCC_CbfTelegramReceived() and ABCC_CbfTelegramTransmitted() ) if
** this define is defined. This information might not be useful to all
** applications.
** If you need to know when a telegram is received/transmitted set this define,
** otherwise comment it away.
**
**------------------------------------------------------------------------------
*/

/* #define ABCC_TX_RX_CALLBACK_ENABLE */


/*------------------------------------------------------------------------------
**
** Define for what kind of physical access mode is used.
** Two options are available:
** - Parallel.
** - Serial asynchronous.
**
**------------------------------------------------------------------------------
*/

/* #define ABCC_ACCESS_MODE_PARALLEL */
#define ABCC_ACCESS_MODE_SERIAL


/*------------------------------------------------------------------------------
**
** Define incompatibility tests.
**
**------------------------------------------------------------------------------
*/

#if defined( ABCC_ACCESS_MODE_PARALLEL ) && defined( ABCC_ACCESS_MODE_SERIAL )
   #error DEFINE EITHER "ABCC_ACCESS_MODE_PARALLEL" OR "ABCC_ACCESS_MODE_SERIAL"!
#endif


/*------------------------------------------------------------------------------
**
** If parallel mode is used the application can choose to poll the ABCC status
** register, in this case this define is set.
** Only defined in case of "ABCC_ACCESS_MODE_PARALLEL".
** Note: It is strongly recommended to use the interrupt pin if possible!
**
**------------------------------------------------------------------------------
*/

/* #define ABCC_PARALLEL_POLL_MODE */


/*------------------------------------------------------------------------------
**
** Define for the base address of the DPRAM used in Parallel mode.
**
**------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
**
** Define if the DPRAM is accessible through a pointer pointing at
** ABCC_PARALLEL_BASE_ADDRESS. This means that "normal" copy routines can be used.
**
** The ABCC_PARALLEL_OFFSET is provided, in order to be able to follow the
** recommendation to always use the highest 2K of the 16K memory range that is
** reserved for the ABCC module (A0..A13). However, the offset can be set to 0,
** but in future implementations with more than 2K DPRAM mounted problems will
** occur.
**                               16K Window (A0-A13)
** ABCC_PARALLEL_BASE_ADDRESS->  +--------------+       ---
**                               |      N       |        ^
**                               |      O       |        |
**                               |      T       |        |
**                               |              |        |
**                               |      U       |        |
**                               |      S       |        |
**                               |      E       |  ABCC_PARALLEL_OFFSET
**                               |      D       |        |
**                               |              |        |
**                               |              |        |
**                               |              |        |
**                               |              |        |
**                               |              |        V
**                               +--------------+       ---
**                               | 2K that is   |
**                               | used today.  |
**                               +--------------+
**
**------------------------------------------------------------------------------
*/

#define ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR

#ifdef ABCC_ACCESS_MODE_PARALLEL
   #define ABCC_PARALLEL_BASE_ADDRESS        0x100000    /* DPRAM @ address 100000h    */
   #define ABCC_PARALLEL_OFFSET              0x3800      /* Offset, use last 2K of 16K */
#endif


/*------------------------------------------------------------------------------
**
** Define for how often the application calls "ABCC_RunTimerSystem()".
** The ABCC-Driver timer system is heavy reliant on a regular cyclic timer.
** Note: It is recommended that this define is set to 10 ms.
**------------------------------------------------------------------------------
*/

#define ABCC_TIMER_RESOLUTION_MS           10


/*------------------------------------------------------------------------------
**
** Define what kind of memory storage model that is used by the microprocessor.
** Define either Big endian ("Motorola") or Little endian ("Intel").
**
**------------------------------------------------------------------------------
*/


/* #define ABCC_SYS_BIG_ENDIAN */
#define ABCC_SYS_LITTLE_ENDIAN


/*------------------------------------------------------------------------------
**
** Defines for how the Operation Mode pins are assigned. Two options are available:
** 1). Define the value.
** 2). Define a macro for reading the physical value.
**
**------------------------------------------------------------------------------
*/

#define ABCC_GET_OPERATION_MODE_OM0        1
#define ABCC_GET_OPERATION_MODE_OM1        1
#define ABCC_GET_OPERATION_MODE_OM2        0


/*------------------------------------------------------------------------------
**
** The ABCC watchdog timeout time in ms.
** Please note that it is possible to use a watchdog time out time that is
** much smaller than 5000 ms. Refer to "Anybus-CompactCom Software Design Guide"
** for more information.
**
**------------------------------------------------------------------------------
*/

#define ABCC_WATCHDOG_TIMEOUT_TIME_MS      5000


/*------------------------------------------------------------------------------
**
** The initial handshake delay time that the ABCC-Driver will wait before trying to
** communicate with the ABCC module after reset. Normally this value does not
** need to be changed.
**
**------------------------------------------------------------------------------
*/

#ifdef ABCC_ACCESS_MODE_PARALLEL
   #define ABCC_INTIAL_HANDSHAKE_DELAY_TIME_MS 1500
#endif
#ifdef ABCC_ACCESS_MODE_SERIAL
   #define ABCC_INTIAL_HANDSHAKE_DELAY_TIME_MS 1500
#endif


/*------------------------------------------------------------------------------
**
** Define for how many commands (unresponded) the ABCC module can send to the
** Application at a time.
**
** The protocol between the application and the ABCC allows several simultaneous
** requests. With this define the application defines how many commands the
** ABCC module can send to the application without receiving any responses.
**
** Note: The driver will allocate buffer space for all messages that can be
** sent/received. This means that more memory will be used if this define is set
** to a higher value. Set to "1" to save memory.
**
**------------------------------------------------------------------------------
*/

#define ABCC_NBR_SIMULTANEOUS_ABCC_COMMANDS       1


/*------------------------------------------------------------------------------
**
** Define for how many commands (unresponded) the APPLICATION can send to the
** ABCC module at a time.
**
** The protocol between the application and the ABCC allows several simultaneous
** requests. With this define the application defines how many commands the
** application can send to the ABCC module without receiving any responses.
**
** Note: The driver will allocate buffer space for all messages that can be
** sent/received. This means that more memory will be used if this define is set
** to a higher value. Set to "1" to save memory.
**
**------------------------------------------------------------------------------
*/

#define ABCC_NBR_SIMULTANEOUS_APPL_COMMANDS       1


/*------------------------------------------------------------------------------
**
** Defines for process data sizes (reduce to save memory).
**
** The driver allocates space for process data that is passed between the
** application and the ABCC module. If the process data size is limited
** (and known) these defines can be set to the known values to reduce memory usage.
** Normally these defines are set to maximum possible process data size.
**
**------------------------------------------------------------------------------
*/

#define ABCC_WRITE_PD_BUFFER_LENGTH              ABP_MAX_PROCESS_DATA
#define ABCC_READ_PD_BUFFER_LENGTH               ABP_MAX_PROCESS_DATA


/*------------------------------------------------------------------------------
**
** With this define it is configured wheter or not the driver shall compare the
** process data received from the ABCC module to the process data received last
** time. If the data is found to be new the Application is notified with call-
** back function "ABCC_CbfNewReadPd()".
**
**------------------------------------------------------------------------------
*/

#define ABCC_COMPARE_READ_PD


/*------------------------------------------------------------------------------
**
** With this define it is configured if the driver shall compare the data
** sent with function "ABCC_WriteProcessData()" to the data sent last time.
** If the application only calls "ABCC_WriteProcessData()" when the data is
** updated there is no need to compare the data (wasting performance).
**
**------------------------------------------------------------------------------
*/

#define ABCC_COMPARE_WRITE_PD


/*------------------------------------------------------------------------------
**
** With this define the function "ABCC_ReadProcessData()" is enabled.
** PLEASE NOTE that the call-back function "ABCC_CbfNewReadPd()" will still be
** available, no matter what.
** Disabling this function might increase performance of the driver the read
** process data is not compared (ABCC_COMPARE_READ_PD is disabled).
**
**------------------------------------------------------------------------------
*/

#define ABCC_ENABLE_READ_PD_FUNCTION


/*******************************************************************************
**
** Public Services Definitions
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** SYS_ClearMemBytes()
**
** Clear a number of bytes.
** This function can be modified to use performance enhancing platform specific
** instructions.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbClearPtr        - Pointer to array that shall be cleared.
**    iNbrOfBytes       - Nbr of bytes to clear.
**
** Outputs:
**          -
**
** Usage:
**    SYS_ClearMemBytes( &MyArray[ 0 ], 10 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void SYS_ClearMemBytes( UINT8* pbClearPtr, UINT16 iNbrOfBytes );


/*------------------------------------------------------------------------------
**
** SYS_CopyMemBytes()
**
** Copy a number of bytes, from the source pointer to the destination pointer.
** This function can be modified to use performance enhancing platform specific
** instructions.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbDestinationPtr     - Pointer to the destination.
**    pbSourcePtr          - Pointer to source data.
**    iNbrOfBytes          - The number of bytes that shall be copied.
**
** Outputs:
**    None.                -
**
** Usage:
**    SYS_CopyMemBytes( pbMyDestination, pbMySource, 20 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void SYS_CopyMemBytes( UINT8* pbDestinationPtr,
                               const UINT8* pbSourcePtr,
                               UINT16 iNbrOfBytes );


/*------------------------------------------------------------------------------
**
** SYS_CompareMemBytes()
**
** Compare a number of bytes from two "arrays". If they are found to differ
** the function immediately returns.
** This function can be modified to use performance enhancing platform specific
** instructions.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbComparePtr1     - Pointer to first data buffer.
**    pbComparePtr2     - Pointer to second data buffer.
**    iNbrOfBytes       - The number of bytes to compare.
**
** Outputs:
**    returns           - NULL      - The two buffers doesn't differ.
**                        != NULL   - Pointer to first byte that differs (of
**                                    first buffer).
**
** Usage:
**    if( SYS_CompareMemBytes( &abBuffer1[ 0 ], &abBuffer2[ 0 ], 10 ) == NULL )
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT8* SYS_CompareMemBytes( UINT8* pbComparePtr1,
                                    UINT8* pbComparePtr2,
                                    UINT16 iNbrOfBytes );


#ifdef ABCC_ACCESS_MODE_SERIAL
/*------------------------------------------------------------------------------
**
** SYS_SendSerialTelegram()
**
** The APPLICATION shall send the data that is passed as argument to this
** function.
** THIS FUNCTION MUST BE ADAPTED TO THE APPLICATION HARDWARE.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbDataToSend         - Pointer to data that shall be sent.
**    iLength              - Length in bytes to send.
**
** Outputs:
**          -
**
** Usage:
**    SYS_SendSerialTelegram( &abSendBuffer[ 0 ], 24 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void SYS_SendSerialTelegram( UINT8* pbDataToSend, UINT16 iLength );


/*------------------------------------------------------------------------------
**
** SYS_ReStartSerialComm()
**
** This function is called when a complete re-start of the serial communication
** is required. This includes clearing DMA buffers etc.
** THIS FUNCTION MUST BE ADAPTED TO THE APPLICATIONS HARDWARE.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iReceptionLength        - The number of bytes that shall be expected.
**                              Note: The driver will restart the communication
**                              when the reception length needs to be changed
**                              (e.g., when I/O is added).
**
** Outputs:
**          -
**
** Usage:
**    SYS_ReStartSerialComm( 42 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void SYS_ReStartSerialComm( UINT16 iReceptionLength );
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


#ifdef ABCC_ACCESS_MODE_PARALLEL
/*------------------------------------------------------------------------------
**
** SYS_ReadParallel()
**
** This function copies a chunk of data from the DPRAM to the destination
** pointer.
** THIS FUNCTION MUST BE ADAPTED TO THE APPLICATIONS HARDWARE.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbDestination        - Pointer where to put the data.
**    iDpramOffset         - Offset from the beginning (lowest address) of the
**                           DPRAM.
**    iLength              - Length in bytes.
**
** Outputs:
**          -
**
** Usage:
**    SYS_ReadParallel( &abCopyOfDPRAM[ 0 ], 0x500, 120 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void SYS_ReadParallel( UINT8* pbDestination,
                               UINT16 iDpramOffset,
                               UINT16 iLength );


/*------------------------------------------------------------------------------
**
** SYS_WriteParallel()
**
** This function copies a chunk of data to the DPRAM from the source pointer.
** THIS FUNCTION MUST BE ADAPTED TO THE APPLICATIONS HARDWARE.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbSource             - Pointer to data to write.
**    iDpramOffset         - Offset from the beginning (lowest address) of the
**                           DPRAM.
**    iLength              - Length in bytes.
**
** Outputs:
**          -
**
** Usage:
**    SYS_WriteParallel( &abMessage[ 0 ], 0x300, 210 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void SYS_WriteParallel( UINT8* pbSource,
                                UINT16 iDpramOffset,
                                UINT16 iLength );


/*------------------------------------------------------------------------------
**
** SYS_VerifyReadParallel()
**
** This function shall read the specified DPRAM position until two consecutive
** readings agree.
** THIS FUNCTION CAN BE ADAPTED TO THE APPLICATIONS HARDWARE.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iDpramOffset         - Offset from the beginning (lowest address) of the
**                           DPRAM.
**
** Outputs:
**    returns              - The data byte read.
**
** Usage:
**    bMyByte = SYS_VerifyReadParallel( APDR_DPRAM_STAT_REG_OFFSET );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT8 SYS_VerifyReadParallel( UINT16 iDpramOffset );
#endif /* End #ifdef ABCC_ACCESS_MODE_PARALLEL */


/*---------------------------------------------------------------------------
**
** SYS_UseCritical()
**
** If any preparation is needed before calling "SYS_EnterCritical()" or
** "SYS_ExitCritical()" this macro is used to add HW specific necesities.
**
** PORTING ALERT!
** Add hardware specific implementation.
**---------------------------------------------------------------------------
*/

#define SYS_UseCritical()


/*---------------------------------------------------------------------------
**
** SYS_EnterCritical()
**
** Disables all interrupts, if they are not already disabled.
**
** PORTING ALERT!
** Add hardware specific implementation.
**---------------------------------------------------------------------------
*/

#define SYS_EnterCritical()


/*---------------------------------------------------------------------------
**
** SYS_ExitCritical()
**
** Restore interrupts to the state they were in when "SYS_EnterCritical()"
** was called.
**
** PORTING ALERT!
** Add hardware specific implementation.
**---------------------------------------------------------------------------
*/

#define SYS_ExitCritical()


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of sys_adpt.h
**
********************************************************************************
*/
