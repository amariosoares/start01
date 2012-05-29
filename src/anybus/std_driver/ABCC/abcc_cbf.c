/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** abcc_cbf.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
** This file contains the functions that are called BY THE DRIVER and are meant
** for the application. The application is free to fill the functions of this
** file with application specific code.
**
********************************************************************************
********************************************************************************
**
** Porting instructions:
** ---------------------
**
** Porting required.
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
#include "abcc.h"


/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/

/*
** Globals used for demonstration purpose only.
*/

ABCC_PdMapType      sMyReadMappings;
ABCC_PdMapType      sMyWriteMappings;
ABCC_PdAdiMapType   asReadAdi[ 2 ];
ABCC_PdAdiMapType   asWriteAdi[ 2 ];


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** ABCC_CbfAutoCfgRequest()
**------------------------------------------------------------------------------
*/

void ABCC_CbfAutoCfgRequest( UINT16 iModuleType,
                             UINT16 iNetworkType,
                             BOOL8 fParameterSupport,
                             ABCC_PdMapType** ppsReadMappings,
                             ABCC_PdMapType** ppsWriteMappings )
{
   /*
   ** This function is called when the driver is about to start the automatic
   ** process data mapping. If no automatic configuration is desired the pointers
   ** are set to NULL, otherwise the are set to point at the structures containing
   ** mapping information.
   **
   ** Refer to document "SDS-7004-003 ID and type number.xls" for a complete
   ** list of module types. The ABCC module is identified as 0x0401. If the
   ** connected module is not an ABCC-module it is recommended that the stack
   ** is shutdown. Read from the ABCC object, instance 1 attribute 1.
   **
   ** If the user wants the possibility to set different mappings depending on
   ** fieldbus/module type the iNetworkType variable can be used as a switch.
   ** Read from the Network object, Instance 1, Attribute 1.
   **
   ** Via the fParameterSupport flag it is possible to determine if the
   ** connected network module supports parameter data.
   ** Read from the Network object, Instance 1, Attribute 4.
   **
   ** For more information please refer to the Anybus-CompactCom Software Design Guide.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */


   /*
   ** Example of application that wants no automatic process data configuration.
   */

/*
   *ppsReadMappings   = NULL;
   *ppsWriteMappings  = NULL;
*/


   /*
   ** Example of application that wants automatic configuration.
   **
   ** Set-up the two ARRAY's of structures: asReadAdi and asWriteAdi with
   ** the corresponding ADI's information.
   **
   ** ABCC_PdAdiMapType   asReadAdi[ 2 ];
   ** ABCC_PdAdiMapType   asWriteAdi[ 2 ];
   **
   ** Attach them to the global structures holding the mapping informations.
   ** ABCC_PdMapType MyReadMapping;
   ** ABCC_PdMapType MyWriteMapping;
   **
   ** ...set the argument pointer to point to these structures
   **
   ** *ppsReadMappings  = &MyReadMapping;
   ** *ppsWriteMappings = &MyWriteMapping;
   */

   asReadAdi[ 0 ].iAdiNbr         = (UINT16)3;
   asReadAdi[ 0 ].bDataType       = (UINT8)ABP_SINT16;
   asReadAdi[ 0 ].bNbrElements    = (UINT8)1;
   asReadAdi[ 0 ].iOrderNumber    = (UINT16)1;

   asReadAdi[ 1 ].iAdiNbr         = (UINT16)200;
   asReadAdi[ 1 ].bDataType       = (UINT8)ABP_UINT32;
   asReadAdi[ 1 ].bNbrElements    = (UINT8)1;
   asReadAdi[ 1 ].iOrderNumber    = (UINT16)4;

   sMyReadMappings.psMaps    = asReadAdi;
   sMyReadMappings.iNbrMaps  = (UINT16)2;


   asWriteAdi[ 0 ].iAdiNbr         = (UINT16)4;
   asWriteAdi[ 0 ].bDataType       = (UINT8)ABP_SINT32;
   asWriteAdi[ 0 ].bNbrElements    = (UINT8)1;
   asWriteAdi[ 0 ].iOrderNumber    = (UINT16)2;

   asWriteAdi[ 1 ].iAdiNbr         = (UINT16)24;
   asWriteAdi[ 1 ].bDataType       = (UINT8)ABP_UINT8;
   asWriteAdi[ 1 ].bNbrElements    = (UINT8)1;
   asWriteAdi[ 1 ].iOrderNumber    = (UINT16)3;

   sMyWriteMappings.psMaps    = asWriteAdi;
   sMyWriteMappings.iNbrMaps  = (UINT16)2;


   *ppsReadMappings   = &sMyReadMappings;
   *ppsWriteMappings  = &sMyWriteMappings;

} /* End of ABCC_CbfAutoCfgRequest() */


