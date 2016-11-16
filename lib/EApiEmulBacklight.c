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
 * EAPI_ID_BACKLIGHT_1 corresponds to DDI1 I2C Device eDP => card0-eDP-1
 * EAPI_ID_BACKLIGHT_2
 * EAPI_ID_BACKLIGHT_3
 */

#define BACKLIGHT_PATH "/sys/class/backlight/"
static unsigned BacklightFound = 0;
char *backlightName= NULL;

static void find_backlight_device()
{
    FILE *f;
    char fstype[NAME_MAX], sysfs[NAME_MAX], n[NAME_MAX];
    int foundsysfs = 0;
    struct dirent *de;
    DIR *dir;
    struct stat sb;
    char *linkname;
    ssize_t r, bufsiz;

    if (BacklightFound == 1)
        return;

    backlightName = NULL;
    /* look in sysfs */
    /* First figure out where sysfs was mounted */
    if ((f = fopen("/proc/mounts", "r")) == NULL)
        return;

    while (fgets(n, NAME_MAX, f)) {
        sscanf(n, "%*[^ ] %[^ ] %[^ ] %*s\n", sysfs, fstype);
        if (strcasecmp(fstype, "sysfs") == 0) {
            foundsysfs++;
            break;
        }
    }
    fclose(f);
    if (! foundsysfs)
        return;

    strcat(sysfs, "/class/backlight");
    if(!(dir = opendir(sysfs)))
        return;

    /* go through the backlights */
    while ((de = readdir(dir)) != NULL) {
        if (!strcmp(de->d_name, "."))
            continue;
        if (!strcmp(de->d_name, ".."))
            continue;

        /* this should work for kernels 2.6.5 or higher and */
        /* is preferred because is unambiguous */
        sprintf(n, "%s/%s", sysfs, de->d_name);

        if (lstat(n, &sb) == -1)
        {
            perror("lstat");
            return;
        }

        if(sb.st_size == 0)
            bufsiz = PATH_MAX;
        else
            bufsiz = sb.st_size + 1;

        linkname = malloc(bufsiz);
        if(linkname == NULL)
        {
            perror("malloc");
            return;
        }
        r = readlink(n, linkname, bufsiz);
        if (r == -1)
        {
            perror("readlink");
            return;
        }
        linkname[r]= '\0';

        if (strstr(linkname, "card0-eDP-1") != NULL)
        {
            BacklightFound = 1;
            backlightName = malloc(sizeof(de->d_name)+1);
            strcpy(backlightName, de->d_name);
            free(linkname);
            break;
        }

        free(linkname);
    }
    closedir(dir);
    return;
}


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

    if ((Id == EAPI_ID_BACKLIGHT_1) || /* card0-eDP-1 */
            (Id == EAPI_DMO_ID_BACKLIGHT_1))
    {
        if (BacklightFound == 0)
            find_backlight_device();

        snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof(backlightName)+sizeof("/device/enabled"),BACKLIGHT_PATH"%s/device/enabled",backlightName);
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
         if (!strcmp(value, "enabled"))
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
    __OUT uint32_t Enable
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
//  unsigned i;

//  for(i=0;i<ARRAY_SIZE(BacklightLookup);i++)
//  {
//      if(BacklightLookup[i].Id==Id)
//      {
//        BacklightLookup[i].Enabled=Enable;
//        EAPI_LIB_RETURN_SUCCESS(
//          EApiVgaSetBacklightEnableEmul,
//          ""
//          );
//      }
//  }
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

  if ((Id == EAPI_ID_BACKLIGHT_1) || /* card0-eDP-1 */
          (Id == EAPI_DMO_ID_BACKLIGHT_1))
  {
      if (BacklightFound == 0)
          find_backlight_device();

      snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof(backlightName)+sizeof("/brightness"),BACKLIGHT_PATH"%s/brightness",backlightName);
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

  if ((Id == EAPI_ID_BACKLIGHT_1) || /* card0-eDP-1 */
          (Id == EAPI_DMO_ID_BACKLIGHT_1))
  {
      if (BacklightFound == 0)
          find_backlight_device();


      snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof(backlightName)+sizeof("/max_brightness"),BACKLIGHT_PATH"%s/max_brightness",backlightName);
      f = fopen(path, "r");
      if (f != NULL)
      {
          int res = fscanf(f, "%" PRIu32, &maxBrightness);
          if (res < 0)
              maxBrightness = 0;
          fclose(f);
      }

      if (Brightness > maxBrightness)
          Brightness = maxBrightness;

      snprintf(path,sizeof(BACKLIGHT_PATH)+sizeof(backlightName)+sizeof("/brightness"),BACKLIGHT_PATH"%s/brightness",backlightName);
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


