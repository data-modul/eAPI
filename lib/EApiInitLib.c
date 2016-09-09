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

FILE *OutputStream=NULL;
uint8_t *eepromBuffer;
int borad_type;
char *hwname;
char err[256];

void __cdecl DebugMsg(__IN const char *const fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  if(OutputStream!=NULL)
    vfprintf(OutputStream, fmt, arg);
  else
    vfprintf(stdout, fmt, arg);

  fflush(OutputStream);
  va_end(arg);
}


void find_hwmon(char **result)
{
    char s[NAME_MAX];
    struct dirent *de;
    DIR *dir;
    FILE *f;
    char sysfs[NAME_MAX], n[NAME_MAX];
    snprintf(sysfs,sizeof(HWMON_PATH),HWMON_PATH);

    if(!(dir = opendir(sysfs)))
    {
        printf("Error: %s =>",strerror(errno));
        return NULL;
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
            printf("Error: %s => ",strerror(errno));
            return NULL;
        }
            char *px;

            px = fgets(s, NAME_MAX, f);
            fclose(f);
            if (!px) {
                fprintf(stderr, "%s: read error\n", n);
                continue;
            }
            if ((px = strchr(s, '\n')) != NULL)
                *px = 0;

            if(!strncmp(s, HWMON_NAME,sizeof(HWMON_NAME)))
            {
                *result = HWMON_NAME;
                break;
            }
            else
                *result = NULL;

    }
    closedir(dir);
    return;
}

EApiStatus_t 
EApiInitLib(){
  if(OutputStream==NULL){
#if EAPI_DBG_USE_OUTPUT_FILE
    OutputStream=fopen(TEXT("EApi.log"), TEXT("w"));
#else
    OutputStream=stdout;
#endif
  }

  int eeprom_bus;
  uint32_t  Cmd ;
  char devname[20];
  int i2cDescriptor = 0;
  int res = 0;
  int flag = 0;
  int iRead = 0;
  char * pBoradType;
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

  /* ******************** EEPROM ************************** */
  /* Find Eeprom i2c-bus and read & fill the buffer */
  eeprom_bus = find_eeprom();
  if (eeprom_bus >= 0)
  {
      // read eeprom
      eepromBuffer = (uint8_t*)calloc(EEPROM_SIZE, sizeof(uint8_t));
      if (eepromBuffer == NULL)
      {
          snprintf(err,sizeof(err),"Error in Eeprom Allocating Memory\n");
          printf("%s",err);
          EAPI_LIB_RETURN_ERROR(
                      EApiInitLib,
                      EAPI_STATUS_ALLOC_ERROR,
                      err);
      }

      Cmd = EAPI_I2C_ENC_EXT_CMD(0x00);

      // open device
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
                      EApiInitLib,
                      EAPI_STATUS_NOT_FOUND,
                      err);
      }
      // set slave address : set working device
      if(ioctl(i2cDescriptor, I2C_SLAVE, EEPROM_DEVICE) < 0)
      {
          snprintf(err,sizeof(err),"Cannot set Eeprom slave ddress: %s\n",strerror(errno));
          printf("%s",err);
          EAPI_LIB_RETURN_ERROR(
                      EApiInitLib,
                      EAPI_STATUS_NOT_FOUND,
                      err);
      }
      if(EEPROM_SIZE)
      {
          while (iRead < EEPROM_SIZE)
          {
              if (flag == 0)
              {
                  res = i2c_smbus_write_byte_data(i2cDescriptor,(Cmd >> 8) & 0x0ff, Cmd & 0x0ff); //write 16bits add
                  if (res < 0)
                  {
                      snprintf(err,sizeof(err),"Cannot write into Eeprom: %s\n",strerror(errno));
                      printf("%s",err);
                      EAPI_LIB_RETURN_ERROR(
                                  EApiInitLib,
                                  EAPI_STATUS_WRITE_ERROR,
                                  err);
                  }
                  flag = 1; // cmd write is done, no needed more
              }
              res = i2c_smbus_read_byte(i2cDescriptor);
              if (res < 0)
              {
                  snprintf(err,sizeof(err),"Cannot read from Eeprom: %s\n",strerror(errno));
                  printf("%s",err);
                  EAPI_LIB_RETURN_ERROR(
                              EApiInitLib,
                              EAPI_STATUS_READ_ERROR,
                              err);
              }
              else
                  eepromBuffer[iRead] = (uint8_t)res;
              iRead++;
          }
          close(i2cDescriptor);
      }
      /* ********************************************** */
      /* detect_board_type */
      pBoradType=(char *)malloc((NAME_MAX) * sizeof(char));
      pBoradType = eeprom_analyze(eepromBuffer,BOARD_ID_TYPE,BOARD_ID_ASCII_IND);
      if(!strncmp(pBoradType,"BBW6",4))
          borad_type = BBW6;
      else if(!strncmp(pBoradType,"CBS6",4))
          borad_type = CBS6;
      else
      {
          printf("Board type is unknown\n");
          borad_type = UNKNOWN;
      }
      free(pBoradType);
  }
  else
      printf("Error: No Eeprom Bus is found.\n");


  /* ********************************************** */
/* find hwmon */
    hwname = NULL;
    find_hwmon(&hwname);
    if (!hwname)
        printf("Info: No HWMON is found.\n");


    DebugMsg("#\n"
              "# Embedded API EApi\n"
              "# Version %u.%u\n"
              "# Lib Version %u.%u.%u\n"
              "#\n",
              EAPI_VER_GET_VER(EAPI_VERSION), EAPI_VER_GET_REV(EAPI_VERSION), 
              LIB_VERSION, LIB_REVISION, LIB_BUILD
            );

    EAPI_LIB_ASSERT_EXIT
  return StatusCode;
}


EApiStatus_t 
EApiUninitLib(){
    DebugMsg("#\n"
              "# Embedded API EApi\n"
              "# Exit \n"
              "#\n"
            );

    if (eepromBuffer != NULL)
        free(eepromBuffer);
  if(OutputStream!=NULL&&OutputStream!=stdout&&OutputStream!=stderr){
      fclose(OutputStream);
  }
 // OutputStream=stdout;
  return EAPI_STATUS_SUCCESS; 
}


