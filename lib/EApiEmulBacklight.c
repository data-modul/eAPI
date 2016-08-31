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


/*
 *
 *  
 *
 *  B A C K L I G H T
 *
 *
 *
 */
typedef struct BackLightDevice_s{
  EApiId_t Id                ; /* EAPI Temperature Id */
  uint32_t Enabled           ; /* Current Enabled State */
  uint32_t Brightness        ; /* Current Brightness */
}BackLightDevice_t;
static BackLightDevice_t BacklightLookup[]={
  {EAPI_ID_BACKLIGHT_1  , EAPI_BACKLIGHT_SET_ON  , EAPI_BACKLIGHT_SET_DIMEST    },
  {EAPI_ID_BACKLIGHT_2  , EAPI_BACKLIGHT_SET_OFF , EAPI_BACKLIGHT_SET_BRIGHTEST },
};

EApiStatus_t 
EApiVgaGetBacklightEnableEmul( 
    __IN  EApiId_t Id       , 
    __OUT uint32_t *pEnable
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i;

  for(i=0;i<ARRAY_SIZE(BacklightLookup);i++)
  {
      if(BacklightLookup[i].Id==Id)
      {
        *pEnable=BacklightLookup[i].Enabled;
        EAPI_LIB_RETURN_SUCCESS(
          EApiVgaGetBacklightEnableEmul, 
          ""
          );
      }
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
  unsigned i;

  for(i=0;i<ARRAY_SIZE(BacklightLookup);i++)
  {
      if(BacklightLookup[i].Id==Id)
      {
        BacklightLookup[i].Enabled=Enable;
        EAPI_LIB_RETURN_SUCCESS(
          EApiVgaSetBacklightEnableEmul, 
          ""
          );
      }
  }
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
  unsigned i;

  for(i=0;i<ARRAY_SIZE(BacklightLookup);i++)
  {
      if(BacklightLookup[i].Id==Id)
      {
        *pBrightness=BacklightLookup[i].Brightness;
        EAPI_LIB_RETURN_SUCCESS(
          EApiVgaGetBacklightBrightnessEmul, 
          ""
          );
      }
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
    __OUT uint32_t Brightness
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i;

  for(i=0;i<ARRAY_SIZE(BacklightLookup);i++)
  {
      if(BacklightLookup[i].Id==Id)
      {
        BacklightLookup[i].Brightness=Brightness;
        EAPI_LIB_RETURN_SUCCESS(
          EApiVgaSetBacklightBrightness, 
          ""
          );
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiVgaSetBacklightBrightnessEmul, 
    EAPI_STATUS_UNSUPPORTED  , 
    "Unrecognised Backlight ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}


