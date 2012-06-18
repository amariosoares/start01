/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** appd.c
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** Source file for the Application Data Object and instances
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
#include "util.h"
#include "appd.h"
#include "string.h"


/*******************************************************************************
**
** Public Globals
**
********************************************************************************
*/

/* 
** instance 1 data values 
*/

SINT16 APPD_iSpeed = 0;
SINT16 APPD_iMaxSpeed = 500;
SINT16 APPD_iMinSpeed = -500;
SINT16 APPD_iDefaultSpeed = 10;


/* 
** instance 2 data values 
*/

SINT16 APPD_iTemperature = 25;
SINT16 APPD_iMaxTemperature = 125;
SINT16 APPD_iMinTemperature = -40;
SINT16 APPD_iDefaultTemperature = 25;


/*
** instance 3 data values 
*/

UINT16 APPD_iRefSpeed = 0;
UINT16 APPD_iMaxRefSpeed = 500;
UINT16 APPD_iMinRefSpeed = 0;
UINT16 APPD_iDefaultRefSpeed = 10;


/* 
** instance 4 data values 
*/

BOOL8 APPD_fForward = 1;
BOOL8 APPD_fMaxForward = 1;
BOOL8 APPD_fMinForward = 0;
BOOL8 APPD_fDefaultForward = 1;


/* 
** instance 5 data values 
*/

UINT8 APPD_bPoles = 4;
UINT8 APPD_bMaxPoles = 6;
UINT8 APPD_bMinPoles = 0;
UINT8 APPD_bDefaultPoles = 4;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/* 
** This array of structs represents the APPD instances 
*/

const appd_InstanceType appd_asInstance[ NUMBER_OF_INSTANCES ] = 
{ 
  { "Speed",                   /* Name                 */
    ABP_UINT16,                /* Data Type            */
    1,                         /* Number of elements   */
    ABP_NC_DESCR_GET_ACCESS,   /* Descriptor           */
    &APPD_iSpeed,              /* Value                */
    &APPD_iMaxSpeed,           /* Max Value            */
    &APPD_iMinSpeed,           /* Min Value            */
    &APPD_iDefaultSpeed,       /* Default Value        */
    APPD_ADI_NBR_SPEED         /* Instance number      */
  },
  { "Temperature",             /* Name                 */
    ABP_SINT16,                /* Data Type            */
    1,                         /* Number of elements   */
    ABP_NC_DESCR_GET_ACCESS,   /* Descriptor           */
    &APPD_iTemperature,        /* Value                */
    &APPD_iMaxTemperature,     /* Max Value            */
    &APPD_iMinTemperature,     /* Min Value            */
    &APPD_iDefaultTemperature, /* Default Value        */
    APPD_ADI_NBR_TEMP          /* Instance number      */
  },
  { "Ref speed",               /* Name                 */
    ABP_UINT16,                /* Data Type            */
    1,                         /* Number of elements   */
    ABP_NC_DESCR_SET_ACCESS | ABP_NC_DESCR_GET_ACCESS,   /* Descriptor           */
    &APPD_iRefSpeed,           /* Value                */
    &APPD_iMaxRefSpeed,        /* Max Value            */
    &APPD_iMinRefSpeed,        /* Min Value            */
    &APPD_iDefaultRefSpeed,    /* Default Value        */
    APPD_ADI_NBR_REF_SPEED     /* Instance number      */
  },
  { "Direction = Forward",     /* Name                 */
    ABP_BOOL,                  /* Data Type            */
    1,                         /* Number of elements   */
    ABP_NC_DESCR_SET_ACCESS | ABP_NC_DESCR_GET_ACCESS,   /* Descriptor           */
    &APPD_fForward,            /* Value                */
    &APPD_fMaxForward,         /* Max Value            */
    &APPD_fMinForward,         /* Min Value            */
    &APPD_fDefaultForward,     /* Default Value        */
    APPD_ADI_NBR_DIRECTION     /* Instance number      */
  },
  { "Trip Current",            /* Name                 */
    ABP_UINT8,                 /* Data Type            */
    1,                         /* Number of elements   */
    ABP_NC_DESCR_SET_ACCESS | ABP_NC_DESCR_GET_ACCESS,   /* Descriptor           */
    &APPD_bPoles,              /* Value                */
    &APPD_bMaxPoles,           /* Max Value            */
    &APPD_bMinPoles,           /* Min Value            */
    &APPD_bDefaultPoles,       /* Default Value        */
    APPD_ADI_NBR_TRIP_CURRENT  /* Instance number      */
  }
};


/*
** We are able to handle up to NUMBER_OF_INSTANCES mapping items on read process data
*/

ABCC_PdAdiMapType   appd_asReadADI[ NUMBER_OF_INSTANCES ];
UINT16              appd_aiInitialReadMaps[] = { APPD_ADI_NBR_REF_SPEED,
                                                 APPD_ADI_NBR_DIRECTION,
                                                 0 };


/*
** We are able to handle up to NUMBER_OF_INSTANCES mapping items on write process data
*/

ABCC_PdAdiMapType   appd_asWriteADI[ NUMBER_OF_INSTANCES ];
UINT16              appd_aiInitialWriteMaps[] = { APPD_ADI_NBR_SPEED,
                                                  0 };

ABCC_PdMapType      appd_sMyReadMappings;
ABCC_PdMapType      appd_sMyWriteMappings;


