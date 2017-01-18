/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulI2C.c $
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
 *I Description: Auto Created for EApiEmulI2C.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulI2C.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulI2C.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>
#include <EApiI2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
 #include <errno.h>


#define CMD_TYPE_0BIT 0
#define CMD_TYPE_8BIT 1
#define CMD_TYPE_16BIT 2

EApiStatus_t 
EAPI_CALLTYPE 
EApiI2CGetBusCapEmul(
        __IN  EApiId_t  Id,
        __OUT uint32_t *pMaxBlkLen
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    if (Id > 0xFFFF)
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CGetBusCapEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised I2C ID"
                    );

    *pMaxBlkLen=512;
    EAPI_LIB_RETURN_SUCCESS(
                EApiI2CGetBusCapEmul,
                ""
                );

    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}
/*
 *
 *
 *
 *      I2C
 *
 *
 *
 */
#define CHECK_I2C_FUNC( var, label ) \
    do { 	if(0 == (var & label)) { \
    fprintf(stderr, "\nError: " \
#label " function is required. Program halted.\n\n"); \
    exit(1); } \
    } while(0);

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadEmul(
        __IN      EApiId_t Id         ,
        __IN      uint8_t  Addr       ,
        __INOPT   uint8_t    *pWBuffer  ,
        __IN      uint32_t WriteBCnt  ,
        __IN     uint32_t  CmdBCnt,
        __OUTOPT  uint8_t    *pRBuffer  ,
        __IN      uint32_t ReadBCnt
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t LclAddr;

    int i2cDescriptor = 0;
    char devname[20];
    int i2cbus;
    int sizeDev = sizeof(devname);
    int size = I2C_SMBUS_BYTE;
    int funcs;
    uint32_t daddress=0;

    if (Id > 0xFFFF)
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised I2C ID"
                    );

    i2cbus = Id;

    if((Addr & 0x80) == 0x80){ //10bit addr
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "10Bit Address is not supported"
                    );
    /*    LclAddr=Addr<<8;
        LclAddr|=*((uint8_t *)pWBuffer);
        pWBuffer=((uint8_t *)pWBuffer)+1;
        WriteBCnt--;*/
    }
    else
        LclAddr=Addr;

#if (STRICT_VALIDATION>1)
    siFormattedMessage_M2(
                'L'                   ,
                __FILE__              ,
                "EApiI2CWriteReadEmul",
                __LINE__              ,
                "Info"                ,
                "ADDR=%02"PRIX16"\n"  ,
                LclAddr
                );
