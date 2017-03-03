/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulGPIO.c $
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
 *I Description: Auto Created for EApiEmulGPIO.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulGPIO.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulGPIO.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>


/*
 *
 *
 *
 *  G P I O
 *
 *
 *
 */

#define pInputsGPIO 0xff
#define pOutputsGPIO 0xff

#define GPIO_PATH "/sys/class/gpio/"


EApiStatus_t pin_export (int mypin) {
    char buf[32];
    int fd;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    if ((fd = open(GPIO_PATH"/export", O_WRONLY)) < 0) {
        snprintf(err,sizeof(err),"Failed to access to pin %d",mypin);
        EAPI_LIB_RETURN_ERROR(
                    pin_export,
                    EAPI_STATUS_ERROR,
                    err);
    }
    snprintf(buf, sizeof(buf), "%d", gpioBase + mypin);
    write(fd, buf, strlen(buf));
    close(fd);
    gpiofdExpert[mypin] = 0;
    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t 
EApiGPIOGetLevelEmul( 
        __IN  EApiId_t  Id      ,
        __IN  uint32_t  BitMask ,
        __OUT uint32_t *pLevel
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    int ret, fd;
    unsigned int i;
    int index = -1;
    int bank = 0;
    char path[NAME_MAX];

    *pLevel = 0;

    if (gpioEnabled == 0)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOGetLevelEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO chip"
                    );
    }

    switch (Id)
    {
    case EAPI_ID_GPIO_GPIO00:
        index = 0;
        break;
    case EAPI_ID_GPIO_GPIO01:
        index = 1;
        break;
    case EAPI_ID_GPIO_GPIO02:
        index = 2;
        break;
    case EAPI_ID_GPIO_GPIO03:
        index = 3;
        break;
    case EAPI_ID_GPIO_GPIO04:
        index = 4;
        break;
    case EAPI_ID_GPIO_GPIO05:
        index = 5;
        break;
    case EAPI_ID_GPIO_GPIO06:
        index = 6;
        break;
    case EAPI_ID_GPIO_GPIO07:
        index = 7;
        break;
    case EAPI_ID_GPIO_BANK00:
        bank = 1;
        break;
    default:
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOGetLevelEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO ID"
                    );
    }
    EAPI_LIB_RETURN_ERROR_IF(
                EApiGPIOGetLevelEmul,
                (BitMask&~(pOutputsGPIO|pInputsGPIO)) ,
                EAPI_STATUS_INVALID_BITMASK,
                "Bit-mask Selects Invalid Bits"
                );

    if (bank == 0 && index > -1) // individual request
    {
        if(gpiofdExpert[index] == -1)
            pin_export(index);

        if (gpiofdValue[index] == -1) /* value file is not opened yet */
        {
            sprintf(path, GPIO_PATH"gpio%d/value", gpioBase+index );
            if ((fd = open(path, O_RDWR )) < 0) {
                snprintf(err,sizeof(err),"Failed to open GPIO %d value: %s",index,strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiGPIOGetLevelEmul,
                            EAPI_STATUS_ERROR,
                            err);
            }
            gpiofdValue[index] = fd;
        }
        char s;
        lseek(gpiofdValue[index],0, SEEK_SET);
        ret = read(gpiofdValue[index], &s, 1);
        if(ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to read GPIO %d value: %s",index,strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOGetLevelEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }
        *pLevel = s - '0';
    }
    else // bank request
    {
        for (i = 0 ; i < gpioLines ; i++)
        {
            if (BitMask & (0x01 << i)) /* Bitmask is EAPI_GPIO_BITMASK_SELECT*/
            {
                if(gpiofdExpert[i] == -1)
                    pin_export(i);

                if (gpiofdValue[i] == -1) /* value file is not opened yet */
                {
                    sprintf(path, GPIO_PATH"gpio%d/value", gpioBase+i );
                    if ((fd = open(path, O_RDWR )) < 0) {
                        snprintf(err,sizeof(err),"Failed to open GPIO %d value: %s",i,strerror(errno));
                        EAPI_LIB_RETURN_ERROR(
                                    EApiGPIOGetLevelEmul,
                                    EAPI_STATUS_ERROR,
                                    err);
                    }
                    gpiofdValue[i] = fd;

                }
                char s;
                lseek(gpiofdValue[i],0, SEEK_SET);
                ret = read(gpiofdValue[i], &s, 1);
                if(ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to read GPIO %d value: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOGetLevelEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
                *pLevel |= ((s - '0') << i);
            }
        }
    }
    EAPI_LIB_RETURN_SUCCESS(EApiGPIOGetLevelEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t 
EApiGPIOSetLevelEmul( 
        __IN EApiId_t Id      ,
        __IN uint32_t BitMask ,
        __IN uint32_t Level
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    unsigned int i = 0;
    int ret,fd = 0;
    int index = -1;
    int bank = 0;
    uint32_t Direction = 0;
    char path[NAME_MAX];

    if (gpioEnabled == 0)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOSetLevelEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO chip"
                    );
    }
    switch (Id)
    {
    case EAPI_ID_GPIO_GPIO00:
        index = 0;
        break;
    case EAPI_ID_GPIO_GPIO01:
        index = 1;
        break;
    case EAPI_ID_GPIO_GPIO02:
        index = 2;
        break;
    case EAPI_ID_GPIO_GPIO03:
        index = 3;
        break;
    case EAPI_ID_GPIO_GPIO04:
        index = 4;
        break;
    case EAPI_ID_GPIO_GPIO05:
        index = 5;
        break;
    case EAPI_ID_GPIO_GPIO06:
        index = 6;
        break;
    case EAPI_ID_GPIO_GPIO07:
        index = 7;
        break;
    case EAPI_ID_GPIO_BANK00:
        bank = 1;
        break;
    default:
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOSetLevelEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO ID"
                    );
    }
    EAPI_LIB_RETURN_ERROR_IF(
                EApiGPIOSetLevelEmul,
                (BitMask&~(pOutputsGPIO|pInputsGPIO)) ,
                EAPI_STATUS_INVALID_BITMASK,
                "Bit-mask Selects Invalid Bits"
                );

    /* check the direction before set the value */
    EApiGPIOGetDirectionEmul( Id, BitMask, &Direction);

    if (bank == 0 && index > -1) /* individual request */
    {
        if (Direction == EAPI_GPIO_INPUT)
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOSetLevelEmul,
                        EAPI_STATUS_INVALID_PARAMETER,
                        "Cant Set Level on Pin that is set to input");

        if(gpiofdExpert[index] == -1)
            pin_export(index);

        if (gpiofdValue[index] == -1) /* value file is not opened yet */
        {
            sprintf(path, GPIO_PATH"gpio%d/value", gpioBase+index );
            if ((fd = open(path, O_RDWR )) < 0) {
                snprintf(err,sizeof(err),"Failed to open GPIO %d value: %s",index,strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiGPIOSetLevelEmul,
                            EAPI_STATUS_ERROR,
                            err);
            }
            gpiofdValue[index] = fd;
        }

        lseek(gpiofdValue[index],0, SEEK_SET);
        if (Level)
            ret = write(gpiofdValue[index], "1", 1);
        else
            ret = write(gpiofdValue[index], "0", 1);

        if(ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to write value to GPIO %d: %s",index,strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOSetLevelEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }
    }
    else /* bank request */
    {
        if((BitMask & Direction) != 0x00)
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOSetLevelEmul,
                        EAPI_STATUS_INVALID_PARAMETER,
                        "Cant Set Level on Pin that is set to input");

        for (i = 0 ; i < gpioLines ;i++)
        {
            if (BitMask & (0x01 << i)) /* Bitmask is EAPI_GPIO_BITMASK_SELECT*/
            {
                if(gpiofdExpert[i] == -1)
                    pin_export(i);

                if (gpiofdValue[i] == -1) /* value file is not opened yet */
                {
                    sprintf(path, GPIO_PATH"gpio%d/value", gpioBase+i );
                    if ((fd = open(path, O_RDWR )) < 0) {
                        snprintf(err,sizeof(err),"Failed to open GPIO %d value: %s",i,strerror(errno));
                        EAPI_LIB_RETURN_ERROR(
                                    EApiGPIOSetLevelEmul,
                                    EAPI_STATUS_ERROR,
                                    err);
                    }
                    gpiofdValue[i] = fd;
                }

                lseek(gpiofdValue[i],0, SEEK_SET);
                if(Level & (0x01 << i))
                    ret = write(gpiofdValue[i], "1", 1);
                else
                    ret = write(gpiofdValue[i], "0", 1);

                if(ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to write value to GPIO %d: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOSetLevelEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
            }
        }
    }

    EAPI_LIB_RETURN_SUCCESS(EApiGPIOSetLevel, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t 
EApiGPIOGetDirectionEmul( 
        __IN  EApiId_t  Id        ,
        __IN  uint32_t  BitMask   ,
        __OUT uint32_t *pDirection
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    int index = -1;
    int bank = 0;
    unsigned i;
    int ret, fd;
    char path[NAME_MAX];

    *pDirection = 0;

    if (gpioEnabled == 0)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOGetDirectionEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO chip"
                    );
    }

    switch (Id)
    {
    case EAPI_ID_GPIO_GPIO00:
        index = 0;
        break;
    case EAPI_ID_GPIO_GPIO01:
        index = 1;
        break;
    case EAPI_ID_GPIO_GPIO02:
        index = 2;
        break;
    case EAPI_ID_GPIO_GPIO03:
        index = 3;
        break;
    case EAPI_ID_GPIO_GPIO04:
        index = 4;
        break;
    case EAPI_ID_GPIO_GPIO05:
        index = 5;
        break;
    case EAPI_ID_GPIO_GPIO06:
        index = 6;
        break;
    case EAPI_ID_GPIO_GPIO07:
        index = 7;
        break;
    case EAPI_ID_GPIO_BANK00:
        bank = 1;
        break;
    default:
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOGetDirectionEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO ID"
                    );
    }
    EAPI_LIB_RETURN_ERROR_IF(
                EApiGPIOGetDirectionEmul,
                (BitMask&~(pOutputsGPIO|pInputsGPIO)) ,
                EAPI_STATUS_INVALID_BITMASK,
                "Bit-mask Selects Invalid Bits"
                );

    if (bank == 0 && index > -1) /* individual request */
    {

        if(gpiofdExpert[index] == -1)
            pin_export(index);

        if (gpiofdDirection[index] == -1) /* direction file is not opened yet */
        {
            sprintf(path, GPIO_PATH"gpio%d/direction", gpioBase+index );
            if ((fd = open(path, O_RDWR )) < 0) {
                snprintf(err,sizeof(err),"Failed to open GPIO %d direction: %s",index,strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiGPIOGetDirectionEmul,
                            EAPI_STATUS_ERROR,
                            err);
            }
            gpiofdDirection[index] = fd;
        }

        char s[3];
        lseek(gpiofdDirection[index],0, SEEK_SET);
        ret = read(gpiofdDirection[index], s, 3);
        if(ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to read GPIO %d direction: %s",index,strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOGetDirectionEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }
        if(!strncmp(s,"out",3))
            *pDirection = EAPI_GPIO_OUTPUT;
        else
            *pDirection = EAPI_GPIO_INPUT;

    }
    else /* bank request */
    {
        for (i =0; i < gpioLines; i++)
        {
            unsigned bit = BitMask & ( 0x01 << i);
            if (bit != 0x00) /* Bitmask is EAPI_GPIO_BITMASK_SELECT*/
            {
                if(gpiofdExpert[i] == -1)
                    pin_export(i);

                if (gpiofdDirection[i] == -1) /* direction file is not opened yet */
                {
                    sprintf(path, GPIO_PATH"gpio%d/direction", gpioBase+i );
                    if ((fd = open(path, O_RDWR )) < 0) {
                        snprintf(err,sizeof(err),"Failed to open GPIO %d direction: %s",i,strerror(errno));
                        EAPI_LIB_RETURN_ERROR(
                                    EApiGPIOGetDirectionEmul,
                                    EAPI_STATUS_ERROR,
                                    err);
                    }
                    gpiofdDirection[i] = fd;
                }
                char s[3];
                lseek(gpiofdDirection[i],0, SEEK_SET);
                ret = read(gpiofdDirection[i], s, 3);
                if(ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to read GPIO %d direction: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOGetDirectionEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
                if(!strncmp(s,"out",3))
                    *pDirection |= (EAPI_GPIO_OUTPUT << i);
                else
                    *pDirection |= (EAPI_GPIO_INPUT << i );
            }
        }
    }
    EAPI_LIB_RETURN_SUCCESS(EApiGPIOGetDirection, "");

    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}


EApiStatus_t
EApiGPIOSetDirectionEmul(
        __IN EApiId_t Id        ,
        __IN uint32_t BitMask   ,
        __IN uint32_t Direction
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    int index = -1;
    int bank = 0;
    unsigned i;
    int ret,fd;
    char path[NAME_MAX];

    if (gpioEnabled == 0)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOSetDirectionEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO chip"
                    );
    }

    switch (Id)
    {
    case EAPI_ID_GPIO_GPIO00:
        index = 0;
        break;
    case EAPI_ID_GPIO_GPIO01:
        index = 1;
        break;
    case EAPI_ID_GPIO_GPIO02:
        index = 2;
        break;
    case EAPI_ID_GPIO_GPIO03:
        index = 3;
        break;
    case EAPI_ID_GPIO_GPIO04:
        index = 4;
        break;
    case EAPI_ID_GPIO_GPIO05:
        index = 5;
        break;
    case EAPI_ID_GPIO_GPIO06:
        index = 6;
        break;
    case EAPI_ID_GPIO_GPIO07:
        index = 7;
        break;
    case EAPI_ID_GPIO_BANK00:
        bank = 1;
        break;
    default:
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOSetDirectionEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO ID"
                    );
    }

    EAPI_LIB_RETURN_ERROR_IF(
                EApiGPIOSetDirectionEmul,
                (BitMask&~(pOutputsGPIO|pInputsGPIO)) ,
                EAPI_STATUS_INVALID_BITMASK,
                "Bit-mask Selects Invalid Bits"
                );
    EAPI_LIB_RETURN_ERROR_IF(
                EApiGPIOSetDirectionEmul,
                (BitMask&Direction&~pInputsGPIO) ,
                EAPI_STATUS_INVALID_DIRECTION,
                "Unsupported Input Bits"
                );
    EAPI_LIB_RETURN_ERROR_IF(
                EApiGPIOSetDirectionEmul,
                ((BitMask&~Direction)&~pOutputsGPIO) ,
                EAPI_STATUS_INVALID_DIRECTION,
                "Unsupported Output Bits"
                );

    if (bank == 0 && index > -1) /* individual request */
    {
        if(gpiofdExpert[index] == -1)
            pin_export(index);

        if (gpiofdDirection[index] == -1) /* direction file is not opened yet */
        {
            sprintf(path, GPIO_PATH"gpio%d/direction", gpioBase+index );
            if ((fd = open(path, O_RDWR )) < 0) {
                snprintf(err,sizeof(err),"Failed to open GPIO %d direction: %s",index,strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiGPIOSetDirectionEmul,
                            EAPI_STATUS_ERROR,
                            err);
            }
            gpiofdDirection[index] = fd;
        }

        lseek(gpiofdDirection[index],0, SEEK_SET);
        if (Direction) /* EAPI_GPIO_INPUT */
            ret = write(gpiofdDirection[index], "in", 2);
        else
            ret = write(gpiofdDirection[index], "out", 3);

        if(ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to write direction to GPIO %d: %s",index,strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOSetDirectionEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }
    }
    else /* bank request */
    {
        for (i =0; i < gpioLines; i++)
        {
            unsigned bit = BitMask & (0x01 << i);
            if (bit != 0x00) /* Bitmask is EAPI_GPIO_BITMASK_SELECT */
            {
                if(gpiofdExpert[i] == -1)
                    pin_export(i);

                if (gpiofdDirection[i] == -1) /* direction file is not opened yet */
                {
                    sprintf(path, GPIO_PATH"gpio%d/direction", gpioBase+i );
                    if ((fd = open(path, O_RDWR )) < 0) {
                        snprintf(err,sizeof(err),"Failed to open GPIO %d direction: %s",i,strerror(errno));
                        EAPI_LIB_RETURN_ERROR(
                                    EApiGPIOSetDirectionEmul,
                                    EAPI_STATUS_ERROR,
                                    err);
                    }
                    gpiofdDirection[i] = fd;
                }

                lseek(gpiofdDirection[i],0, SEEK_SET);
                if(Direction & (0x01 << i)) /* EAPI_GPIO_INPUT */
                    ret = write(gpiofdDirection[i], "in", 2);
                else
                    ret = write(gpiofdDirection[i], "out", 3);

                if(ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to write direction to GPIO %d: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOSetDirectionEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
            }
        }
    }
    EAPI_LIB_RETURN_SUCCESS(EApiGPIOSetDirection, "");

    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}

EApiStatus_t
EApiGPIOGetDirectionCapsEmul(
        __IN      EApiId_t  Id      ,
        __OUTOPT  uint32_t *pInputs ,
        __OUTOPT  uint32_t *pOutputs
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    if (gpioEnabled == 0)
    {
        *pInputs=0x00;
        *pOutputs=0x00;
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOGetDirectionCapsEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO chip"
                    );
    }

    if((Id != EAPI_ID_GPIO_GPIO00) &&
            (Id != EAPI_ID_GPIO_GPIO01) &&
            (Id != EAPI_ID_GPIO_GPIO02) &&
            (Id != EAPI_ID_GPIO_GPIO03) &&
            (Id != EAPI_ID_GPIO_GPIO04) &&
            (Id != EAPI_ID_GPIO_GPIO05) &&
            (Id != EAPI_ID_GPIO_GPIO06) &&
            (Id != EAPI_ID_GPIO_GPIO07) &&
            (Id != EAPI_ID_GPIO_BANK00))
    {
        *pInputs=0x00;
        *pOutputs=0x00;
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOSetDirectionEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO ID"
                    );
    }

    *pInputs=pInputsGPIO;
    *pOutputs=pOutputsGPIO;

    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}