/* 
** This struct represents the APPD object 
*/

const appd_ObjectType appd_sObject = 
{
   "Application Data",     /* Name                    */
   0x01,                   /* Revision                */
   NUMBER_OF_INSTANCES,    /* Number of instances     */
   NUMBER_OF_INSTANCES     /* Highest instance number */
};


/*
** Variable containing the network byte order
*/

ABCC_DataFormatType appd_eBigEndianNetOrder = ABCC_DF_MSB_FIRST;


/*
** Contains the amount of mapped process data
*/

UINT16 appd_iWriteSize = 0;
UINT16 appd_iReadSize = 0;

/*
** Lookup table for retrieving the size of ABCC data types
*/

const UINT8 appd_abTypesize[] =
{  ABP_BOOL_SIZEOF,
   ABP_SINT8_SIZEOF,
   ABP_SINT16_SIZEOF,
   ABP_SINT32_SIZEOF,
   ABP_UINT8_SIZEOF,
   ABP_UINT16_SIZEOF,
   ABP_UINT32_SIZEOF,
   ABP_CHAR_SIZEOF,
   ABP_ENUM_SIZEOF,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   ABP_SINT64_SIZEOF,
   ABP_UINT64_SIZEOF,
   ABP_FLOAT_SIZEOF
};

/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** APPD_CbfNewReadProcessData()
**------------------------------------------------------------------------------
*/

void APPD_CbfNewReadProcessData( void )
{
   UINT16   iAdiIndex;
   UINT8    abData[ 256 ];
   UINT8    bIndex = 0;
   UINT8    bCnt;
   SINT8    bDummy;
   UINT8    bAdiMap;
   ABCC_PdAdiMapType* psAdiMap;
   

   /* 
   ** Retrieve the new process data
   */
   
   ABCC_ReadProcessData( abData );

   /* 
   ** Update ADI values with the new data
   */

   psAdiMap = appd_asReadADI;
   for( bAdiMap = 0; bAdiMap < appd_sMyReadMappings.iNbrMaps; bAdiMap++ )
   {
      iAdiIndex = APPD_FindAdiIndex( psAdiMap->iAdiNbr );

      for( bCnt = 0; bCnt < psAdiMap->bNbrElements; bCnt++ )
      {
         bIndex += appd_UpdateAdiVal( &appd_asInstance[ iAdiIndex ], &abData[ bIndex ], psAdiMap->bFirstElement + bCnt, &bDummy );
      }

      psAdiMap++;
   }
   
} /* end of APPD_CbfNewReadProcessData( ) */


/*------------------------------------------------------------------------------
** APPD_CbfRequestPdSize()
**------------------------------------------------------------------------------
*/

void APPD_CbfRequestPdSize( UINT16* piReadLength, UINT16* piWriteLength )
{
   /* 
   ** This function retrieves the read and write process data sizes 
   */
   
   *piReadLength = appd_iReadSize;
   *piWriteLength = appd_iWriteSize;

} /* End of APPD_CbfRequestPdSize( ) */


/*------------------------------------------------------------------------------
** APPD_MapAdiToPd()
**------------------------------------------------------------------------------
*/

void APPD_MapAdiToPd( ABCC_PdMapType** ppsReadMappings,
                      ABCC_PdMapType** ppsWriteMappings )
{
   UINT16    iAdiIndex;
   UINT16*   piAdiNbr;
   UINT16    iCnt;


   /*
   ** This function maps ADIs to process data
   */

   appd_iWriteSize = 0;
   appd_iReadSize = 0;

   /*
   ** Map to Write Process Data
   */

   piAdiNbr = appd_aiInitialWriteMaps;
   iCnt = 0;

   while( *piAdiNbr != 0 )
   {
      iAdiIndex = APPD_FindAdiIndex( *piAdiNbr );

      appd_asWriteADI[ iCnt ].iAdiNbr       = appd_asInstance[ iAdiIndex ].iInstanceNumber;
      appd_asWriteADI[ iCnt ].bDataType     = appd_asInstance[ iAdiIndex ].bDataType;
      appd_asWriteADI[ iCnt ].bNbrElements  = appd_asInstance[ iAdiIndex ].bNumberOfElements;
      appd_asWriteADI[ iCnt ].bFirstElement = 0;
      appd_asWriteADI[ iCnt ].iOrderNumber  = iAdiIndex + 1;

      appd_iWriteSize += appd_SizeOfAdiVal( appd_asInstance[ iAdiIndex ] );

      iCnt++;
      piAdiNbr++;
   }

   appd_sMyWriteMappings.psMaps    = appd_asWriteADI;
   appd_sMyWriteMappings.iNbrMaps  = (UINT16) iCnt;

   /*
   ** Map to Read Process Data
   */

   piAdiNbr = appd_aiInitialReadMaps;
   iCnt = 0;

   while( *piAdiNbr != 0 )
   {
      iAdiIndex = APPD_FindAdiIndex( *piAdiNbr );

      appd_asReadADI[ iCnt ].iAdiNbr       = appd_asInstance[ iAdiIndex ].iInstanceNumber;
      appd_asReadADI[ iCnt ].bDataType     = appd_asInstance[ iAdiIndex ].bDataType;
      appd_asReadADI[ iCnt ].bNbrElements  = appd_asInstance[ iAdiIndex ].bNumberOfElements;
      appd_asReadADI[ iCnt ].bFirstElement = 0;
      appd_asReadADI[ iCnt ].iOrderNumber  = iAdiIndex + 1;

      appd_iReadSize += appd_SizeOfAdiVal( appd_asInstance[ iAdiIndex ] );

      iCnt++;
      piAdiNbr++;
   }

   appd_sMyReadMappings.psMaps    = appd_asReadADI;
   appd_sMyReadMappings.iNbrMaps  = (UINT16) iCnt;

   /*
   ** Make sure the caller get the info
   */

   *ppsWriteMappings  = &appd_sMyWriteMappings;
   *ppsReadMappings   = &appd_sMyReadMappings;
   
} /* End of APPD_MapAdiToPd() */


