/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulBoardInfo.c $
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
 *I Description: Auto Created for EApiEmulBoardInfo.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulBoardInfo.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulBoardInfo.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
/*
 *
 *
 *
 *  Define
 *
 *
 *
 */
#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define RUNTIME_PATH "/sys/bus/platform/drivers/dmec-rtm/dmec-rtm.3/rtm_time"
#define BOOT_COUNTER_PATH "/sys/bus/platform/drivers/dmec-rtm/dmec-rtm.3/rtm_boot_count"
#define BOARDINFO_PATH "/sys/class/dmi/id/"

/*
 *
 *
 *
 *  Get String
 *
 *
 *
 */

EApiStatus_t
EApiBoardGetStringAEmul(
        __IN    EApiId_t  Id      ,
        __OUT       char *pBuffer ,
        __INOUT uint32_t *pBufLen
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t BufLenSav;
    FILE *f = NULL;
    char *info = NULL;
    char line[NAME_MAX];
    char path[NAME_MAX];
    char *p;

    BufLenSav=*pBufLen;

   // info = (char *)calloc(NAME_MAX, sizeof(char));
//    if (!info)
//    {
//        EAPI_LIB_RETURN_ERROR(
//                    EApiBoardGetStringAEmul,
//                    EAPI_STATUS_ALLOC_ERROR,
//                    "Error in Allocating Memory");
//    }
    if (Id == EAPI_ID_BOARD_MANUFACTURER_STR)
        info = (char*)eeprom_analyze(eepromBuffer,MANUFACTURE_TYPE,MANUFACTURE_ASCII_IND);

    else if (Id == EAPI_ID_BOARD_NAME_STR)
        info = (char*)eeprom_analyze(eepromBuffer,PRODUCT_TYPE,PRODUCT_ASCII_IND);

    else if (Id ==  EAPI_ID_BOARD_SERIAL_STR)
        info = (char*)eeprom_analyze(eepromBuffer,SN_TYPE,SN_ASCII_IND);

    else if ((Id ==  EAPI_ID_BOARD_HW_REVISION_STR) ||
             ( Id == EAPI_ID_BOARD_REVISION_STR))
        info = (char*)eeprom_analyze(eepromBuffer,VERSION_TYPE,VERSION_ASCII_IND);
    else if (Id ==  EAPI_DMO_ID_BOARD_MANUFACTURING_DATE_STR)
        info = (char*)eeprom_analyze(eepromBuffer,MANUFACTURE_DATE_TYPE,MANUFACTURE_DATE_ASCII_IND);
    else if (Id ==  EAPI_DMO_ID_BOARD_ID_STR)
        info = (char*)eeprom_analyze(eepromBuffer,BOARD_ID_TYPE,BOARD_ID_ASCII_IND);






    else if (Id == EAPI_ID_BOARD_BIOS_REVISION_STR)
    {
        info = (char *)calloc(NAME_MAX, sizeof(char));
        if (!info)
        {
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetStringAEmul,
                        EAPI_STATUS_ALLOC_ERROR,
                        "Error in Allocating Memory");
        }
        strncpy(path, BOARDINFO_PATH"bios_version", sizeof(BOARDINFO_PATH"bios_version"));
        f = fopen(path, "r");
        if (f != NULL)
        {
            char* res = fgets(line, sizeof(line), f);

            if (res == NULL)
            {
                snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiBoardGetStringAEmul,
                            EAPI_STATUS_ERROR  ,
                            err
                            );
            }
            p=strrchr(line, '\n');//search last space
            *p = '\0';//split at last space
            strcpy(info, line);
            fclose(f);
        }
        else
        {
            snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetStringAEmul,
                        EAPI_STATUS_ERROR  ,
                        err);
        }
    }
    else if (Id == EAPI_ID_BOARD_PLATFORM_TYPE_STR)
    {
        info = (char *)calloc(NAME_MAX, sizeof(char));
        if (!info)
        {
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetStringAEmul,
                        EAPI_STATUS_ALLOC_ERROR,
                        "Error in Allocating Memory");
        }
