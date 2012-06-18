/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** sys_adpt.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Source file for system related functions. These functions might be adapted
** for the target system for optimal performance.
**
********************************************************************************
********************************************************************************
**
** Porting instructions:
** ---------------------
**
** The following functions must be ported:
**    SYS_SendSerialTelegram()   - In serial mode.
**    SYS_ReStartSerialComm()    - In serial mode.
**    SYS_ReadParallel()         - In parallel mode (might not be needed).
**    SYS_WriteParallel()        - In parallel mode (might not be needed).
**
** The other functions might be adapted for better performance, but it is not
** necessary.
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
#include "serial.h"
#include "abcc_td.h"
#include "abcc_com.h"
#include "sys_adpt.h"
#include "abcc.h"


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** SYS_ClearMemBytes()
**------------------------------------------------------------------------------
*/

void SYS_ClearMemBytes( UINT8* pbClearPtr, UINT16 iNbrOfBytes )
{
   UINT16   i;


   /*
   ** This function will clear iNbrOfBytes in the pbClearPtr.
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform for higher performance.
   */

   for( i = 0; i < iNbrOfBytes; i++ )
   {
      *pbClearPtr++ = 0x00;
   } /* End for loop */

} /* End of SYS_ClearMemBytes() */


/*------------------------------------------------------------------------------
** SYS_CopyMemBytes()
**------------------------------------------------------------------------------
*/

void SYS_CopyMemBytes( UINT8* pbDestinationPtr,
                       const UINT8* pbSourcePtr,
                       UINT16 iNbrOfBytes )
{
   UINT16   i;


   /*
   ** This function will copy iNbrOfBytes from pbSourcePtr to pbDestinationPtr.
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform for higher performance.
   */

   for( i = 0; i < iNbrOfBytes; i++ )
   {
      *pbDestinationPtr++ = *pbSourcePtr++;
   } /* End for loop */

} /* End of SYS_CopyMemBytes() */


/*------------------------------------------------------------------------------
** SYS_CompareMemBytes()
**------------------------------------------------------------------------------
*/

UINT8* SYS_CompareMemBytes( UINT8* pbComparePtr1,
                            UINT8* pbComparePtr2,
                            UINT16 iNbrOfBytes )
{
   UINT16   i;


   /*
   ** Compare the two strings with each other. If they are found to differ
   ** return a pointer to the first differing position.
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform for higher performance.
   */

   for( i = 0; i < iNbrOfBytes; i++ )
   {
      if( *pbComparePtr1 != *pbComparePtr2 )
      {
         /*
         ** The data of the two arrays differ.
         ** Return a pointer to the differing byte of the first pointer.
         */

         return( pbComparePtr1 );
      }
      else
      {
         /*
         ** Increase the pointers and compare next bytes.
         */

         pbComparePtr1++;
         pbComparePtr2++;

      } /* End if( Compare data ) */

   } /* End for( compare loop ) */


   return( NULL );

} /* End of SYS_CompareMemBytes() */


#ifdef ABCC_ACCESS_MODE_SERIAL
/*------------------------------------------------------------------------------
** SYS_SendSerialTelegram()
**------------------------------------------------------------------------------
*/

struct TSerialDevice* anybus_usart = NULL;
//anybus
static TSerialDesc anybus_desc={
	.baudrate=B115200,
	.mode=SERIAL_RX_INT_MODE |SERIAL_TX_INT_MODE,
};

void SYS_SendSerialTelegram( UINT8* pbDataToSend, UINT16 iLength )
{
   /*
   ** This function shall send the requested data using the serial channel.
   ** The number of bytes that shall be sent is "iLength".
   ** We recommend that the data to transmitt is sent with a DMA mechanism. The
   ** pointer to the data (pbDataToSend) will still be valid when this function
   ** returns.
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform.
   */

   	if(anybus_usart){
		usart_sendstring(anybus_usart,pbDataToSend,iLength);
	}
} /* End of SYS_SendSerialTelegram() */


