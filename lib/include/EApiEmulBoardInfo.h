/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/include/
 *I            EApiEmulBoardInfo.h $
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
 *I Description: Auto Created for EApiEmulBoardInfo.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulBoardInfo.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision: 38 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-01-26 07:48:22 +0100 (Tue, 26 Jan 2010)$
 *I  ID                   : $Id: EApiEmulBoardInfo.h 38 2010-01-26 06:48:22Z
 *I                              dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
/* EAPI(Embedded Application Interface) */

#ifndef _EAPIEMULBOARDINFO_H_
#define _EAPIEMULBOARDINFO_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "EApiEmulEeprom.h"

#define SMBIOS_BLOCK           0xd0

#define MANUFACTURE_TYPE        0x02
#define PRODUCT_TYPE            2
#define VERSION_TYPE            2
#define SN_TYPE                 2
#define MANUFACTURE_DATE_TYPE   0xa0
#define BOARD_ID_TYPE           0xa0

#define MANUFACTURE_ASCII_IND   7
#define PRODUCT_ASCII_IND       8
#define VERSION_ASCII_IND       9
#define SN_ASCII_IND            10
#define MANUFACTURE_DATE_ASCII_IND 17
#define BOARD_ID_ASCII_IND         16

uint32_t
EApiBoardGetValueEmul(
    __IN  uint32_t Id,
    __OUT uint32_t *pValue
    );

uint32_t
EApiBoardGetStringAEmul(
    __IN    uint32_t  Id,
    __OUT   char     *pBuffer,
    __INOUT uint32_t *pBufLen
    );

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULBOARDINFO_H_ */



