/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** abp.h
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
** This file contains the generic portion used by all Anybus-CC modules.
**
** Network specific definitions that may be required by some applications are
** published in the corresponding abp_xxx.h file(s).
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    ABP_SetMsgErrorResponse()  - Convert message command to an error response.
**    ABP_SetMsgResponse()       - Convert message command to a response.
**
********************************************************************************
********************************************************************************
**                                                                            **
** COPYRIGHT NOTIFICATION (c) 2009 HMS Industrial Networks AB                 **
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
*/

#ifndef ABP_H
#define ABP_H



/*******************************************************************************
**
** Common telegram and message constants
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** Control register bitmasks.
**
**------------------------------------------------------------------------------
*/

#define ABP_CTRL_T_BIT              0x80
#define ABP_CTRL_M_BIT              0x40
#define ABP_CTRL_R_BIT              0x20
#define ABP_CTRL_A_BIT              0x10
#define ABP_CTRL_G_BIT              0x01


/*------------------------------------------------------------------------------
**
** Status register bitmasks.
**
**------------------------------------------------------------------------------
*/

#define ABP_STAT_T_BIT              0x80
#define ABP_STAT_M_BIT              0x40
#define ABP_STAT_R_BIT              0x20
#define ABP_STAT_A_BIT              0x10
#define ABP_STAT_SUP_BIT            0x08
#define ABP_STAT_S_BITS             0x07


/*------------------------------------------------------------------------------
**
** The maximum amount of process data in a telegram (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_MAX_PROCESS_DATA        256


/*------------------------------------------------------------------------------
**
** The maximum amount of data in a message data field (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_MAX_MSG_DATA_BYTES      255


/*------------------------------------------------------------------------------
**
** Message header bitmasks.
**
**------------------------------------------------------------------------------
*/

#define ABP_MSG_HEADER_C_BIT        0x40
#define ABP_MSG_HEADER_E_BIT        0x80
#define ABP_MSG_HEADER_CMD_BITS     0x3F


/*------------------------------------------------------------------------------
**
** Message commands.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_MsgCmdType
{
   ABP_CMD_GET_ATTR            = 0x01,    /* Get attribute                    */
   ABP_CMD_SET_ATTR            = 0x02,    /* Set attribute                    */
   ABP_CMD_CREATE              = 0x03,    /* Create                           */
   ABP_CMD_DELETE              = 0x04,    /* Delete                           */
   ABP_CMD_RESET               = 0x05,    /* Reset                            */
   ABP_CMD_GET_ENUM_STR        = 0x06,    /* Get enumeration string           */
   ABP_CMD_GET_INDEXED_ATTR    = 0x07,    /* Get indexed attribute            */
   ABP_CMD_SET_INDEXED_ATTR    = 0x08     /* Set indexed attribute            */
}
ABP_MsgCmdType;


/*------------------------------------------------------------------------------
**
** Reset command types.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_ResetType
{
   ABP_RESET_POWER_ON                  = 0x00,  /* Power-on reset             */
   ABP_RESET_FACTORY_DEFAULT           = 0x01,  /* Factory default reset      */
   ABP_RESET_POWER_ON_FACTORY_DEFAULT  = 0x02   /* Power-on + Factory reset   */
}
ABP_ResetType;


/*------------------------------------------------------------------------------
**
** Message error codes.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_MsgErrorCodeType
{
   ABP_ERR_NO_ERROR            = 0x00,    /* No error                         */

   ABP_ERR_INV_MSG_FORMAT      = 0x02,    /* Invalid message format           */
   ABP_ERR_UNSUP_OBJ           = 0x03,    /* Unsupported object               */
   ABP_ERR_UNSUP_INST          = 0x04,    /* Unsupported instance             */
   ABP_ERR_UNSUP_CMD           = 0x05,    /* Unsupported command              */
   ABP_ERR_INV_CMD_EXT_0       = 0x06,    /* Invalid CmdExt[ 0 ]              */
   ABP_ERR_INV_CMD_EXT_1       = 0x07,    /* Invalid CmdExt[ 1 ]              */
   ABP_ERR_ATTR_NOT_SETABLE    = 0x08,    /* Attribute access is not set-able */
   ABP_ERR_ATTR_NOT_GETABLE    = 0x09,    /* Attribute access is not get-able */
   ABP_ERR_TOO_MUCH_DATA       = 0x0A,    /* Too much data in msg data field  */
   ABP_ERR_NOT_ENOUGH_DATA     = 0x0B,    /* Not enough data in msg data field*/
   ABP_ERR_OUT_OF_RANGE        = 0x0C,    /* Out of range                     */
   ABP_ERR_INV_STATE           = 0x0D,    /* Invalid state                    */
   ABP_ERR_NO_RESOURCES        = 0x0E,    /* Out of resources                 */
   ABP_ERR_SEG_FAILURE         = 0x0F,    /* Segmentation failure             */
   ABP_ERR_SEG_BUF_OVERFLOW    = 0x10,    /* Segmentation buffer overflow     */

   ABP_ERR_OBJ_SPECIFIC        = 0xFF     /* Object specific error            */
}
ABP_MsgErrorCodeType;


