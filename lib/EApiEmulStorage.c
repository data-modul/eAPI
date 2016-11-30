/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulStorage.c $
 *+=========================================================================
 *I   Copyright: Copyright (c) 2002-2009, Kontron Embedded Modules GmbH
 *I      Author: John Kearney,                  John.Kearney@kontron.com
 *I
 *I     License: All rights reserved. This program and the accompanying
 *I              materials are licensed and made available under the
 *I              terms and conditions of the BSD License which
 *I              accompanies this distribution. The full text of the
 *I              license may be found at
 *I              http://opensource.org/licenses/bsd-license.php
 *I
 *I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "
 *I              AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF
 *I              ANY KIND, EITHER EXPRESS OR IMPLIED.
 *I
 *I Description: Auto Created for EApiEmulStorage.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulStorage.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulStorage.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>
#include "EApiEmulEeprom.h"


#define USERSPACE_BLOCKLENGTH 1

/*
 *
 *
 *
 *  S T O R A G E
 *
 *
 *
 */

EApiStatus_t 
EApiStorageCapEmul (
        __IN      EApiId_t  Id          ,
        __OUTOPT  uint32_t *pStorageSize,
        __OUTOPT  uint32_t *pBlockLength
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t storageSize = 0;

    if(Id != EAPI_ID_STORAGE_STD)
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageCapEmul  ,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised Storage ID"
                    );

    /* find vendor specific block of Eeprom */
    if (eeprom_userSpaceBuf == NULL || userspaceBuffer_Cmd == -1)
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageCapEmul        ,
                    EAPI_STATUS_UNSUPPORTED   ,
                    "Unrecognised Storage"
                    );

    /* validation: F0,?,?,11,AF,0,... */
    if ((eeprom_userSpaceBuf [0] != 0xF0) ||
            (eeprom_userSpaceBuf [3] != 0x11) ||
            (eeprom_userSpaceBuf [4] != 0xAF) ||
            (eeprom_userSpaceBuf [5] != 0x00))
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageCapEmul        ,
                    EAPI_STATUS_READ_ERROR   ,
                    "Unrecognised Storage"
                    );

    storageSize = eeprom_userSpaceBuf[1] << 8;
    storageSize |= eeprom_userSpaceBuf[2];
    storageSize = storageSize*2;
    storageSize = storageSize - 6;

    *pStorageSize = storageSize;
    *pBlockLength=USERSPACE_BLOCKLENGTH;

    EAPI_LIB_RETURN_SUCCESS(EApiStorageCap, "");

    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}
