/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** abcc_com.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Includes definitions of structures/enumerations and definitions that are
** used by both application and driver.
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

#ifndef ABCC_COM_H
#define ABCC_COM_H


#include "abp.h"


/*******************************************************************************
**
** Data Types
**
********************************************************************************
*/


/*------------------------------------------------------------------------------
**
** ABCC_PdAdiMapType
**
**------------------------------------------------------------------------------
*/

typedef struct ABCC_PdAdiMapType
{
   UINT16         iAdiNbr;       /* ADI number - set by the application       */
   UINT8          bDataType;     /* Data type - set by the application        */
   UINT8          bNbrElements;  /* Number of elements - set by the appl.     */
   UINT16         iOrderNumber;  /* Order number of instance - set by the appl*/

   UINT8          bAreaOffset;   /* ADI's data area offset in bytes, starting */
                                 /* from offset zero. Set by ABCC driver.     */
   UINT8          bMapStatus;    /* 0 = Driver failed to map ADI, 1 = Map OK. */
                                 /* Set by the Driver.                        */
}
ABCC_PdAdiMapType;


/*------------------------------------------------------------------------------
**
** ABCC_PdMapType
**
**------------------------------------------------------------------------------
*/

typedef struct ABCC_PdMapType
{
   UINT16            iNbrMaps;      /* Number of ADI Write/Read mappings   */
                                    /* present in asMaps[] array.          */
   ABCC_PdAdiMapType*  psMaps;      /* Pointer to Array of ADI mappings    */

}
ABCC_PdMapType;


/*------------------------------------------------------------------------------
**
** Enumeration of the different severity-codes of an error message.
**
**------------------------------------------------------------------------------
*/

typedef enum ABCC_SeverityType
{
   /*
   ** Information about an event that has occured (e.g., serial message lost).
   */

   ABCC_SEV_INFORMATION              = 0,


   /*
   ** An error of minor importance has occured. The system can recover from this
   ** error.
   */

   ABCC_SEV_WARNING,


   /*
   ** A fatal event has occured, the system cannot recover (e.g., driver is out
   ** of timers).
   */

   ABCC_SEV_FATAL

}
ABCC_SeverityType;


/*------------------------------------------------------------------------------
**
** Enumeration of the different error codes of an error message.
**
**------------------------------------------------------------------------------
*/

typedef enum ABCC_ErrorCodeType
{
   /*
   ** Error codes indicating that the ABCC driver is out of resources.
   */

   ABCC_EC_OUT_OF_QUEUES,
   ABCC_EC_OUT_OF_BUFFERS,
   ABCC_EC_OUT_OF_TIMERS,
   ABCC_EC_TIMER_SYSTEM_MALFUNCTION,
   ABCC_EC_RESOURCE_MISSING,
   ABCC_EC_QUEUE_SELECTION_ERROR,


   /*
   ** Error code indicating that the operating mode selected is not correct.
   */

   ABCC_EC_INCORRECT_OPERATING_MODE,


   /*
   ** Error codes indicating serial fragmentation errors.
   */

   ABCC_EC_SERIAL_FRAG_ERROR,


   /*
   ** Parallel related error codes.
   */

   ABCC_EC_FALSE_PARALLEL_INT_GENERATED,
   ABCC_EC_INCORRECT_INIT_HS_STATE,


   /*
   ** Driver state-machine related issues.
   */

   ABCC_EC_UNKNOWN_STATE_ENTERED,
   ABCC_EC_STATE_TRANSITION_NOT_ALLOWED,


   /*
   ** Configuration state-machine related issues.
   */

   ABCC_EC_INVALID_MODULE_TYPE_RECEIVED,
   ABCC_EC_INVALID_NETWORK_TYPE_RECEIVED,
   ABCC_EC_INVALID_PARAM_SUPPORT_RECEIVED,
   ABCC_EC_INVALID_DATA_FORMAT_RECEIVED,
   ABCC_EC_ERROR_IN_READ_MAP_CONFIG,
   ABCC_EC_ERROR_IN_WRITE_MAP_CONFIG,
   ABCC_EC_ILLEGAL_CFG_STATE

}
ABCC_ErrorCodeType;


/*------------------------------------------------------------------------------
**
** ABCC_StatusType
**
**------------------------------------------------------------------------------
*/

typedef enum ABCC_StatusType
{
   ABCC_STATUS_OK,
   ABCC_STATUS_NOT_ALLOWED_DRIVER_STATE,
   ABCC_STATUS_OUT_OF_RESOURCES

}
ABCC_StatusType;


/*------------------------------------------------------------------------------
**
** ABCC_DataFormatType
**
**------------------------------------------------------------------------------
*/

typedef enum ABCC_DataFormatType
{
   ABCC_DF_LSB_FIRST       = 0,
   ABCC_DF_MSB_FIRST       = 1
}
ABCC_DataFormatType;

#endif  /* inclusion lock */

/*******************************************************************************
**
** End of abcc_com.h
**
********************************************************************************
*/
