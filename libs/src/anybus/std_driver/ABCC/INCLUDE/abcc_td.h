/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** abcc_td.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This file contains type definitions for data-types that are used by the driver.
** Minor porting might be used.
**
********************************************************************************
********************************************************************************
**
** Porting instructions:
** ---------------------
**
** Porting might be required (host dependent).
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

#ifndef ABCC_TD_H
#define ABCC_TD_H



/*******************************************************************************
**
** Constants
**
********************************************************************************
*/

/****************************************************************************
**
** Compiler dependent general purpose definitions
**
*****************************************************************************
*/

/*---------------------------------------------------------------------------
**
** FALSE
** TRUE
**
** These are the symbolic constants for true and false used in boolean
** data type comparisons.
**
**---------------------------------------------------------------------------
*/

#ifndef FALSE
   #define FALSE     0
#endif

#ifndef TRUE
   #define TRUE      ( !FALSE )
#endif


/*---------------------------------------------------------------------------
**
** BOOL8
**
** SINT8
** SINT16
** SINT32
**
** UINT8
** UINT16
** UINT32
**
** FLOAT32
**
** The standard boolean data type, 8-bit.
** The standard signed 8, 16, and 32 bit data types (respectively).
** The standard unsigned 8, 16, and 32 bit data types (respectively).
**
** PORTING ALERT!
** Make sure that the defined data types match your host system.
**
**---------------------------------------------------------------------------
*/

typedef unsigned char   BOOL8;
typedef signed char     SINT8;
typedef signed short    SINT16;
typedef signed int      SINT32;
typedef signed __int64  SINT64;

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32;
typedef unsigned __int64 UINT64;
typedef float           FLOAT32;


/*---------------------------------------------------------------------------
**
** NULL
**
** Default value for invalid pointers.
**
**---------------------------------------------------------------------------
*/

#ifndef NULL
#define NULL 0
#endif


/*------------------------------------------------------------------------------
**
** Define for struct
** Compiler independent symbols to pack structures for compilers that
** need an in-line directive.
**
** PORTING ALERT!
**
**------------------------------------------------------------------------------
*/

#define  start_packet_struct     struct
#define  end_packet_struct


/*---------------------------------------------------------------------------
**
** CPLUSPLUS
** EXTFUNC
**
** C/C++ Compatibility Macros (for C++ compatibility)
**
**---------------------------------------------------------------------------
*/

#ifdef __cplusplus
   #define CPLUSPLUS
#endif


#ifdef CPLUSPLUS
   #define EXTFUNC extern "C"
#else
   #define EXTFUNC extern
#endif


/*---------------------------------------------------------------------------
**
** ABCC_TD_pFuncType
**
** C/C++ Function Pointer Types - Without parameters
**
**---------------------------------------------------------------------------
*/

typedef void    (*ABCC_TD_pFuncType)( void );


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of abcc_td.h
**
********************************************************************************
*/
