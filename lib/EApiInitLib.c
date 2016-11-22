/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiInitLib.c $
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
 *I Description: Auto Created for EApiInitLib.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiInitLib.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiInitLib.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
#include <EApiLib.h>
#include <stdio.h>
#include <fcntl.h>
#include <EApiI2c-dev.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <linux/gpio.h>
#include <EApiDmo.h>

FILE *OutputStream=NULL;
uint8_t *eepromBuffer;
int board_type;
char *hwname;
char err[256];

struct gpiohandle_request *req = NULL;
static char *gpioName = NULL;
unsigned int gpioLines = 0;
int gpiofd  = -1;
int gpioEnabled = 0 ;



struct gpio_flag {
    char *name;
    unsigned long mask;
};

struct gpio_flag flagnames[] = {
{
    .name = "kernel",
    .mask = GPIOLINE_FLAG_KERNEL,
},
{
    .name = "output",
    .mask = GPIOLINE_FLAG_IS_OUT,
},
{
    .name = "active-low",
    .mask = GPIOLINE_FLAG_ACTIVE_LOW,
},
{
    .name = "open-drain",
    .mask = GPIOLINE_FLAG_OPEN_DRAIN,
},
{
    .name = "open-source",
    .mask = GPIOLINE_FLAG_OPEN_SOURCE,
},
};

void __cdecl DebugMsg(__IN const char *const fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    if(OutputStream!=NULL) {
        vfprintf(OutputStream, fmt, arg);
        /*else
        vfprintf(stdout, fmt, arg);*/

        fflush(OutputStream);
    }
    va_end(arg);
}