/*------------------------------------------------------------------------------
** APPD_ProcessCmdMsg()
**------------------------------------------------------------------------------
*/

void APPD_ProcessCmdMsg( ABP_MsgType* psNewMessage )
{
   /* 
   ** This function processes commands to the Application Data Object and it's Instances 
   */

   if( psNewMessage->sHeader.iInstance == ABP_INST_OBJ )
   {
      /*
      ** Application data object Command
      */
      
      appd_ObjectCommand( psNewMessage );
   }
   else
   {
      /*
      ** Application data instance Command
      */
      
      appd_InstanceCommand( psNewMessage );
   }

   ABCC_SendMessage( psNewMessage );
   
} /* end of APPD_ProcessCmdMsg( ABP_MsgType* psNewMessage ) */


/*------------------------------------------------------------------------------
** APPD_ProcessDataChanged()
**------------------------------------------------------------------------------
*/

void APPD_ProcessDataChanged( void )
{
   const appd_InstanceType* psAdi;
   ABCC_PdAdiMapType* psAdiMap;
   UINT8*  pbSrc;
   UINT16  iAdiIndex;
   UINT8   abData[ 256 ];
   UINT8   bIndex = 0;
   UINT8   bAdiMap;


   /*
   ** Update process data with the new data
   */

   psAdiMap = appd_asWriteADI;

   for( bAdiMap = 0; bAdiMap < appd_sMyWriteMappings.iNbrMaps; bAdiMap++ )
   {
      /*
      ** For all mapping items
      */

      iAdiIndex = APPD_FindAdiIndex( psAdiMap->iAdiNbr );
      psAdi = &appd_asInstance[ iAdiIndex ];

      /*
      ** Find out the source address to the first mapped element
      */

      pbSrc  = (UINT8*) psAdi->pxValue;
      pbSrc += psAdiMap->bFirstElement * appd_abTypesize[ psAdi->bDataType ];

      /*
      ** Move data
      */

      bIndex += appd_SwapToNetOrder( &abData[ bIndex ],
                                     pbSrc,
                                     psAdi->bDataType,
                                     psAdiMap->bNbrElements );

      /*
      ** Step up to next mapping item
      */

      psAdiMap++;
   }

   /*
   ** Hand over the data to the driver.
   */

   ABCC_WriteProcessData( abData );

} /* End of APPD_ProcessDataChanged() */


/*------------------------------------------------------------------------------
** APPD_SetNetworkByteOrder()
**------------------------------------------------------------------------------
*/

void APPD_SetNetworkByteOrder( ABCC_DataFormatType eFormat )
{
   /* 
   ** Store the network byte order for later use
   */
   
   appd_eBigEndianNetOrder = eFormat;
   
} /* end of APPD_SetNetworkByteOrder( ABCC_DataFormatType eFormat ) */


/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** appd_Compare()
**------------------------------------------------------------------------------
*/

SINT32 appd_Compare( void* pVal1, void* pVal2, UINT8 bDataType )
{
   switch(bDataType)
   {
   case ABP_BOOL:
   
      return ( (SINT32)*(BOOL8*)pVal1 - (SINT32)*(BOOL8*)pVal2 );
      
   case ABP_SINT8:
   
      return ( (SINT32)*(SINT8*)pVal1 - (SINT32)*(SINT8*)pVal2 );
      
   case ABP_SINT16:
   
      return ( (SINT32)*(SINT16*)pVal1 - (SINT32)*(SINT16*)pVal2 );
      
   case ABP_SINT32:
   
      if( *(SINT32*)pVal1 > *(SINT32*)pVal2 )
      {
         return ( 1 );
      }
      if( *(SINT32*)pVal1 < *(SINT32*)pVal2 )
      {
         return ( -1 );
      }
      return ( 0 );
      
   case ABP_UINT8:
   
      return ( (SINT32)*(UINT8*)pVal1 - (SINT32)*(UINT8*)pVal2 );
      
   case ABP_UINT16:
   
      return ( (SINT32)*(UINT16*)pVal1 - (SINT32)*(UINT16*)pVal2 );
      
   case ABP_UINT32:
   
      if( *(UINT32*)pVal1 > *(UINT32*)pVal2 )
      {
         return ( 1 );
      }
      if( *(UINT32*)pVal1 < *(UINT32*)pVal2 )
      {
         return ( -1 );
      }
      return 0;
      
   case ABP_CHAR:
   
      return ( (SINT32)*(UINT8*)pVal1 - (SINT32)*(UINT8*)pVal2 );
      
   case ABP_ENUM:
   
      return ( (SINT32)*(UINT8*)pVal1 - (SINT32)*(UINT8*)pVal2 );
      
   case ABP_SINT64:
      if((*(SINT64*)pVal1)>(*(SINT64*)pVal2))
      {
         return 1;
      }
      if((*(SINT64*)pVal1)<(*(SINT64*)pVal2))
      {
         return -1;
      }
      return 0;

   case ABP_UINT64:
      if((*(UINT64*)pVal1)>(*(UINT64*)pVal2))
      {
         return 1;
      }
      if((*(UINT64*)pVal1)<(*(UINT64*)pVal2))
      {
         return -1;
      }
      return 0;

   case ABP_FLOAT:
   
      if( *(FLOAT32*)pVal1 > *(FLOAT32*)pVal2 )
      {
         return ( 1 );
      }
      if( *(FLOAT32*)pVal1 < *(FLOAT32*)pVal2 )
      {
         return ( -1 );
      }
      return ( 0 );
      
   default:
   
      return ( 0 );
      
   }
} /* End of appd_Compare() */


