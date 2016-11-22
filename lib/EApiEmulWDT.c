/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulWDT.c $
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
 *I Description: Auto Created for EApiEmulWDT.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulWDT.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulWDT.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

/*
 *
 *
 *
 *  W A T C H D O G
 *
 *
 *
 */
#define WATCHDOG_ENABLED  ((unsigned)-1)
#define WATCHDOG_DISABLED ((unsigned)0)
static unsigned WatchdogState=WATCHDOG_DISABLED;
static unsigned WatchdogFound = 0;
char watchdogName[NAME_MAX];
static int wdogDescriptor = 0;

#define MIN_IN_sec(x) ((x)*60)
//#define MIN_IN_millisec(x) ((x)*60)
#define MaxDelay        MIN_IN_sec(0)
#define MaxEventTimeout MIN_IN_sec(140)
#define MaxResetTimeout MIN_IN_sec(140)

static void find_wdog_device()
{
    FILE *f;
    char fstype[NAME_MAX], sysfs[NAME_MAX], n[NAME_MAX];
    int foundsysfs = 0;
    struct dirent *de;
    DIR *dir;
    struct stat sb;
    char *linkname;
    ssize_t r, bufsiz;

    if (WatchdogFound == 1)
        return;

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

    strcat(sysfs, "/class/watchdog");
    if(!(dir = opendir(sysfs)))
        return;

    /* go through the watchdogs */
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

        if (strstr(linkname, "dmec-wdt") != NULL)
        {
            WatchdogFound = 1;
            strncpy(watchdogName, de->d_name,sizeof(de->d_name));
            free(linkname);
            break;
        }

        free(linkname);
    }
    closedir(dir);
    return;
}

EApiStatus_t 
EAPI_CALLTYPE
EApiWDogGetCapEmul(
        __OUTOPT uint32_t *pMaxDelay       ,/* Maximum Supported
                                                 * Delay in milliseconds
                                                 */
        __OUTOPT uint32_t *pMaxEventTimeout,/* Maximum Supported
                                                 * Event Timeout in
                                                 * milliseconds
                                                 * 0 == Unsupported
                                                 */
        __OUTOPT uint32_t *pMaxResetTimeout /* Maximum Supported
                                                 * Reset Timeout in
                                                 * milliseconds
                                                 */
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;

    find_wdog_device();
    if (WatchdogFound == 0) // no WDG
    {
        *pMaxDelay = 0;
        *pMaxEventTimeout = 0 ;
        *pMaxResetTimeout = 0;
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogGetCapEmul,
                    EAPI_STATUS_UNSUPPORTED,
                    "No Watchdog found");
    }

    if(pMaxDelay)
        *pMaxDelay = MaxDelay * 1000; /* in msec */
    if (pMaxEventTimeout)
        *pMaxEventTimeout =MaxEventTimeout * 1000;/* in msec */
    if(pMaxResetTimeout)
        *pMaxResetTimeout =MaxResetTimeout * 1000;/* in msec */

    EAPI_LIB_RETURN_SUCCESS(EApiWDogGetCapEmul, "");

    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}