EApiStatus_t find_hwmon()
{
    char s[NAME_MAX];
    struct dirent *de;
    DIR *dir;
    FILE *f;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char *px;

    char sysfs[NAME_MAX], n[NAME_MAX];
    snprintf(sysfs,sizeof(HWMON_PATH),HWMON_PATH);

    if(!(dir = opendir(sysfs)))
    {
        hwname = NULL;

        snprintf(err,sizeof(err),"%s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    find_hwmon,
                    EAPI_STATUS_UNSUPPORTED,
                    err);
    }
    /* go through the hwmons */
    while ((de = readdir(dir)) != NULL) {

        if (!strcmp(de->d_name, "."))
            continue;
        if (!strcmp(de->d_name, ".."))
            continue;
        /* this should work for kernels 2.6.5 or higher and */
        /* is preferred because is unambiguous */
        sprintf(n, "%s/%s/name", sysfs, de->d_name);
        f = fopen(n, "r");
        /* this seems to work for ISA */
        if(f == NULL) {
            sprintf(n, "%s/%s/device/name", sysfs, de->d_name);
            f = fopen(n, "r");
        }
        if(f == NULL)
        {
            hwname = NULL;
            closedir(dir);
            snprintf(err,sizeof(err),"%s",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        find_hwmon,
                        EAPI_STATUS_UNSUPPORTED,
                        err);
        }
        px = fgets(s, NAME_MAX, f);
        fclose(f);
        if (!px) {
            fprintf(stderr, "%s: read error\n", n);
            continue;
        }
        if ((px = strchr(s, '\n')) != NULL)
            *px = 0;

        if( (s != NULL) && (!strncmp(s, HWMON_NAME,sizeof(HWMON_NAME))))
        {
            hwname = (char*)malloc(sizeof(de->d_name)*sizeof(char));
            if (!hwname)
            {
                EAPI_LIB_RETURN_ERROR(
                            find_hwmon,
                            EAPI_STATUS_ALLOC_ERROR,
                            "Error in Allocating Memory");
            }
            strncpy(hwname, de->d_name,sizeof(de->d_name));
            StatusCode = EAPI_STATUS_SUCCESS;
            break;
        }
        else
        {
            hwname = NULL;
            StatusCode = EAPI_STATUS_UNSUPPORTED;
        }

    }

    closedir(dir);
    if (StatusCode == EAPI_STATUS_UNSUPPORTED )
        EAPI_LIB_RETURN_ERROR(
                    find_hwmon,
                    EAPI_STATUS_UNSUPPORTED,
                    "Info: No HWMON is found.");
    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t fill_eepromBuffer(uint8_t **result)
{
    int eeprom_bus;
    uint32_t  Cmd ;
    char devname[20];
    int i2cDescriptor = 0;
    int res = 0;
    int iRead = 0;
    //   uint8_t * pBoardType;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    /* ******************** EEPROM ************************** */
    /* Find Eeprom i2c-bus and read & fill the buffer */
    eeprom_bus = find_eeprom();

    if (eeprom_bus >= 0)
    {
        /* read eeprom */
        *result = (uint8_t *)calloc(EEPROM_SIZE, sizeof(uint8_t));
        if (!*result)
        {
            snprintf(err,sizeof(err),"Error in Eeprom Allocating Memory\n");
            printf("%s",err);
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_ALLOC_ERROR,
                        err);
        }
        Cmd = EAPI_I2C_ENC_EXT_CMD(0x00);

        /* open device */
        snprintf(devname,sizeof(devname),"/dev/i2c/%d",eeprom_bus);
        devname[sizeof(devname) - 1] = '\0';
        i2cDescriptor = open(devname,O_RDWR);
        if(i2cDescriptor < 0)
        {
            sprintf(devname,"/dev/i2c-%d",eeprom_bus);
            i2cDescriptor = open(devname,O_RDWR);
        }
        if(i2cDescriptor < 0)
        {
            snprintf(err,sizeof(err),"Unrecognised Eeprom Address: %s\n ",strerror(errno));
            printf("%s",err);
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_NOT_FOUND,
                        err);
        }
        /* set slave address : set working device */
        if(ioctl(i2cDescriptor, I2C_SLAVE, EEPROM_DEVICE) < 0)
        {
            snprintf(err,sizeof(err),"Cannot set Eeprom slave ddress: %s\n",strerror(errno));
            printf("%s",err);
            EAPI_LIB_RETURN_ERROR(
                        fill_eepromBuffer,
                        EAPI_STATUS_NOT_FOUND,
                        err);
        }
        if(EEPROM_SIZE)
        {
            res = i2c_smbus_write_byte_data(i2cDescriptor,(Cmd >> 8) & 0x0ff, Cmd & 0x0ff); /* write 16bits add */
            if (res < 0)
            {
                snprintf(err,sizeof(err),"Cannot write into Eeprom: %s\n",strerror(errno));
                printf("%s",err);
                EAPI_LIB_RETURN_ERROR(
                            fill_eepromBuffer,
                            EAPI_STATUS_WRITE_ERROR,
                            err);
            }
            while (iRead < EEPROM_SIZE)
            {
                res = i2c_smbus_read_byte(i2cDescriptor);
                if (res < 0)
                {
                    snprintf(err,sizeof(err),"Cannot read from Eeprom: %s\n",strerror(errno));
                    printf("%s",err);
                    EAPI_LIB_RETURN_ERROR(
                                fill_eepromBuffer,
                                EAPI_STATUS_READ_ERROR,
                                err);
                }
                else
                {
                    (*result)[iRead] = (uint8_t) res;
                }
                iRead++;
            }
            close(i2cDescriptor);
        }
        /* ********************************************** */
        //        /* detect_board_type */
        //        pBoardType = eeprom_analyze(*result,BOARD_ID_TYPE,BOARD_ID_ASCII_IND);
        //        board_type = UNKNOWN;
        //        if (pBoardType)
        //        {
        //            if(!strncmp((const char*)pBoardType,"BBW6",4))
        //                board_type = BBW6;
        //            else if(!strncmp((const char*)pBoardType,"CBS6",4))
        //                board_type = CBS6;
        //            free(pBoardType);
        //        }
    }
    else
    {
        EAPI_LIB_RETURN_ERROR(
                    fill_eepromBuffer,
                    EAPI_STATUS_ERROR,
                    "No Eeprom Bus is found");
    }
    EAPI_LIB_ASSERT_EXIT
            return StatusCode;

}