/*------------------------------------------------------------------------------
** APPD_FindAdiIndex()
**------------------------------------------------------------------------------
*/

UINT16 APPD_FindAdiIndex( UINT16 iAdiNbr )
{
   UINT16   iAdiIndex;


   for( iAdiIndex = 0; iAdiIndex < appd_sObject.iNumberOfInstances; iAdiIndex++ )
   {
      if( appd_asInstance[ iAdiIndex ].iInstanceNumber == iAdiNbr )
      {
         /*
         ** Instance found
         */

         return( iAdiIndex );
      }
   }

   /*
   ** Instance not found
   */

   return( 0xFFFF );

} /* end of APPD_FindAdiIndex() */


/*------------------------------------------------------------------------------
** appd_InstanceCommand()
**------------------------------------------------------------------------------
*/

void appd_InstanceCommand( ABP_MsgType* psNewMessage )
{
   UINT16 iIndex;
   UINT16 iStrLength;
   SINT8  bNotInRange;
   UINT8  bCnt;
   UINT8* pbSrc;
   
   /* 
   ** This function processes commands to Application Data Instances
   **
   ** First we try to find the requested instance.
   */

   iIndex = APPD_FindAdiIndex( psNewMessage->sHeader.iInstance );
   
   if( iIndex == 0xFFFF )
   {
      /*
      ** The requested Instance does not exist
      */
      
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_INST );
      
      return;
   }

   switch ( ( psNewMessage->sHeader.bCmd ) & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_APPD_IA_NAME:
         iStrLength = ( UINT16 )strlen( appd_asInstance[ iIndex ].acName );
         SYS_CopyMemBytes( psNewMessage->abData, 
                           appd_asInstance[ iIndex ].acName, 
                           iStrLength );  
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_APPD_IA_DATA_TYPE:
         psNewMessage->abData[ 0 ] = appd_asInstance[ iIndex ].bDataType;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );                  
         break;

      case ABP_APPD_IA_NUM_ELEM:
         psNewMessage->abData[ 0 ] = appd_asInstance[ iIndex ].bNumberOfElements;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      case ABP_APPD_IA_DESCRIPTOR:
         psNewMessage->abData[ 0 ] = appd_asInstance[ iIndex ].bDescriptor;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );
         break;

      case ABP_APPD_IA_VALUE:
         if(! ( appd_asInstance[ iIndex ].bDescriptor & ABP_NC_DESCR_GET_ACCESS ) )
         {
            /* 
            ** Attribute not get-able 
            */
            
            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_GETABLE );
         }
         else
         {
            /* 
            ** Proper access rights: Copy attribute to message
            */
            
            appd_SwapToNetOrder( psNewMessage->abData,
                                 appd_asInstance[ iIndex ].pxValue, 
                                 appd_asInstance[ iIndex ].bDataType, 
                                 appd_asInstance[ iIndex ].bNumberOfElements);

            ABP_SetMsgResponse( psNewMessage, appd_SizeOfAdiVal( appd_asInstance[ iIndex ] ) );
         }
         break;

      case ABP_APPD_IA_MAX_VALUE:
         appd_SwapToNetOrder( psNewMessage->abData,
                              appd_asInstance[ iIndex ].pxMaxValue, 
                              appd_asInstance[ iIndex ].bDataType, 
                              appd_asInstance[ iIndex ].bNumberOfElements );

         ABP_SetMsgResponse( psNewMessage, appd_SizeOfAdiVal( appd_asInstance[ iIndex ] ) );
         break;

      case ABP_APPD_IA_MIN_VALUE:
         appd_SwapToNetOrder( psNewMessage->abData,
                              appd_asInstance[ iIndex ].pxMinValue, 
                              appd_asInstance[ iIndex ].bDataType, 
                              appd_asInstance[ iIndex ].bNumberOfElements );

         ABP_SetMsgResponse( psNewMessage, appd_SizeOfAdiVal( appd_asInstance[ iIndex ] ) );
         break;

      case ABP_APPD_IA_DFLT_VALUE:
         appd_SwapToNetOrder( psNewMessage->abData,
                              appd_asInstance[ iIndex ].pxDefaultValue, 
                              appd_asInstance[ iIndex ].bDataType, 
                              appd_asInstance[ iIndex ].bNumberOfElements );

         ABP_SetMsgResponse( psNewMessage, appd_SizeOfAdiVal( appd_asInstance[ iIndex ] ) );
         break;

      default:
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
         
      } /* End of switch( attribute number ) */
      
      break;

   case ABP_CMD_GET_INDEXED_ATTR:
   case ABP_CMD_SET_INDEXED_ATTR:

      /*
      ** Command Get or Set indexed attribute - do a more detailed check.
      */

      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_APPD_IA_NAME:
      case ABP_APPD_IA_DATA_TYPE:
      case ABP_APPD_IA_NUM_ELEM:
      case ABP_APPD_IA_DESCRIPTOR:
      case ABP_APPD_IA_MAX_VALUE:
      case ABP_APPD_IA_MIN_VALUE:
      case ABP_APPD_IA_DFLT_VALUE:

         /*
         ** Only the Value attribute can be an array.
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
         break;

      case ABP_APPD_IA_VALUE:

         if( appd_asInstance[ iIndex ].bNumberOfElements < 2 )
         {
            /*
            ** Command not supported if the instance isn't an array.
            */

            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_TOO_MUCH_DATA );
            break;
         }

         if( appd_asInstance[ iIndex ].bDataType == ABP_CHAR )
         {
            /*
            ** Command not supported for array of CHAR.
            */

            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
            break;
         }

         if( psNewMessage->sHeader.bCmdExt1 >=
             appd_asInstance[ iIndex ].bNumberOfElements )
         {
            /*
            ** Out of range for this array.
            */

            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_1 );
            break;
         }

         if( ( ( psNewMessage->sHeader.bCmd ) & ABP_MSG_HEADER_CMD_BITS ) ==
             ABP_CMD_GET_INDEXED_ATTR )
         {
            /*
            ** Check if this Get indexed attribute command
            ** is valid for this Value attribute.
            */

            if( !( appd_asInstance[ iIndex ].bDescriptor & ABP_NC_DESCR_GET_ACCESS ) )
            {
               /*
               ** Get access is not supported for this instance.
               */

               ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_GETABLE );
               break;
            }

            /*
            ** Copy attribute to message
            */

            appd_SwapToNetOrder( psNewMessage->abData,
                                 (UINT8*) appd_asInstance[ iIndex ].pxValue +
                                    ( appd_abTypesize[ appd_asInstance[ iIndex ].bDataType ] *
                                      psNewMessage->sHeader.bCmdExt1 ),
                                 appd_asInstance[ iIndex ].bDataType,
                                 1 );

            ABP_SetMsgResponse( psNewMessage, appd_abTypesize[ appd_asInstance[ iIndex ].bDataType ] );
         }
         else
         {
            /*
            ** Check if this Set indexed attribute command
            ** is valid for this Value attribute.
            */

            if( !( appd_asInstance[ iIndex ].bDescriptor & ABP_NC_DESCR_SET_ACCESS ) )
            {
               /*
               ** Set access is not supported for this instance.
               */

               ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_SETABLE );
               break;
            }

            if( psNewMessage->sHeader.bDataSize <
                appd_abTypesize[ appd_asInstance[ iIndex ].bDataType ] )
            {
               /*
               ** Not enough data.
               */

               ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_NOT_ENOUGH_DATA );
               break;
            }

            if( psNewMessage->sHeader.bDataSize >
                appd_abTypesize[ appd_asInstance[ iIndex ].bDataType ] )
            {
               /*
               ** Too much data.
               */

               ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_TOO_MUCH_DATA );
               break;
            }

            /*
            ** Proper access rights and the data size is correct.
            ** Assume all will go well and
            ** update ADI value with new data.
            */

            appd_UpdateAdiVal( &appd_asInstance[ iIndex ], psNewMessage->abData,
                               psNewMessage->sHeader.bCmdExt1, &bNotInRange );

            if( bNotInRange )
            {
               ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_OUT_OF_RANGE );
               break;
            }
            else
            {
               ABP_SetMsgResponse( psNewMessage, 0 );
            }
         } /* end if( Get indexed attribute ) */
         break;

      default:

         /*
         ** The attribute doesn't exist.
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_SETABLE );
         break;

      } /* end of switch ( attribute number ) */

      break;

   case ABP_CMD_SET_ATTR:
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_APPD_IA_NAME:
      case ABP_APPD_IA_DATA_TYPE:
      case ABP_APPD_IA_NUM_ELEM:
      case ABP_APPD_IA_DESCRIPTOR:
      case ABP_APPD_IA_MAX_VALUE:
      case ABP_APPD_IA_MIN_VALUE:
      case ABP_APPD_IA_DFLT_VALUE:
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_SETABLE );
         break;

      case ABP_APPD_IA_VALUE:

         /*
         ** Check access rights
         */

         if( !( appd_asInstance[ iIndex ].bDescriptor & ABP_NC_DESCR_SET_ACCESS ) )
         {
            /*
            ** Attribute not set-able
            */

            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_ATTR_NOT_SETABLE );
         }
         /*
         ** Check the data size
         */

         else if( psNewMessage->sHeader.bDataSize >
                  appd_SizeOfAdiVal( appd_asInstance[ iIndex ] ) )
         {
            /*
            ** Too much data
            */

            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_TOO_MUCH_DATA );

         }
         else if( psNewMessage->sHeader.bDataSize <
                  appd_SizeOfAdiVal( appd_asInstance[ iIndex ] ) )
         {
            /*
            ** Not enough data
            */

            ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_NOT_ENOUGH_DATA );
         }
         else
         {
            /*
            ** Proper access rights and the data size is correct.
            ** Assume all will go well and
            ** update ADI value with new data.
            */

            ABP_SetMsgResponse( psNewMessage, 0 );

            pbSrc = psNewMessage->abData;

            for( bCnt = 0; bCnt < appd_asInstance[ iIndex ].bNumberOfElements; bCnt++ )
            {
               pbSrc += appd_UpdateAdiVal( &appd_asInstance[ iIndex ], pbSrc, bCnt, &bNotInRange );

               if( bNotInRange != 0 )
               {
                  ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_OUT_OF_RANGE );
                  break;
               }
            }

         }
         break;

      default:
         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;

      } /* end of switch ( attribute number ) */

      break;


   case ABP_APPD_GET_ADI_INFO:

      psNewMessage->abData[ 0 ] = appd_asInstance[ iIndex ].bDataType;
      psNewMessage->abData[ 1 ] = appd_asInstance[ iIndex ].bNumberOfElements;
      psNewMessage->abData[ 2 ] = appd_asInstance[ iIndex ].bDescriptor;
      ABP_SetMsgResponse( psNewMessage, 3 * ABP_UINT8_SIZEOF );

      break;

   default:

      /*
      ** Unsupported command
      */

      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;

   } /* End of switch ( command number ) */
} /* End of appd_InstanceCommand() */


