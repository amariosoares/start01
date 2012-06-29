/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** abcc.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public interface to the ABCC-driver. This is the only header-file
** that needs to be included by the application.
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
**   ABCC_ReadProcessData()         - Read process data from the ABCC-module.
**   ABCC_RunDriver()               - Main routine for the ABCC-driver.
**   ABCC_RunTimerSystem()          - This function runs the timers of the ABCC
**                                    driver.
**   ABCC_SendMessage()             - Send a new message to the ABCC-module.
**   ABCC_ShutdownDriver()          - Shutdown the ABCC-driver.
**   ABCC_StartDriver()             - Start the ABCC-driver.
**   ABCC_WriteProcessData()        - Write process data to the ABCC-module.
**
** Callback functions:
**   ABCC_CbfAutoCfgRequest()       - Request pointers to mapping configuration.
**   ABCC_CbfCfgCompleted()         - Signals that the configuration.
**   ABCC_CbfDebugInfo()            - Displays debug information.
**   ABCC_CbfDriverError()          - Signals that there's an error in the driver.
**   ABCC_CbfEnableInterruptParallel()- Clear and Enable parallel interrupts.
**   ABCC_CbfNetworkDataFormat()    - Indicate the data-format of network.
**   ABCC_CbfNewMsgFromAbcc()       - A new message is received from the ABCC.
**   ABCC_CbfNewReadPd()            - Signals that new read process data/status
**                                    is available.
**   ABCC_CbfRequestPdSize()        - Request the PD size from the application.
**   ABCC_CbfTelegramReceived()     - Telegram received.
**   ABCC_CbfTelegramTransmitted()  - Telegram transmitted.
**   ABCC_CbfWdTimeout()            - Signals that communication with ABCC is lost.
**   ABCC_CbfWdTimeoutRecovered()   - Signals that communication is restored.
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

#ifndef ABCC_H
#define ABCC_H


/*******************************************************************************
**
** Includes
**
********************************************************************************
*/

#include "abcc_td.h"
#include "abcc_com.h"
#include "abp.h"
#include "sys_adpt.h"
#include "util.h"
#include "aplink.h"
#include "apdr.h"


/*******************************************************************************
**
** Defines
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Defines for the ABCC driver version.
**
**------------------------------------------------------------------------------
*/

#define ABCC_DRIVER_VERSION            "V.2.02.01"

/*******************************************************************************
**
** Public globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_eAbccState
**------------------------------------------------------------------------------
*/


EXTFUNC ABP_AnbStateType        ABCC_eAbccState;


/*******************************************************************************
**
** Public Services Definitions
**
********************************************************************************
*/

#ifdef ABCC_ENABLE_READ_PD_FUNCTION
/*------------------------------------------------------------------------------
**
** ABCC_ReadProcessData()
** This function will copy data from the ABCC module to the application. The
** length of the data is set during configuration.
**
**------------------------------------------------------------------------------
**
** Inputs:
**   pbData     - Pointer to the start of data.
**
** Outputs:
**   None       -
**
** Usage:
**    ABCC_ReadProcessData( &pbMyReadData[ 0 ] );
**
**------------------------------------------------------------------------------
*/

#define ABCC_ReadProcessData( pbData )    APLINK_ReadProcessData( pbData )
#endif


/*------------------------------------------------------------------------------
**
** ABCC_RunDriver()
**
** This is the MAIN function of the ABCC-driver. This function is responsible
** for running everything but the timers. The application calls this function on
** a regular basis.
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
**    ABCC_RunDriver();
**
**------------------------------------------------------------------------------
*/

#define ABCC_RunDriver()      APLINK_RunStateMachine()


/*------------------------------------------------------------------------------
**
** ABCC_RunTimerSystem()
**
** This function is responsible for handling all timers of the ABCC-driver and
** MUST be called on a regular basis by the application. The application needs
** to specify how often this function is called with define
** "ABCC_TIMER_RESOLUTION_MS".
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
**    ABCC_RunTimerSystem();
**
**------------------------------------------------------------------------------
*/

#define ABCC_RunTimerSystem()    UTIL_RunTimerSystem( ABCC_TIMER_RESOLUTION_MS )