EApiStatus_t list_gpio_device()
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    struct dirent *de;
    DIR *dir;
    struct gpiochip_info cinfo;
    char chrdev_name[NAME_MAX];
    int fd;
    int ret;

    gpioName = NULL;
    gpioLines = 0;

    if(!(dir = opendir("/dev/")))
    {
        snprintf(err,sizeof(err),"Failed to open /dev: %s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    list_gpio_device,
                    EAPI_STATUS_UNSUPPORTED,
                    err);
    }

    /* go through the gpio devices */
    while ((de = readdir(dir)) != NULL) {
        if (strstr(de->d_name, "gpiochip") != NULL)
        {
            sprintf(chrdev_name, "/dev/%s", de->d_name);
            fd = open(chrdev_name, 0);
            if (fd == -1) {
                snprintf(err,sizeof(err),"Failed to open %s: %s",chrdev_name,strerror(errno));
                EAPI_FORMATED_MES('E',
                                  list_gpio_device,
                                  EAPI_STATUS_UNSUPPORTED,
                                  err
                                  );
                continue;
            }

            /* Inspect this GPIO chip */
            ret = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo);
            if (ret == -1) {
                snprintf(err,sizeof(err),"Failed to issue CHIPINFO IOCTL: %s",strerror(errno));
                EAPI_FORMATED_MES('E',
                                  list_gpio_device,
                                  EAPI_STATUS_UNSUPPORTED,
                                  err
                                  );
            }
            else
            {
                if (!strncmp(cinfo.label, "dmec-gpio.1", sizeof("dmec-gpio.1"))) /* dmec gpio found */
                {
                    gpioName = (char*)malloc(sizeof(cinfo.name)*sizeof(char));
                    strncpy(gpioName, cinfo.name,sizeof(cinfo.name));
                    gpioLines = cinfo.lines;
                    gpioEnabled = 1;
                    close(fd);
                    break;
                }
            }
            close(fd);
        }
        else
            continue;
    }
    closedir(dir);

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t gpio_dev_open(const char *device_name)
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char chrdev_name[NAME_MAX];
    int ret = -1;
    unsigned int i;


    snprintf(chrdev_name, sizeof("/dev/")+sizeof(device_name)+1,"/dev/%s",device_name);
    gpiofd = open(chrdev_name, O_WRONLY);
    if(gpiofd < 0)
    {
        gpioEnabled = 0;
        snprintf(err,sizeof(err),"Failed to open gpio device: %s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    gpio_dev_open,
                    EAPI_STATUS_ERROR,
                    err);
    }
    req = (struct gpiohandle_request*) malloc(sizeof(struct gpiohandle_request)*gpioLines);
    if(req == NULL)
    {
        gpioEnabled = 0;
        snprintf(err,sizeof(err),"Failed to malloc memory for gpio request handler.");
        EAPI_LIB_RETURN_ERROR(
                    gpio_dev_open,
                    EAPI_STATUS_ERROR,
                    err);
    }
    for (i =0; i < gpioLines; i++)
    {
        struct gpioline_info linfo;

        memset(&linfo, 0, sizeof(linfo));
        memset(&req[i], 0, sizeof(req[i]));

        req[i].lineoffsets[0] = i;
        req[i].lines = 1;

        linfo.line_offset = i;
        ret = ioctl(gpiofd, GPIO_GET_LINEINFO_IOCTL, &linfo);
        if (ret == -1)
        {
            snprintf(err,sizeof(err),"Failed to issue lineinfo ioctl %d: %s",i,strerror(errno));
            EAPI_FORMATED_MES('E',
                              gpio_dev_open,
                              EAPI_STATUS_ERROR,
                              err
                              );
        }
        else
        {
            if((linfo.flags & GPIOLINE_FLAG_IS_OUT) == GPIOLINE_FLAG_IS_OUT)
           {
                req[i].flags = GPIOHANDLE_REQUEST_OUTPUT;
                req[i].default_values[0] = 0;
            }
            else
                req[i].flags = GPIOHANDLE_REQUEST_INPUT;
        }

        ret = ioctl(gpiofd, GPIO_GET_LINEHANDLE_IOCTL, &req[i]);
        if(ret == -1 || req[i].fd <= 0)
        {
            snprintf(err,sizeof(err),"Failed to issue GET LINEHANDLE IOCTL for pin %d: %s",i,strerror(errno));
            EAPI_FORMATED_MES('E',
                              gpio_dev_open,
                              EAPI_STATUS_ERROR,
                              err
                              );
        }
    }

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t 
EApiInitLib(){

    char* logpath = getenv("LOGPATH");
    char path[NAME_MAX];
    FILE *f = NULL;
    char line[NAME_MAX];

    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    EApiStatus_t StatusCode2=EAPI_STATUS_SUCCESS;

    if(logpath == NULL)
        OutputStream=NULL;
    else {
        strcat(logpath, "/EApi.log");
        OutputStream=fopen(logpath, TEXT("w"));
        if (OutputStream == NULL)
            printf("Cannot create log file in %s: %s\n",logpath,strerror(errno));
    }

    DebugMsg("#\n"
             "# Embedded API EApi\n"
             "# Version %u.%u\n"
             "# Lib Version %u.%u.%u\n"
             "#\n",
             EAPI_VER_GET_VER(EAPI_VERSION), EAPI_VER_GET_REV(EAPI_VERSION),
             LIB_VERSION, LIB_REVISION, LIB_BUILD
             );
    /* ******************** EEPROM ************************** */
    eepromBuffer = NULL;
    fill_eepromBuffer(&eepromBuffer);

    /* ******************** Detect_board_type ************************** */
    strncpy(path, "/sys/class/dmi/id/board_name", sizeof("/sys/class/dmi/id/board_name"));
    board_type = UNKNOWN;
    f = fopen(path, "r");
    if (f != NULL)
    {
        char* res = fgets(line, sizeof(line), f);
        fclose(f);
        if (res == NULL)
        {
            EAPI_FORMATED_MES('E',
                              EApiInitLib,
                              EAPI_STATUS_UNSUPPORTED,
                              err
                              );
        }
        else
        {
            if(!strncmp(line,"eDM-COMC-BS6",12))
                board_type = CBS6;
            else if(!strncmp(line,"eDM-COMB-BW6",12))
                board_type = BBW6;
        }
    }
    else
    {
        snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
        EAPI_FORMATED_MES('E',
                          EApiInitLib,
                          EAPI_STATUS_UNSUPPORTED,
                          err
                          );
    }

    /* ******************** HWMON ************************** */
    hwname = NULL;
    find_hwmon();

    /* ******************** GPIO ************************** */
    gpioName = NULL;
    StatusCode = list_gpio_device();
    if((StatusCode == EAPI_STATUS_SUCCESS) &&
       (gpioName != NULL && gpioLines > 0))
    {
        StatusCode2 = gpio_dev_open(gpioName);
        if (StatusCode2 != EAPI_STATUS_SUCCESS)
            EAPI_FORMATED_MES('E',
                              EApiInitLib,
                              EAPI_STATUS_ERROR,
                              "Issue with Some GPIO pins"
                              );
    }
    else
        EAPI_FORMATED_MES('E',
                          EApiInitLib,
                          EAPI_STATUS_UNSUPPORTED,
                          "No GPIO chip found"
                          );

    return EAPI_STATUS_SUCCESS;
}


EApiStatus_t 
EApiUninitLib(){
    DebugMsg("#\n"
             "# Embedded API EApi\n"
             "# Exit \n"
             "#\n"
             );
    unsigned i =0;

    if (eepromBuffer != NULL)
        free(eepromBuffer);

    if(OutputStream!=NULL){
        fclose(OutputStream);
        OutputStream=NULL;
    }

    if(hwname != NULL)
        free(hwname);

    if(gpioName != NULL)
        free(gpioName);

    if(req != NULL)
    {
        for (i = 0; i< gpioLines; i++)
            if(req[i].fd > 0)
                close(req[i].fd);
        free(req);
    }

    if(gpiofd >= 0)
        close(gpiofd);

    return EAPI_STATUS_SUCCESS;
}