/*------------------------------------------------------------------------------
** appd_ObjectCommand()
**------------------------------------------------------------------------------
*/

void appd_ObjectCommand( ABP_MsgType* psNewMessage )
{
   UINT16*  piData;
   UINT16   iInstanceOrder;
   UINT16   iStrLength;


   /*
   ** This function processes commands to the Application Data Object (Instance 0) 
   */

   switch ( ( psNewMessage->sHeader.bCmd ) & ABP_MSG_HEADER_CMD_BITS )
   {
   case ABP_CMD_GET_ATTR:        /* Get attribute */
      switch( psNewMessage->sHeader.bCmdExt0 )
      {
      case ABP_OA_NAME:          /* Name */
         iStrLength = ( UINT16 )strlen( appd_sObject.acName );
         SYS_CopyMemBytes( psNewMessage->abData,
                           appd_sObject.acName,
                           iStrLength );
         ABP_SetMsgResponse( psNewMessage, (UINT8)iStrLength );
         break;

      case ABP_OA_REV:           /* Revision */
         psNewMessage->abData[ 0 ] = appd_sObject.bRevision;
         ABP_SetMsgResponse( psNewMessage, ABP_UINT8_SIZEOF );                  
         break;

      case ABP_OA_NUM_INST:      /* Number of Instances */
         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( appd_sObject.iNumberOfInstances );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      case ABP_OA_HIGHEST_INST:  /* Highest Instance Number */
         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( appd_sObject.iHighestInstanceNo );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
         break;

      default:

         /*
         ** Error: Unsupported attribute
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
         break;
      }
      break;

   case ABP_APPD_CMD_GET_INST_BY_ORDER:  /* Get instance number by order */
      iInstanceOrder =  psNewMessage->sHeader.bCmdExt1 << 8;
      iInstanceOrder += psNewMessage->sHeader.bCmdExt0;

      if( ( iInstanceOrder > appd_sObject.iNumberOfInstances ) ||
          ( iInstanceOrder == 0 ) )
      {
         /*
         ** There is no ADI that corresponds to the submitted order number 
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_0 );
      }
      else
      {
         /*
         ** Copy instance number to message 
         */

         *(UINT16*)psNewMessage->abData = UTIL_WordToNative( appd_asInstance[ iInstanceOrder-1 ].iInstanceNumber );
         ABP_SetMsgResponse( psNewMessage, ABP_UINT16_SIZEOF );
      }
      break;

   case ABP_APPD_REMAP_ADI_WRITE_AREA:
   case ABP_APPD_REMAP_ADI_READ_AREA:

      appd_RemapProcessDataCommand( psNewMessage );
      break;

   case ABP_APPD_GET_PROFILE_INST_NUMBERS:

      if( psNewMessage->sHeader.bCmdExt1 != 1 )
      {
         /*
         ** We do not support this profile
         */

         ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_INV_CMD_EXT_1 );
         break;
      }

      /*
      ** Return profile instance numbers.
      */

	  /*
      ** Fill out the ADI numbers of the profile parameters
      ** in the proper order
      */

      piData = (UINT16*) psNewMessage->abData;

      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData++ = UTIL_WordToNative( 0x00 );
      *piData   = UTIL_WordToNative( 0x00 );

      ABP_SetMsgResponse( psNewMessage, APPD_MAX_DRIVEPROFILE_PARAMS * ABP_UINT16_SIZEOF );

      break;

   default:
      ABP_SetMsgErrorResponse( psNewMessage, 1, ABP_ERR_UNSUP_CMD );
      break;
   }
} /* end of appd_ObjectCommand() */


