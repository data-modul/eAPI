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


/*
 *
 *  
 *
 *  W A T C H D O G
 *
 *
 *
 */
//#define WATCHDOG_ENABLED  ((unsigned)-1)
//#define WATCHDOG_DISABLED ((unsigned)0)
//static unsigned WatchdogState=WATCHDOG_DISABLED;


//#define MIN_IN_millisec(x) ((x)*60*1000)
//#define MaxDelay        MIN_IN_millisec(10)
//#define MaxEventTimeout MIN_IN_millisec(10)
//#define MaxResetTimeout MIN_IN_millisec(10)
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
//  *pMaxDelay        =MaxDelay;
//  *pMaxEventTimeout =MaxEventTimeout;
//  *pMaxResetTimeout =MaxResetTimeout;
//  EAPI_LIB_RETURN_SUCCESS(EApiWDogGetCapEmul, "");

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
//  EAPI_LIB_RETURN_ERROR_IF(
//      EApiWDogStartEmul,
//      (WatchdogState==WATCHDOG_ENABLED),
//      EAPI_STATUS_RUNNING,
//      "Watchdog alread runing, need to stop before starting"
//      );
//  EAPI_LIB_ASSERT_PARAMATER_CHECK(
//    EApiWDogStartEmul,
//    (Delay>MaxDelay),
//   "(Delay>pMaxDelay)"
//  );
//  EAPI_LIB_ASSERT_PARAMATER_CHECK(
//    EApiWDogStartEmul,
//    (EventTimeout>MaxEventTimeout),
//   "(EventTimeout>pMaxEventTimeout)"
//  );
//  EAPI_LIB_ASSERT_PARAMATER_CHECK(
//    EApiWDogStartEmul,
//    (ResetTimeout>MaxResetTimeout),
//   "(ResetTimeout>pMaxResetTimeout)"
//  );
//  WatchdogState=WATCHDOG_ENABLED;
//  EAPI_LIB_RETURN_SUCCESS(EApiWDogStartEmul, "");
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}
EApiStatus_t 
EApiWDogTriggerEmul(void)
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
//  EAPI_LIB_RETURN_ERROR_IF(
//      EApiWDogTriggerEmul,
//      (WatchdogState==WATCHDOG_DISABLED),
//      EAPI_STATUS_ERROR,
//      "Watchdog Disabled therefore not possible to trigger"
//      );

//  EAPI_LIB_RETURN_SUCCESS(
//      EApiWDogTriggerEmul,
//      ""
//      );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}
EApiStatus_t 
EApiWDogStopEmul(void)
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
//  WatchdogState=WATCHDOG_DISABLED;
//  EAPI_LIB_RETURN_SUCCESS(
//      EApiWDogStopEmul,
//      ""
//      );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}



