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

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadRawEmul(
        __IN      EApiId_t Id         , /* I2C Bus Id */
        __IN      uint8_t  Addr       , /* Encoded 7Bit I2C
                                         * Device Address
                                         */
        __INOPT   uint8_t  *pWBuffer  , /* Write Data pBuffer */
        __IN      uint32_t WriteBCnt  , /* Number of Bytes to write */
        __OUTOPT  uint8_t  *pRBuffer  , /* Read Data pBuffer */
        __IN      uint32_t ReadBCnt     /* Number of Bytes to read */
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t LclAddr;

    int i2cDescriptor = 0;
    char devname[20];
    int i2cbus;
    int sizeDev = sizeof(devname);
    int funcs;
    int nmsgs = 1;
    int nmsgs_sent;

    if (Id > 0xFFFF)
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadRawEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised I2C ID"
                    );

    i2cbus = Id;

    /* EAPI specification expects 8bit address from user, which should be 
     * shifted one bit to have 7bit address */
     LclAddr=EAPI_I2C_DEC_7BIT_ADDR(Addr);

#if (STRICT_VALIDATION>1)
    siFormattedMessage_M2(
                'L'                   ,
                __FILE__              ,
                "EApiI2CWriteReadRawEmul",
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
                    EApiI2CWriteReadRawEmul,
                    EAPI_STATUS_NOT_FOUND,
                   err);
    }

    /* set slave address : set working device */
    if(ioctl(i2cDescriptor, I2C_SLAVE, LclAddr) < 0)
    {
        snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadRawEmul,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }
    
    if (ReadBCnt)
        nmsgs = 2;
    
    nmsgs_sent = i2c_access(i2cDescriptor, LclAddr, WriteBCnt, pWBuffer, ReadBCnt, pRBuffer);
    
    if (nmsgs_sent < 0) 
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadRawEmul,
                    EAPI_STATUS_ERROR,
                    "i2c transaction failed");
    }
    else if (nmsgs_sent < nmsgs) 
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadRawEmul,
                    EAPI_STATUS_ERROR,
                    "not all i2c messages were sent");
    }    
    
    EAPI_LIB_RETURN_SUCCESS(EApiI2CWriteReadRawEmul, "");

    EAPI_LIB_ASSERT_EXIT
        return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadEmul(
        __IN      EApiId_t Id         , /* I2C Bus Id */
        __IN      uint8_t  Addr       , /* Encoded 7Bit I2C
                                         * Device Address
                                         */
        __INOPT   uint8_t  *pWBuffer  , /* Write Data pBuffer */
        __IN      uint32_t WriteBCnt  , /* Number of Bytes to write plus 1 */
        __IN     uint32_t  CmdBCnt,     /* Number of Cmd Bytes to write */
        __OUTOPT  uint8_t  *pRBuffer  , /* Read Data pBuffer */
        __IN      uint32_t ReadBCnt     /* Number of Bytes to read plus 1 */
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t LclAddr;
    int size = I2C_SMBUS_BYTE;
    uint32_t daddress=0;

    int i2cDescriptor = 0;
    char devname[20];
    int i2cbus;
    int sizeDev = sizeof(devname);
    int funcs;

    if (Id > 0xFFFF)
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised I2C ID"
                    );

    i2cbus = Id;

    /* EAPI specification expects 8bit address from user, which should be 
     * shifted one bit to have 7bit address */
     LclAddr=EAPI_I2C_DEC_7BIT_ADDR(Addr);

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
    
    /* set slave address : set working device */
    if(ioctl(i2cDescriptor, I2C_SLAVE_FORCE, LclAddr) < 0)
    {
        snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
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

    /* check for I2C protocol */
    if(funcs & I2C_FUNC_I2C)
    {
        close(i2cDescriptor);

        if(WriteBCnt) WriteBCnt--;
        if(ReadBCnt) ReadBCnt--;        

        StatusCode = EApiI2CWriteReadRawEmul(Id, Addr, pWBuffer, WriteBCnt, pRBuffer, ReadBCnt);

        EAPI_LIB_RETURN_ERROR_IF(
                    EApiI2CWriteReadEmul,
                    StatusCode,
                    StatusCode,
                    "EApiI2CWriteReadRawEmul failed"
                    );
        EAPI_LIB_RETURN_SUCCESS(EApiI2CWriteReadEmul, "");
    }
    
    /* check for SMBUS protocol funcs */
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_QUICK );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );
        
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
    
    /* with SMBUS protocol we have either write or read. never combined */

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

        while (iRead < ReadBCnt)
        {
            res = 0;
            if (size == CMD_TYPE_16BIT)
            {
                res = i2c_smbus_write_byte_data(i2cDescriptor,(daddress >> 8) & 0x0ff, daddress & 0x0ff); /* write 16bits add */
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
            }
            else if (size == CMD_TYPE_8BIT)
            {
                res = i2c_smbus_read_byte_data(i2cDescriptor, daddress & 0x0ff);
                if (res < 0){
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
                else
                    LpRBuffer[iRead] = (uint8_t)res;
            }
            iRead++;
            daddress++;
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
        close(i2cDescriptor);
        return StatusCode;
}

