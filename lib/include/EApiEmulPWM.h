/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApi-Extension Application Development Kit
 *+=========================================================================
 *I   Copyright: Copyright (c) 2017, DATA MODUL AG
 *I      Author: Reyhaneh Yazdani,           ryazdani@data-modul.com
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
 *I Description: Auto Created for EApiEmulPWM.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulPWM.h
 *I  File Location        : lib\include
 *I
 *+=========================================================================
 *</KHeader>
 */
/* EAPI-Ex (Embedded-Extension Application Interface) */

#ifndef _EAPIEMULPWM_H_        
#define _EAPIEMULPWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_SRC_CLK           50000000    /* default input clock is 50MHz */

#define DMEC_PWM_MODE_8BIT              0
#define DMEC_PWM_MODE_16BIT             1

#define DMEC_PWM_CHANNEL0_FEATURES      (EAPIEX_PWM_FEATURE_CENTER_MODE | EAPIEX_PWM_FEATURE_16BIT)
#define DMEC_PWM_CHANNEL1_FEATURES      (EAPIEX_PWM_FEATURE_CENTER_MODE)

#define DMEC_PWM_CHANNEL0_COMPANION     EAPIEX_ID_PWM_CHANNEL_1
#define DMEC_PWM_CHANNEL1_COMPANION     EAPIEX_ID_PWM_CHANNEL_NONE

#define DMEC_PWM_MIN_STEPS              8
#define DMEC_PWM_MAX_STEPS_8BIT         (uint32_t)UINT8_MAX
#define DMEC_PWM_MAX_STEPS_16BIT        (uint32_t)UINT16_MAX
#define DMEC_PWM_MIN_SCALER             0
#define DMEC_PWM_MAX_SCALER             UINT8_MAX
#define DMEC_PWM_CLK_GRANULARITY        (uint32_t)(1000000000/PWM_SRC_CLK)

#define DMEC_PWM_MIN_PRESCALER_VALUE    0
#define DMEC_PWM_MIN_PRESCALER          (1 << DMEC_PWM_MIN_PRESCALER_VALUE)

#define DMEC_PWM_MAX_PRESCALER_VALUE    7
#define DMEC_PWM_MAX_PRESCALER          (1 << DMEC_PWM_MAX_PRESCALER_VALUE)

#define DMEC_MIN_PWM_GRANULARITY(f)     (uint32_t)(DMEC_PWM_CLK_GRANULARITY * DMEC_PWM_MIN_PRESCALER * (DMEC_PWM_MIN_SCALER+ 1) * ((f & EAPIEX_PWM_FEATURE_CENTER_MODE)?2:1))

#define DMEC_MAX_PWM_GRANULARITY(f)     (uint32_t)(DMEC_PWM_CLK_GRANULARITY * DMEC_PWM_MAX_PRESCALER * (DMEC_PWM_MAX_SCALER+ 1) * (((f) & (EAPIEX_PWM_FEATURE_CENTER_MODE))?2:1))


#define DMEC_MIN_PWM_PERIOD(f)          (uint32_t)(DMEC_MIN_PWM_GRANULARITY(f) * DMEC_PWM_MIN_STEPS)
#define DMEC_MAX_PWM_PERIOD(f)          (uint32_t)(((f) & (EAPIEX_PWM_FEATURE_16BIT))?UINT32_MAX:(DMEC_MAX_PWM_GRANULARITY(f) * DMEC_PWM_MAX_STEPS_8BIT))

#define DMEC_PWM_GRANULARITY(p, s, a)   (uint32_t)(DMEC_PWM_CLK_GRANULARITY * (uint32_t)(1 << p) * (uint32_t)(s + 1) * (a?2:1))



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
        );

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
        );

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
        );

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetDutyEmul (
        __IN    EApiId_t Id             ,
        __IN    uint32_t *pPwmDutyCycle
        );

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetDutyEmul (
        __IN    EApiId_t Id             ,
        __OUT   uint32_t PwmDutyCycle
        );


EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetEnableEmul (
        __IN    EApiId_t Id         ,
        __OUT   uint32_t *pPwmEnable
        );

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetEnableEmul (
        __IN    EApiId_t Id         ,
        __IN    uint32_t PwmEnable
        );

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetPolarityEmul (
        __IN    EApiId_t Id           ,
        __OUT   uint32_t *pPwmPolarity
        );

EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetPolarityEmul (
        __IN    EApiId_t Id           ,
        __IN    uint32_t PwmPolarity
        );


#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULPWM_H_ */