/*------------------------------------------------------------------------------
** SYS_ReStartSerialComm()
**------------------------------------------------------------------------------
*/
u16 anybus_recv_len = 5;
void SYS_ReStartSerialComm( UINT16 iReceptionLength )
{
   /*
   ** The serial communication shall be restarted (i.e., transmission shall be
   ** stopped, reception shall be cleared and reception length shall be set to
   ** iReceptionLength). After these actions are done reception can be started
   ** again.
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform.
   */
   
   	printf("SYS_ReStartSerialComm len=%d\n",iReceptionLength);
   	anybus_recv_len = iReceptionLength;
	anybus_usart= usart_request("ttystm2");
	if(anybus_usart){
		if(!usart_open(anybus_usart,&anybus_desc)){
			printf("ttystm2 open failed\n");
		}
	}

} /* End of SYS_ReStartSerialComm() */
#endif /* End #ifdef ABCC_ACCESS_MODE_SERIAL */


#ifdef ABCC_ACCESS_MODE_PARALLEL
/*------------------------------------------------------------------------------
** SYS_ReadParallel()
**------------------------------------------------------------------------------
*/

void SYS_ReadParallel( UINT8* pbDestination,
                       UINT16 iDpramOffset,
                       UINT16 iLength )
{
#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   UINT16            iBytes;
   volatile UINT8*   pabDpram;
#endif /* End #ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR */


   /*
   ** This function shall read a bunch of data from the DPRAM. The requested
   ** data shall be copied from the DPRAM to the "pbDestination"-pointer.
   ** Source is DPRAM address "ABCC_PARALLEL_BASE_ADDRESS" + "iDpramOffset"
   ** and the number of bytes are "iLength".
   */

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   /*
   ** Example implementation.
   **
   ** PORTING ALERT!
   ** Adapt this function for better performance.
   */

   pabDpram = (UINT8*)( ABCC_PARALLEL_BASE_ADDRESS + iDpramOffset );

   for( iBytes = 0; iBytes < iLength; iBytes++ )
   {
      *pbDestination++ = *pabDpram++;
   }
#else
   /*
   ** The DPRAM is not directly accessible through a pointer.
   ** PORTING ALERT!
   ** Adapt this function to your platform.
   */

#endif /* End #ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR */

} /* End of SYS_ReadParallel() */


/*------------------------------------------------------------------------------
** SYS_WriteParallel()
**------------------------------------------------------------------------------
*/

void SYS_WriteParallel( UINT8* pbSource,
                     UINT16 iDpramOffset,
                     UINT16 iLength )
{
#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   UINT16      iBytes;
   UINT8*      pabDpram;
#endif /* End #ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR */


   /*
   ** This function shall write a bunch of data to the DPRAM. The data shall be
   ** copied from "pbSource" to DPRAM address "ABCC_PARALLEL_BASE_ADDRESS" +
   ** "iDpramOffset" and the number of bytes are "iLength".
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform.
   */

#ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR
   /*
   ** Example implementation.
   **
   ** PORTING ALERT!
   ** Adapt this function for better performance.
   */

   pabDpram = (UINT8*)( ABCC_PARALLEL_BASE_ADDRESS + iDpramOffset );

   for( iBytes = 0; iBytes < iLength; iBytes++ )
   {
      *pabDpram++ = *pbSource++;
   }
#else
   /*
   ** The DPRAM is not directly accessible through a pointer.
   **
   ** PORTING ALERT!
   ** Adapt this function to your platform.
   */

#endif /* End #ifdef ABCC_PARALLEL_DIRECT_ACCESS_THROUGH_PTR */

} /* End of SYS_WriteParallel() */


/*------------------------------------------------------------------------------
** SYS_VerifyReadParallel()
**------------------------------------------------------------------------------
*/

UINT8 SYS_VerifyReadParallel( UINT16 iDpramOffset )
{
   volatile UINT8    bFirstRead;
   volatile UINT8    bSecondRead;


   /*
   ** This function reads the DPRAM memory until two consecutive reads
   ** generate the same value.
   **
   ** PORTING ALERT!
   ** This function might be adapted to the hardware for better performance.
   */

   do
   {
      SYS_ReadParallel( (UINT8*)&bFirstRead, iDpramOffset, 1 );
      SYS_ReadParallel( (UINT8*)&bSecondRead, iDpramOffset, 1 );
   }
   while( bFirstRead != bSecondRead );


   return( bFirstRead );

} /* End of SYS_VerifyReadParallel() */
#endif /* End #ifdef ABCC_ACCESS_MODE_PARALLEL */

/*******************************************************************************
**
** End of sys_adpt.c
**
********************************************************************************
*/