/*------------------------------------------------------------------------------
**
** ABCC_SendMessage()
**
** This function will send a message to the ABCC module.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psMsg       - Pointer to message to send.
**
** Outputs:
**    Returns     - Status of the request:
**                  ABCC_STATUS_OK                       - Request sent OK.
**                  ABCC_STATUS_NOT_ALLOWED_DRIVER_STATE - Module is in config.
**                                                         mode, appl cannot
**                                                         send messages.
**                  ABCC_STATUS_OUT_OF_RESOURCES         - Command buffer full.
**
** Usage:
**    ABCC_SendMessage( psMsg );
**
**------------------------------------------------------------------------------
*/
#define ABCC_SendMessage( xMsg )    APLINK_SendMessage( xMsg )


/*------------------------------------------------------------------------------
**
** ABCC_ShutdownDriver()
**
** This function will shutdown the driver.
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
**    ABCC_ShutdownDriver();
**
**------------------------------------------------------------------------------
*/

#define ABCC_ShutdownDriver()       APLINK_ChangeDriverState( APLINK_SM_IN_SHUTDOWN ); \
                                    APLINK_RunStateMachine()


/*------------------------------------------------------------------------------
**
** ABCC_StartDriver()
**
** This function will start the driver.
** NOTE: This function can only be called directly after a hardware reset of
** the ABCC module.
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
**    ABCC_StartDriver();
**
**------------------------------------------------------------------------------
*/

#define ABCC_StartDriver()    APLINK_ChangeDriverState( APLINK_SM_SETUP )


/*------------------------------------------------------------------------------
**
** ABCC_WriteProcessData()
**
** This function will copy data from the application to the ABCC module. The
** length of the data is set during configuration.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pbData   - Pointer to start of data.
**
** Outputs:
**             -
**
** Usage:
**    ABCC_WriteProcessData( &abMyWriteProcessData[ 0 ] );
**
**------------------------------------------------------------------------------
*/

#define ABCC_WriteProcessData( pbData )  APLINK_WriteProcessData( pbData )


/*------------------------------------------------------------------------------
**
** ABCC_CbfAutoCfgRequest()
**
** This function is called when the driver is about to start the automatic
** process data mapping. If no automatic configuration is desired the pointers
** are set to NULL, otherwise the pointers are set to point at the structures
** containing mapping information.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iModuleType          - Module type of the connected module (0x0401 == ABCC).
**    iNetworkType         - The kind of network module connected (e.g.,
**                           Profibus, DeviceNet etc).
**    fParameterSupport    - Flag that indicates if the connected network
**                           supports parameter data.
**    ppsReadMappings      - Pointer to requested configuration structure pointer.
**    ppsWriteMappings     - Pointer to requested configuration structure pointer.
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfAutoCfgRequest()
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfAutoCfgRequest( UINT16 iModuleType,
                                     UINT16 iNetworkType,
                                     BOOL8 fParameterSupport,
                                     ABCC_PdMapType** ppsReadMappings,
                                     ABCC_PdMapType** ppsWriteMappings );


/*------------------------------------------------------------------------------
**
** ABCC_CbfCfgCompleted()
**
** The driver has now completed the process data configuration. If the
** application is content with the configuration it is now time for the
** application to set the “Setup complete” attribute.
** After that the application can start to communicate with the ABCC module.
**
**------------------------------------------------------------------------------
**
** Inputs:
**     None     -
**
** Outputs:
**      None    -
**
** Usage:
**    ABCC_CbfCfgCompleted();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfCfgCompleted( void );


#ifdef ABCC_DRIVER_DEBUG
/*------------------------------------------------------------------------------
**
** ABCC_CbfDebugInfo()
**
** Log some debug information.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pabText        - Pointer to string of characters (NULL terminated).
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfDebugInfo( "Some debug information" );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfDebugInfo( UINT8* pabText );
#else
#define ABCC_CbfDebugInfo( pabText )
#endif


/*------------------------------------------------------------------------------
**
** ABCC_CbfDriverError()
**
** This is a call-back function that is called from the ABCC-driver in case
** an error occurs.
** THIS FUNCTION CAN BE ADAPTED TO THE APPLICATION.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    eSeverity         - Severity of the event.
**    eErrorCode        - Error code.
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfDriverError();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfDriverError( ABCC_SeverityType eSeverity,
                                  ABCC_ErrorCodeType eErrorCode );


#ifdef ABCC_ACCESS_MODE_PARALLEL
#ifndef ABCC_PARALLEL_POLL_MODE
/*------------------------------------------------------------------------------
**
** ABCC_CbfEnableInterruptParallel()
**
** 1. Clear any pending parallel interrupts.
** 2. Enable the interrupt.
** Until this function is called no (parallel) interrupts shall be enabled.
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
**    ABCC_CbfEnableInterruptParallel();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfEnableInterruptParallel( void );
#endif /* End #ifndef ABCC_PARALLEL_POLL_MODE */
#endif /* End #ifdef ABCC_ACCESS_MODE_PARALLEL */


