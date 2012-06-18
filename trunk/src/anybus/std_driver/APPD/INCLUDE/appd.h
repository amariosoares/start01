/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** appd.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public header file for the APPD object.
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    APPD_CbfNewReadProcessData()  - Must be called whenever new process data
**                                    is available from the driver.
**    APPD_CbfRequestPdSize()       - Retrieves the read and write process data
**                                    sizes.
**    APPD_MapAdiToPd()             - Map ADIs to process data.
**    APPD_ProcessCmdMsg()          - Processes commands sent to the APPD 
**                                    object.
**    APPD_ProcessDataChanged()     - Must be called whenever write process data
**                                    have changed and must be sent to the 
**                                    driver.
**    APPD_SetNetworkByteOrder()    - Must be called to let the APPD object 
**                                    know the network byte order.
**
** Private Services:
**
**    appd_Compare()                - Compare two values of any ABCC datatype,
**                                    where one is in network byte order,
**                                    and the other is in system byte order.
**    appd_InstanceCommand()        - Processes commands to Application Data 
**                                    Instances
**    appd_ObjectCommand()          - Processes commands to the Application 
**                                    Data Object (Instance 0).
**    appd_SizeOfAdiVal()           - Calculates the total size of a ADI value.
**    appd_SwapToNetOrder()         - Converts any ABCC datatype array between
**                                    system and network byte order.
**    appd_UpdateAdiVal()           - Updates the ADI value if the new value is 
**                                    within the allowed range of the ADI.
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

#ifndef APPD_H
#define APPD_H

/*******************************************************************************
**
** Constants
**
********************************************************************************
*/

#define APPD_MAX_DRIVEPROFILE_PARAMS 			   27
#define NUMBER_OF_INSTANCES                     5

#define  APPD_ADI_NBR_SPEED                     1
#define  APPD_ADI_NBR_TEMP                      2
#define  APPD_ADI_NBR_REF_SPEED                 3
#define  APPD_ADI_NBR_DIRECTION                 4
#define  APPD_ADI_NBR_TRIP_CURRENT              5


/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** appd_ObjectType
**
** Structure describing an Application Data Object.
**
**------------------------------------------------------------------------------
*/

typedef struct appd_ObjectType 
{
  char* acName;
  UINT8 bRevision;
  UINT16 iNumberOfInstances;
  UINT16 iHighestInstanceNo;
} 
appd_ObjectType;


/*------------------------------------------------------------------------------
**
** APPD_ADInstanceType
**
** Structure describing an Application Data Instance.
**
**------------------------------------------------------------------------------
*/

typedef struct appd_InstanceType 
{
  char* acName;
  UINT8 bDataType;
  UINT8 bNumberOfElements;
  UINT8 bDescriptor;
  void* pxValue;
  void* pxMaxValue;
  void* pxMinValue;
  void* pxDefaultValue;
  UINT16 iInstanceNumber;
} 
appd_InstanceType;


/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/



/* 
** instance 1 data values 
*/

EXTFUNC SINT16 APPD_iSpeed;
EXTFUNC SINT16 APPD_iMaxSpeed;
EXTFUNC SINT16 APPD_iMinSpeed;
EXTFUNC SINT16 APPD_iDefaultSpeed;

/* 
** instance 2 data values 
*/

EXTFUNC SINT16 APPD_iTemperature;
EXTFUNC SINT16 APPD_iMaxTemperature;
EXTFUNC SINT16 APPD_iMinTemperature;
EXTFUNC SINT16 APPD_iDefaultTemperature;


/*
** instance 3 data values 
*/

EXTFUNC UINT16 APPD_iRefSpeed;
EXTFUNC UINT16 APPD_iMaxRefSpeed;
EXTFUNC UINT16 APPD_iMinRefSpeed;
EXTFUNC UINT16 APPD_iDefaultRefSpeed;


/* 
** instance 4 data values 
*/

EXTFUNC BOOL8 APPD_fForward;
EXTFUNC BOOL8 APPD_fMaxForward;
EXTFUNC BOOL8 APPD_fMinForward;
EXTFUNC BOOL8 APPD_fDefaultForward;


/* 
** instance 5 data values 
*/

EXTFUNC UINT8 APPD_bPoles;
EXTFUNC UINT8 APPD_bMaxPoles;
EXTFUNC UINT8 APPD_bMinPoles;
EXTFUNC UINT8 APPD_bDefaultPoles;

/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** appd_asInstance
**
**------------------------------------------------------------------------------
**
** Array of structures representing the APPD instances (ADIs).
**
**------------------------------------------------------------------------------
*/

EXTFUNC const appd_InstanceType appd_asInstance[NUMBER_OF_INSTANCES];


/*------------------------------------------------------------------------------
**
** appd_asReadADI
**
**------------------------------------------------------------------------------
**
** Array of structures used for process data mapping.
**
**------------------------------------------------------------------------------
*/

