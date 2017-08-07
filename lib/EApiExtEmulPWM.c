/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApi-Extension Application Development Kit
 *+=========================================================================
 *I   Copyright: Copyright (c) 2017, DATA MODUL AG
 *I      Author: Reyhaneh Yazdani,             ryazdani@data-modul.com
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
 *I  File Name            : EApiEmulPWM.c
 *I  File Location        : lib
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
#include <stdbool.h>

/*
 *
 *
 *
 *  P U L S E   W I T H   M O D U L A T I O N
 *
 *
 *
 */

#define PWM_PATH "/sys/bus/platform/drivers/dmec-pwm/"

static unsigned int npwm = 0;

enum pwmlabel {ALIGNMENT, MODE, MINSTEPS, MAXSTEPS, DUTY, ENABLE, PERIOD, POLARITY, GRANULARITY};

static EApiStatus_t pin_export (int mypin) {
    char buf[32];
    int fd;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    char *pwmexport;
    int ret = 0;

    pwmexport = (char*)malloc((strlen(pwmchip)+ strlen("/export"))*sizeof(char));
    snprintf(pwmexport,strlen(pwmchip)+strlen("/export")+1,"%s/export",pwmchip);

    if ((fd = open(pwmexport, O_WRONLY)) < 0) {
        snprintf(err,sizeof(err),"Failed to access to pin %d: %s",mypin,strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    pin_export,
                    EAPI_STATUS_ERROR,
                    err);
    }

    if (npwm == 1)
    {
        snprintf(buf, sizeof(buf), "%d", 0);
        ret = write(fd, buf, strlen(buf));
        if (ret < 0)
        {
            snprintf(err,sizeof(err),"pwm channel%d is already exported: %s",mypin, strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        pin_export,
                        EAPI_STATUS_ERROR,
                        err);
        }
        if (mypin == EAPIEX_ID_PWM_CHANNEL_0)
        {
            pathPwmChannel0 = (char*)malloc((strlen(pwmchip)+ strlen("/pwm0/"))*sizeof(char));
            snprintf(pathPwmChannel0,strlen(pwmchip)+strlen("/pwm0/")+1,"%s/pwm0/",pwmchip);
            pwmChannel[0].exported = 1;
        }
        else if (mypin == EAPIEX_ID_PWM_CHANNEL_1)
        {
            pathPwmChannel1 = (char*)malloc((strlen(pwmchip)+ strlen("/pwm0/"))*sizeof(char));
            snprintf(pathPwmChannel1,strlen(pwmchip)+strlen("/pwm0/")+1,"%s/pwm0/",pwmchip);
            pwmChannel[1].exported = 1;
        }
    }
    else if (npwm == 2)
    {
        if (mypin == EAPIEX_ID_PWM_CHANNEL_0)
        {
            snprintf(buf, sizeof(buf), "%d", 0);
            ret = write(fd, buf, strlen(buf));
            if (ret < 0)
            {
                snprintf(err,sizeof(err),"pwm channel%d is already exported: %s",mypin, strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            pin_export,
                            EAPI_STATUS_ERROR,
                            err);
            }
            pathPwmChannel0 = (char*)malloc((strlen(pwmchip)+ strlen("/pwm0/"))*sizeof(char));
            snprintf(pathPwmChannel0,strlen(pwmchip)+strlen("/pwm0/")+1,"%s/pwm0/",pwmchip);
            pwmChannel[0].exported = 1;
        }
        else if (mypin == EAPIEX_ID_PWM_CHANNEL_1)
        {
            snprintf(buf, sizeof(buf), "%d", 1);
            ret = write(fd, buf, strlen(buf));
            if (ret < 0)
            {
                snprintf(err,sizeof(err),"pwm channel%d is already exported: %s",mypin,strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            pin_export,
                            EAPI_STATUS_ERROR,
                            err);
            }
            pathPwmChannel1 = (char*)malloc((strlen(pwmchip)+ strlen("/pwm1/"))*sizeof(char));
            snprintf(pathPwmChannel1,strlen(pwmchip)+strlen("/pwm1/")+1,"%s/pwm1/",pwmchip);
            pwmChannel[1].exported = 1;
        }
    }

    close(fd);
    free(pwmexport);

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

static EApiStatus_t pwmCheck(EApiId_t Id)
{
    struct dirent *de;
    DIR *dir;
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    FILE *f = NULL;
    int ret = 0;
    char *pwmname;
    char sysfs[NAME_MAX];
    char npwmPath[NAME_MAX];
    char pin[NAME_MAX];
    char *p;
    char line[NAME_MAX];
    uint32_t value=0;

    if (Id != EAPIEX_ID_PWM_CHANNEL_0 && Id != EAPIEX_ID_PWM_CHANNEL_1)
        return EAPI_STATUS_UNSUPPORTED;

    if (Id == EAPIEX_ID_PWM_CHANNEL_0 && pwmChannel[0].exported)
        return EAPI_STATUS_SUCCESS;
    else if (Id == EAPIEX_ID_PWM_CHANNEL_1 && pwmChannel[1].exported)
        return EAPI_STATUS_SUCCESS;

    snprintf(sysfs,sizeof(PWM_PATH),PWM_PATH);
    if(!(dir = opendir(sysfs)))
    {
        pwmname = NULL;
        snprintf(err,sizeof(err),"%s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    pwmCheck,
                    EAPI_STATUS_UNSUPPORTED,
                    err);
    }

    /* go through the pwm*/
    pwmname = NULL;
    StatusCode = EAPI_STATUS_UNSUPPORTED;
    while ((de = readdir(dir)) != NULL) {
        if (strstr(de->d_name, "dmec") != NULL)
        {
            pwmname = (char*)malloc(sizeof(de->d_name)*sizeof(char));
            strncpy(pwmname, de->d_name,sizeof(de->d_name));

            pathPwmConfigure = (char*)malloc((strlen(PWM_PATH)+ strlen(pwmname))*sizeof(char));
            snprintf(pathPwmConfigure,strlen(PWM_PATH)+strlen(pwmname)+1,PWM_PATH"%s",pwmname);
            StatusCode = EAPI_STATUS_SUCCESS;
            break;
        }
        else
            continue;
    }
    closedir(dir);
    if (StatusCode == EAPI_STATUS_UNSUPPORTED )
        EAPI_LIB_RETURN_ERROR(
                    pwmCheck,
                    EAPI_STATUS_UNSUPPORTED,
                    "Info: No PWM is found.");

    /* find pwmchip path  */
    pwmchip = (char*)malloc((strlen(PWM_PATH)+ strlen(pwmname)+ strlen("/pwm/pwmchip0"))*sizeof(char));
    snprintf(pwmchip,strlen(PWM_PATH)+strlen(pwmname)+strlen("/pwm/pwmchip0")+1,PWM_PATH"%s/pwm/pwmchip0",pwmname);

    snprintf(npwmPath,strlen(pwmchip)+strlen("/npwm")+1,"%s/npwm",pwmchip);
    npwm = 0;
    f = fopen(npwmPath, "r");
    if (f != NULL)
    {
        ret = fscanf(f, "%" PRIu32, &value);
        fclose(f);
        if (ret < 0)
        {
            snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        pwmCheck,
                        EAPI_STATUS_ERROR,
                        err
                        );
        }
    }
    else
    {
        snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    pwmCheck,
                    EAPI_STATUS_ERROR,
                    err);
    }
    npwm = value;
    if(npwm == 0)
        EAPI_LIB_RETURN_ERROR(
                    pwmCheck,
                    EAPI_STATUS_UNSUPPORTED,
                    "No PWM pin is found");
    else if (npwm == 1)
    {
        snprintf(pin,strlen(pathPwmConfigure)+strlen("/pin")+1,"%s/pin",pathPwmConfigure);
        f = fopen(pin, "r");
        if (f != NULL)
        {
            char* res = fgets(line, sizeof(line), f);
            fclose(f);
            if (res == NULL)
            {
                snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            pwmCheck,
                            EAPI_STATUS_ERROR,
                            err
                            );
            }
            p=strrchr(line, '\n');/* search last space */
            *p = '\0';/* split at last space */
            if((!strcmp(line, "pin0")) && (Id == EAPIEX_ID_PWM_CHANNEL_1))
                EAPI_LIB_RETURN_ERROR(
                            pwmCheck,
                            EAPI_STATUS_UNSUPPORTED,
                            "No PWM pin is found");
            if((!strcmp(line, "pin1")) && (Id == EAPIEX_ID_PWM_CHANNEL_0))
                EAPI_LIB_RETURN_ERROR(
                            pwmCheck,
                            EAPI_STATUS_UNSUPPORTED,
                            "No PWM pin is found");
            if((!strcmp(line, "nopin")))
                EAPI_LIB_RETURN_ERROR(
                            pwmCheck,
                            EAPI_STATUS_UNSUPPORTED,
                            "No PWM pin is found");
        }
        else
        {
            snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        pwmCheck,
                        EAPI_STATUS_ERROR,
                        err);
        }
    }

    StatusCode = pin_export(Id);
    free(pwmname);

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