#endif

    if (CmdBCnt == 1) // there is 8bit-Cmd
    {
        size = CMD_TYPE_8BIT;
        daddress=*((uint8_t *)pWBuffer);
        WriteBCnt--;
        pWBuffer=((uint8_t *)pWBuffer)+1;
    }
    else if (CmdBCnt == 2) // there is 16bit-Cmd
    {
        size = CMD_TYPE_16BIT;
        daddress = (uint32_t)(*((uint8_t *)pWBuffer) <<8);
        daddress|=(uint32_t)*(((uint8_t *)pWBuffer+1));
        pWBuffer=((uint8_t *)pWBuffer)+2;
        WriteBCnt=WriteBCnt-2;
    }
    else //no-cmd
    {
        size = CMD_TYPE_0BIT;
        daddress = 0;
    }
    /* open device */
    snprintf(devname,sizeDev,"/dev/i2c/%d",i2cbus);
    devname[sizeDev - 1] = '\0';
    i2cDescriptor = open(devname,O_RDWR);

    if(i2cDescriptor < 0)
    {
        sprintf(devname,"/dev/i2c-%d",i2cbus);
        i2cDescriptor = open(devname,O_RDWR);
    }

    if(i2cDescriptor < 0)
    {
        snprintf(err,sizeof(err),"Unrecognised i2c device %d: %s\n ",i2cbus,strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_NOT_FOUND,
                   err);
    }

    /* get funcs list */
    if(ioctl(i2cDescriptor, I2C_FUNCS, &funcs) < 0)
    {
        snprintf(err,sizeof(err),"Unrecognised i2c funcs: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }

    /* check for req funcs */
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );

    /* set slave address : set working device */
    if(ioctl(i2cDescriptor, I2C_SLAVE, LclAddr) < 0)
    {
        snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }
    /*  write to the device */
    if (WriteBCnt > 1)
        WriteBCnt--;
    else
        WriteBCnt =0;
    if(WriteBCnt)
    {
        uint32_t iWrite = 0;
        int res = 0;
        uint8_t value;

        if ((size == CMD_TYPE_8BIT) && (WriteBCnt == 32)) /* write block */
        {
            res = i2c_smbus_write_block_data(i2cDescriptor, daddress & 0x0ff, WriteBCnt, pWBuffer);
            if (res < 0){
                close(i2cDescriptor);
                snprintf(err,sizeof(err),"i2c writing failed: %s\n",strerror(errno));

                if (res == -ETIMEDOUT)
                    EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_TIMEOUT,
                                err);
                else
                    EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_WRITE_ERROR,
                                err);

            }
            else
                iWrite = WriteBCnt;
        }
        else
        {
            while (iWrite < WriteBCnt)
            {
                if (size == CMD_TYPE_16BIT){ /* 2bytes cmd */
                    value = *((uint8_t *)pWBuffer);
                    res = i2c_smbus_write_word_data(i2cDescriptor,(daddress >> 8) & 0x00ff, value << 8 | (daddress & 0x00ff));
                }
                else  if (size == CMD_TYPE_8BIT) /* one cmd */
                {
                    if (pWBuffer == NULL) /* no value */
                    {
                        res = i2c_smbus_write_byte(i2cDescriptor,daddress & 0x0ff);
                    }
                    else /* one cmd and values */
                    {
                        value = *((uint8_t *)pWBuffer);
                        res = i2c_smbus_write_byte_data(i2cDescriptor,daddress & 0x00ff, value);
                    }
                }
                if (res < 0){
                    close(i2cDescriptor);
                    snprintf(err,sizeof(err),"i2c writing failed: %s\n",strerror(errno));
                    if (res == -ETIMEDOUT)
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_TIMEOUT,
                                    err);
                    else
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_WRITE_ERROR,
                                    err);
                }
                pWBuffer=((uint8_t *)pWBuffer)+1;
                daddress++;
                iWrite++;
            }
        }
        close(i2cDescriptor);
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiI2CWriteReadEmul,
                    (iWrite != WriteBCnt) ,
                    EAPI_STATUS_WRITE_ERROR,
                    "i2c writing failed"
                    );
        EAPI_LIB_RETURN_SUCCESS(EApiI2CWriteReadEmul, "");
    }


    /*   Read from the device */
    if (ReadBCnt > 1)
        ReadBCnt--;
    else
        ReadBCnt =0;
    if(ReadBCnt)
    {
        uint8_t* LpRBuffer=pRBuffer;
        uint32_t iRead = 0;
        int res = 0;

        if ((ReadBCnt == 32) && (size == CMD_TYPE_8BIT))
        {
            res = i2c_smbus_read_i2c_block_data(i2cDescriptor, daddress & 0x0ff, ReadBCnt, LpRBuffer);
            if (res < 0)
            {
                close(i2cDescriptor);
                snprintf(err,sizeof(err),"i2c reading failed: %s\n",strerror(errno));

                if (res == -ETIMEDOUT)
                    EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_TIMEOUT,
                                err);
                else
                    EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_READ_ERROR,
                                err);
            }
            else if ((unsigned int)res != ReadBCnt)
            {
                close(i2cDescriptor);
                EAPI_LIB_RETURN_ERROR(
                            EApiI2CWriteReadEmul,
                            EAPI_STATUS_READ_ERROR,
                            "i2c reading failed: cannot read all requested bytes");
            }
            iRead = res;
        }
        else /* read bytes */
        {
            while (iRead < ReadBCnt)
            {
                res = 0;
                if (size == CMD_TYPE_16BIT)
                {
                    res = i2c_smbus_write_byte_data(i2cDescriptor,(daddress >> 8) & 0x0ff, daddress & 0x0ff); /* write 16bits add */
                }
                else if (size == CMD_TYPE_8BIT)
                {
                    res = i2c_smbus_write_byte(i2cDescriptor,daddress & 0x0ff);
                }

                if (res < 0)
                {
                    close(i2cDescriptor);
                    snprintf(err,sizeof(err),"i2c writing Cmd for reading failed: %s\n",strerror(errno));
                    if (res == -ETIMEDOUT)
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_TIMEOUT,
                                    err);
                    else
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_WRITE_ERROR,
                                    err);
                }


                res = i2c_smbus_read_byte(i2cDescriptor);
                snprintf(err,sizeof(err),"i2c reading failed: %s\n",strerror(errno));
                if (res < 0){
                    close(i2cDescriptor);
                    if (res == -ETIMEDOUT)
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_TIMEOUT,
                                    err);
                    else
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_READ_ERROR,
                                    err);
                }
                else
                    LpRBuffer[iRead] = (uint8_t)res;
                iRead++;
                daddress++;
            }
        }

        close(i2cDescriptor);
        EAPI_LIB_RETURN_ERROR_IF(
                    EApiI2CWriteReadEmul,
                    (iRead != ReadBCnt) ,
                    EAPI_STATUS_READ_ERROR,
                    "Device Read Error"
                    );

        LpRBuffer[iRead]= '\0';
        pRBuffer = LpRBuffer;

        EAPI_LIB_RETURN_SUCCESS(EApiI2CWriteReadEmul, "");
    }

     EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadEmulUniversal(
        __IN      EApiId_t Id         ,
        __IN      uint8_t  Addr       ,
        __INOPT   uint8_t    *pWBuffer  ,
        __IN      uint32_t WriteBCnt  ,
        __OUTOPT  uint8_t    *pRBuffer  ,
        __IN      uint32_t ReadBCnt
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t LclAddr;

    int i2cDescriptor = 0;
    char devname[20];
    int i2cbus;
    int sizeDev = sizeof(devname);
    int funcs;

    if (Id > 0xFFFF)
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmulUniversal,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised I2C ID"
                    );

    i2cbus = Id;

    if((Addr & 0x80) == 0x80){ //10bit addr
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmulUniversal,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "10Bit Address is not supported"
                    );
  /*      LclAddr=Addr<<8;
        LclAddr|=*((uint8_t *)pWBuffer);
        pWBuffer=((uint8_t *)pWBuffer)+1;
        WriteBCnt--; */
    }
    else
        LclAddr=Addr;