/*------------------------------------------------------------------------------
**
** ABCC_CbfNetworkDataFormat()
**
** This function is called by the driver during start-up. The passed variable
** (bFormat) indicates the data-format of the ADI Value/Max value/Min value
** and default value.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    eFormat           - Format of data (Refer to ABCC_DataFormatType).
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbNetworkDataFormat( ABCC_DF_LSB_FIRST );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfNetworkDataFormat( ABCC_DataFormatType eFormat );


/*------------------------------------------------------------------------------
**
** ABCC_CbfNewMsgFromAbcc()
**
** A new message is received from the ABCC module.
** Please note that pointer is only valid within this function.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage         - Pointer to structure containig the new message.
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfNewMsgFromAbcc( &sMessage );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfNewMsgFromAbcc( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** ABCC_CbfNewReadPd()
**
** This function indicates that the driver has received new data from the ABCC
** module.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    bAbccStatus    - Updated (possibly) Abcc state and SUP-bit.
**                     Bit 0 - 2 - The current state of the ABCC-module
**                     Bit 3     - If the module is under supervision.
**                     Bit 4 - 7 - Not used.
**    fNewData       - Flag to indicate if the Process data is new.
**    iPdReadLength  - The length of the process data.
**    pabData        - Pointer to the process data.
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfNewReadPd( bAbccStatus, fNewData, iPdReadLength, pabData );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfNewReadPd( UINT8 bAbccStatus,
                                BOOL8 fNewData,
                                UINT16 iPdReadLength,
                                UINT8* pabData );


/*------------------------------------------------------------------------------
**
** ABCC_CbfRequestPdSize()
**
** This function will request the process data lengths from the application.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    piReadLength         - Pointer to variable setting the PD Read length.
**    piWriteLength        - Pointer to variable setting the PD Write length.
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfRequestPdSize( iReadlength, iWriteLength );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfRequestPdSize( UINT16* piReadLength, UINT16* piWriteLength );


/*------------------------------------------------------------------------------
**
** ABCC_CbfTelegramReceived()
**
** A Telegram (serial or parallel) is received from the ABCC module.
** This function is provided as status information. No action from the
** application is required.
** This callback function is called if "ABCC_TX_RX_CALLBACK_ENABLE" is defined.
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
**    ABCC_CbfTelegramReceived();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfTelegramReceived( void );


/*------------------------------------------------------------------------------
**
** ABCC_CbfTelegramTransmitted()
**
** A telegram (serial or parallel) is transmitted to the ABCC module.
** This function is provided as status information. No action from the
** application is required.
** This callback function is called if "ABCC_TX_RX_CALLBACK_ENABLE" is defined.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    fReTransmission         - Set to "TRUE" if re-transmission.
**
** Outputs:
**          -
**
** Usage:
**    ABCC_CbfTelegramTransmitted( FALSE );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfTelegramTransmitted( BOOL8 fReTransmission );


/*------------------------------------------------------------------------------
**
** ABCC_CbfWdTimeout()
**
** Signals that there has been no communication with the ABCC module for at
** least the predefined time-out value.
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
**    ABCC_CbfWdTimeout();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfWdTimeout( void );


/*------------------------------------------------------------------------------
**
** ABCC_CbfWdTimeoutRecovered()
**
** This function signals that we have previously had a ABCC watchdog timeout,
** but now the communication works again.
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
**    ABCC_CbfWdTimeoutRecovered();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void ABCC_CbfWdTimeoutRecovered( void );

#endif  /* inclusion lock */

/*******************************************************************************
**
** End of abcc.h
**
********************************************************************************
*/