/*------------------------------------------------------------------------------
** appd_RemapProcessDataCommand()
**------------------------------------------------------------------------------
*/

void appd_RemapProcessDataCommand( ABP_MsgType* psMsg )
{
   ABCC_PdMapType*   psPdMaps;
   UINT16            iItemsToRemove;
   UINT16            iItemsToAdd;
   UINT16            iMapIndex;
   UINT16            iAdiNbr;
   UINT16            iAdiIndex;
   UINT16            iCnt;
   UINT16            iNewMapSize;
   UINT8             bStartOfRemap;


   /*
   ** Determine if we are to remap the read pd or the write pd
   */

   if( ( psMsg->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS ) == ABP_APPD_REMAP_ADI_WRITE_AREA )
   {
      psPdMaps = &appd_sMyWriteMappings;
      iNewMapSize = appd_iWriteSize;
   }
   else
   {
      psPdMaps = &appd_sMyReadMappings;
      iNewMapSize = appd_iReadSize;
   }

   /*
   ** A lot of sanity checks first since all actions of the command shall
   ** either be carried out or rejected.
   */

   if( psMsg->sHeader.bDataSize < 4 )
   {
      /*
      ** Not enough data provided
      */

      ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_NOT_ENOUGH_DATA );
      return;
   }

   bStartOfRemap = psMsg->sHeader.bCmdExt0;
   if( bStartOfRemap > psPdMaps->iNbrMaps )
   {
      /*
      ** Not an allowed mapping number
      */

      ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_INV_CMD_EXT_0 );
      return;
   }

   iItemsToRemove = *((UINT16*)psMsg->abData );
   if( bStartOfRemap + iItemsToRemove > psPdMaps->iNbrMaps )
   {
      /*
      ** Cannot remove more than currently is mapped
      */

      ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_OUT_OF_RANGE );
      return;
   }

   /*
   ** Calculate how much data will be removed.
   */

   for( iCnt = 0; iCnt < iItemsToRemove; iCnt++ )
   {
      iNewMapSize -= psPdMaps->psMaps[ bStartOfRemap + iCnt ].bNbrElements *
                     appd_abTypesize[ psPdMaps->psMaps[ bStartOfRemap + iCnt ].bDataType ];
   }


   iItemsToAdd = *((UINT16*)&psMsg->abData[ 2 ] );
   if( ( psPdMaps->iNbrMaps - iItemsToRemove ) + iItemsToAdd > 32 )
   {
      /*
      ** This will result in more maps than we can handle
      */

      ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_NO_RESOURCES );
      return;
   }

   if( psMsg->sHeader.bDataSize < 4 + ( iItemsToAdd * 4 ) )
   {
      ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_NOT_ENOUGH_DATA );
      return;
   }
   if( psMsg->sHeader.bDataSize > 4 + ( iItemsToAdd * 4 ) )
   {
      ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_TOO_MUCH_DATA );
      return;
   }

   for( iCnt = 0; iCnt < iItemsToAdd; iCnt++ )
   {
      iAdiNbr = *((UINT16*)&psMsg->abData[ 4 + ( iCnt * 4 ) ] );

      iAdiIndex = APPD_FindAdiIndex( iAdiNbr );

      if( iAdiIndex == 0xFFFF )
      {
         /*
         ** Invalid ADI number
         */

         ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_OUT_OF_RANGE );
         return;
      }
      else if( iAdiNbr == 6 )
      {
         /*
         ** Make sure the drive control mask is entirely mapped
         ** we do not allow only one element...
         */

         if( ( psMsg->abData[ 6 + ( iCnt * 4 ) ] != 0 ) ||  /* Index to first element */
             ( psMsg->abData[ 7 + ( iCnt * 4 ) ] != 2 ) )   /* Elements to map */
         {
            ABP_SetMsgErrorResponse( psMsg, 1, ABP_ERR_NOT_ENOUGH_DATA );
            return;
         }
      }

      if( ( psMsg->abData[ 6 + ( iCnt * 4 ) ]   /* Index to first element */
          + psMsg->abData[ 7 + ( iCnt * 4 ) ] ) /* Elements to map */
        > ( appd_asInstance[ iAdiIndex ].bNumberOfElements ) ) /* Total amount of elements */
      {
         /*
         ** ADI does not have that many elements
         */

         ABP_SetMsgErrorResponse( psMsg, 2, ABP_ERR_OBJ_SPECIFIC );
         psMsg->abData[ 1 ] = 0x02; /* ABP_APPD_ERR_INVALID_NUM_ELEMENTS */
         return;
      }

      /*
      ** Adding process data
      */

      iNewMapSize += psMsg->abData[ 7 + ( iCnt * 4 ) ] * /* Elements to map */
                     appd_abTypesize[ appd_asInstance[ iAdiIndex ].bDataType ];
   }

   if( iNewMapSize > 256 )
   {
         ABP_SetMsgErrorResponse( psMsg, 2, ABP_ERR_OBJ_SPECIFIC );
         psMsg->abData[ 1 ] = 0x03; /* ABP_APPD_ERR_INVALID_TOTAL_SIZE */
      return;
   }

   if(  iItemsToRemove > iItemsToAdd )
   {
      /*
      ** Overall we are to remove mapping items.
      ** We might however also add items.
      ** Check how many steps we shall move the items in the array.
      */

      iCnt = iItemsToRemove - iItemsToAdd;

      for( iMapIndex = bStartOfRemap + iItemsToRemove; iMapIndex < psPdMaps->iNbrMaps; iMapIndex++ )
      {
         psPdMaps->psMaps[ iMapIndex - iCnt ] = psPdMaps->psMaps[ iMapIndex ];
      }
   }
   else if( iItemsToRemove < iItemsToAdd )
   {
      /*
      ** Overall we are to add mapping items.
      ** We might however also remove items.
      ** Check how many steps we shall move the items in the array.
      */

      iCnt = iItemsToAdd - iItemsToRemove;

      for( iMapIndex = psPdMaps->iNbrMaps - 1; iMapIndex >= bStartOfRemap + iItemsToRemove; iMapIndex-- )
      {
         psPdMaps->psMaps[ iMapIndex ] = psPdMaps->psMaps[ iMapIndex - iCnt ];
      }
   }
   else
   {
      /*
      ** We are adding the same amount as we are removing.
      ** We do not need to move any items in the array.
      */
   }

   /*
   ** Put the new entries into the list
   */

   iCnt = 4; /* offset in abData */
   for( iMapIndex = bStartOfRemap; iMapIndex < iItemsToAdd + bStartOfRemap; iMapIndex++ )
   {
      psPdMaps->psMaps[ iMapIndex ].iAdiNbr       = *((UINT16*)&psMsg->abData[ iCnt ]);
      iCnt += 2;
      psPdMaps->psMaps[ iMapIndex ].bFirstElement = psMsg->abData[ iCnt++ ];
      psPdMaps->psMaps[ iMapIndex ].bNbrElements  = psMsg->abData[ iCnt++ ];

      iAdiIndex = APPD_FindAdiIndex( psPdMaps->psMaps[ iMapIndex ].iAdiNbr );
      psPdMaps->psMaps[ iMapIndex ].bDataType = appd_asInstance[ iAdiIndex ].bDataType;
      psPdMaps->psMaps[ iMapIndex ].iOrderNumber = iAdiIndex + 1;

      /*
      ** NOTE: We do not use the bAreaOffset or bMapStatus fields.
      */

   }

   /*
   ** Update the number of maps
   */

   psPdMaps->iNbrMaps += iItemsToAdd;
   psPdMaps->iNbrMaps -= iItemsToRemove;

   /*
   ** Setup the amount of data we now have in the response
   */

   if( ( psMsg->sHeader.bCmd & ABP_MSG_HEADER_CMD_BITS ) == ABP_APPD_REMAP_ADI_WRITE_AREA )
   {
      appd_iWriteSize = iNewMapSize;
   }
   else
   {
      appd_iReadSize = iNewMapSize;
   }

   /*
   ** Build a success message with the new size as the response data
   */

   *((UINT16*)psMsg->abData) = UTIL_WordToNative( iNewMapSize );
   ABP_SetMsgResponse( psMsg, 2 );

} /* end of appd_RemapProcessDataCommand() */


