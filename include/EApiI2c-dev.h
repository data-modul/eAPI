/*
     i2c-dev.h - i2c-bus driver, char device interface

     Copyright (C) 1995-97 Simon G. Vogl
     Copyright (C) 1998-99 Frodo Looijaard <frodol@dds.nl>

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
     MA 02110-1301 USA.
*/

#ifndef EAPII2CDEV_H
#define EAPII2CDEV_H

#include <linux/types.h>
#include <sys/ioctl.h>
#include <stddef.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/* To determine what functionality is present */

 #define I2C_FUNC_I2C                   0x00000001
 #define I2C_FUNC_SMBUS_READ_BYTE       0x00020000
 #define I2C_FUNC_SMBUS_WRITE_BYTE      0x00040000
 #define I2C_FUNC_SMBUS_READ_BYTE_DATA  0x00080000
 #define I2C_FUNC_SMBUS_WRITE_BYTE_DATA 0x00100000
 #define I2C_FUNC_SMBUS_READ_WORD_DATA  0x00200000
 #define I2C_FUNC_SMBUS_WRITE_WORD_DATA 0x00400000


#define I2C_SMBUS_BLOCK_MAX 32  /* As specified in SMBus standard */


#define I2C_SMBUS_READ 1
#define I2C_SMBUS_WRITE 0


/* SMBus transaction types (size parameter in the above functions)
    Note: these no longer correspond to the (arbitrary) PIIX4 internal codes! */
#define I2C_SMBUS_QUICK    0
#define I2C_SMBUS_BYTE    1
#define I2C_SMBUS_BYTE_DATA    2
#define I2C_SMBUS_WORD_DATA   3
#define I2C_SMBUS_PROC_CALL    4
#define I2C_SMBUS_BLOCK_DATA    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8


/* /dev/i2c-X ioctl commands.  The ioctl's parameter is always an
 * unsigned long, except for:
 *»·····- I2C_FUNCS, takes pointer to an unsigned long
 *»·····- I2C_RDWR, takes pointer to struct i2c_rdwr_ioctl_data
 *»·····- I2C_SMBUS, takes pointer to struct i2c_smbus_ioctl_data
*/
/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
*/
#define I2C_SLAVE       0x0703 /* Use this slave address */

#define I2C_FUNCS       0x0705 /* Get the adapter functionality mask */

#define I2C_SMBUS       0x0720 /* SMBus transfer */

#define CHECK_I2C_FUNC( var, label ) \
    do { 	if(0 == (var & label)) { \
    fprintf(stderr, "\nError: " \
#label " function is required. Program halted.\n\n"); \
    exit(1); } \
    } while(0);


static inline __s32 i2c_access(int file, __u32 LclAddr, __u32 WriteBCnt, __u8 *pWBuffer, __u32 ReadBCnt, __u8 *pRBuffer)
{
    int nmsgs, nmsgs_sent;
    struct i2c_rdwr_ioctl_data rdwr;
    struct i2c_msg msgs[2];

    
    if(!ReadBCnt) 
    {
        msgs[0].addr = (__u16)(LclAddr & 0xFFFF);
        msgs[0].flags = 0;
        msgs[0].len = (__u16)(WriteBCnt & 0xFFFF);
        msgs[0].buf = pWBuffer;
        nmsgs = 1;
    }
    else 
    {
        msgs[0].addr = (__u16)(LclAddr & 0xFFFF);
        msgs[0].flags = 0;
        msgs[0].len = (__u16)(WriteBCnt & 0xFFFF);
        msgs[0].buf = pWBuffer;
        msgs[1].addr = msgs[0].addr;
        msgs[1].flags = I2C_M_RD;
        msgs[1].len = (__u16)(ReadBCnt & 0xFFFF);
        msgs[1].buf = pRBuffer;                        
        nmsgs = 2;
    }
    
    rdwr.msgs = msgs;
    rdwr.nmsgs = nmsgs;
    nmsgs_sent = ioctl(file, I2C_RDWR, &rdwr);
    
    return nmsgs_sent;
}

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,int size, union i2c_smbus_data *data)
{
struct i2c_smbus_ioctl_data args;

args.read_write = read_write;
args.command = command;
args.size = size;
args.data = data;
return ioctl(file,I2C_SMBUS,&args);
}

static inline __s32 i2c_smbus_read_byte(int file)
{
union i2c_smbus_data data;
if (i2c_smbus_access(file,I2C_SMBUS_READ,0,I2C_SMBUS_BYTE,&data))
return -1;
else
return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte(int file, __u8 value)
{
return i2c_smbus_access(file,I2C_SMBUS_WRITE,value,I2C_SMBUS_BYTE,NULL);
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value)
{
union i2c_smbus_data data;
data.byte = value;
return i2c_smbus_access(file,I2C_SMBUS_WRITE,command, I2C_SMBUS_BYTE_DATA, &data);
}


static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value)
{
union i2c_smbus_data data;
data.word = value;
return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,I2C_SMBUS_WORD_DATA, &data);
}

/* Returns the number of read bytes */
/* Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
   ask for less than 32 bytes, your code will only work with kernels
   2.6.23 and later. */
static inline __s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 length, __u8 *values)
{
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    data.block[0] = length;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                         length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
                         I2C_SMBUS_I2C_BLOCK_DATA,&data))
        return -1;
    else {
        for (i = 1; i <= data.block[0]; i++)
            values[i-1] = data.block[i];
        return data.block[0];
    }
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(file,I2C_SMBUS_READ,command, I2C_SMBUS_BYTE_DATA,&data))
        return -1;
    else
        return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_block_data(int file, __u8 command,__u8 length, const __u8 *values)
{
    union i2c_smbus_data data;
    int i;
    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;
    return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,I2C_SMBUS_BLOCK_DATA, &data);
}

#endif /* EAPII2CDEV_H */
