/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** appc.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public header file for the APPC object.
**
********************************************************************************
********************************************************************************
**
** Services List
** -------------
**
** Public Services:
**
**    APPC_Init()                - Initialize APPC client.
**    APPC_ProcessRespMsg()      - Processes responses to commands sent by APPC
**    APPC_RunStateMachine()     - Runs the APPC state machine
**    APPC_SetSwitch1()          - Updates DIP Switch settings.
**    APPC_SetSwitch2()          - Updates DIP Switch settings.
**
** Private Services:
**
**    appc_GetAttribute()        - Builds and sends a 'Get Attribute' command.
**    appc_Min()                 - Returns the lowest of two values.
**    appc_SetByteAttribute()    - Builds and sends a 'Set Attribute' command.
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

#ifndef APPC_H
#define APPC_H

/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** The states in the APPC state machine
**------------------------------------------------------------------------------
*/

typedef enum APPC_StateType
{
   APPC_NW_TYPE,
   APPC_FIRMWARE,
   APPC_SERIAL_NO,
   APPC_DIP_SW1,
   APPC_DIP_SW2,
   APPC_SETUP_COMPLETE,
   APPC_DONE
}
APPC_StateType;


/*******************************************************************************
**
** Private Globals
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
**
** appc_bSwitch1
**
**------------------------------------------------------------------------------
**
** Holds the DIP switch 1 setting.
**
**------------------------------------------------------------------------------
*/

extern UINT8 appc_bSwitch1;


/*------------------------------------------------------------------------------
**
** appc_bSwitch2
**
**------------------------------------------------------------------------------
**
** Holds the DIP switch 2 setting.
**
**------------------------------------------------------------------------------
*/

extern UINT8 appc_bSwitch2;


/*------------------------------------------------------------------------------
**
** appc_fSwitch1Set
**
**------------------------------------------------------------------------------
**
** Flag inducating if DIP switch 1 have been set.
**
**------------------------------------------------------------------------------
*/

extern BOOL8 appc_fSwitch1Set;

/*------------------------------------------------------------------------------
**
** appc_fSwitch2Set
**
**------------------------------------------------------------------------------
**
** Flag inducating if DIP switch 2 have been set.
**
**------------------------------------------------------------------------------
*/

extern BOOL8 appc_fSwitch2Set;


/*------------------------------------------------------------------------------
**
** appc_eState
**
**------------------------------------------------------------------------------
**
** Holds a string describing the network type.
**
**------------------------------------------------------------------------------
*/

extern APPC_StateType appc_eState;


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** APPC_Init( )
**
** Initialize the APPC client
**------------------------------------------------------------------------------
**
** Inputs:
**    -
**
** Outputs:
**    -
**
** Usage:
**    APPC_Init( );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPC_Init( void );

/*------------------------------------------------------------------------------
** void APPC_ProcessRespMsg( )
**
** Processes responses to the APPC client
**------------------------------------------------------------------------------
**
** Inputs:
**    psNewMessage      - Pointer to a ABP_MsgType message.
**
** Outputs:
**    -
**
** Usage:
**    APPC_ProcessRespMsg( &sMessage );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPC_ProcessRespMsg( ABP_MsgType* psNewMessage );


/*------------------------------------------------------------------------------
** void APPC_RunStateMachine( )
**
** Runs the APPC state machine 
**------------------------------------------------------------------------------
**
** Inputs:
**    -
**
** Outputs:
**    -
**
** Usage:
**    APPC_RunStateMachine();
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPC_RunStateMachine( void );


/*------------------------------------------------------------------------------
** APPC_SetSwitch1()
**
** Set DIP switch 1 value
**------------------------------------------------------------------------------
**
** Inputs:
**    bSw1           - DIP Switch 1 value
**
** Outputs:
**    -
**
** Usage:
**    APPC_SetSwitch1( 123 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPC_SetSwitch1( UINT8 bSw1 );

/*------------------------------------------------------------------------------
** APPC_SetSwitch2()
**
** Set DIP switch 2 value
**------------------------------------------------------------------------------
**
** Inputs:
**    bSw1           - DIP Switch 2 value
**
** Outputs:
**    -
**
** Usage:
**    APPC_SetSwitch2( 3 );
**
**------------------------------------------------------------------------------
*/

EXTFUNC void APPC_SetSwitch2( UINT8 bSw2 );

/*******************************************************************************
**
** Private Services
**
********************************************************************************
*/

/*------------------------------------------------------------------------------
** AB_StatusType appc_GetAttribute( );
**
** This function create and send a "Get Attribute" command message.
**------------------------------------------------------------------------------
**
** Inputs:
**    bObject     - Destination object
**    iInstance   - Destination instance
**    bAttribute  - Destination attribute
**    bSourceId   - Message source ID
**
** Outputs:
**    Returns     - Status of the request:
**                  AB_STATUS_OK                      - Request sent OK.
**                  AB_STATUS_NOT_ALLOWED_DRIVER_STATE - Module is in configuration
**                                                      mode, appl cannot send messages.
**                  AB_STATUS_OUT_OF_RESOURCES        - Command buffer is full.
**
** Usage:
**    appc_GetAttribute( ABP_OBJ_NUM_ANB,
**                       1,
**                       ABP_ANB_IA_MODULE_TYPE,
**                       123 );
**
**------------------------------------------------------------------------------
*/

ABCC_StatusType appc_GetAttribute( UINT8 bObject, 
                                   UINT16 iInstance, 
                                   UINT8 bAttribute, 
                                   UINT8 bSourceId );


/*------------------------------------------------------------------------------
**
** appc_Min()
**
** Returns the lowest of two values.
**
**------------------------------------------------------------------------------
**
** Inputs:
**    a        - Val 1
**    b        - Val 2
**
** Outputs:
**    Returns  - The lowest of a and b
**
** Usage:
**    x = appc_Min( 3, 7 );
**
**------------------------------------------------------------------------------
*/

#define appc_Min( a, b ) ( (a) < (b) ? (a) : (b) )


/*------------------------------------------------------------------------------
** AB_StatusType appc_SetByteAttribute( )
**
** This function create and send a "Set Attribute" command message.
** It works only for single byte attributes.
**------------------------------------------------------------------------------
**
** Inputs:
**    bObject     - Destination object
**    iInstance   - Destination instance
**    bAttribute  - Destination attribute
**    bVal        - Byte value
**    bSourceId   - Message source ID
**
** Outputs:
**    Returns     - Status of the request:
**                  AB_STATUS_OK                      - Request sent OK.
**                  AB_STATUS_NOT_ALLOWED_DRIVER_STATE - Module is in configuration
**                                                      mode, appl cannot send messages.
**                  AB_STATUS_OUT_OF_RESOURCES        - Command buffer is full.
**
** Usage:
**    appc_SetByteAttribute( ABP_OBJ_NUM_ANB,
**                           1,
**                           ABP_ANB_IA_SETUP_COMPLETE,
**                           TRUE,
**                           123 );
**
**------------------------------------------------------------------------------
*/

ABCC_StatusType appc_SetByteAttribute( UINT8 bObject, 
                                       UINT16 iInstance, 
                                       UINT8 bAttribute, 
                                       UINT8 bVal, 
                                       UINT8 bSourceId );


#endif  /* inclusion lock */


/*******************************************************************************
**
** End of appc.h
**
********************************************************************************
*/