EApiStatus_t 
EApiWDogStartEmul(
        __IN  uint32_t Delay       , /* Delay in milliseconds */
        __IN  uint32_t EventTimeout, /* Event Timeout in
                                          * milliseconds
                                          */
        __IN  uint32_t ResetTimeout  /* Reset Timeout in
                                          * milliseconds
                                          */
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char devname[100];
    int ret;

    find_wdog_device();
    if (WatchdogFound == 0) // no WDG
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogStartEmul,
                    EAPI_STATUS_UNSUPPORTED,
                    "No Watchdog found");

    EAPI_LIB_RETURN_ERROR_IF(
                EApiWDogStartEmul,
                (WatchdogState==WATCHDOG_ENABLED),
                EAPI_STATUS_RUNNING,
                "Watchdog already runing, need to stop before starting"
                );

    /* change input msec time to second*/
    uint32_t SavedDelaySec = Delay / 1000;
    uint32_t SavedEventTimeoutSec = EventTimeout / 1000;
    uint32_t SavedResetTimeoutSec = ResetTimeout / 1000;

    EAPI_LIB_ASSERT_PARAMATER_CHECK(
                EApiWDogStartEmul,
                (SavedDelaySec>MaxDelay),
                "(Delay>pMaxDelay)"
                );
    EAPI_LIB_ASSERT_PARAMATER_CHECK(
                EApiWDogStartEmul,
                (SavedEventTimeoutSec>MaxEventTimeout),
                "(EventTimeout>pMaxEventTimeout)"
                );
    EAPI_LIB_ASSERT_PARAMATER_CHECK(
                EApiWDogStartEmul,
                (SavedResetTimeoutSec>MaxResetTimeout),
                "(ResetTimeout>pMaxResetTimeout)"
                );

    /* open watchdog device */
    snprintf(devname,sizeof(devname),"/dev/%s",watchdogName);
    devname[sizeof(devname) - 1] = '\0';

    wdogDescriptor = open(devname,O_WRONLY);
    if(wdogDescriptor < 0)
    {
        snprintf(err,sizeof(err),"Unsupported watchdog device %s: %s\n ",devname,strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogStartEmul,
                    EAPI_STATUS_ERROR,
                    err);
    }
    WatchdogState=WATCHDOG_ENABLED;

    /* set delay*/
    EAPI_FORMATED_MES('L',
                      EApiWDogStartEmul,
                      EAPI_STATUS_UNSUPPORTED,
                      "Delay time is not supported by the driver."
                      );

    /* set EventTimeOut */
    uint32_t EventTimeoutSec = SavedEventTimeoutSec;
    ret = ioctl(wdogDescriptor, WDIOC_SETPRETIMEOUT, &EventTimeoutSec);
    if(ret)
    {
        EApiWDogStopEmul();
        snprintf(err,sizeof(err),"Setting EventTimeout failed.\n ");
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogStartEmul,
                    EAPI_STATUS_ERROR,
                    err);
    }
    /* read back EventTimeout*/
    ret = ioctl(wdogDescriptor, WDIOC_GETPRETIMEOUT, &EventTimeoutSec);
    if(ret)
    {
        snprintf(err,sizeof(err),"Getting EventTimeout failed.\n ");
        EAPI_FORMATED_MES('E',
                          EApiWDogStartEmul,
                          EAPI_STATUS_ERROR,
                          err
                          );
    }
    else if(EventTimeoutSec != SavedEventTimeoutSec)
    {
        snprintf(err,sizeof(err),"Requested EventTimeoutSec cannot be set. Set to %d sec.",EventTimeoutSec);
        EAPI_FORMATED_MES('E',
                          EApiWDogStartEmul,
                          EAPI_STATUS_UNSUPPORTED,
                          err
                          );
    }

    /* set ResetTimeOut */
    uint32_t ResetTimeoutSec = SavedResetTimeoutSec;
    ret = ioctl(wdogDescriptor, WDIOC_SETTIMEOUT, &ResetTimeoutSec);
    if(ret)
    {
        EApiWDogStopEmul();
        snprintf(err,sizeof(err),"Setting ResetTimeout failed.\n ");
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogStartEmul,
                    EAPI_STATUS_ERROR,
                    err);
    }
    /* read back ResetTimeout*/
    ret = ioctl(wdogDescriptor, WDIOC_GETTIMEOUT, &ResetTimeoutSec);
    if(ret)
    {
        snprintf(err,sizeof(err),"Getting ResetTimeoutSec failed.\n ");
        EAPI_FORMATED_MES('E',
                          EApiWDogStartEmul,
                          EAPI_STATUS_ERROR,
                          err
                          );
    }
    else if(ResetTimeoutSec != SavedResetTimeoutSec)
    {
        snprintf(err,sizeof(err),"Requested ResetTimeoutSec cannot be set. Set to %d sec.",ResetTimeoutSec);
        EAPI_FORMATED_MES('E',
                          EApiWDogStartEmul,
                          EAPI_STATUS_UNSUPPORTED,
                          err
                          );
    }

    EAPI_LIB_RETURN_SUCCESS(EApiWDogStartEmul, "");

    EAPI_LIB_ASSERT_EXIT
    return StatusCode;
}

EApiStatus_t 
EApiWDogTriggerEmul(void)
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    int dummy,ret;

    if (WatchdogFound == 0) // no WDG
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogTriggerEmul,
                    EAPI_STATUS_UNSUPPORTED,
                    "No Watchdog found");
    }

    EAPI_LIB_RETURN_ERROR_IF(
                EApiWDogTriggerEmul,
                (WatchdogState==WATCHDOG_DISABLED),
                EAPI_STATUS_ERROR,
                "Watchdog not started, therefore not possible to trigger"
                );

    ret = ioctl(wdogDescriptor, WDIOC_KEEPALIVE, &dummy);
    if (ret)
    {
        snprintf(err,sizeof(err),"Trigger Watchdog failed.\n ");
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogTriggerEmul,
                    EAPI_STATUS_ERROR,
                    err);
    }

    EAPI_LIB_RETURN_SUCCESS(
                EApiWDogTriggerEmul,
                ""
                );
    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}
EApiStatus_t 
EApiWDogStopEmul(void)
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    int ret;

    if (WatchdogFound == 0) // no WDG
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogStopEmul,
                    EAPI_STATUS_UNSUPPORTED,
                    "No Watchdog found");
    }

    EAPI_LIB_RETURN_ERROR_IF(
                EApiWDogStopEmul,
                (WatchdogState==WATCHDOG_DISABLED),
                EAPI_STATUS_ERROR,
                "Watchdog not started, therefore not possible to stop"
                );

    ret = write(wdogDescriptor,"V", sizeof("V"));
    if (ret <= 0)
    {
        snprintf(err,sizeof(err),"Stop Watchdog failed.\n ");
        EAPI_LIB_RETURN_ERROR(
                    EApiWDogStopEmul,
                    EAPI_STATUS_ERROR,
                    err);
    }

    WatchdogState=WATCHDOG_DISABLED;
    close(wdogDescriptor);
    EAPI_LIB_RETURN_SUCCESS(
                EApiWDogStopEmul,
                ""
                );
    EAPI_LIB_ASSERT_EXIT

            return StatusCode;
}



