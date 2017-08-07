/** ---------------------------------------------------------------------------
*
*              Copyright (c) 2017, Data Modul AG
*                                                                         
*              This program contains proprietary and confidential         
*              information. All rights reserved except as may be          
*                  permitted by prior written consent.                    
*                                                                         
* -----------------------------------------------------------------------------
*                                                                             
*  $Revision: $
*  $Author:   $
*  $Date:     $
*  $Id:       $  
*
* -----------------------------------------------------------------------------
*    
*  @file    EApiExt.h
*  @author  JanK
*  @date    03/13/17
*  @brief   Data Modul AG EApi extensions. 
*  @todo    Add WdtEx interface, PIM Muxing (?), RTM OOS, extra I2C buses etc.
*
*  ----------------------------------------------------------------------------
*
*  Revision History
*  ----------------
*  $Log:  $
*
*
*---------------------------------------------------------------------------**/

#ifndef  _DMO_EAPI_EXT_H  // Multiple inclusion Guard
#define  _DMO_EAPI_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "EApi.h"

/**
  @addtogroup EApiEx_PUBLIC_FUNCTIONS DATA MODUL EAPi Extensions public functions & definitions.

  Public functions.
  
  @{
**/

#define DMO_EAPI_EXT_VERSION   0
#define DMO_EAPI_EXT_REVISION  1 

/*
 *
 *      EApiExt additional values/sensors.
 *
 */
#define EAPI_ID_GET_EAPIEX_SPEC_VERSION   EAPI_UINT32_C(0x30000)  /**< EAPi Extensions specification version.*/
 
#define EAPIEX_ID_DIMM00_CPU_TEMP         EAPI_UINT32_C(0x40000)  /**< Memory DIMM0 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM01_CPU_TEMP         EAPI_UINT32_C(0x40001)  /**< Memory DIMM1 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM02_CPU_TEMP         EAPI_UINT32_C(0x40002)  /**< Memory DIMM2 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM03_CPU_TEMP         EAPI_UINT32_C(0x40003)  /**< Memory DIMM3 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM04_CPU_TEMP         EAPI_UINT32_C(0x40004)  /**< Memory DIMM4 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM05_CPU_TEMP         EAPI_UINT32_C(0x40005)  /**< Memory DIMM5 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM06_CPU_TEMP         EAPI_UINT32_C(0x40006)  /**< Memory DIMM6 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM07_CPU_TEMP         EAPI_UINT32_C(0x40007)  /**< Memory DIMM7 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM08_CPU_TEMP         EAPI_UINT32_C(0x40008)  /**< Memory DIMM8 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM19_CPU_TEMP         EAPI_UINT32_C(0x40009)  /**< Memory DIMM9 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM10_CPU_TEMP         EAPI_UINT32_C(0x4000A)  /**< Memory DIMM10 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM11_CPU_TEMP         EAPI_UINT32_C(0x4000B)  /**< Memory DIMM11 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM12_CPU_TEMP         EAPI_UINT32_C(0x4000C)  /**< Memory DIMM12 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM13_CPU_TEMP         EAPI_UINT32_C(0x4000D)  /**< Memory DIMM13 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM14_CPU_TEMP         EAPI_UINT32_C(0x4000E)  /**< Memory DIMM14 temperature in 0.1 Kelvins.*/
#define EAPIEX_ID_DIMM15_CPU_TEMP         EAPI_UINT32_C(0x4000F)  /**< Memory DIMM15 temperature in 0.1 Kelvins.*/

#define EAPIEX_ID_MISC_0_TEMP             EAPI_UINT32_C(0x40100)  /**< Miscellaneous temperature #0 in 0.1 Kelvins.*/
#define EAPIEX_ID_MISC_1_TEMP             EAPI_UINT32_C(0x40101)  /**< Miscellaneous temperature #1 in 0.1 Kelvins.*/
#define EAPIEX_ID_MISC_2_TEMP             EAPI_UINT32_C(0x40102)  /**< Miscellaneous temperature #2 in 0.1 Kelvins.*/

/*
 *
 *      DMO EApi Extensions
 *
 */

/*
 *
 *      PWM
 *
 */

