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
#include <linux/gpio.h>
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

EApiStatus_t 
EApiGPIOGetLevelEmul( 
        __IN  EApiId_t  Id      ,
        __IN  uint32_t  BitMask ,
        __OUT uint32_t *pLevel
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    int ret;
    unsigned int i;
    int index = -1;
    int bank = 0;

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
        if(req[index].fd <= 0)
        {
            snprintf(err,sizeof(err),"Failed to access to pin %d",index);
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOGetLevelEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }

        struct gpiohandle_data data;
        memset(&data, 0, sizeof(data));

        ret = ioctl(req[index].fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
        if(ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to issue GPIOHANDLE GET LINE VALUES IOCTL %d: %s",index,strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOGetLevelEmul,
                        EAPI_STATUS_UNSUPPORTED,
                        err);
        }
        *pLevel = data.values[0];
    }
    else // bank request
    {
        for (i = 0 ; i < gpioLines ; i++)
        {
            if (BitMask & (0x01 << i)) /* Bitmask is EAPI_GPIO_BITMASK_SELECT*/
            {
                if(req[i].fd <= 0)
                {
                    snprintf(err,sizeof(err),"Failed to access to pin %d",i);
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOGetLevelEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }

                struct gpiohandle_data data;
                memset(&data, 0, sizeof(data));
                ret = ioctl(req[i].fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
                if(ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to issue GPIOHANDLE GET LINE VALUES of pin %d: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOGetLevelEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
                *pLevel |= (data.values[0] << i);
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
    struct gpiohandle_data data;//, readBack;
    int ret = 0;
    int index = -1;
    int bank = 0;
    uint32_t Direction = 0;

    if (gpioEnabled == 0)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiGPIOSetLevelEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised GPIO chip"
                    );
    }

    memset(&data, 0, sizeof(data));
    // memset(&readBack, 0, sizeof(readBack));

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

        if(req[index].fd <= 0)
        {
            snprintf(err,sizeof(err),"Failed to access to pin %d",index);
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOSetLevelEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }
        else
        {
            memset(&data, 0, sizeof(data));

            if (Level)
                data.values[0] = EAPI_GPIO_HIGH;
            else
                data.values[0] = EAPI_GPIO_LOW;
            /* set requested GPIO level*/
            ret = ioctl(req[index].fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
            if(ret == -1)
            {
                snprintf(err,sizeof(err),"Failed to issue GPIOHANDLE SET LINE VALUES IOCTL %d: %s",index,strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            EApiGPIOSetLevelEmul,
                            EAPI_STATUS_ERROR,
                            err);
            }
        }
    }
    else /* bank request */
    {
        if((BitMask & Direction) != 0x00)
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOSetLevelEmul,
                        EAPI_STATUS_INVALID_PARAMETER,
                        "Cant Set Level on Pin that is set to input");

        for (i = 0 ; i < gpioLines ; i++)
        {
            if (BitMask & (0x01 << i)) /* Bitmask is EAPI_GPIO_BITMASK_SELECT*/
            {
                if(req[i].fd <= 0)
                {
                    snprintf(err,sizeof(err),"Failed to access to pin %d",i);
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOSetLevelEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }

                memset(&data, 0, sizeof(data));

                if(Level & (0x01 << i))
                    data.values[0] = EAPI_GPIO_HIGH;
                else
                    data.values[0] = EAPI_GPIO_LOW;

                ret = ioctl(req[i].fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
                if(ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to issue GPIOHANDLE GET LINE VALUES of pin %d: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOSetLevelEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
            }
        }
    }

    /* read back GPIO level*/
    //        ret = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &readBack);
    //        if(ret == -1)
    //        {
    //             printf("debug_4\n");
    //            snprintf(err,sizeof(err),"Failed to issue GPIOHANDLE GET LINE VALUES IOCTL: %s",strerror(errno));
    //            EAPI_LIB_RETURN_ERROR(
    //                        EApiGPIOSetLevelEmul,
    //                        EAPI_STATUS_ERROR,
    //                        err);
    //        }
    //        for(i=0; i< req.lines; i++)
    //        {
    //            if(data.values[i] != readBack.values[i])
    //            {
    //                 printf("debug_5\n");
    //                snprintf(err,sizeof(err),"Failed to SET GPIO LINE VALUES IOCTL as requested");
    //                EAPI_LIB_RETURN_ERROR(
    //                            EApiGPIOSetLevelEmul,
    //                            EAPI_STATUS_ERROR,
    //                            err);
    //            }
    //        }


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
    int ret;

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
        struct gpioline_info linfo;
        memset(&linfo, 0, sizeof(linfo));

        linfo.line_offset = index;
        ret = ioctl(gpiofd, GPIO_GET_LINEINFO_IOCTL, &linfo);
        if (ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to issue lineinfo ioctl %d: %s",index,strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        EApiGPIOGetDirectionEmul,
                        EAPI_STATUS_ERROR,
                        err);
        }
        else
        {
            if((linfo.flags & GPIOLINE_FLAG_IS_OUT) == GPIOLINE_FLAG_IS_OUT)
                *pDirection = EAPI_GPIO_OUTPUT;
            else
                *pDirection = EAPI_GPIO_INPUT;
        }
    }
    else /* bank request */
    {
        for (i =0; i < gpioLines; i++)
        {
            unsigned bit = BitMask & ( 0x01 << i);
            if (bit != 0x00) /* Bitmask is EAPI_GPIO_BITMASK_SELECT*/
            {
                struct gpioline_info linfo;
                memset(&linfo, 0, sizeof(linfo));

                linfo.line_offset = i;
                ret = ioctl(gpiofd, GPIO_GET_LINEINFO_IOCTL, &linfo);
                if (ret == -1)
                {
                    snprintf(err,sizeof(err),"Failed to issue lineinfo ioctl %d: %s",i,strerror(errno));
                    EAPI_LIB_RETURN_ERROR(
                                EApiGPIOGetDirectionEmul,
                                EAPI_STATUS_ERROR,
                                err);
                }
                else
                {
                    if((linfo.flags & GPIOLINE_FLAG_IS_OUT) == GPIOLINE_FLAG_IS_OUT)
                        *pDirection |= (EAPI_GPIO_OUTPUT << i);
                    else
                        *pDirection |= (EAPI_GPIO_INPUT << i );
                }
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
    int ret;

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
        if(req[index].fd > 0)
            close(req[index].fd);

        req[index].lineoffsets[0] = index;
        req[index].lines = 1;

        if (Direction) /* EAPI_GPIO_INPUT */
            req[index].flags = GPIOHANDLE_REQUEST_INPUT;
        else
        {
            req[index].flags = GPIOHANDLE_REQUEST_OUTPUT;
           // req[index].default_values[0] = 0;
        }
        req[index].default_values[0] = 0;

        ret = ioctl(gpiofd, GPIO_GET_LINEHANDLE_IOCTL, &req[index]);
         if(ret == -1 || req[index].fd <= 0)
        {
            snprintf(err,sizeof(err),"Failed to issue GET LINEHANDLE IOCTL %d: %s",index,strerror(errno));
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
                 if(req[i].fd > 0)
                     close(req[i].fd);
                req[i].lineoffsets[0] = i;
                req[i].lines = 1;
                if(Direction & (0x01 << i))
                    req[i].flags = GPIOHANDLE_REQUEST_INPUT;
                else
                {
                    req[i].flags = GPIOHANDLE_REQUEST_OUTPUT;
                   // req[i].default_values[0] = 0;
                }

                 req[i].default_values[0] = 0;

                ret = ioctl(gpiofd, GPIO_GET_LINEHANDLE_IOCTL, &req[i]);
                if(ret == -1 || req[i].fd <= 0)
                {
                    snprintf(err,sizeof(err),"Failed to issue GET LINEHANDLE IOCTL %d: %s",i,strerror(errno));
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