/*------------------------------------------------------------------------------
** ABCC_CbfCfgCompleted()
**------------------------------------------------------------------------------
*/

void ABCC_CbfCfgCompleted( void )
{
   /*
   ** The driver has now completed the process data configuration. If the
   ** application is content with the configuration it is now time for the
   ** application to set the "Setup complete" attribute.
   ** After that the application can start to communicate with the ABCC module.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfCfgCompleted() */


#ifdef ABCC_DRIVER_DEBUG
/*------------------------------------------------------------------------------
** ABCC_CbfDebugInfo()
**------------------------------------------------------------------------------
*/
#include <stdio.h>
void ABCC_CbfDebugInfo( UINT8* pabText )
{
   /*
   ** Function enabling an application specific DEBUG channel.
   ** This function might be very useful when starting to implement the driver.
   ** When the implementation is completed this functionality should be
   ** disabled by commenting away "#define ABCC_DRIVER_DEBUG" in file
   ** "sys_adpt.h".
   ** This debug information can be sent to, for example, to a serial channel,
   ** a telnet terminal or a file.
   */

   /*
   ** PORTING ALERT!
   */

   /* e.g., printf( "%s \r\n", pabText ); */

   /* ADD HANDLING OF EVENT */
	printf( "%s \r\n", pabText );

} /* End of ABCC_CbfDebugInfo() */
#endif


/*------------------------------------------------------------------------------
** ABCC_CbfDriverError()
**------------------------------------------------------------------------------
*/

void ABCC_CbfDriverError( ABCC_SeverityType eSeverity,
                          ABCC_ErrorCodeType eErrorCode )
{
   /*
   ** An error has occured in the driver.
   */

   /*
   ** PORTING ALERT!
   */

#ifdef ABCC_DRIVER_DEBUG
   ABCC_CbfDebugInfo( "ABCC_CbfDriverError() encountered!" );
#endif

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfDriverError() */


#ifdef ABCC_ACCESS_MODE_PARALLEL
#ifndef ABCC_PARALLEL_POLL_MODE
/*------------------------------------------------------------------------------
** ABCC_CbfEnableInterruptParallel()
**------------------------------------------------------------------------------
*/

void ABCC_CbfEnableInterruptParallel( void )
{
   /*
   ** Clear any pending parallel interrupts. When cleared the interrupts shall
   ** be turned on. Until this function is called no (parallel) interrupts shall
   ** be enabled.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfEnableInterruptParallel() */
#endif /* End #ifdef ABCC_PARALLEL_POLL_MODE */
#endif /* End #ifndef ABCC_ACCESS_MODE_PARALLEL  */


/*------------------------------------------------------------------------------
** ABCC_CbfNetworkDataFormat()
**------------------------------------------------------------------------------
*/

void ABCC_CbfNetworkDataFormat( ABCC_DataFormatType eFormat )
{
   /*
   ** This function is called by the driver during start-up. The passed variable
   ** (bFormat) indicates the data-format of the ADI Value/Max value/Min value
   ** and default value.
   ** If bFormat is set to "0" theses values are sent with LSB first (INTEL).
   ** If bFormat is set to "1" theses values are sent with MSB first (Motorola).
   ** The data-format is controlled by the network, the data is
   ** transparently through the ABCC module to/from the network.
   ** If the application has the same data-format as indicated with bFormat, no
   ** swapping is needed. However, if the formats differ the above mentioned
   ** data needs to be swapped. Ad this is also true for the process data.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfNetworkDataFormat() */


/*------------------------------------------------------------------------------
** ABCC_CbfNewMsgFromAbcc()
**------------------------------------------------------------------------------
*/

void ABCC_CbfNewMsgFromAbcc( ABP_MsgType* psNewMessage )
{
   /*
   ** A new message is received from the ABCC module.
   ** Please note that pointer is only valid within this function.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */


   /*
   ** Example code of how these messages can be handled.
   */

   if( ( psNewMessage->sHeader.bCmd & (UINT8)ABP_MSG_HEADER_C_BIT ) != (UINT8)0 )
   {
      /*
      ** This is a new command.
      */


      /*
      ** The application has got two options:
      ** 1). The application can generate an answer to this request directly
      **     and can send an answer directly to the ABCC module.
      **
      ** 2). The application cannot generate a quick response to this request.
      **     Therefore the request needs to be copied and passed to some
      **     "background" task that can handle the command request when resources
      **     are available.
      */


      /* Option 1. */
      /* Call some function that generates a response and return pointer to it*/
      /* Send the response to the driver with ABCC_SendMessage()              */
      /* Return this function                                                 */



      /* Option 2. */
      /* Allocate a chunk of memory.                                       */
      /* Copy the request from psNewMessage to the newly allocated memory  */
      /* Send the message (that were allocated) to some other resource.    */
      /* Return this function                                              */


      /*
      ** Example handling:
      ** Respond to the request with "Unsupported object". Of course this
      ** code shall be removed once an application specific implementation of
      ** this funcition is completed.
      */

      ABP_SetMsgErrorResponse( psNewMessage, 0, ABP_ERR_UNSUP_OBJ );
      ABCC_SendMessage( psNewMessage );

   }
   else
   {
      /*
      ** This message is a response to one of the commands that are sent from
      ** the application.
      ** Please note that if several Commands are sent from the application
      ** before the ABCC has answered the first, it cannot be guaranteed that
      ** the responses are sent from the ABCC in the order the commands are
      ** received. Therefore, it is vital that the "bSourceId" is used wisely,
      ** since it is a good way of telling what command the response message
      ** corresponds to.
      */

   } /* End if( Message == Command ) */

} /* End of ABCC_CbfNewMsgFromAbcc() */


/*------------------------------------------------------------------------------
** ABCC_CbfNewReadPd()
**------------------------------------------------------------------------------
*/

void ABCC_CbfNewReadPd( UINT8 bAbccStatus,
                        BOOL8 fNewData,
                        UINT16 iPdReadLength,
                        UINT8* pabData )
{
   /*
   ** This function indicates that the driver has received new data from the
   ** ABCC module or if the ABCC status is changed.
   ** Please note that the pointer to the data (pabData) is not valid when
   ** this function has returned.
   */

   /*
   ** PORTING ALERT!
   */

   if( fNewData )
   {
      /*
      ** New Process data received. Possibly the Status is also new.
      */

      /* ADD HANDLING OF EVENT */
   }
   else
   {
      /*
      ** The Status is updated. The process data is NOT updated.
      */

      /* ADD HANDLING OF EVENT */

   } /* End if( fNewData ) */

} /* End of ABCC_CbfNewReadPd() */


/*------------------------------------------------------------------------------
** ABCC_CbfRequestPdSize()
**------------------------------------------------------------------------------
*/

void ABCC_CbfRequestPdSize( UINT16* piReadLength, UINT16* piWriteLength )
{
   /*
   ** This function will request the process data lengths from the application.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */

   /*
   ** Application needs to fill in the correct Process Data READ length.
   */

   *piReadLength = (UINT16)6;


   /*
   ** Application needs to fill in the correct Process Data WRITE length.
   */

   *piWriteLength = (UINT16)5;

} /* End of ABCC_CbfRequestPdSize() */


/*------------------------------------------------------------------------------
** ABCC_CbfTelegramReceived()
**------------------------------------------------------------------------------
*/

void ABCC_CbfTelegramReceived( void )
{
   /*
   ** A telegram (serial or parallel) is received from the ABCC module.
   ** This function is provided as status information. No action from the
   ** application is required.
   ** This callback function is called if "ABCC_TX_RX_CALLBACK_ENABLE" is defined.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfTelegramReceived() */


/*------------------------------------------------------------------------------
** ABCC_CbfTelegramTransmitted()
**------------------------------------------------------------------------------
*/

void ABCC_CbfTelegramTransmitted( BOOL8 fReTransmission )
{
   /*
   ** A telegram (serial or parallel) is transmitted to the ABCC module.
   ** This function is provided as status information. No action from the
   ** application is required.
   ** This callback function is called if "ABCC_TX_RX_CALLBACK_ENABLE" is defined.
   */

   /*
   ** PORTING ALERT!
   */

} /* End of ABCC_CbfTelegramTransmitted() */


/*------------------------------------------------------------------------------
** ABCC_CbfWdTimeout()
**------------------------------------------------------------------------------
*/

void ABCC_CbfWdTimeout( void )
{
   /*
   ** We have (temporary?) lost the connection with the ABCC module. It is
   ** possible that the communication can be restored, but not likely.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfWdTimeout() */


/*------------------------------------------------------------------------------
** ABCC_CbfWdTimeoutRecovered()
**------------------------------------------------------------------------------
*/

void ABCC_CbfWdTimeoutRecovered( void )
{
   /*
   ** This function signals that we have previously had a ABCC watchdog timeout,
   ** but now the communication works again.
   */

   /*
   ** PORTING ALERT!
   */

   /* ADD HANDLING OF EVENT */

} /* End of ABCC_CbfWdTimeoutRecovered() */


/*******************************************************************************
**
** End of abcc_cbf.c
**
********************************************************************************
*/
