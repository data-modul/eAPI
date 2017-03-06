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
#include <EApiDmo.h>

FILE *OutputStream=NULL;
int eeprom_bus = -1;
uint8_t *eeprom_userSpaceBuf = NULL;
int board_type;
char *hwname;
char *rtmname;
char err[256];

unsigned int gpioLines = 0;
unsigned int gpioBase = 0;
int gpioEnabled = 0 ;
int *gpiofdExpert = NULL ;
int *gpiofdValue = NULL ;
int *gpiofdDirection = NULL ;



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
        int retclose = fclose(f);
	if (retclose != 0 )
	{
		hwname = NULL;
            closedir(dir);
            snprintf(err,sizeof(err),"%s",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        find_hwmon,
                        EAPI_STATUS_UNSUPPORTED,
                        err);
	}
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

EApiStatus_t find_rtm()
{
    struct dirent *de;
    DIR *dir;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    char sysfs[NAME_MAX];
    snprintf(sysfs,sizeof(RTM_PATH),RTM_PATH);

    if(!(dir = opendir(sysfs)))
    {
        rtmname = NULL;

        snprintf(err,sizeof(err),"%s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    find_rtm,
                    EAPI_STATUS_UNSUPPORTED,
                    err);
    }
    /* go through the rtm*/
    rtmname = NULL;
    StatusCode = EAPI_STATUS_UNSUPPORTED;
    while ((de = readdir(dir)) != NULL) {

         if (strstr(de->d_name, "dmec") != NULL)
         {
             rtmname = (char*)malloc(sizeof(de->d_name)*sizeof(char));
             if (!rtmname)
             {
                 EAPI_LIB_RETURN_ERROR(
                             find_rtm,
                             EAPI_STATUS_ALLOC_ERROR,
                             "Error in Allocating Memory");
             }
             strncpy(rtmname, de->d_name,sizeof(de->d_name));
             StatusCode = EAPI_STATUS_SUCCESS;
             break;
         }
         else
             continue;
    }
    closedir(dir);
    if (StatusCode == EAPI_STATUS_UNSUPPORTED )
        EAPI_LIB_RETURN_ERROR(
                    find_rtm,
                    EAPI_STATUS_UNSUPPORTED,
                    "Info: No RTM is found.");
    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t list_gpio_device()
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    struct dirent *de;
    DIR *dir;
    char chrdev_name[NAME_MAX],s[NAME_MAX];
    FILE* fd;
    char *px;
    int retclose = 0;

    gpioLines = 0;
    gpioBase = 0;

    if(!(dir = opendir("/sys/class/gpio/")))
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
            sprintf(chrdev_name, "/sys/class/gpio/%s/label", de->d_name);
            fd = fopen(chrdev_name, "r");
            if (fd == NULL) {
                snprintf(err,sizeof(err),"Failed to open %s: %s",chrdev_name,strerror(errno));
                EAPI_FORMATED_MES('E',
                                  list_gpio_device,
                                  EAPI_STATUS_UNSUPPORTED,
                                  err
                                  );
                continue;
            }

            /* Inspect this GPIO chip */
            px = fgets(s, NAME_MAX, fd);
            if (!px) {
                snprintf(err,sizeof(err),"Failed to read CHIPINFO : %s",strerror(errno));
                EAPI_FORMATED_MES('E',
                                  list_gpio_device,
                                  EAPI_STATUS_UNSUPPORTED,
                                  err
                                  );
            }
            else
            {
                if (strstr(s, "dmec") != NULL)
                {
                    retclose = fclose(fd);
		    if (retclose != 0)
		    {
		    snprintf(err,sizeof(err),"Failed to close %s: %s",chrdev_name,strerror(errno));
                        EAPI_FORMATED_MES('E',list_gpio_device,EAPI_STATUS_UNSUPPORTED,err);
		    }

                    sprintf(chrdev_name, "/sys/class/gpio/%s/ngpio", de->d_name);
                    fd = fopen(chrdev_name,"r");
                    if(fd == NULL) {
                        snprintf(err,sizeof(err),"Failed to open %s: %s",chrdev_name,strerror(errno));
                        EAPI_FORMATED_MES('E',list_gpio_device,EAPI_STATUS_UNSUPPORTED,err);
                        break;
                    }
                    px = fgets(s, NAME_MAX, fd);
                    if (!px) {
                        snprintf(err,sizeof(err),"Failed to read CHIPINFO : %s",strerror(errno));
                        EAPI_FORMATED_MES('E',
                                          list_gpio_device,
                                          EAPI_STATUS_UNSUPPORTED,
                                          err
                                          );
                    }
                    gpioLines = atoi(s);
                    fclose(fd);

                    sprintf(chrdev_name, "/sys/class/gpio/%s/base", de->d_name);
                    fd = fopen(chrdev_name,"r");
                    if(fd == NULL) {
                        snprintf(err,sizeof(err),"Failed to open %s: %s",chrdev_name,strerror(errno));
                        EAPI_FORMATED_MES('E',list_gpio_device,EAPI_STATUS_UNSUPPORTED,err);
                        break;
                    }
                    px = fgets(s, NAME_MAX, fd);
                    if (!px) {
                        snprintf(err,sizeof(err),"Failed to read CHIPINFO : %s",strerror(errno));
                        EAPI_FORMATED_MES('E',
                                          list_gpio_device,
                                          EAPI_STATUS_UNSUPPORTED,
                                          err
                                          );
                    }
                    gpioBase = atoi(s);
                    gpioEnabled = 1;
                    retclose = fclose(fd);
		    if (retclose != 0)
		    {
		    snprintf(err,sizeof(err),"Failed to close %s: %s",chrdev_name,strerror(errno));
                        EAPI_FORMATED_MES('E',list_gpio_device,EAPI_STATUS_UNSUPPORTED,err);
		    }
                    break;
                }
            }
            retclose = fclose(fd);
	    if (retclose != 0)
	    {
		    snprintf(err,sizeof(err),"Failed to close %s: %s",chrdev_name,strerror(errno));
                        EAPI_FORMATED_MES('E',list_gpio_device,EAPI_STATUS_UNSUPPORTED,err);
	    }
        }
        else
            continue;
    }
    closedir(dir);

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t gpio_files_create(void)
{
    unsigned int i;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    //    printf("debug_1\n");
    gpiofdExpert = (int *)malloc(gpioLines*sizeof(int));
    if(gpiofdExpert == NULL)
    {
        gpioEnabled = 0;
        snprintf(err,sizeof(err),"Failed to malloc memory for gpio request handler.");
        EAPI_LIB_RETURN_ERROR(
                    gpio_files_create,
                    EAPI_STATUS_ERROR,
                    err);
    }

    gpiofdDirection = (int *)malloc(gpioLines*sizeof(int));
    if(gpiofdDirection == NULL)
    {
        gpioEnabled = 0;
        snprintf(err,sizeof(err),"Failed to malloc memory for gpio request handler.");
        EAPI_LIB_RETURN_ERROR(
                    gpio_files_create,
                    EAPI_STATUS_ERROR,
                    err);
    }

    gpiofdValue = (int *)malloc(gpioLines*sizeof(int));
    if(gpiofdValue == NULL)
    {
        gpioEnabled = 0;
        snprintf(err,sizeof(err),"Failed to malloc memory for gpio request handler.");
        EAPI_LIB_RETURN_ERROR(
                    gpio_files_create,
                    EAPI_STATUS_ERROR,
                    err);
    }

    for (i =0; i< gpioLines; i++)
    {
        gpiofdExpert[i] =  -1 ;
        gpiofdDirection[i] =  -1 ;
        gpiofdValue[i] =  -1 ;
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
    eeprom_bus = find_eeprom();
    /* ******************** USER SPACE EEPROM ************************** */
    eeprom_userSpaceBuf = eeprom_userSpace();

    /* ******************** Detect_board_type ************************** */
    strncpy(path, "/sys/class/dmi/id/board_name", sizeof("/sys/class/dmi/id/board_name"));
    board_type = UNKNOWN;
    f = fopen(path, "r");
    if (f != NULL)
    {
        char* res = fgets(line, sizeof(line), f);
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
            if (strstr(line, "COMC") != NULL)
                board_type = CBS6;
            else if (strstr(line, "COMB") != NULL)
                board_type = BBW6;
        }
	int retclose = fclose(f);
	if (retclose != 0 )
	{
		EAPI_FORMATED_MES('E',
                              EApiInitLib,
                              EAPI_STATUS_UNSUPPORTED,
                              "error in closeing file dmi/id/board_name"
                              );
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

    /* ******************** RTM ************************** */
    rtmname = NULL;
    find_rtm();

    /* ******************** GPIO ************************** */
    StatusCode = list_gpio_device();
    if((StatusCode == EAPI_STATUS_SUCCESS) &&
            (gpioLines > 0))
    {
        StatusCode2 = gpio_files_create();
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
    unsigned int i =0;
    char buf[32];
    int fd;

    if(OutputStream!=NULL){
        fclose(OutputStream);
        OutputStream=NULL;
    }

    if(hwname != NULL)
        free(hwname);

    if(rtmname != NULL)
        free(rtmname);

    if(gpiofdDirection != NULL)
    {
        for (i =0; i< gpioLines; i++)
        {
            if (gpiofdDirection[i] != -1)
                close(gpiofdDirection[i]);
        }
        free(gpiofdDirection);
    }
    if(gpiofdValue != NULL)
    {
        for (i =0; i< gpioLines; i++)
        {
            if (gpiofdValue[i] != -1)
                close(gpiofdValue[i]);
        }
        free(gpiofdValue);
    }


    if(gpiofdExpert != NULL)
    {
        for (i =0; i< gpioLines; i++)
        {
            if(gpiofdExpert[i] == 0)
            {
                if ((fd = open("/sys/class/gpio/unexport", O_WRONLY)) < 0) {
                    snprintf(err,sizeof(err),"Failed to access to pin %d",i);
                    EAPI_FORMATED_MES('E',
                                      EApiUninitLib,
                                      EAPI_STATUS_ERROR,
                                      err
                                      );
                }
                snprintf(buf, sizeof(buf), "%d", gpioBase + i);
                write(fd, buf, strlen(buf));
                close(fd);
            }
        }

        free(gpiofdExpert);
    }

    if(eeprom_userSpaceBuf)
        free(eeprom_userSpaceBuf);

    return EAPI_STATUS_SUCCESS;
}


