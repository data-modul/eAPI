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
    int retclose;

    BufLenSav=*pBufLen;


    if (Id ==  EAPI_DMO_ID_BOARD_MANUFACTURING_DATE_STR)
        info = (char*)eeprom_analyze(SMBIOS_BLOCK, MANUFACTURE_DATE_TYPE,MANUFACTURE_DATE_ASCII_IND);
    else if (Id ==  EAPI_DMO_ID_BOARD_ID_STR)
        info = (char*)eeprom_analyze(SMBIOS_BLOCK, BOARD_ID_TYPE,BOARD_ID_ASCII_IND);
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
    {

        if (Id == EAPI_ID_BOARD_MANUFACTURER_STR)
            strncpy(path, BOARDINFO_PATH"board_vendor", sizeof(BOARDINFO_PATH"board_vendor"));
        else if (Id == EAPI_ID_BOARD_NAME_STR)
            strncpy(path, BOARDINFO_PATH"board_name", sizeof(BOARDINFO_PATH"board_name"));
        else if (Id ==  EAPI_ID_BOARD_SERIAL_STR)
            strncpy(path, BOARDINFO_PATH"board_serial", sizeof(BOARDINFO_PATH"board_serial"));
        else if ((Id ==  EAPI_ID_BOARD_HW_REVISION_STR) ||
                 ( Id == EAPI_ID_BOARD_REVISION_STR))
            strncpy(path, BOARDINFO_PATH"board_version", sizeof(BOARDINFO_PATH"board_version"));
        else if (Id == EAPI_ID_BOARD_BIOS_REVISION_STR)
            strncpy(path, BOARDINFO_PATH"bios_version", sizeof(BOARDINFO_PATH"bios_version"));
        else
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetStringAEmul,
                        EAPI_STATUS_UNSUPPORTED  ,
                        "Unrecognised String ID"
                        );


        info = (char *)calloc(NAME_MAX, sizeof(char));
        if (!info)
        {
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetStringAEmul,
                        EAPI_STATUS_ALLOC_ERROR,
                        "Error in Allocating Memory");
        }
        f = fopen(path, "r");
        if (f != NULL)
        {
            retclose = 0;
            char* res = fgets(line, sizeof(line), f);
            if (res == NULL)
            {
                fclose(f);
                snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiBoardGetStringAEmul,
                            EAPI_STATUS_ERROR  ,
                            err
                            );
            }
            else
            {
                retclose = fclose(f);

                if (retclose != 0 )
                {
                    snprintf(err,sizeof(err),"Error during close operation: %s\n ",strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiBoardGetStringAEmul,
                                EAPI_STATUS_ERROR  ,
                                err
                                );
                }
            }

            p=strrchr(line, '\n');/* search last space */
            *p = '\0';/* split at last space */
            strcpy(info, line);
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
            if(info){
        free(info);
    }
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
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (rtmname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding RTM"
                    );
        snprintf(path,strlen(RTM_PATH)+strlen(rtmname)+strlen("/rtm_boot_count")+1,RTM_PATH"%s/rtm_boot_count",rtmname);
        break;
    case EAPI_ID_BOARD_RUNNING_TIME_METER_VAL:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (rtmname==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding RTM"
                    );
        snprintf(path,strlen(RTM_PATH)+strlen(rtmname)+strlen("/rtm_time")+1,RTM_PATH"%s/rtm_time",rtmname);
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
    case EAPI_ID_HWMON_CPU_TEMP:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_temp_cpu")+ 1,ACPIHWMON_PATH"%s/acpi_temp_cpu",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_CHIPSET_TEMP:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_temp_chipset")+ 1,ACPIHWMON_PATH"%s/acpi_temp_chipset",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_SYSTEM_TEMP:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_temp_sys")+1,ACPIHWMON_PATH"%s/acpi_temp_sys",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_FAN_CPU:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_fan_cpu")+1,ACPIHWMON_PATH"%s/acpi_fan_cpu",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_FAN_SYSTEM:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_fan_sys")+1,ACPIHWMON_PATH"%s/acpi_fan_sys",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_VCORE:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_vcore")+1,ACPIHWMON_PATH"%s/acpi_vol_vcore",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_2V5:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_2v5")+1,ACPIHWMON_PATH"%s/acpi_vol_2v5",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_3V3:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_3v3")+1,ACPIHWMON_PATH"%s/acpi_vol_3v3",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_VBAT:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_vbat")+1,ACPIHWMON_PATH"%s/acpi_vol_vbat",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_5V:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_5v")+1,ACPIHWMON_PATH"%s/acpi_vol_5v",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_5VSB:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_5vsb")+1,ACPIHWMON_PATH"%s/acpi_vol_5vsb",acpiHwmonName);
        break;
    case EAPI_ID_HWMON_VOLTAGE_12V:
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiBoardGetValueEmul,
                    (acpiHwmonName==NULL),
                    EAPI_STATUS_ERROR,
                    "Error finding ACPI-HWMON"
                    );
        snprintf(path,strlen(ACPIHWMON_PATH)+strlen(acpiHwmonName)+strlen("/acpi_vol_12v")+1,ACPIHWMON_PATH"%s/acpi_vol_12v",acpiHwmonName);
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
        int retclose = 0;
        int res = fscanf(f, "%" PRIu32, &value);
        if (res < 0)
        {
            fclose(f);
            snprintf(err,strlen(err),"Error during read operation: %s",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiBoardGetValueEmul,
                        EAPI_STATUS_ERROR  ,
                        err);
        }
        else
        {
            retclose = fclose(f);
            if (retclose != 0)
            {
                snprintf(err,strlen(err),"Error during close operation: %s",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiBoardGetValueEmul,
                            EAPI_STATUS_ERROR  ,
                            err);
            }
        }
    }
    else
    {
        snprintf(err,strlen(err),"Error in open file operation: %s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_ERROR  ,
                    err);
    }
    if(Id == EAPI_ID_HWMON_VOLTAGE_VBAT && value!=0xffffffff && value < 2000) /* less than 2V means no Battery */
        *pValue = 0xffffffff;
    else
        *pValue = value;

    if (*pValue==0xffffffff)
        EAPI_LIB_RETURN_ERROR(
                    EApiBoardGetValueEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unsupported ID"
                    );

    EAPI_LIB_RETURN_SUCCESS(EApiBoardGetValueEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