/*******************************************************************************
**
** Anybus-CC data types.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Anybus-CC data type numbers.
**
**------------------------------------------------------------------------------
*/

#define ABP_BOOL                    0        /* Boolean                       */
#define ABP_SINT8                   1        /* Signed 8 bit integer          */
#define ABP_SINT16                  2        /* Signed 16 bit integer         */
#define ABP_SINT32                  3        /* Signed 32 bit integer         */
#define ABP_UINT8                   4        /* Unsigned 8 bit integer        */
#define ABP_UINT16                  5        /* Unsigned 16 bit integer       */
#define ABP_UINT32                  6        /* Unsigned 32 bit integer       */
#define ABP_CHAR                    7        /* Character                     */
#define ABP_ENUM                    8        /* Enumeration                   */

#define ABP_SINT64                  16       /* Signed 64 bit integer         */
#define ABP_UINT64                  17       /* Unsigned 64 bit integer       */
#define ABP_FLOAT                   18       /* Floating point/real number    */


/*------------------------------------------------------------------------------
**
** The size of the Anybus-CC data types (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_BOOL_SIZEOF             1        /* Boolean                       */
#define ABP_SINT8_SIZEOF            1        /* Signed 8 bit integer          */
#define ABP_SINT16_SIZEOF           2        /* Signed 16 bit integer         */
#define ABP_SINT32_SIZEOF           4        /* Signed 32 bit integer         */
#define ABP_UINT8_SIZEOF            1        /* Unsigned 8 bit integer        */
#define ABP_UINT16_SIZEOF           2        /* Unsigned 16 bit integer       */
#define ABP_UINT32_SIZEOF           4        /* Unsigned 32 bit integer       */
#define ABP_CHAR_SIZEOF             1        /* Character                     */
#define ABP_ENUM_SIZEOF             1        /* Enumeration                   */

#define ABP_SINT64_SIZEOF           8        /* Signed 64 bit integer         */
#define ABP_UINT64_SIZEOF           8        /* Unsigned 64 bit integer       */
#define ABP_FLOAT_SIZEOF            4        /* Floating point/real number    */


/*------------------------------------------------------------------------------
**
** The Anybus-CC data type maximum values.
**
**------------------------------------------------------------------------------
*/

#define ABP_BOOL_MAX                !0
#define ABP_SINT8_MAX               0x7F
#define ABP_SINT16_MAX              0x7FFF
#define ABP_SINT32_MAX              0x7FFFFFFFL
#define ABP_UINT8_MAX               0xFFU
#define ABP_UINT16_MAX              0xFFFFU
#define ABP_UINT32_MAX              0xFFFFFFFFLU
#define ABP_CHAR_MAX                0xFFU
#define ABP_ENUM_MAX                0xFFU

#define ABP_SINT64_MAX              0x7FFFFFFFFFFFFFFFL
#define ABP_UINT64_MAX              0xFFFFFFFFFFFFFFFFLU
#define ABP_FLOAT_MAX               3.40282347E+38F


/*------------------------------------------------------------------------------
**
** The Anybus-CC data type minimum values.
**
**------------------------------------------------------------------------------
*/