/* IDs */
#define EAPIEX_ID_PWM_CHANNEL_0       EAPI_UINT32_C(0)            /* channel ID for 8Bit PWM channel 0 */      
#define EAPIEX_ID_PWM_CHANNEL_1       EAPI_UINT32_C(1)            /* channel ID for 8Bit PWM channel 1 */
#define EAPIEX_ID_PWM_CHANNEL_NONE    EAPI_UINT32_C(0xFFFFFFFF)   /* no channel assigned */

/* PWM values */
#define EAPIEX_PWM_SET_OFF            EAPI_UINT32_C(0)
#define EAPIEX_PWM_SET_ON             EAPI_UINT32_C(1)

/* Polarity */
#define EAPIEX_PWM_POLARITY_NORMAL    0 
#define EAPIEX_PWM_POLARITY_INVERSED  1

/* Feature flags */
#define EAPIEX_PWM_FEATURE_CENTER_MODE    EAPI_UINT32_C(1<<0)
#define EAPIEX_PWM_FEATURE_16BIT          EAPI_UINT32_C(1<<1)
#define EAPIEX_PWM_QUERY_FLAG             EAPI_UINT32_C(1<<31)

/**

  Get PWM channel capabilities.
      
  @param[in]        Id                        PWM Channel Id.
  @param[out]       pPwmMinPeriod             Minimum PWM signal period in ns.
  @param[out]       pPwmMaxPeriod             maximum PWM signal period in ns.
  @param[out]       pPwmMinGranularity        Granularity if PwmMinPeriod is selected.
  @param[out]       pPwmMaxGranularity        Granularity if PwmMaxPeriod is selected.
  @param[out]       pPwmCompanionChannelId    Id of the PWM channel that is used to form a 16Bit channel, 
                                              EAPIEX_ID_PWM_CHANNEL_NONE if none.
  @param[in,out]    pPwmFeatureFlags          Supported features, see Feature flags. If EAPIEX_PWM_QUERY_FLAG is set 
                                              on entry, the function will return the values for the case where 
                                              the feature flag combination passed is enabled, otherwise ist returns
                                              absolut maximum/minimum values that.
                                              
  @retval   EAPI_STATUS_NOT_INITIALIZED       Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER     Invalid input parameter, e.g. NULL-pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED           Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR                 Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS               Success, output values valid.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetCaps(
    __IN      EApiId_t Id                  ,
    __OUT     uint32_t *pPwmMinPeriod      ,
    __OUT     uint32_t *pPwmMaxPeriod      ,
    __OUT     uint32_t *pPwmMinGranularity ,
    __OUT     uint32_t *pPwmMaxGranularity ,
    __OUT     EApiId_t *pPwmCompanionChannelId, 
    __INOUT   uint32_t *pPwmFeatureFlags
    );

/**

  Get PWM channel configuration. Pointers that are passed in as null will
  not be filled.
      
  @param[in]        Id                    PWM Channel Id.
  @param[out]       pPwmPeriod            Current PWM signal period in ns.
  @param[out]       pPwmDutyCycle         Current Duty cycle in ns.
  @param[out]       pPwmPolarity          Current Polarity.
  @param[out]       pPwmFeatureFlags      Current Features enable mask.
  @param[out]       pPwmEnabled           PWM Enable/disable output state.
  @param[out]       pPwmGranularity       PW granularity in ns, can be used to calculate the number of steps
                                          available for this PWM configuration.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. NULL-pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetConfig(
    __IN      EApiId_t Id               ,
    __OUTOPT  uint32_t *pPwmPeriod      ,
    __OUTOPT  uint32_t *pPwmDutyCycle   ,
    __OUTOPT  uint32_t *pPwmPolarity    ,
    __OUTOPT  uint32_t *pPwmFeatureFlags,
    __OUTOPT  uint32_t *pPwmEnable      ,
    __OUTOPT  uint32_t *pPwmGranularity
    );

/**

  Set PWM channel configuration.
      
  @param[in]        Id                    PWM Channel Id.
  @param[in,out]    pPwmPeriod            PWM signal period to set in Hz, must be between 
                                          MinPwmPeriod and MaxPwmPeriod. If the given period
                                          can not be achieved exactly due to hardware restrictions, the
                                          function will calculate the closest match and return it in this
                                          parameter.
  @param[in,out]    pPwmDutyCycle         Duty cycle to set in ns, must be between 0 and selected PwmPeriod.
  @param[in]        pPwmPolarity          Polarity to set, either normal or inverted.
  @param[in,out]    pPwmFeatureFlags      Features enable mask.
  @param[in]        pPwmMinSteps          The minimum number of steps the PWM should provide, used to calculate 
                                          the best PWM parameter match. 0 means auto select.
  @param[in]        pPwmMaxSteps          The minimum number of steps the PWM should provide, used to calculate 
                                          the best PWM parameter match. 0 means auto select.
  @param[in]        pPwmEnable            Enable/disable PWM output, 0 = disable, 1 = enable.  
  @param[out]       pPwmGranularity       PW granularity in ns, can be used to calculate the number of steps
                                          available for this PWM configuration.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. DutyCycle or PwmPeriod out of range.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             If a previous call to EApiExPwmGetCaps for this channel did succeed,
                                          this return value means that the channel can't be used since one of
                                          the 8Bit channels is already in use. Disable both 8Bit channels first
                                          to use the 16Bit channel.
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetConfig(
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

/**

  get PWM channel duty cycle.
      
  @param[in]    Id                        PWM Channel Id.
  @param[in]    pPwmDutyCycle             Duty cycle in ns.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. NULL pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetDuty (
    __IN    EApiId_t Id             ,
    __OUT    uint32_t *pPwmDutyCycle
    );

/**

  Set PWM channel duty cycle.
      
  @param[in]    Id                        PWM Channel Id.
  @param[in]    PwmDutyCycle              Duty cycle to set in ns, must be between 0 and PwmPeriod.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. DutyCycle out of range.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetDuty (
    __IN    EApiId_t Id             ,
    __IN    uint32_t PwmDutyCycle
    );

/**

  Get PWM output Enable/disable state.
      
  @param[in]        Id                    PWM Channel Id.
  @param[in,out]    pPwmEnable            Returns either EAPIEX_PWM_SET_ON or EAPIEX_PWM_SET_OFF.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. NULL-pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetEnable (
    __IN    EApiId_t Id         ,
    __OUT   uint32_t *pPwmEnable
    );

/**

  Set PWM output Enable/disable state.
      
  @param[in]        Id                    PWM Channel Id.
  @param[in,out]    PwmEnable             Either EAPIEX_PWM_SET_ON or EAPIEX_PWM_SET_OFF.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. NULL-pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetEnable (
    __IN    EApiId_t Id         ,
    __IN    uint32_t PwmEnable
    );

/**

  Get PWM output polarity.
      
  @param[in]        Id                    PWM Channel Id.
  @param[in,out]    pPwmPolarity          Returns either EAPIEX_PWM_POLARITY_NORMAL or EAPIEX_PWM_POLARITY_INVERSED.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. NULL-pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmGetPolarity (
    __IN    EApiId_t Id           ,
    __OUT   uint32_t *pPwmPolarity
    );

/**

  Set PWM polarity.
      
  @param[in]        Id                    PWM Channel Id.
  @param[in,out]    PwmPolarity           Either EAPIEX_PWM_POLARITY_NORMAL or EAPIEX_PWM_POLARITY_INVERSED.

  @retval   EAPI_STATUS_NOT_INITIALIZED   Library not initialized.
  @retval   EAPI_STATUS_INVALID_PARAMETER Invalid input parameter, e.g. NULL-pointer passed in.
  @retval   EAPI_STATUS_UNSUPPORTED       Function/Channel not supported.
  @retval   EAPI_STATUS_ERROR             Unspecified error.  
  @retval   EAPI_STATUS_SUCCESS           Success.

**/
EApiStatus_t
EAPI_CALLTYPE 
EApiExPwmSetPolarity (
    __IN    EApiId_t Id           ,
    __IN    uint32_t PwmPolarity
    );

#ifdef __cplusplus
}
#endif

#endif  // Multiple inclusion Guard