EXTFUNC ABCC_PdAdiMapType   appd_asReadADI[ NUMBER_OF_INSTANCES ];

/*------------------------------------------------------------------------------
**
** appd_asWriteADI
**
**------------------------------------------------------------------------------
**
** Array of structures used for process data mapping.
**
**------------------------------------------------------------------------------
*/

EXTFUNC ABCC_PdAdiMapType   appd_asWriteADI[ NUMBER_OF_INSTANCES ];

/*------------------------------------------------------------------------------
**
** appd_sMyReadMappings
**
**------------------------------------------------------------------------------
**
** Structure containing read process data mapping information
**
**------------------------------------------------------------------------------
*/

extern ABCC_PdMapType      appd_sMyReadMappings;

/*------------------------------------------------------------------------------
**
** appd_sMyWriteMappings
**
**------------------------------------------------------------------------------
**
** Structure containing write process data mapping information
**
**------------------------------------------------------------------------------
*/

extern ABCC_PdMapType      appd_sMyWriteMappings;

/*------------------------------------------------------------------------------
**
** appd_sObject
**
**------------------------------------------------------------------------------
**
** Structure representing the APPD object (instance 0)
**
**------------------------------------------------------------------------------
*/

extern const appd_ObjectType appd_sObject;

/*------------------------------------------------------------------------------
**
** appd_bBigEndianNetOrder
**
**------------------------------------------------------------------------------
**
** Variable holding the network byte order.
** 0 = Little endian, 1 = Big endian.
**
**------------------------------------------------------------------------------
*/

extern UINT8 appd_bBigEndianNetOrder;

/*------------------------------------------------------------------------------
**
** appd_iWriteSize
**
**------------------------------------------------------------------------------
**
** Contains the amount of mapped write process data
**
**------------------------------------------------------------------------------
*/

extern UINT16 appd_iWriteSize;

/*------------------------------------------------------------------------------
**
** appd_iReadSize
**
**------------------------------------------------------------------------------
**
** Contains the amount of mapped read process data
**
**------------------------------------------------------------------------------
*/

extern UINT16 appd_iReadSize;

/*------------------------------------------------------------------------------
**
** appd_abTypesize
**
**------------------------------------------------------------------------------
**
** Lookup table for retrieving the size of ABCC data types
**
**------------------------------------------------------------------------------
*/

extern const UINT8 appd_abTypesize[];


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** APPD_CbfNewReadProcessData( )
**
** This function must be called whenever new process data is available 
** from the driver.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    -
**
** Outputs:
**    -
**
** Usage:
**    APPD_CbfNewReadProcessData();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPD_CbfNewReadProcessData( void );


/*------------------------------------------------------------------------------
**
** APPD_CbfRequestPdSize( )
**
** This function retrieves the read and write process data sizes
**
**------------------------------------------------------------------------------
**
** Inputs:
**    piReadLength   - Pointer to a UINT16.
**    piWriteLength  - Pointer to a UINT16.
**
** Outputs:
**    *piReadLength  - Read process data size.
**    *piWriteLength - Read process data size.
**
** Usage:
**    APPD_CbfRequestPdSize( &iReadLength, &iWriteLength );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPD_CbfRequestPdSize( UINT16* piReadLength, UINT16* piWriteLength );


/*------------------------------------------------------------------------------
**
** APPD_FindAdiIndex( )
**
** Attempts to find out the index of the given ADI number in the
** appd_asInstance[] array.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    iAdiNbr  - The ADI number to find the index for
**
** Outputs:
**    Returns  - The index into the array which holds the ADI number
**                  OR 0xFFFF if the index cannot be found.
**
** Usage:
**    APPD_FindAdiIndex( APPD_ADI_NBR_DRIVESTATUS );
**
**------------------------------------------------------------------------------
*/

EXTFUNC UINT16 APPD_FindAdiIndex( UINT16 iAdiNbr );