#define ABP_BOOL_MIN                0
#define ABP_SINT8_MIN               ( - ABP_SINT8_MAX - 1 )
#define ABP_SINT16_MIN              ( - ABP_SINT16_MAX - 1 )
#define ABP_SINT32_MIN              ( - ABP_SINT32_MAX - 1L )
#define ABP_UINT8_MIN               0
#define ABP_UINT16_MIN              0
#define ABP_UINT32_MIN              0
#define ABP_CHAR_MIN                0
#define ABP_ENUM_MIN                0

#define ABP_SINT64_MIN              ( - ABP_SINT64_MAX - 1 )
#define ABP_UINT64_MIN              0
#define ABP_FLOAT_MIN               1.17549435E-38F


/*******************************************************************************
**
** The languages supported by multilingual objects.
**
********************************************************************************
*/

typedef enum ABP_LangType
{
   ABP_LANG_ENG,                          /* English                          */
   ABP_LANG_DEU,                          /* German                           */
   ABP_LANG_SPA,                          /* Spanish                          */
   ABP_LANG_ITA,                          /* Italian                          */
   ABP_LANG_FRA,                          /* French                           */

   ABP_LANG_NUM_LANG                      /* Number of supported languages    */
}
ABP_LangType;


/*******************************************************************************
**
** Anybus-CC protocol object number pool.
**
** Each object, whether it’s a network specific or a common object, an Anybus
** module or a host application object, must have a unique object number. This
** number list is therefore common to both the application and the Anybus.
**
** Anybus objects are numbered from 1 and up while application objects
** are numbered from 255 and down.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** Anybus module objects
**------------------------------------------------------------------------------
*/

#define ABP_OBJ_NUM_ANB               1   /* Anybus                           */
#define ABP_OBJ_NUM_DI                2   /* Diagnostic                       */
#define ABP_OBJ_NUM_NW                3   /* Network                          */
#define ABP_OBJ_NUM_NC                4   /* Network Configuration            */
#define ABP_OBJ_NUM_ADD               5   /* PROFIBUS DP-V1 Additional Diag   */
#define ABP_OBJ_NUM_RSV1              6   /* Reserved                         */
#define ABP_OBJ_NUM_SOC               7   /* Socket Interface                 */
#define ABP_OBJ_NUM_NWCCL             8   /* Network CC-Link                  */
#define ABP_OBJ_NUM_SMTP              9   /* SMTP Client                      */
#define ABP_OBJ_NUM_FSI              10   /* File System Interface            */
#define ABP_OBJ_NUM_NWDPV1           11   /* Network PROFIBUS DP-V1           */
#define ABP_OBJ_NUM_NWETN            12   /* Network Ethernet                 */
#define ABP_OBJ_NUM_CPC              13   /* CIP Port Configuration           */
#define ABP_OBJ_NUM_NWPNIO           14   /* Network PROFINET IO              */
#define ABP_OBJ_NUM_PNIOADD          15   /* PROFINET IO Additional Diag      */


/*------------------------------------------------------------------------------
** Host application objects
**------------------------------------------------------------------------------
*/

#define ABP_OBJ_NUM_SRC3            241   /* SERCOS III                       */
#define ABP_OBJ_NUM_PRD             242   /* PROFIdrive                       */
#define ABP_OBJ_NUM_CNT             243   /* ControlNet                       */
#define ABP_OBJ_NUM_CPN             244   /* CompoNet                         */
#define ABP_OBJ_NUM_ECT             245   /* EtherCAT                         */
#define ABP_OBJ_NUM_PNIO            246   /* PROFINET IO                      */
#define ABP_OBJ_NUM_CCL             247   /* CC-Link                          */
#define ABP_OBJ_NUM_EIP             248   /* EtherNet/IP                      */
#define ABP_OBJ_NUM_ETN             249   /* Ethernet                         */
#define ABP_OBJ_NUM_MOD             250   /* Modbus                           */
#define ABP_OBJ_NUM_COP             251   /* CANopen                          */
#define ABP_OBJ_NUM_DEV             252   /* DeviceNet                        */
#define ABP_OBJ_NUM_DPV1            253   /* PROFIBUS DP-V1                   */
#define ABP_OBJ_NUM_APPD            254   /* Application Data                 */
#define ABP_OBJ_NUM_APP             255   /* Application                      */


