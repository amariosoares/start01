/*******************************************************************************
********************************************************************************
**
** File Name
** ---------
**
** crc.h
**
********************************************************************************
********************************************************************************
**
** Description
** -----------
**
** This is the public header file for the Crc calculation routines.
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
**    CRC_Crc16()          - Calculates a CRC16 checksum
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

#ifndef CRC_H
#define CRC_H


/*******************************************************************************
**
** Typedefs
**
********************************************************************************
*/


/*******************************************************************************
**
** Public Services
**
********************************************************************************
*/

/*---------------------------------------------------------------------------
**
** CRC_Crc16()
**
** Calculates a CRC16 checksum on the indicated bytes.
**
**---------------------------------------------------------------------------
**
** Inputs:
**    pbBufferStart            - Where to start calculation
**    iLength                  - The amount of bytes to include
**
** Outputs:
**    Returns                  - The calculated CRC16 checksum
**
** Usage:
**    iCrc = CRC_Crc16( pbStart, 20 );
**
**---------------------------------------------------------------------------
*/

EXTFUNC UINT16 CRC_Crc16( UINT8* pbBufferStart, UINT16 iLength );


#endif  /* inclusion lock */

/*******************************************************************************
**
** End of crc.h
**
********************************************************************************
*/