#if (EAPI_PLATFORM==EAPI_PLATFORM_COM0)
        strncpy(info, EAPI_COM0_PLATFORM_STR, sizeof(EAPI_COM0_PLATFORM_STR));
#elif (EAPI_PLATFORM==EAPI_PLATFORM_ETX)
        strncpy(info, EAPI_ETX_PLATFORM_STR, sizeof(EAPI_ETX_PLATFORM_STR));
#elif (EAPI_PLATFORM==EAPI_PLATFORM_Q7)
        strncpy(info, EAPI_Q7_PLATFORM_STR, sizeof(EAPI_Q7_PLATFORM_STR));
#elif (EAPI_PLATFORM==EAPI_PLATFORM_TCA)
        strncpy(info, EAPI_TCA_PLATFORM_STR, sizeof(EAPI_TCA_PLATFORM_STR));
#else
        strncpy(info, "Unknown Platform Type", sizeof("Unknown Platform Type"));
#endif
    }
    else
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetStringAEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised String ID"
                    );
    /*
         * Basically equivilant to
         * strncpy(pBuffer, info, *pBufLen);
         * info[*pBufLen-1]='\0';
         * *pBufLen=strlen(info);
         *
         */
    if(info == NULL)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetStringAEmul,
                    EAPI_STATUS_ERROR  ,
                    "The board string info is not found."
                    );
    }
    *pBufLen=(uint32_t)strlen(info)+1;

    if(BufLenSav<*pBufLen)
        StatusCode=EAPI_STATUS_MORE_DATA;
    else
        BufLenSav=*pBufLen;

    if(BufLenSav && (pBuffer!=NULL))
    {
        snprintf(pBuffer,BufLenSav,"%s",info);
        pBuffer[BufLenSav-1]='\0';
    }
    EAPI_LIB_RETURN_ERROR_IF(
                EApiBoardGetStringAEmul,
                (StatusCode==EAPI_STATUS_MORE_DATA),
                EAPI_STATUS_MORE_DATA  ,
                "pBuffer Overrun Prevented"
                );

    EAPI_LIB_RETURN_SUCCESS(
                EApiBoardGetStringAEmul,
                ""
                );

    EAPI_LIB_ASSERT_EXIT
           if(info)
            free(info);
            return StatusCode;
}

/*
 *
 *
 *
 *  Get Value
 *
 *
 *
 */
EApiStatus_t
EApiBoardGetValueEmul(
        __IN  EApiId_t Id     ,
        __OUT uint32_t *pValue
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    FILE *f = NULL;
    uint32_t value=0;
    char path[NAME_MAX];

    switch (Id)
    {
    case EAPI_ID_BOARD_BOOT_COUNTER_VAL:
        snprintf(path,sizeof(BOOT_COUNTER_PATH),BOOT_COUNTER_PATH);
		break;
    case EAPI_ID_BOARD_RUNNING_TIME_METER_VAL:
        snprintf(path,sizeof(RUNTIME_PATH),RUNTIME_PATH);
        break;
    case EAPI_ID_BOARD_PNPID_VAL:
        *pValue=EAPI_PNPID_DM;
        EAPI_LIB_RETURN_SUCCESS(EApiBoardGetValueEmul, "");
    case EAPI_ID_BOARD_PLATFORM_REV_VAL:
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unsupported ID"
                    );
    case EAPI_ID_BOARD_DRIVER_VERSION_VAL:
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unsupported ID"
                    );
    case EAPI_ID_BOARD_LIB_VERSION_VAL:
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unsupported ID"
                    );
    case EAPI_ID_HWMON_CPU_TEMP://it is different for BBW6 and CBS6
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        if (borad_type == BBW6)
            snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/temp5_input"),HWMON_PATH"%s/temp5_input",hwname);
        else if (borad_type == CBS6)
            snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/temp1_input"),HWMON_PATH"%s/temp1_input",hwname);
        else
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetValueEmul,
                        EAPI_STATUS_UNSUPPORTED  ,
                        "Not support CPU Temperature"
                        );
        break;
    case EAPI_ID_HWMON_CHIPSET_TEMP://it is different for BBW6 and CBS6
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        if (borad_type == BBW6)
            snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/temp1_input"),HWMON_PATH"%s/temp1_input",hwname);
        else
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetValueEmul,
                        EAPI_STATUS_UNSUPPORTED  ,
                        "Not support CPU Temperature"
                        );
        break;
    case EAPI_ID_HWMON_SYSTEM_TEMP:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/temp4_input"),HWMON_PATH"%s/temp4_input",hwname);
        break;
    case EAPI_ID_HWMON_FAN_CPU:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/fan2_input"),HWMON_PATH"%s/fan2_input",hwname);
        break;
    case EAPI_ID_HWMON_FAN_SYSTEM:
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unsupported ID"
                    );