/*******************************************************************************
**
** Common object constants.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The object standard attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_OA_NAME                 1     /* Object name                      */
#define ABP_OA_REV                  2     /* Object revision                  */
#define ABP_OA_NUM_INST             3     /* Current number of instances      */
#define ABP_OA_HIGHEST_INST         4     /* Current highest instance number  */


/*------------------------------------------------------------------------------
**
** The data size of the object standard attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_OA_REV_DS               ABP_UINT8_SIZEOF
#define ABP_OA_NUM_INST_DS          ABP_UINT16_SIZEOF
#define ABP_OA_HIGHEST_INST_DS      ABP_UINT16_SIZEOF


/*------------------------------------------------------------------------------
**
** The object instance number.
**
**------------------------------------------------------------------------------
*/

#define ABP_INST_OBJ                0


/*******************************************************************************
**
** Anybus object constants.
**
** Object revision: 4.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Anybus instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_ANB_IA_MODULE_TYPE      1
#define ABP_ANB_IA_FW_VERSION       2
#define ABP_ANB_IA_SERIAL_NUM       3
#define ABP_ANB_IA_WD_TIMEOUT       4
#define ABP_ANB_IA_SETUP_COMPLETE   5
#define ABP_ANB_IA_EXCEPTION        6
#define ABP_ANB_IA_FATAL_EVENT      7
#define ABP_ANB_IA_ERROR_CNTRS      8
#define ABP_ANB_IA_LANG             9
#define ABP_ANB_IA_PROVIDER_ID      10
#define ABP_ANB_IA_PROVIDER_INFO    11
#define ABP_ANB_IA_LED_COLOURS      12
#define ABP_ANB_IA_LED_STATUS       13
#define ABP_ANB_IA_RESERVED1        14
#define ABP_ANB_IA_AUX_BIT_FUNC     15


/*------------------------------------------------------------------------------
**
** The data size of the Anybus instance attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_ANB_IA_MODULE_TYPE_DS         ABP_UINT16_SIZEOF
#define ABP_ANB_IA_FW_VERSION_DS          ( 3 * ABP_UINT8_SIZEOF )
#define ABP_ANB_IA_SERIAL_NUM_DS          ABP_UINT32_SIZEOF
#define ABP_ANB_IA_WD_TIMEOUT_DS          ABP_UINT16_SIZEOF
#define ABP_ANB_IA_SETUP_COMPLETE_DS      ABP_BOOL_SIZEOF
#define ABP_ANB_IA_EXCEPTION_DS           ABP_ENUM_SIZEOF
#define ABP_ANB_IA_ERROR_CNTRS_DS         ( 4 * ABP_UINT16_SIZEOF )
#define ABP_ANB_IA_LANG_DS                ABP_ENUM_SIZEOF
#define ABP_ANB_IA_PROVIDER_ID_DS         ABP_UINT16_SIZEOF
#define ABP_ANB_IA_PROVIDER_INFO_DS       ABP_UINT16_SIZEOF
#define ABP_ANB_IA_LED_COLOURS_DS         ( 4 * ABP_UINT8_SIZEOF )
#define ABP_ANB_IA_LED_STATUS_DS          ABP_UINT8_SIZEOF
#define ABP_ANB_IA_AUX_BIT_FUNC_DS        ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** The Anybus object specific error codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_ANB_ERR_INV_PRD_CFG       0x01  /* Invalid process data config    */
#define ABP_ANB_ERR_INV_DEV_ADDR      0x02  /* Invalid device address         */
#define ABP_ANB_ERR_INV_COM_SETTINGS  0x03  /* Invalid communication settings */