static EApiStatus_t channelRead(EApiId_t Id, enum pwmlabel label)
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    char  *nr = "0";
    char path[NAME_MAX];
    char line[NAME_MAX];
    char pathValues[NAME_MAX];
    FILE *f = NULL;
    int index = 0;
    int ret = 0;
    uint32_t value=0;
    uint32_t flag = 0;

    if (Id == EAPIEX_ID_PWM_CHANNEL_0)
    {
        index = 0;
        snprintf(pathValues,strlen(pathPwmChannel0),"%s",pathPwmChannel0);
        nr = "0";
    }
    else if (Id == EAPIEX_ID_PWM_CHANNEL_1)
    {
        index = 1;
        snprintf(pathValues,strlen(pathPwmChannel1),"%s",pathPwmChannel1);
        nr = "1";
    }
    else
        return EAPI_STATUS_UNSUPPORTED;

    if (label == ALIGNMENT) /* alignment */
        snprintf(path,strlen(pathPwmConfigure)+strlen("/alignment")+strlen(nr)+1,"%s/alignment%s",pathPwmConfigure,nr);
    else if (label == MODE) /* mode */
        snprintf(path,strlen(pathPwmConfigure)+strlen("/mode")+1,"%s/mode",pathPwmConfigure);
    else if (label == GRANULARITY)
        snprintf(path,strlen(pathPwmConfigure)+strlen("/granularity")+strlen(nr)+1,"%s/granularity%s",pathPwmConfigure,nr);
    else if (label == DUTY) /* duty */
        snprintf(path,strlen(pathValues)+strlen("/duty_cycle")+1,"%s/duty_cycle",pathValues);
    else if (label == ENABLE) /* enable */
        snprintf(path,strlen(pathValues)+strlen("/enable")+1,"%s/enable",pathValues);
    else if (label == PERIOD) /* period */
        snprintf(path,strlen(pathValues)+strlen("/period")+1,"%s/period",pathValues);
    else if (label == POLARITY) /* polarity */
        snprintf(path,strlen(pathValues)+strlen("/polarity")+1,"%s/polarity",pathValues);


    /* read configuration*/
    f = fopen(path, "r");
    if (f != NULL)
    {
        if (label == POLARITY)
        {
            char* res = fgets(line, sizeof(line), f);
            fclose(f);
            if (res == NULL)
            {
                snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            channelRead,
                            EAPI_STATUS_ERROR,
                            err
                            );
            }
            char *p;
            p=strrchr(line, '\n');/* search last space */
            *p = '\0';/* split at last space */
            if(!strcmp(line, "inversed"))
                value = 1;
            else if(!strcmp(line, "normal"))
                value = 0;
            else
                EAPI_LIB_RETURN_ERROR(
                            channelRead,
                            EAPI_STATUS_UNSUPPORTED,
                            "No PWM polarity found");
        }
        else
        {
            ret = fscanf(f, "%" PRIu32, &value);
            fclose(f);
            if (ret < 0)
            {
                snprintf(err,sizeof(err),"Error during read operation: %s\n ",strerror(errno));
                EAPI_LIB_RETURN_ERROR(
                            channelRead,
                            EAPI_STATUS_ERROR,
                            err
                            );
            }
        }
    }
    else
    {
        if (label == MODE) /* mode */
            flag = 1;
        else
        {
            snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
            EAPI_LIB_RETURN_ERROR(
                        channelRead,
                        EAPI_STATUS_ERROR,
                        err);
        }
    }

    if (label == ALIGNMENT) /* alignment */
        pwmChannel[index].alignment = value;
    else if (label == MODE) /* mode */
        if (flag == 1)
            pwmChannel[index].mode = 0;
        else
            pwmChannel[index].mode = value;
    else if (label == GRANULARITY)
        pwmChannel[index].granularity = value;
    else if (label == DUTY) /* duty */
        pwmChannel[index].duty = value;
    else if (label == ENABLE) /* enable */
        pwmChannel[index].enabled = value;
    else if (label == PERIOD) /* period */
        pwmChannel[index].period = value;
    else if (label == POLARITY) /* polarity */
        pwmChannel[index].polarity = value;

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

