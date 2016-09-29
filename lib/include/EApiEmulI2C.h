/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/include/
 *I		EApiEmulI2C.h $
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
 *I Description: Auto Created for EApiEmulI2C.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulI2C.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision: 38 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-01-26 07:48:22 +0100 (Tue, 26 Jan 2010)$
 *I  ID                   : $Id: EApiEmulI2C.h 38 2010-01-26 06:48:22Z
 *I				dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
/* EAPI(Embedded Application Interface) */

#ifndef _EAPIEMULI2C_H_
#define _EAPIEMULI2C_H_

#ifdef __cplusplus
extern "C" {
#endif


uint32_t
EAPI_CALLTYPE EApiI2CGetBusCapEmul(
	__IN  uint32_t  Id,
	__OUT uint32_t *pMaxBlkLen
	);
uint32_t
EAPI_CALLTYPE
EApiI2CWriteReadEmul(
	__IN      uint32_t  Id,
	__IN      uint8_t   Addr,
    __INOPT   uint8_t     *pWpBuffer,
	__IN      uint32_t  WriteBCnt,
     __IN     uint32_t  CmdBCnt,
    __OUTOPT  uint8_t     *pRpBuffer,
	__IN      uint32_t  ReadBCnt
	);

uint32_t
EAPI_CALLTYPE
EApiI2CWriteReadEmulUniversal(
    __IN      uint32_t  Id,
    __IN      uint8_t   Addr,
    __INOPT   uint8_t     *pWpBuffer,
    __IN      uint32_t  WriteBCnt,
    __OUTOPT  uint8_t     *pRpBuffer,
    __IN      uint32_t  ReadBCnt
    );

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULI2C_H_ */


