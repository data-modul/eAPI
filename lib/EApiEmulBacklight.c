/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulBacklight.c $
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
 *I Description: Auto Created for EApiEmulBacklight.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulBacklight.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulBacklight.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
/*
 *
 *
 *
 *  B A C K L I G H T
 *
 *
 *
 */


/*
 * EAPI_ID_BACKLIGHT_1 corresponds to internal pwm => card0-eDP-1
 * EAPI_ID_BACKLIGHT_2
 * EAPI_ID_BACKLIGHT_3
 */

#define BACKLIGHT_PATH "/sys/class/backlight/intel_backlight"

EApiStatus_t 
EApiVgaGetBacklightEnableEmul( 
        __IN  EApiId_t Id       ,
        __OUT uint32_t *pEnable
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char path[NAME_MAX];
    FILE *f = NULL;
    char value[NAME_MAX];

    if (Id == EAPI_ID_BACKLIGHT_1) /* card0-eDP-1 */
    {
        snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof("/bl_power"),BACKLIGHT_PATH"/bl_power");
        f = fopen(path, "r");
        if (f != NULL)
        {
            int res = fscanf(f, "%s", value);
            if (res < 0)
            {
                snprintf(err,sizeof(err),"Error during read operation: %s",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiVgaGetBacklightEnableEmul,
                            EAPI_STATUS_ERROR  ,
                            err);
            }
            fclose(f);
        }
        else
        {
            snprintf(err,sizeof(err),"Error in open file operation: %s",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiVgaGetBacklightEnableEmul,
                        EAPI_STATUS_ERROR  ,
                        err);
        }
        if (!strcmp(value, "0"))
            *pEnable=EAPI_BACKLIGHT_SET_ON;
        else
            *pEnable=EAPI_BACKLIGHT_SET_OFF;
        EAPI_LIB_RETURN_SUCCESS(
                    EApiVgaGetBacklightEnableEmul,
                    ""
                    );
    }
    EAPI_LIB_RETURN_ERROR(
                EApiVgaGetBacklightEnableEmul,
                EAPI_STATUS_UNSUPPORTED  ,
                "Unrecognised Backlight ID"
                );
    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}

EApiStatus_t 
EApiVgaSetBacklightEnableEmul( 
        __IN  EApiId_t Id       ,
        __IN uint32_t Enable
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    if (Id == EAPI_ID_BACKLIGHT_1) /* card0-eDP-1 */
    {
        if ((Enable == EAPI_BACKLIGHT_SET_ON) ||
                (Enable == EAPI_BACKLIGHT_SET_OFF))
        EAPI_LIB_RETURN_ERROR(
                    EApiVgaSetBacklightEnableEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Not support enable/disable backlight"
                    );
    }
    EAPI_LIB_RETURN_ERROR(
                EApiVgaSetBacklightEnableEmul,
                EAPI_STATUS_UNSUPPORTED  ,
                "Unrecognised Backlight ID"
                );
    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}
EApiStatus_t 
EApiVgaGetBacklightBrightnessEmul( 
        __IN  EApiId_t Id           ,
        __OUT uint32_t *pBrightness
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char path[NAME_MAX];
    FILE *f = NULL;
    uint32_t value=0;

    if (Id == EAPI_ID_BACKLIGHT_1) /* card0-eDP-1 */
    {
        snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof("/brightness"),BACKLIGHT_PATH"/brightness");
        f = fopen(path, "r");
        if (f != NULL)
        {
            int res = fscanf(f, "%" PRIu32, &value);
            if (res < 0)
            {
                snprintf(err,sizeof(err),"Error during read operation: %s",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiVgaGetBacklightBrightnessEmul,
                            EAPI_STATUS_ERROR  ,
                            err);
            }
            fclose(f);
        }
        else
        {
            snprintf(err,sizeof(err),"Error in open file operation: %s",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiVgaGetBacklightBrightnessEmul,
                        EAPI_STATUS_ERROR  ,
                        err);
        }
        *pBrightness=value;
        EAPI_LIB_RETURN_SUCCESS(
                    EApiVgaGetBacklightBrightnessEmul,
                    ""
                    );
    }
    EAPI_LIB_RETURN_ERROR(
                EApiVgaGetBacklightBrightnessEmul,
                EAPI_STATUS_UNSUPPORTED  ,
                "Unrecognised Backlight ID"
                );
    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}

EApiStatus_t 
EApiVgaSetBacklightBrightnessEmul( 
        __IN  EApiId_t Id           ,
        __IN uint32_t Brightness
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char path[NAME_MAX];
    FILE *f = NULL;
    uint32_t maxBrightness = 0;

    if (Id == EAPI_ID_BACKLIGHT_1) /* card0-eDP-1 */
    {
        snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof("/max_brightness"),BACKLIGHT_PATH"/max_brightness");
        f = fopen(path, "r");
        if (f != NULL)
        {
            int res = fscanf(f, "%" PRIu32, &maxBrightness);
            if (res < 0)
                maxBrightness = 0;
            fclose(f);
        }

        if (Brightness > maxBrightness)
            EAPI_LIB_RETURN_ERROR(
                        EApiVgaSetBacklightBrightness,
                        EAPI_STATUS_ERROR  ,
                        "Brightness Invalid Value");

        snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof("/brightness"),BACKLIGHT_PATH"/brightness");
        f = fopen(path, "w");
        if (f != NULL)
        {
            int res = fprintf(f, "%" PRIu32, Brightness);
            if (res < 0)
            {
                snprintf(err,sizeof(err),"Error during write operation: %s",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiVgaSetBacklightBrightnessEmul,
                            EAPI_STATUS_ERROR  ,
                            err);
            }
            fclose(f);
        }
        else
        {
            snprintf(err,sizeof(err),"Error in open file operation: %s",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiVgaSetBacklightBrightnessEmul,
                        EAPI_STATUS_ERROR  ,
                        err);
        }
        EAPI_LIB_RETURN_SUCCESS(
                    EApiVgaSetBacklightBrightnessEmul,
                    ""
                    );
    }
    EAPI_LIB_RETURN_ERROR(
                EApiVgaSetBacklightBrightnessEmul,
                EAPI_STATUS_UNSUPPORTED  ,
                "Unrecognised Backlight ID"
                );
    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}