//        EAPI_LIB_RETURN_ERROR_IF(
//                    EApiBoardGetValueEmul,
//                    (hwname==NULL),
//                    EAPI_STATUS_ERROR,
//                    "Error finding HWMON"
//                    );
//        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/fan1_input"),HWMON_PATH"%s/fan1_input",hwname);
//        break;
    case EAPI_ID_HWMON_VOLTAGE_VCORE:
        *pValue=0xffffff;
                EAPI_LIB_RETURN_ERROR(
                            EApiBoardGetValueEmul,
                            EAPI_STATUS_UNSUPPORTED  ,
                            "Unsupported Value"
                            );
    case EAPI_ID_HWMON_VOLTAGE_2V5:
        *pValue=0xffffff;
                EAPI_LIB_RETURN_ERROR(
                            EApiBoardGetValueEmul,
                            EAPI_STATUS_UNSUPPORTED  ,
                            "Unsupported Value"
                            );
    case EAPI_ID_HWMON_VOLTAGE_3V3:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/in0_input"),HWMON_PATH"%s/in0_input",hwname);
        break;
    case EAPI_ID_HWMON_VOLTAGE_VBAT:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/in1_input"),HWMON_PATH"%s/in1_input",hwname);
        break;
    case EAPI_ID_HWMON_VOLTAGE_5V:
        *pValue=0xffffff;
                EAPI_LIB_RETURN_ERROR(
                            EApiBoardGetValueEmul,
                            EAPI_STATUS_UNSUPPORTED  ,
                            "Unsupported Value"
                            );
    case EAPI_ID_HWMON_VOLTAGE_5VSB:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/in3_input"),HWMON_PATH"%s/in3_input",hwname);
        break;
    case EAPI_ID_HWMON_VOLTAGE_12V:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (hwname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding HWMON"
                    );
        snprintf(path,sizeof(HWMON_PATH)+sizeof(hwname)+sizeof("/in4_input"),HWMON_PATH"%s/in4_input",hwname);
        break;
    default:
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unsupported Value ID"
                    );
    }

    f = fopen(path, "r");
    if (f != NULL)
    {
        int res = fscanf(f, "%" PRIu32, &value);
        if (res < 0)
        {
            snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetValueEmul,
                        EAPI_STATUS_ERROR  ,
                        err);
        }
        fclose(f);
    }
    else
    {
        snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_ERROR  ,
                    err);
    }

    if(Id == EAPI_ID_HWMON_VOLTAGE_VBAT && value!=0xffffff)
    {
        *pValue = (value * 1702) / 1000;
        if (*pValue < 2000) // less than 2V means no Battery
            *pValue = 0xffffff;
    }
    else if(Id == EAPI_ID_HWMON_VOLTAGE_5VSB && value!=0xffffff)
        *pValue = (value * 3000) / 1000;
    else if(Id == EAPI_ID_HWMON_VOLTAGE_12V && value!=0xffffff)
        *pValue = (value * 10000) / 1000;
    else if((Id == EAPI_ID_HWMON_CPU_TEMP ||
             Id == EAPI_ID_HWMON_CHIPSET_TEMP ||
             Id == EAPI_ID_HWMON_SYSTEM_TEMP ) && value!=0xffffff)
        *pValue = (value + 273150)/100; //mCelcius + (273.15*1000)/100 =>0.1K
    else
        *pValue=value;

    if (*pValue==0xffffff)
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_ERROR  ,
                    "Invalid Value"
                    );

    EAPI_LIB_RETURN_SUCCESS(EApiBoardGetValueEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