#if (STRICT_VALIDATION>1)
    siFormattedMessage_M2(
                'L'                   ,
                __FILE__              ,
                "EApiI2CWriteReadEmulUniversal",
                __LINE__              ,
                "Info"                ,
                "ADDR=%02"PRIX16"\n"  ,
                LclAddr
                );
#endif


    /* open device */
    snprintf(devname,sizeDev,"/dev/i2c/%d",i2cbus);
    devname[sizeDev - 1] = '\0';
    i2cDescriptor = open(devname,O_RDWR);

    if(i2cDescriptor < 0)
    {
        sprintf(devname,"/dev/i2c-%d",i2cbus);
        i2cDescriptor = open(devname,O_RDWR);
    }

    if(i2cDescriptor < 0)
    {
        snprintf(err,sizeof(err),"Unrecognised i2c device %d: %s\n ",i2cbus,strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmulUniversal,
                    EAPI_STATUS_NOT_FOUND,
                   err);
    }

    /* get funcs list */
    if(ioctl(i2cDescriptor, I2C_FUNCS, &funcs) < 0)
    {
        snprintf(err,sizeof(err),"Unrecognised i2c funcs: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmulUniversal,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }

    /* check for req funcs */
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );

    /* set slave address : set working device */
    if(ioctl(i2cDescriptor, I2C_SLAVE, LclAddr) < 0)
    {
        snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmulUniversal,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }


    /*  write to the device */
    if (WriteBCnt > 1)
        WriteBCnt--;
    else
        WriteBCnt =0;
    if(WriteBCnt)
    {
        if (write(i2cDescriptor, pWBuffer, WriteBCnt) != WriteBCnt)
        {
            EAPI_LIB_RETURN_ERROR(
                        EApiI2CWriteReadEmulUniversal,
                        EAPI_STATUS_WRITE_ERROR,
                        "i2c transaction write failed");
        }
    }


    /*   Read from the device */
    if (ReadBCnt > 1)
        ReadBCnt--;
    else
        ReadBCnt =0;
    if(ReadBCnt)
    {
        if (read(i2cDescriptor, pRBuffer, ReadBCnt) != ReadBCnt)
        {
            EAPI_LIB_RETURN_ERROR(
                        EApiI2CWriteReadEmulUniversal,
                        EAPI_STATUS_READ_ERROR,
                        "i2c transacrion read failed");
        }
    }

    EAPI_LIB_RETURN_SUCCESS(EApiI2CWriteReadEmulUniversal, "");
     EAPI_LIB_ASSERT_EXIT
             close(i2cDescriptor);
            return StatusCode;
}