/*------------------------------------------------------------------------------
** appd_SwapToNetOrder()
**------------------------------------------------------------------------------
*/

UINT16 appd_SwapToNetOrder( void* pDst, void* pSrc, UINT8 bDataType, UINT8 bElements )
{
   UINT16 i;
   UINT8 element = bElements;
   UINT32 temp[ 2 ];


   /*
   ** This function converts any ABCC datatype array between system and 
   ** network byte order.
   */

   for( ; bElements > 0; bElements-- ){
#ifdef ABCC_SYS_BIG_ENDIAN
      if( appd_eBigEndianNetOrder )
#else
      if( !appd_eBigEndianNetOrder )
#endif
      {
         SYS_CopyMemBytes( pDst, pSrc, appd_abTypesize[ bDataType ] );
      }
      else
      {
         SYS_CopyMemBytes( (UINT8*)temp, pSrc, appd_abTypesize[ bDataType ] );

         for( i = 0; i < appd_abTypesize[ bDataType ]; i++ )
         {
            ((UINT8*)pDst)[ i ] = ( (UINT8*)temp )[ ( appd_abTypesize[ bDataType ] - 1 ) - i ];
         }
      }
      (UINT8*)pSrc += appd_abTypesize[ bDataType ];
      (UINT8*)pDst += appd_abTypesize[ bDataType ];
   }
   return ( element * appd_abTypesize[ bDataType ] );
   
} /* End of appd_SwapToNetOrder() */