/*------------------------------------------------------------------------------
**
** Anybus state.
**
** The current Anybus state is presented by status bits S[0..2] of the
** status register, transferred to the application in each telegram.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_AnbStateType
{
   ABP_ANB_STATE_SETUP             =  0x00,
   ABP_ANB_STATE_NW_INIT           =  0x01,
   ABP_ANB_STATE_WAIT_PROCESS      =  0x02,
   ABP_ANB_STATE_IDLE              =  0x03,
   ABP_ANB_STATE_PROCESS_ACTIVE    =  0x04,
   ABP_ANB_STATE_ERROR             =  0x05,
   ABP_ANB_STATE_EXCEPTION         =  0x07
}
ABP_AnbStateType;


/*------------------------------------------------------------------------------
**
** Anybus exception codes.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_AnbExceptionCodeType
{
   ABP_ANB_EXCPT_NONE               = 0x00, /* No exception                   */
   ABP_ANB_EXCPT_APP_TO             = 0x01, /* Application timeout            */
   ABP_ANB_EXCPT_INV_DEV_ADDR       = 0x02, /* Invalid device address         */
   ABP_ANB_EXCPT_INV_COM_SETTINGS   = 0x03, /* Invalid communication settings */
   ABP_ANB_EXCPT_MAJ_UNREC_APP_EVNT = 0x04, /* Major unrecoverable app event  */
   ABP_ANB_EXCPT_WAIT_APP_RESET     = 0x05, /* Waiting for application reset  */
   ABP_ANB_EXCPT_INV_PRD_CFG        = 0x06, /* Invalid process data config    */
   ABP_ANB_EXCPT_INV_APP_RESPONSE   = 0x07, /* Invalid application response   */
   ABP_ANB_EXCPT_NVS_CHECKSUM_ERROR = 0x08, /* NVS memory checksum error      */

   ABP_ANB_EXCPT_NUM_CODES                  /* Number of exception codes      */
}
ABP_AnbExceptionCodeType;


/*------------------------------------------------------------------------------
**
** LED colour codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_ANB_LED_COLOUR_NONE     0x00
#define ABP_ANB_LED_COLOUR_GREEN    0x01
#define ABP_ANB_LED_COLOUR_RED      0x02
#define ABP_ANB_LED_COLOUR_YELLOW   0x03
#define ABP_ANB_LED_COLOUR_ORANGE   0x04
#define ABP_ANB_LED_COLOUR_BLUE     0x05
#define ABP_ANB_LED_COLOUR_WHITE    0x06


/*------------------------------------------------------------------------------
**
** Auxiliary bit functionality codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_ANB_AUX_BIT_FUNC_NONE   0x00     /* Not used                      */
#define ABP_ANB_AUX_BIT_FUNC_CDI    0x01     /* Changed data indication       */


/*******************************************************************************
**
** Diagnostic object constants.
**
** Object revision: 1.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Diagnostic object specific object attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_DI_OA_MAX_INST                11    /* Max number of instances    */


/*------------------------------------------------------------------------------
**
** The data size of the Diagnostic object specific attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_DI_OA_MAX_INST_DS             ABP_UINT16_SIZEOF


/*------------------------------------------------------------------------------
**
** The Diagnostic instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_DI_IA_SEVERITY                1
#define ABP_DI_IA_EVENT_CODE              2
#define ABP_DI_IA_NW_SPEC_EVENT_INFO      3


/*------------------------------------------------------------------------------
**
** The data size of the Diagnostic instance attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_DI_IA_SEVERITY_DS             ABP_UINT8_SIZEOF
#define ABP_DI_IA_EVENT_CODE_DS           ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** The Diagnostic object specific error codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_DI_ERR_NOT_REMOVED            0x01  /* Event could not be removed */


/*------------------------------------------------------------------------------
**
** Diagnostic object event severity.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_DiEventSeverityType
{
   ABP_DI_EVENT_SEVERITY_MINOR_REC   = 0x00, /* Minor, recoverable            */
   ABP_DI_EVENT_SEVERITY_MINOR_UNREC = 0x10, /* Minor, unrecoverable          */
   ABP_DI_EVENT_SEVERITY_MAJOR_REC   = 0x20, /* Major, recoverable            */
   ABP_DI_EVENT_SEVERITY_MAJOR_UNREC = 0x30  /* Major, unrecoverable          */
}
ABP_DiEventSeverityType;


/*------------------------------------------------------------------------------
**
** Diagnostic object event codes.
**
**------------------------------------------------------------------------------
*/