EApiStatus_t 
EApiStorageAreaReadEmul(
        __IN  EApiId_t Id     ,
        __IN  uint32_t Offset ,
        __OUT    uint8_t *pvBuffer,
        __IN  uint32_t ByteCnt
        )
{
    uint32_t storageSize = 0;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    if(Id != EAPI_ID_STORAGE_STD)
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaReadEmul  ,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised Storage ID"
                    );

    /* find vendor specific block of Eeprom */
    if (eeprom_userSpaceBuf == NULL || userspaceBuffer_Cmd == -1)
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaReadEmul        ,
                    EAPI_STATUS_UNSUPPORTED   ,
                    "Unrecognised Storage for reading"
                    );

    /* validation: F0,?,?,11,AF,0,... */
    if ((eeprom_userSpaceBuf [0] != 0xF0) ||
            (eeprom_userSpaceBuf [3] != 0x11) ||
            (eeprom_userSpaceBuf [4] != 0xAF) ||
            (eeprom_userSpaceBuf [5] != 0x00))
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaReadEmul        ,
                    EAPI_STATUS_READ_ERROR   ,
                    "Unrecognised Storage for reading"
                    );

    storageSize = eeprom_userSpaceBuf[1] << 8;
    storageSize |= eeprom_userSpaceBuf[2];
    storageSize = storageSize*2;
    storageSize = storageSize - 6;

    EAPI_LIB_RETURN_ERROR_IF(
                EApiStorageAreaReadEmul                         ,
                (Offset+ByteCnt)>storageSize  ,
                EAPI_STATUS_INVALID_BLOCK_LENGTH                ,
                "Read Len extends beyond End of Storage Area"
                );

    StatusCode = EApiI2CReadTransfer(eeprom_bus,EEPROM_DEVICE,EAPI_I2C_ENC_EXT_CMD(userspaceBuffer_Cmd+6+Offset), pvBuffer,ByteCnt,ByteCnt);
    if(!EAPI_TEST_SUCCESS(StatusCode))
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaReadEmul        ,
                    EAPI_STATUS_READ_ERROR   ,
                    "Reading User space Failed"
                    );

    EAPI_LIB_RETURN_SUCCESS(EApiStorageAreaRead, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t 
EApiStorageAreaWriteEmul(
        __IN EApiId_t Id,
        __IN uint32_t Offset,
        __IN uint8_t *pvBuffer,
        __IN uint32_t ByteCnt
        )
{
    uint32_t storageSize = 0;
    uint32_t new_Offset = 0;
    uint32_t iWrite=0;

    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    if(Id != EAPI_ID_STORAGE_STD)
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaWriteEmul  ,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised Storage ID"
                    );

    /* find vendor specific block of Eeprom */
    if (eeprom_userSpaceBuf == NULL || userspaceBuffer_Cmd == -1)
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaWriteEmul        ,
                    EAPI_STATUS_READ_ERROR   ,
                    "Unrecognised Storage for reading"
                    );

    /* validation: F0,?,?,AF,11,0,... */
    if ((eeprom_userSpaceBuf [0] != 0xF0) ||
            (eeprom_userSpaceBuf [3] != 0x11) ||
            (eeprom_userSpaceBuf [4] != 0xAF) ||
            (eeprom_userSpaceBuf [5] != 0x00))
        EAPI_LIB_RETURN_ERROR(
                    EApiStorageAreaWriteEmul        ,
                    EAPI_STATUS_READ_ERROR   ,
                    "Unrecognised Storage for reading"
                    );

            EAPI_LIB_RETURN_ERROR_IF(
              EApiStorageAreaWriteEmul,
              (ByteCnt%USERSPACE_BLOCKLENGTH)       ,
              EAPI_STATUS_INVALID_BLOCK_ALIGNMENT,
              "Write length Not Aligned"
              );
            EAPI_LIB_RETURN_ERROR_IF(
              EApiStorageAreaWriteEmul,
              (Offset%USERSPACE_BLOCKLENGTH)        ,
              EAPI_STATUS_INVALID_BLOCK_ALIGNMENT,
              "Write Base Address Not Aligned"
              );

    storageSize = eeprom_userSpaceBuf[1] << 8;
    storageSize |= eeprom_userSpaceBuf[2];
    storageSize = storageSize*2;
    storageSize = storageSize - 6;

    EAPI_LIB_RETURN_ERROR_IF(
                EApiStorageAreaWriteEmul                         ,
                (Offset+ByteCnt)>storageSize  ,
                EAPI_STATUS_INVALID_BLOCK_LENGTH                ,
                "Write Len extends beyond End of Storage Area"
                );

    new_Offset = userspaceBuffer_Cmd + 6 + Offset;
    while(iWrite < ByteCnt)
    {
        StatusCode = EApiI2CWriteTransfer(eeprom_bus,EEPROM_DEVICE,EAPI_I2C_ENC_EXT_CMD(new_Offset), &pvBuffer[iWrite],1);
        if(!EAPI_TEST_SUCCESS(StatusCode))
            EAPI_LIB_RETURN_ERROR(
                        EApiStorageAreaWriteEmul        ,
                        EAPI_STATUS_WRITE_ERROR   ,
                        "Writing into User space Failed"
                        );
        iWrite++;
        new_Offset++;
        usleep(5000);
    }




    EAPI_LIB_RETURN_SUCCESS(EApiStorageAreaWriteEmul, "");


    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

