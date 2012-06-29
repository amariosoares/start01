/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** ABP_DPV1.H
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** ABP - Anybus-CC Protocol Definitions.
**
** This software component contains protocol definitions used by Anybus-CC
** modules as well as applications designed to use such modules.
**
** This file contains the Profibus DPV1 specific definitions.
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

#ifndef ABP_DPV1_H
#define ABP_DPV1_H


/*******************************************************************************
**
** Additional diagnostic object constants.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Additional diagnotic object specific object attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_ADD_OA_MAX_INST            11
#define ABP_ADD_OA_EXT_DIAG_OVERFLOW   12
#define ABP_ADD_OA_STATIC_DIAG         13


/*------------------------------------------------------------------------------
**
** The data size of the Additional diagnostic object specific object
** attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_ADD_OA_MAX_INST_DS            ABP_UINT16_SIZEOF
#define ABP_ADD_OA_EXT_DIAG_OVERFLOW_DS   ABP_UINT8_SIZEOF
#define ABP_ADD_OA_STATIC_DIAG_DS         ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** Additional Diagnostic instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_ADD_IA_MODULE_NUMBER    1
#define ABP_ADD_IA_IO_TYPE          2
#define ABP_ADD_IA_CHANNEL_NUMBER   3
#define ABP_ADD_IA_CHANNEL_TYPE     4
#define ABP_ADD_IA_ERROR_TYPE       5


/*------------------------------------------------------------------------------
**
** The data size of the Additional Diagnostic instance attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_ADD_IA_MODULE_NUMBER_DS    ABP_UINT8_SIZEOF
#define ABP_ADD_IA_IO_TYPE_DS          ABP_UINT8_SIZEOF
#define ABP_ADD_IA_CHANNEL_NUMBER_DS   ABP_UINT8_SIZEOF
#define ABP_ADD_IA_CHANNEL_TYPE_DS     ABP_UINT8_SIZEOF
#define ABP_ADD_IA_ERROR_TYPE_DS       ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** Additional Diagnostic object specific error codes
**
**------------------------------------------------------------------------------
*/

enum
{
   ABP_ADD_ERR_NO_ERROR                = 0x00,
   ABP_ADD_ERR_INVALID_MODULE_NUMBER   = 0x01,
   ABP_ADD_ERR_INVALID_IO_TYPE         = 0x02,
   ABP_ADD_ERR_INVALID_CHANNEL_NUMBER  = 0x03,
   ABP_ADD_ERR_INVALID_CHANNEL_TYPE    = 0x04,
   ABP_ADD_ERR_INVALID_ERROR_TYPE      = 0x05
};

/*------------------------------------------------------------------------------
**
** Maximum values of Additional diagnostic instance attributes
**
**------------------------------------------------------------------------------
*/

#define ABP_ADD_MODULE_NUMBER_MAX   0x3F
#define ABP_ADD_CHANNEL_NUMBER_MAX  0x3F


/*------------------------------------------------------------------------------
**
** Values of IO type attribute
**
**------------------------------------------------------------------------------
*/

enum
{
   ABP_ADD_IO_TYPE_INPUT         = 0x01,
   ABP_ADD_IO_TYPE_OUTPUT        = 0x02,
   ABP_ADD_IO_TYPE_INPUT_OUTPUT  = 0x03
};


/*------------------------------------------------------------------------------
**
** Values of Channel type attribute
**
**------------------------------------------------------------------------------
*/

enum
{
   ABP_ADD_CHANNEL_TYPE_BIT      = 0x01,
   ABP_ADD_CHANNEL_TYPE_2_BITS   = 0x02,
   ABP_ADD_CHANNEL_TYPE_4_BITS   = 0x03,
   ABP_ADD_CHANNEL_TYPE_BYTE     = 0x04,
   ABP_ADD_CHANNEL_TYPE_WORD     = 0x05,
   ABP_ADD_CHANNEL_TYPE_2_WORDS  = 0x06
};


/*------------------------------------------------------------------------------
**
** Values of Error type attribute
**
** 0x00 - 0x09: Defined error codes in Profibus specification
** 0x0A - 0x0F: Reserved
** 0x10 - 0x1F: Device related
**
**------------------------------------------------------------------------------
*/

enum
{
   ABP_ADD_ERROR_TYPE_SHORT_CIRCUIT          = 0x01,
   ABP_ADD_ERROR_TYPE_UNDER_VOLTAGE          = 0x02,
   ABP_ADD_ERROR_TYPE_OVER_VOLTAGE           = 0x03,
   ABP_ADD_ERROR_TYPE_OVERLOAD               = 0x04,
   ABP_ADD_ERROR_TYPE_OVER_TEMPERATURE       = 0x05,
   ABP_ADD_ERROR_TYPE_WIRE_BREAK             = 0x06,
   ABP_ADD_ERROR_TYPE_UPPER_LIMIT_EXCEEDED   = 0x07,
   ABP_ADD_ERROR_TYPE_LOWER_LIMIT_EXCEEDED   = 0x08,
   ABP_ADD_ERROR_TYPE_ERROR_GENERAL          = 0x09,

   ABP_ADD_ERROR_TYPE_MAX                    = 0x1F
};


/*******************************************************************************
**
** Profibus DPV1 object constants.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Profibus DPV1 instance attributes
**
**------------------------------------------------------------------------------
*/

#define ABP_DPV1_IA_IDENT_NUMBER         1
#define ABP_DPV1_IA_PRM_DATA             2
#define ABP_DPV1_IA_EXPECTED_CFG_DATA    3
#define ABP_DPV1_IA_SSA_ENABLED          4
#define ABP_DPV1_IA_SIZEOF_ID_REL_DIAG   5
#define ABP_DPV1_IA_BUFFER_MODE          6


/*------------------------------------------------------------------------------
**
** The data size of the Profibus DPV1 instance attributes
**
**------------------------------------------------------------------------------
*/

#define ABP_DPV1_IA_IDENT_NUMBER_DS         ABP_UINT16_SIZEOF
#define ABP_DPV1_IA_SSA_ENABLED_DS          ABP_BOOL_SIZEOF
#define ABP_DPV1_IA_SIZEOF_ID_REL_DIAG_DS   ABP_UINT8_SIZEOF
#define ABP_DPV1_IA_BUFFER_MODE_DS          ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** Values of Buffer mode attribute
**
**------------------------------------------------------------------------------
*/

enum
{
   ABP_DPV1_DEF_BUFFER_MODE = 0,
   ABP_DPV1_PRM_BUFFER_MODE,
   ABP_DPV1_PD_BUFFER_MODE
};


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of ABP.H
**
********************************************************************************
*/