/*------------------------------------------------------------------------------
**
** APPD_MapAdiToPd( )
**
** Map ADIs to process data
**
**------------------------------------------------------------------------------
**
** Inputs:
**    ppsReadMappings   - pointer to a ABCC_PdMapType pointer.
**    ppsWriteMappings  - pointer to a ABCC_PdMapType pointer.
**
** Outputs:
**    *ppsReadMappings  - Pointer to a ABCC_PdMapType containing read mappings
**    *ppsWriteMappings - Pointer to a ABCC_PdMapType containing write mappings
**
** Usage:
**    APPD_MapAdiToPd( AB_PDMapType** ppsReadMappings,
**                      AB_PDMapType** ppsWriteMappings );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPD_MapAdiToPd( ABCC_PdMapType** ppsReadMappings,
                               ABCC_PdMapType** ppsWriteMappings );


/*------------------------------------------------------------------------------
**
** APPD_ProcessCmdMsg( )
**
** Processes commands sent to the APPD object
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage   - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    APPD_ProcessCmdMsg( sMessage );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPD_ProcessCmdMsg( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** APPD_ProcessDataChanged( )
**
** This function must be called whenever write process data have changed 
** and must be sent to the driver.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    -
**
** Outputs:
**    -
**
** Usage:
**    APPD_ProcessDataChanged();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPD_ProcessDataChanged( void );


/*------------------------------------------------------------------------------
**
** APPD_SetNetworkByteOrder()
**
** Must be called to let the APPD object know the network byte order.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    bFormat     - Network byte order.
**                  0 = Little endian
**                  1 = Big endian
**
** Outputs:
**    -
**
** Usage:
**    APPD_SetNetworkByteOrder( 1 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPD_SetNetworkByteOrder( ABCC_DataFormatType eFormat );


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** appd_Compare( )
**
** Compare two values of any ABCC datatype, where one is in network byte order,
** and the other is in system byte order.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pVal1       - Pointer to the first value. 
**                  This one is assumed to be in network byte order.
**    pVal2       - Pointer to the second value. 
**                  This one is assumed to be in system byte order.
**    bDataType   - Data type (ABP_UINT8, ABP_SINT32, ...)
**
** Outputs:
**    Returns:    - Positive if *pVal1 > *pVal2.
**                  Negative if *pVal1 < *pVal2.
**                  Zero if the two values are equal.
**
** Usage:
**    appd_Compare( pbData, &Value, ABP_UINT8, 1 );
**
**------------------------------------------------------------------------------
*/

SINT32 appd_Compare( void* pVal1, void* pVal2, UINT8 bDataType );


/*------------------------------------------------------------------------------
**
** appd_InstanceCommand( )
**
** Processes commands to Application Data Instances
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage    - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    appd_InstanceCommand( psMessage );
**
**------------------------------------------------------------------------------
*/

void appd_InstanceCommand( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** appd_ObjectCommand( )
**
** Processes commands to the Application Data Object (Instance 0)
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage    - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    appd_ObjectCommand( psMessage );
**
**------------------------------------------------------------------------------
*/

void appd_ObjectCommand( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
**
** appd_RemapProcessDataCommand()
**
** Handles process data remapping commands from the ABCC module.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psMsg       - Message containing the command
**
** Outputs:
**    Returns:    - Overwrites psMsg with a proper response
**
** Usage:
**    appd_RemapProcessDataCommand( psMsg );
**
**------------------------------------------------------------------------------
*/

void appd_RemapProcessDataCommand( ABP_MsgType* psMsg );


/*------------------------------------------------------------------------------
**
** appd_SizeOfAdiVal( )
**
** Calculates the total size of a ADI value
**
**------------------------------------------------------------------------------
**
** Inputs:
**    sAdi        - The ADI
**
** Outputs:
**    Returns:    - The total size of the ADI value
**
** Usage:
**    size = appd_SizeOfAdiVal( sAdi );
**
**------------------------------------------------------------------------------
*/

#define appd_SizeOfAdiVal( sAdi ) ( appd_abTypesize[ (sAdi).bDataType ] * (sAdi).bNumberOfElements )


/*------------------------------------------------------------------------------
**
** appd_SwapToNetOrder( )
**
** This function converts any ABCC datatype array between system and 
** network byte order.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    pDst        - Where the converted data is to be written.
**    pSrc        - Where the input data is located
**    bDataType   - Type of data, (ABP_UINT8, ABP_SINT32, ...)
**    bElements   - Number of elements
**
** Outputs:
**    *pDst       - The converted data
**    Returns:    - Number of bytes converted
**
** Usage:
**    appd_SwapToNetOrder( pDst, pSrc, ABP_UINT16, 1 );
**
**------------------------------------------------------------------------------
*/

UINT16 appd_SwapToNetOrder( void* pDst, void* pSrc, UINT8 bDataType, UINT8 bElements );


/*------------------------------------------------------------------------------
**
** appd_UpdateAdiVal( )
**
** This function compare the new data to the allowed range of the ADI.
**
** If the new data is outside the allowed range, the ADI value will not be
** updated, and a "1" (too high value) or "-1" (too low value) will be written
** to the byte at pbNotInRange.
**
** If the new data is within the allowed range, the new data will be written 
** to the ADI value, and a "0" will be written to the byte at pbNotInRange.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psAdi        - Pointer to a ADI which value is to be updated
**    pVal         - Pointer to the new data. The data is in network byte order
**    bElement     - Element of the ADI to update.
**    pbNotInRange - Pointer to a byte where the result of the range-check will 
**                   be written.
**
** Outputs:
**    Returns:     - Total size of value in bytes.
**
** Usage:
**    appd_UpdateAdiVal( psAdi, pbData, &bRangeCheck );
**
**------------------------------------------------------------------------------
*/

UINT16 appd_UpdateAdiVal( const appd_InstanceType* psAdi, void* pVal, UINT8 bElement, SINT8* pbNotInRange);


#endif  /* inclusion lock */


/*******************************************************************************
**
** End of appd.h
**
********************************************************************************
*/