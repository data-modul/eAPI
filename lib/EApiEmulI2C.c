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
        __INOPT   void    *pWBuffer  ,
        __IN      uint32_t WriteBCnt  ,
        __OUTOPT  void    *pRBuffer  ,
        __IN      uint32_t ReadBCnt
        )
{

    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t LclAddr;
    ////
    int i2cDescriptor = 0;
    char devname[20];
    int i2cbus; //Id
    int sizeDev = sizeof(devname);
    int size = I2C_SMBUS_BYTE;
    uint32_t daddress=0;
    int no_add = 0;

    if (Id > 0xFFFF)
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_UNSUPPORTED  ,
                    "Unrecognised I2C ID"
                    );

    i2cbus = Id;

    if (Addr <  0x03 || Addr > 0x77)
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_NOT_FOUND,
                    "Chip address out of range"
                    );
    }
    if(EAPI_I2C_IS_10BIT_ADDR(Addr<<8)){
        LclAddr=Addr<<8;
        LclAddr|=*((uint8_t *)pWBuffer);
        pWBuffer=((uint8_t *)pWBuffer)+1;
        WriteBCnt--;
        if (ReadBCnt > 0 && pRBuffer==NULL) //means some data in wbuffer is not for write, there are info
            ReadBCnt--;
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


    if (ReadBCnt > 0 && pRBuffer!=NULL) // it means func is called for ReadTransfer
    {

        if(WriteBCnt > 1) //there is a Cmd
        {
            //cmd is 8 bit
            size = I2C_SMBUS_BYTE;
            daddress=*((uint8_t *)pWBuffer);
            WriteBCnt--;

            //cmd is 16bits
            if (WriteBCnt > 1)
            {
                size = I2C_SMBUS_BYTE_DATA;
                daddress|=*(((uint8_t *)pWBuffer+1));
                WriteBCnt--;
            }
        }
        else
        {
            size = I2C_SMBUS_BYTE;
            no_add = 1;
        }
    }
    else if (ReadBCnt > 0 && pRBuffer==NULL)// it means func is called  for WriteTransfer
    {
        if (ReadBCnt == 1) // it has only 1byte-Cmd
        {
            daddress = *((uint8_t *)pWBuffer);
            pWBuffer=((uint8_t *)pWBuffer)+1;
            WriteBCnt--;
            size = I2C_SMBUS_BYTE;
        }
        else if (ReadBCnt == 2)// it has 2bytes-Cmd
        {
            daddress = (uint32_t)(*((uint8_t *)pWBuffer) <<8);
            daddress|=(uint32_t)*(((uint8_t *)pWBuffer+1));

            pWBuffer=((uint8_t *)pWBuffer)+2;
            WriteBCnt=WriteBCnt-2;
            ReadBCnt=ReadBCnt-2;
            size = I2C_SMBUS_BYTE_DATA;
        }
        else //no cmd
        {
            EAPI_LIB_RETURN_ERROR(
                        EApiI2CWriteReadEmul,
                        EAPI_STATUS_NOT_FOUND,
                        "Proper Command is required."
                        );
        }

    }

    // open device
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

    // get funcs list
    int funcs;
    if(ioctl(i2cDescriptor, I2C_FUNCS, &funcs) < 0)
    {
        snprintf(err,sizeof(err),"Unrecognised i2c funcs: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }

    // check for req funcs
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
    CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );

    // set slave address : set working device
    if(ioctl(i2cDescriptor, I2C_SLAVE, LclAddr) < 0)
    {
        snprintf(err,sizeof(err),"Cannot set i2c slave ddress: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiI2CWriteReadEmul,
                    EAPI_STATUS_NOT_FOUND,
                    err);
    }

    //  write to the device
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
            if (size == I2C_SMBUS_BYTE_DATA){
                value = *((uint8_t *)pWBuffer);
                res = i2c_smbus_write_word_data(i2cDescriptor,(daddress >> 8) & 0x00ff, value << 8 | (daddress & 0x00ff));
            }
            else  if (size == I2C_SMBUS_BYTE) // one cmd
            {
                if (pWBuffer == NULL) //no value
                {
                    res = i2c_smbus_write_byte(i2cDescriptor,daddress & 0x0ff);
                }
                else // one cmd and values
                {
                    value = *((uint8_t *)pWBuffer);
                    res = i2c_smbus_write_byte_data(i2cDescriptor,daddress & 0x00ff, value);
                }
            }
            if (res < 0){
                close(i2cDescriptor);

                if (res == -ETIMEDOUT)
                    EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_TIMEOUT,
                                "i2c writing failed: time-out");
                else
                    EAPI_LIB_RETURN_ERROR(
                            EApiI2CWriteReadEmul,
                            EAPI_STATUS_WRITE_ERROR,
                            "i2c writing failed");
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


    //   Read from the device

    if (ReadBCnt > 1)
        ReadBCnt--;
    else
        ReadBCnt =0;

    if(ReadBCnt)
    {
        uint8_t* LpRBuffer=pRBuffer;
        uint32_t iRead = 0;
        int res = 0;
        int flag = 0;

        while (iRead < ReadBCnt)
        {
            if (flag == 0 && no_add != 1 )
            {
                if (size == I2C_SMBUS_BYTE_DATA)
                    res = i2c_smbus_write_byte_data(i2cDescriptor,(daddress >> 8) & 0x0ff, daddress & 0x0ff); //write 16bits add
                else if (size == I2C_SMBUS_BYTE)
                    res = i2c_smbus_write_byte(i2cDescriptor,daddress & 0x0ff);

                flag = 1; // cmd write is done, no needed more
                if (res < 0)
                {
                    close(i2cDescriptor);
                    if (res == -ETIMEDOUT)
                        EAPI_LIB_RETURN_ERROR(
                                    EApiI2CWriteReadEmul,
                                    EAPI_STATUS_TIMEOUT,
                                    "i2c writing failed: time-out");
                    else
                        EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_WRITE_ERROR,
                                "Cmd-i2c writing failed");
                    break;
                }
            }

            res = i2c_smbus_read_byte(i2cDescriptor);
            if (res < 0){
                close(i2cDescriptor);
                if (res == -ETIMEDOUT)
                    EAPI_LIB_RETURN_ERROR(
                                EApiI2CWriteReadEmul,
                                EAPI_STATUS_TIMEOUT,
                                "i2c reading failed: time-out");
                else
                    EAPI_LIB_RETURN_ERROR(
                            EApiI2CWriteReadEmul,
                            EAPI_STATUS_READ_ERROR,
                            "i2c reading failed");
            }
            else
                LpRBuffer[iRead] = (uint8_t)res;

            iRead++;
        }

        close(i2cDescriptor);

        EAPI_LIB_RETURN_ERROR_IF(
                    EApiI2CWriteReadEmul,
                    (iRead != ReadBCnt) ,
                    EAPI_STATUS_READ_ERROR,
                    "Device Read Error"
                    );

        pRBuffer = LpRBuffer;
        EAPI_LIB_RETURN_SUCCESS(EApiI2CWriteReadEmul, "");
    }

     EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

