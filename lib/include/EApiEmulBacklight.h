/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/include/EApiEmulBacklight.h $
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
 *I Description: Auto Created for EApiEmulBacklight.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulBacklight.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision: 38 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-01-26 07:48:22 +0100 (Tue, 26 Jan 2010) $
 *I  ID                   : $Id: EApiEmulBacklight.h 38 2010-01-26 06:48:22Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
/* EAPI(Embedded Application Interface) */

#ifndef _EAPIEMULBACKLIGHT_H_        
#define _EAPIEMULBACKLIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif

uint32_t 
EApiVgaGetBacklightEnableEmul( 
    __IN  uint32_t Id       , 
    __OUT uint32_t *pEnable
    );
uint32_t 
EApiVgaSetBacklightEnableEmul( 
    __IN  uint32_t Id       , 
    __OUT uint32_t Enable
    );
uint32_t 
EApiVgaGetBacklightBrightnessEmul( 
    __IN  uint32_t Id           , 
    __OUT uint32_t *pBrightness
    );
uint32_t 
EApiVgaSetBacklightBrightnessEmul( 
    __IN  uint32_t Id           , 
    __OUT uint32_t Brightness
    );

int calculateGetBacklightInRange(int input);
int calculateSetBacklightInRange(int input);

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULBACKLIGHT_H_ */