typedef enum ABP_DiEventCodeType
{
   ABP_DI_EVENT_NONE                  = 0x00, /* No event                     */
   ABP_DI_EVENT_GENERIC_ERROR         = 0x10, /* Generic Error                */
   ABP_DI_EVENT_CURRENT               = 0x20, /* Current                      */
   ABP_DI_EVENT_CURRENT_DEVICE_INPUT  = 0x21, /* Current, device input side   */
   ABP_DI_EVENT_CURRENT_INSIDE        = 0x22, /* Current, inside the device   */
   ABP_DI_EVENT_CURRENT_DEVICE_OUTPUT = 0x23, /* Current, device output side  */
   ABP_DI_EVENT_VOLTAGE               = 0x30, /* Voltage                      */
   ABP_DI_EVENT_MAINS_VOLTAGE         = 0x31, /* Mains Voltage                */
   ABP_DI_EVENT_VOLTAGE_INSIDE_DEVICE = 0x32, /* Voltage inside the device    */
   ABP_DI_EVENT_OUTPUT_VOLTAGE        = 0x33, /* Output Voltage               */
   ABP_DI_EVENT_TEMPERATURE           = 0x40, /* Temperature                  */
   ABP_DI_EVENT_AMBIENT_TEMPERATURE   = 0x41, /* Ambient Temperature          */
   ABP_DI_EVENT_DEVICE_TEMPERATURE    = 0x42, /* Device Temperature           */
   ABP_DI_EVENT_DEVICE_HARDWARE       = 0x50, /* Device Hardware              */
   ABP_DI_EVENT_DEVICE_SOFTWARE       = 0x60, /* Device Software              */
   ABP_DI_EVENT_INTERNAL_SOFTWARE     = 0x61, /* Internal Software            */
   ABP_DI_EVENT_USER_SOFTWARE         = 0x62, /* User Software                */
   ABP_DI_EVENT_DATA_SET              = 0x63, /* Data Set                     */
   ABP_DI_EVENT_ADDITIONAL_MODULES    = 0x70, /* Additional Modules           */
   ABP_DI_EVENT_MONITORING            = 0x80, /* Monitoring                   */
   ABP_DI_EVENT_COMMUNICATION         = 0x81, /* Communication                */
   ABP_DI_EVENT_PROTOCOL_ERROR        = 0x82, /* Protocol Error               */
   ABP_DI_EVENT_EXTERNAL_ERROR        = 0x90, /* External Error               */
   ABP_DI_EVENT_ADDITIONAL_FUNCTIONS  = 0xF0, /* Additional Functions         */
   ABP_DI_EVENT_NW_SPECIFIC           = 0xFF  /* Network specific             */
}
ABP_DiEventCodeType;


/*******************************************************************************
**
** Network object constants.
**
** Object revision: 2.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Network instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_NW_IA_NW_TYPE                 1
#define ABP_NW_IA_NW_TYPE_STR             2
#define ABP_NW_IA_DATA_FORMAT             3
#define ABP_NW_IA_PARAM_SUPPORT           4
#define ABP_NW_IA_WRITE_PD_SIZE           5
#define ABP_NW_IA_READ_PD_SIZE            6
#define ABP_NW_IA_EXCEPTION_INFO          7


/*------------------------------------------------------------------------------
**
** The data size of the Network instance attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_NW_IA_NW_TYPE_DS              ABP_UINT16_SIZEOF
#define ABP_NW_IA_DATA_FORMAT_DS          ABP_ENUM_SIZEOF
#define ABP_NW_IA_PARAM_SUPPORT_DS        ABP_BOOL_SIZEOF
#define ABP_NW_IA_WRITE_PD_SIZE_DS        ABP_UINT16_SIZEOF
#define ABP_NW_IA_READ_PD_SIZE_DS         ABP_UINT16_SIZEOF
#define ABP_NW_IA_EXCEPTION_INFO_DS       ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** The Network object specific message commands.
**
**------------------------------------------------------------------------------
*/

#define ABP_NW_CMD_MAP_ADI_WRITE_AREA        0x10
#define ABP_NW_CMD_MAP_ADI_READ_AREA         0x11