static EApiStatus_t channelReadAll(EApiId_t Id)
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (Id == EAPIEX_ID_PWM_CHANNEL_0)
        index = 0;
    else if (Id == EAPIEX_ID_PWM_CHANNEL_1)
        index = 1;
    else
        return EAPI_STATUS_UNSUPPORTED;

    StatusCode = channelRead(Id, ALIGNMENT);
    StatusCode |= channelRead(Id, MODE);
    StatusCode |= channelRead(Id, GRANULARITY);
    StatusCode |= channelRead(Id, DUTY);
    StatusCode |= channelRead(Id, ENABLE);
    StatusCode |= channelRead(Id, PERIOD);
    StatusCode |= channelRead(Id, POLARITY);

    if (StatusCode != EAPI_STATUS_SUCCESS)
        pwmChannel[index].initialized = 0;
    else
        pwmChannel[index].initialized = 1;

    return StatusCode;
}

static EApiStatus_t channelBasicWrite(EApiId_t Id, enum pwmlabel label, uint32_t value);
static EApiStatus_t channelConfigureWrite(EApiId_t Id, enum pwmlabel label, uint32_t value)
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    char  *nr = "0";
    char path[NAME_MAX];
    int ret = 0;
    int fd;
    char buf[32];

    if (Id == EAPIEX_ID_PWM_CHANNEL_0)
        nr = "0";
    else if (Id == EAPIEX_ID_PWM_CHANNEL_1)
        nr = "1";
    else
        return EAPI_STATUS_UNSUPPORTED;

    /* write configuration*/

    if (label == ALIGNMENT) /* alignment */
        snprintf(path,strlen(pathPwmConfigure)+strlen("/alignment")+strlen(nr)+1,"%s/alignment%s",pathPwmConfigure,nr);
    else if (label == MODE) /* mode */
        snprintf(path,strlen(pathPwmConfigure)+strlen("/mode")+1,"%s/mode",pathPwmConfigure);
    else if (label == MINSTEPS) /* minsteps */
        snprintf(path,strlen(pathPwmConfigure)+strlen("/minSteps")+1,"%s/minSteps",pathPwmConfigure);
    else if (label == MAXSTEPS) /* maxsteps */
        snprintf(path,strlen(pathPwmConfigure)+strlen("/maxSteps")+1,"%s/maxSteps",pathPwmConfigure);

    if ((fd = open(path, O_WRONLY)) < 0) {
        snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    channelConfigureWrite,
                    EAPI_STATUS_ERROR,
                    err);
    }
    snprintf(buf, sizeof(buf), "%d", value);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret < 0)
    {
        snprintf(err,sizeof(err),"write into pwm channel failed: %s",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    channelConfigureWrite,
                    EAPI_STATUS_ERROR,
                    err);
    }
    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