/*------------------------------------------------------------------------------
** appd_UpdateAdiVal()
**------------------------------------------------------------------------------
*/

UINT16 appd_UpdateAdiVal( const appd_InstanceType* psAdi, void* pVal, UINT8 bElement, SINT8* pbNotInRange )
{
   UINT8* pbDest;

   appd_SwapToNetOrder( pVal, 
                        pVal, 
                        psAdi->bDataType, 
                        psAdi->bNumberOfElements );

   if( appd_Compare( pVal, psAdi->pxMaxValue, psAdi->bDataType ) > 0 )
   {
      /*
      ** Value too high,
      */

      *pbNotInRange = 1;
   }
   else if( appd_Compare( pVal, psAdi->pxMinValue, psAdi->bDataType ) < 0 )
   {
      /*
      ** Value too low,
      */

      *pbNotInRange = -1;
   }
   else
   {
      /*
      ** Update the corresponding ADI value
      */

      *pbNotInRange = 0;

      /*
      ** Handle adi's
      */

      pbDest = (UINT8*) psAdi->pxValue +
                  ( appd_abTypesize[ psAdi->bDataType ] * bElement );

      if( SYS_CompareMemBytes( pbDest, pVal, appd_abTypesize[ psAdi->bDataType ] ) )
      {
         /*
         ** Data differs. Copy it.
         */

         SYS_CopyMemBytes( pbDest, pVal, appd_abTypesize[ psAdi->bDataType ] );
      }
   }
   return ( appd_abTypesize[ psAdi->bDataType ] );

} /* End of appd_UpdateAdiVal() */


/*******************************************************************************
**
** End of appd.c
**
********************************************************************************
*/