/*------------------------------------------------------------------------------
**
** The Network object specific error codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_NW_ERR_INVALID_ADI_DATA_TYPE  0x01  /* Invalid ADI data type      */
#define ABP_NW_ERR_INVALID_NUM_ELEMENTS   0x02  /* Invalid number of elements */
#define ABP_NW_ERR_INVALID_TOTAL_SIZE     0x03  /* Invalid total size         */
#define ABP_NW_ERR_MULTIPLE_MAPPING       0x04  /* Multiple mapping           */
#define ABP_NW_ERR_INVALID_ORDER_NUM      0x05  /* Invalid ADI order number   */
#define ABP_NW_ERR_INVALID_MAP_CMD_SEQ    0x06  /* Invalid mapp cmd sequence  */


/*------------------------------------------------------------------------------
**
** Common Network specific exception information codes.
**
** Please see the network specific header file for the remaining codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_NW_EXCPT_INFO_NONE            0x00  /* No information             */


/*******************************************************************************
**
** Network configuration object constants.
**
** Object revision: 1.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Network configuration instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_NC_VAR_IA_NAME                1
#define ABP_NC_VAR_IA_DATA_TYPE           2
#define ABP_NC_VAR_IA_NUM_ELEM            3
#define ABP_NC_VAR_IA_DESCRIPTOR          4
#define ABP_NC_VAR_IA_VALUE               5


/*------------------------------------------------------------------------------
**
** The data size of the Network configuration instance attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_NC_VAR_IA_DATA_TYPE_DS        ABP_UINT8_SIZEOF
#define ABP_NC_VAR_IA_NUM_ELEM_DS         ABP_UINT8_SIZEOF
#define ABP_NC_VAR_IA_DESCRIPTOR_DS       ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** The Network configuration instance attribute descriptor bits.
**
**------------------------------------------------------------------------------
*/

#define ABP_NC_DESCR_GET_ACCESS           0x01
#define ABP_NC_DESCR_SET_ACCESS           0x02
#define ABP_NC_DESCR_SHARED_ACCESS        0x04


/*------------------------------------------------------------------------------
**
** The Network configuration instances common to most Anybus-CC modules.
**
** Note:
**    1. Although all Network configuration instances are network specific,
**       the instances listed here are meant to offer some degree of ANY
**       functionality in that they are always of an eight-bit data type and
**       that they may be set by the application during setup.
**
**    2. In case the values of these instances originate from input devices
**       controlled by the end user (DIP switches or similar), the application
**       shall keep these instances updated at all times because some networks
**       require that a changed switch is indicated by the LED’s.
**
**------------------------------------------------------------------------------
*/

#define ABP_NC_INST_NUM_SW1               0x01
#define ABP_NC_INST_NUM_SW2               0x02


/*******************************************************************************
**
** Application data object constants.
**
** Object revision: 2.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Application data object instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_APPD_IA_NAME                  1
#define ABP_APPD_IA_DATA_TYPE             2
#define ABP_APPD_IA_NUM_ELEM              3
#define ABP_APPD_IA_DESCRIPTOR            4
#define ABP_APPD_IA_VALUE                 5
#define ABP_APPD_IA_MAX_VALUE             6
#define ABP_APPD_IA_MIN_VALUE             7
#define ABP_APPD_IA_DFLT_VALUE            8


/*------------------------------------------------------------------------------
**
** The Application data object attribute data sizes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_APPD_IA_DATA_TYPE_DS          ABP_UINT8_SIZEOF
#define ABP_APPD_IA_NUM_ELEM_DS           ABP_UINT8_SIZEOF
#define ABP_APPD_IA_DESCRIPTOR_DS         ABP_UINT8_SIZEOF


/*------------------------------------------------------------------------------
**
** The Application data instance attribute descriptor bits.
**
**------------------------------------------------------------------------------
*/

#define ABP_APPD_DESCR_GET_ACCESS         0x01
#define ABP_APPD_DESCR_SET_ACCESS         0x02
#define ABP_APPD_DESCR_MAPPABLE_WRITE_PD  0x08
#define ABP_APPD_DESCR_MAPPABLE_READ_PD   0x10


/*------------------------------------------------------------------------------
**
** The Application data object specific message commands.
**
**------------------------------------------------------------------------------
*/

#define ABP_APPD_CMD_GET_INST_BY_ORDER    0x10
#define ABP_APPD_GET_PROFILE_INST_NUMBERS 0x11
#define ABP_APPD_GET_ADI_INFO             0x12
#define ABP_APPD_REMAP_ADI_WRITE_AREA     0x13
#define ABP_APPD_REMAP_ADI_READ_AREA      0x14