static EApiStatus_t channelBasicWrite(EApiId_t Id, enum pwmlabel label, uint32_t value)
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    char path[NAME_MAX];
    char pathValues[NAME_MAX];
    int ret = 0;
    int fd;
    char buf[32];

    if (Id == EAPIEX_ID_PWM_CHANNEL_0)
        snprintf(pathValues,strlen(pathPwmChannel0),"%s",pathPwmChannel0);
    else if (Id == EAPIEX_ID_PWM_CHANNEL_1)
        snprintf(pathValues,strlen(pathPwmChannel1),"%s",pathPwmChannel1);
    else
        return EAPI_STATUS_UNSUPPORTED;

    /* basic values */

    if (label == DUTY) /* duty */
        snprintf(path,strlen(pathValues)+strlen("/duty_cycle")+1,"%s/duty_cycle",pathValues);
    else if (label == ENABLE) /* enable */
        snprintf(path,strlen(pathValues)+strlen("/enable")+1,"%s/enable",pathValues);
    else if (label == PERIOD) /* period */
        snprintf(path,strlen(pathValues)+strlen("/period")+1,"%s/period",pathValues);
    else if (label == POLARITY) /* polarity */
        snprintf(path,strlen(pathValues)+strlen("/polarity")+1,"%s/polarity",pathValues);

    if ((fd = open(path, O_WRONLY)) < 0) {
        snprintf(err,sizeof(err),"Error in open file operation: %s\n ",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    channelBasicWrite,
                    EAPI_STATUS_ERROR,
                    err);
    }

    if (label == POLARITY && value == 0)
        snprintf(buf, sizeof(buf), "normal");
    else if (label == POLARITY && value == 1)
        snprintf(buf, sizeof(buf), "inversed");
    else
        snprintf(buf, sizeof(buf), "%d", value);

    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret < 0)
    {
        snprintf(err,sizeof(err),"write into pwm channel failed: %s\n",strerror(errno));
        EAPI_LIB_RETURN_ERROR(
                    channelBasicWrite,
                    EAPI_STATUS_ERROR,
                    err);
    }

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t 
EAPI_CALLTYPE
EApiExPwmGetCapsEmul(
        __IN      EApiId_t Id                  ,
        __OUT     uint32_t *pPwmMinPeriod      ,
        __OUT     uint32_t *pPwmMaxPeriod      ,
        __OUT     uint32_t *pPwmMinGranularity ,
        __OUT     uint32_t *pPwmMaxGranularity ,
        __OUT     EApiId_t *pPwmCompanionChannelId,
        __INOUT   uint32_t *pPwmFeatureFlags
        )
{
    EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
    uint32_t FeatureFlagsIn = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    FeatureFlagsIn = *pPwmFeatureFlags;

    switch (Id) {
    case EAPIEX_ID_PWM_CHANNEL_0:
        *pPwmFeatureFlags = DMEC_PWM_CHANNEL0_FEATURES;
        *pPwmCompanionChannelId = DMEC_PWM_CHANNEL0_COMPANION;
        break;
    case EAPIEX_ID_PWM_CHANNEL_1:
        *pPwmFeatureFlags = DMEC_PWM_CHANNEL1_FEATURES;
        *pPwmCompanionChannelId = DMEC_PWM_CHANNEL1_COMPANION;
        break;
    default:
        return (EAPI_STATUS_UNSUPPORTED);
        break;
    }

    if (((FeatureFlagsIn & ~EAPIEX_PWM_QUERY_FLAG) & *pPwmFeatureFlags) != (FeatureFlagsIn & ~EAPIEX_PWM_QUERY_FLAG)) {
        return (EAPI_STATUS_UNSUPPORTED);
    }

    if(FeatureFlagsIn & EAPIEX_PWM_QUERY_FLAG) {
        *pPwmMinPeriod = DMEC_MIN_PWM_PERIOD(FeatureFlagsIn);
        *pPwmMaxPeriod = DMEC_MAX_PWM_PERIOD(FeatureFlagsIn);
        *pPwmMinGranularity  = DMEC_MIN_PWM_GRANULARITY(FeatureFlagsIn);
        *pPwmMaxGranularity  = DMEC_MAX_PWM_GRANULARITY(FeatureFlagsIn);
    } else {
        *pPwmMinPeriod = DMEC_MIN_PWM_PERIOD(0);
        *pPwmMaxPeriod = DMEC_MAX_PWM_PERIOD(EAPIEX_PWM_FEATURE_CENTER_MODE | EAPIEX_PWM_FEATURE_16BIT);
        *pPwmMinGranularity  = DMEC_MIN_PWM_GRANULARITY(0);
        *pPwmMaxGranularity  = DMEC_MAX_PWM_GRANULARITY(EAPIEX_PWM_FEATURE_CENTER_MODE);
    }

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmGetCapsEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetConfigEmul(
        __IN      EApiId_t Id               ,
        __OUTOPT  uint32_t *pPwmPeriod      ,
        __OUTOPT  uint32_t *pPwmDutyCycle   ,
        __OUTOPT  uint32_t *pPwmPolarity    ,
        __OUTOPT  uint32_t *pPwmFeatureFlags,
        __OUTOPT  uint32_t *pPwmEnable      ,
        __OUTOPT  uint32_t *pPwmGranularity
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    uint8_t index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
    {
        if(!pwmChannel[0].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_0);
        index = 0;
    }
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
    {
        if(!pwmChannel[1].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_1);
        index = 1;
    }
    else
        return EAPI_STATUS_UNSUPPORTED;


    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmGetConfigEmul,
                    EAPI_STATUS_ERROR,
                    "Error during get pwm channel data."
                    );

    if (pPwmGranularity != NULL) *pPwmGranularity = pwmChannel[index].granularity;
    if (pPwmPeriod != NULL) *pPwmPeriod = pwmChannel[index].period;
    if (pPwmDutyCycle != NULL) *pPwmDutyCycle = pwmChannel[index].duty;
    if (pPwmPolarity != NULL) *pPwmPolarity = pwmChannel[index].polarity?EAPIEX_PWM_POLARITY_INVERSED:EAPIEX_PWM_POLARITY_NORMAL;
    if (pPwmEnable != NULL) *pPwmEnable = pwmChannel[index].enabled?EAPIEX_PWM_SET_ON:EAPIEX_PWM_SET_OFF;
    if (pPwmFeatureFlags != NULL) {
        *pPwmFeatureFlags = pwmChannel[index].alignment?EAPIEX_PWM_FEATURE_CENTER_MODE:0;
        *pPwmFeatureFlags |= pwmChannel[index].mode?EAPIEX_PWM_FEATURE_16BIT:0;
    }

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmGetConfigEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return StatusCode;
}

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetConfigEmul(
        __IN      EApiId_t Id               ,
        __INOUT   uint32_t *pPwmPeriod      ,
        __INOUT   uint32_t *pPwmDutyCycle   ,
        __IN      uint32_t *pPwmPolarity    ,
        __INOUT   uint32_t *pPwmFeatureFlags,
        __INOPT   uint32_t *pPwmMinSteps    ,
        __INOPT   uint32_t *pPwmMaxSteps    ,
        __IN      uint32_t *pPwmEnable      ,
        __OUT     uint32_t *pPwmGranularity
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;
    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
    {
        if(!pwmChannel[0].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_0);
        index = 0;
    }
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
    {
        if(!pwmChannel[1].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_1);
        index = 1;
    }
    else
        return EAPI_STATUS_UNSUPPORTED;

    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetConfigEmul,
                    EAPI_STATUS_ERROR,
                    "Error during get pwm channel data."
                    );

    /* for channel0: if Channel 1 enabled, 16bit can't be enable, disable channel 1 first */
    if ((Id == EAPIEX_ID_PWM_CHANNEL_0) && (*pPwmFeatureFlags & EAPIEX_PWM_FEATURE_16BIT) && pwmChannel[1].enabled)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetConfigEmul,
                    EAPI_STATUS_ERROR,
                    "Channel 1 is enable. It should be set disable at first."
                    );

    /*for channel1: If PWM16 enabled on channel 0, Channel 1 can't be used */
    if ((Id == EAPIEX_ID_PWM_CHANNEL_1) && (pwmChannel[0].mode))
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetConfigEmul,
                    EAPI_STATUS_ERROR,
                    "mode is set to 16Bit. channel 1 cannot be used."
                    );


    if (pwmChannel[index].period == 0) /* when period == 0 , no changes can be applied. period should have value at first*/
    {
        StatusCode = channelBasicWrite(Id, PERIOD, *pPwmPeriod);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "period cannot be set."
                        );
        StatusCode = channelRead(Id, PERIOD);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read period value."
                        );
    }

    if ((((pwmChannel[index].mode != (*pPwmFeatureFlags & EAPIEX_PWM_FEATURE_16BIT))||(pwmChannel[index].period != *pPwmPeriod))
         && *pPwmEnable == EAPIEX_PWM_SET_ON) || (*pPwmEnable==EAPIEX_PWM_SET_OFF))
    {
        StatusCode = channelBasicWrite(Id, ENABLE, 0); //OFF
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot disable channel."
                        );
        StatusCode = channelRead(Id, ENABLE);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read enable value."
                        );
    }

    if(pwmChannel[index].mode != (*pPwmFeatureFlags & EAPIEX_PWM_FEATURE_16BIT))
    {
        StatusCode = channelConfigureWrite(Id, MODE, (*pPwmFeatureFlags & EAPIEX_PWM_FEATURE_16BIT)?1:0);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "mode cannot be set."
                        );
        StatusCode = channelRead(Id, MODE);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read mode value."
                        );
    }

    if(pwmChannel[index].alignment != (*pPwmFeatureFlags & EAPIEX_PWM_FEATURE_CENTER_MODE))
    {
        StatusCode = channelConfigureWrite(Id, ALIGNMENT, (*pPwmFeatureFlags & EAPIEX_PWM_FEATURE_CENTER_MODE)?1:0);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "alignment cannot be set."
                        );
        StatusCode = channelRead(Id, ALIGNMENT);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read alignment value."
                        );
    }

    StatusCode = channelConfigureWrite(Id, MINSTEPS, (pPwmMinSteps==NULL)?0:*pPwmMinSteps);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetConfigEmul,
                    EAPI_STATUS_ERROR,
                    "min step cannot be set."
                    );

    StatusCode = channelConfigureWrite(Id, MAXSTEPS, (pPwmMaxSteps==NULL)?0:*pPwmMaxSteps);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetConfigEmul,
                    EAPI_STATUS_ERROR,
                    "max step cannot be set."
                    );

    if(pwmChannel[index].period != *pPwmPeriod)
    {
        StatusCode = channelBasicWrite(Id, PERIOD, *pPwmPeriod);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "period cannot be set."
                        );
        StatusCode = channelRead(Id, PERIOD);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read period value."
                        );
    }

    if(pwmChannel[index].duty != *pPwmDutyCycle)
    {
        StatusCode = channelBasicWrite(Id, DUTY, *pPwmDutyCycle);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "duty-cycle cannot be set."
                        );
        StatusCode = channelRead(Id, DUTY);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read duty-cycle value."
                        );
    }

    if(pwmChannel[index].polarity != *pPwmPolarity)
    {
        StatusCode = channelBasicWrite(Id, POLARITY, (uint8_t)*pPwmPolarity);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "polarity cannot be set."
                        );
        StatusCode = channelRead(Id, POLARITY);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read polarity value."
                        );
    }

    if(*pPwmEnable == EAPIEX_PWM_SET_ON)
    {
        StatusCode = channelBasicWrite(Id, ENABLE, 1);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "enable cannot be set."
                        );
        StatusCode = channelRead(Id, ENABLE);
        if (StatusCode != EAPI_STATUS_SUCCESS)
            EAPI_LIB_RETURN_ERROR(
                        EApiExPwmSetConfigEmul,
                        EAPI_STATUS_ERROR,
                        "cannot read enable value."
                        );
    }

    // place resulting period and duty cycle time in callers buffer
    *pPwmPeriod = pwmChannel[index].period;
    *pPwmDutyCycle = pwmChannel[index].duty;
    *pPwmGranularity = pwmChannel[index].granularity;

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmSetConfigEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetDutyEmul (
        __IN    EApiId_t Id             ,
        __OUT    uint32_t *pPwmDutyCycle
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
        index = 0;
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
        index = 1;
    else
        return EAPI_STATUS_UNSUPPORTED;

    StatusCode = channelRead(Id, DUTY);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmGetDutyEmul,
                    EAPI_STATUS_ERROR,
                    "cannot read duty-cycle value."
                    );

    *pPwmDutyCycle = pwmChannel[index].duty;

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmGetDutyEmul, "");
    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetDutyEmul (
        __IN    EApiId_t Id             ,
        __IN   uint32_t PwmDutyCycle
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
    {
        if(!pwmChannel[0].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_0);
        index = 0;
    }
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
    {
        if(!pwmChannel[1].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_1);
        index = 1;
    }
    else
        return EAPI_STATUS_UNSUPPORTED;

    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetDutyEmul,
                    EAPI_STATUS_ERROR,
                    "Error during get pwm channel data."
                    );

    /* if channel0 is in 16Bit mode, refuse call */
    if (Id == EAPIEX_ID_PWM_CHANNEL_1) {
        if (pwmChannel[0].mode != 0)
            return (EAPI_STATUS_ERROR);
    }

    if (pwmChannel[index].period == 0) /* when period == 0 , no changes can be applied. period should have value at first*/
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetDutyEmul,
                    EAPI_STATUS_ERROR,
                    "Cannot apply Duty-cycle when period value is zero."
                    );
    }

    StatusCode = channelBasicWrite(Id, DUTY, PwmDutyCycle);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetDutyEmul,
                    EAPI_STATUS_ERROR,
                    "Error during set pwm channel."
                    );

    StatusCode = channelRead(Id, DUTY);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetDutyEmul,
                    EAPI_STATUS_ERROR,
                    "cannot read duty-cycle value."
                    );

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmSetDutyEmul, "");
    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}  

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetEnableEmul (
        __IN    EApiId_t Id         ,
        __OUT   uint32_t *pPwmEnable
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
        index = 0;
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
        index = 1;
    else
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmGetEnableEmul,
                    EAPI_STATUS_UNSUPPORTED,
                    "Channel ID not supported."
                    );

    StatusCode = channelRead(Id, ENABLE);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmGetEnableEmul,
                    EAPI_STATUS_ERROR,
                    "cannot read enable value."
                    );

    *pPwmEnable = pwmChannel[index].enabled;

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmGetEnableEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetEnableEmul (
        __IN    EApiId_t Id         ,
        __IN    uint32_t PwmEnable
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
    {
        if(!pwmChannel[0].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_0);
        index = 0;
    }
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
    {
        if(!pwmChannel[1].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_1);
        index = 1;
    }
    else
        return EAPI_STATUS_UNSUPPORTED;

    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetEnableEmul,
                    EAPI_STATUS_ERROR,
                    "Error during get pwm channel data."
                    );
    
    /* if channel0 is in 16Bit mode, refuse call */
    if (Id == EAPIEX_ID_PWM_CHANNEL_1) {
        if (pwmChannel[0].mode != 0)
            return (EAPI_STATUS_ERROR);
    }

    if (pwmChannel[index].period == 0) /* when period == 0 , no changes can be applied. period should have value at first*/
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetEnable,
                    EAPI_STATUS_ERROR,
                    "Cannot apply enable when period value is zero."
                    );
    }

    StatusCode = channelBasicWrite(Id, ENABLE, (PwmEnable==EAPIEX_PWM_SET_ON)?1:0);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetEnableEmul,
                    EAPI_STATUS_ERROR,
                    "Error during set pwm channel."
                    );

    StatusCode = channelRead(Id, ENABLE);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetEnableEmul,
                    EAPI_STATUS_ERROR,
                    "cannot read enable value."
                    );

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmSetEnableEmul, "");

    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetPolarityEmul (
        __IN    EApiId_t Id           ,
        __OUT   uint32_t *pPwmPolarity
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
        index = 0;
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
        index = 1;
    else
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmGetPolarityEmul,
                    EAPI_STATUS_UNSUPPORTED,
                    "Channel ID not supported."
                    );

    StatusCode = channelRead(Id, POLARITY);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmGetPolarityEmul,
                    EAPI_STATUS_ERROR,
                    "cannot read polarity value."
                    );
    *pPwmPolarity = pwmChannel[index].polarity?EAPIEX_PWM_POLARITY_INVERSED:EAPIEX_PWM_POLARITY_NORMAL;
    EAPI_LIB_RETURN_SUCCESS(EApiExPwmGetPolarityEmul, "");
    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}  

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetPolarityEmul (
        __IN    EApiId_t Id           ,
        __IN    uint32_t PwmPolarity
        )
{
    EApiStatus_t StatusCode = EAPI_STATUS_SUCCESS;
    int index = 0;

    if (EAPI_STATUS_SUCCESS != pwmCheck(Id)) return (EAPI_STATUS_UNSUPPORTED);

    if(Id == EAPIEX_ID_PWM_CHANNEL_0)
    {
        if(!pwmChannel[0].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_0);
        index = 0;
    }
    else if(Id == EAPIEX_ID_PWM_CHANNEL_1)
    {
        if(!pwmChannel[1].initialized)
            StatusCode = channelReadAll(EAPIEX_ID_PWM_CHANNEL_1);
        index = 1;
    }
    else
        return EAPI_STATUS_UNSUPPORTED;

    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetPolarityEmul,
                    EAPI_STATUS_ERROR,
                    "Error during get pwm channel data."
                    );

    if (pwmChannel[index].period == 0) /* when period == 0 , no changes can be applied. period should have value at first*/
    {
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetPolarityEmul,
                    EAPI_STATUS_ERROR,
                    "Cannot apply polarity when period value is zero."
                    );
    }
    StatusCode = channelBasicWrite(Id, POLARITY, (PwmPolarity==EAPIEX_PWM_POLARITY_INVERSED)?1:0);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetPolarityEmul,
                    EAPI_STATUS_ERROR,
                    "Error during set pwm channel."
                    );

    StatusCode = channelRead(Id, POLARITY);
    if (StatusCode != EAPI_STATUS_SUCCESS)
        EAPI_LIB_RETURN_ERROR(
                    EApiExPwmSetPolarityEmul,
                    EAPI_STATUS_ERROR,
                    "cannot read polarity value."
                    );

    EAPI_LIB_RETURN_SUCCESS(EApiExPwmSetPolarityEmul, "");
    EAPI_LIB_ASSERT_EXIT
            return (StatusCode);
}