/*------------------------------------------------------------------------------
**
** The Application data object specific error codes.
**
**------------------------------------------------------------------------------
*/

#define ABP_APPD_ERR_MAPPING_ITEM_NAK      0x01 /* Mapping item NAK           */
#define ABP_APPD_ERR_INVALID_TOTAL_SIZE    0x02 /* Invalid total size         */


/*******************************************************************************
**
** Application object constants.
**
** Object revision: 1.
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** The Application instance attributes.
**
**------------------------------------------------------------------------------
*/

#define ABP_APP_IA_CONFIGURED             1
#define ABP_APP_IA_SUP_LANG               2


/*------------------------------------------------------------------------------
**
** The data size of the Application instance attributes (in bytes).
**
**------------------------------------------------------------------------------
*/

#define ABP_APP_IA_CONFIGURED_DS          ABP_BOOL_SIZEOF


/*------------------------------------------------------------------------------
**
** The Application object specific message commands.
**
**------------------------------------------------------------------------------
*/

#define ABP_APP_CMD_RESET_REQUEST         0x10
#define ABP_APP_CMD_CHANGE_LANG_REQUEST   0x11


/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** ABP_MsgHeaderType
**
** Structure describing a message header.
**
**------------------------------------------------------------------------------
*/

typedef start_packet_struct ABP_MsgHeaderType
{
   UINT8    bSourceId;
   UINT8    bDestObj;
   UINT16   iInstance;
   UINT8    bCmd;
   UINT8    bDataSize;
   UINT8    bCmdExt0;
   UINT8    bCmdExt1;
}
end_packet_struct ABP_MsgHeaderType;


/*------------------------------------------------------------------------------
**
** ABP_MsgType
**
** Structure describing a message.
**
**------------------------------------------------------------------------------
*/

typedef start_packet_struct ABP_MsgType
{
   /*
   ** The message header part.
   */

   ABP_MsgHeaderType sHeader;

   /*
   ** The message data.
   */

   UINT8    abData[ ABP_MAX_MSG_DATA_BYTES ];
}
end_packet_struct ABP_MsgType;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** ABP_SetMsgErrorResponse()
**
** Converts a message command header into an error response header.
** It clears the C-bit, sets the E-bit and enters the submitted error code.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psMsg                - Pointer to the message command to convert.
**    bMsgDataSize         - The number of valid message data field bytes.
**    eErr                 - The requested error code (ABP_MsgErrorCodeType).
**
** Outputs:
**    None
**
** Usage:
**    ABP_SetMsgErrorResponse( psMsg, bMsgDataSize, eErr );
**
**------------------------------------------------------------------------------
*/

#define ABP_SetMsgErrorResponse( psMsg, bMsgDataSize, eErr )                   \
{                                                                              \
   (psMsg)->sHeader.bCmd      &= ~ABP_MSG_HEADER_C_BIT;                        \
   (psMsg)->sHeader.bCmd      |=  ABP_MSG_HEADER_E_BIT;                        \
   (psMsg)->sHeader.bDataSize  =  (bMsgDataSize);                              \
   (psMsg)->abData[ 0 ]        =  (UINT8)(eErr);                               \
                                                                               \
} /* end of ABP_SetMsgErrorResponse() */


/*------------------------------------------------------------------------------
**
** ABP_SetMsgResponse()
**
** Converts a message command header into a response header.
** It clears the C-bit and enters the submitted data size.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    psMsg                - Pointer to the message command to convert.
**    bMsgDataSize         - The number of valid message data field bytes.
**
** Outputs:
**    None
**
** Usage:
**    ABP_SetMsgResponse( psMsg, bMsgDataSize );
**
**------------------------------------------------------------------------------
*/

#define ABP_SetMsgResponse( psMsg, bMsgDataSize )                              \
{                                                                              \
   (psMsg)->sHeader.bCmd      &= ~ABP_MSG_HEADER_C_BIT;                        \
   (psMsg)->sHeader.bDataSize  =  (bMsgDataSize);                              \
                                                                               \
} /* end of ABP_SetMsgResponse() */


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of abp.h
**
********************************************************************************
*/
