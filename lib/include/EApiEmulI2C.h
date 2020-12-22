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


EApiStatus_t
EAPI_CALLTYPE EApiI2CGetBusCapEmul(
	__IN  uint32_t  Id,
	__OUT uint32_t *pMaxBlkLen
	);


EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadRawEmul(
        __IN      EApiId_t Id         , /* I2C Bus Id */
        __IN      uint8_t  Addr       , /* Encoded 7Bit I2C
                                         * Device Address
                                         */
        __INOPT   uint8_t  *pWBuffer  , /* Write Data pBuffer */
        __IN      uint32_t WriteBCnt  , /* Number of Bytes to write */
        __OUTOPT  uint8_t  *pRBuffer  , /* Read Data pBuffer */
        __IN      uint32_t ReadBCnt     /* Number of Bytes to read */
        );

EApiStatus_t
EAPI_CALLTYPE
EApiI2CWriteReadEmul(
        __IN      EApiId_t Id         , /* I2C Bus Id */
        __IN      uint8_t  Addr       , /* Encoded 7Bit I2C
                                         * Device Address
                                         */
        __INOPT   uint8_t  *pWBuffer  , /* Write Data pBuffer */
        __IN      uint32_t WriteBCnt  , /* Number of Bytes to write plus 1 */
        __IN     uint32_t  CmdBCnt,     /* Number of Cmd Bytes to write */
        __OUTOPT  uint8_t  *pRBuffer  , /* Read Data pBuffer */
        __IN      uint32_t ReadBCnt     /* Number of Bytes to read plus 1 */
        );

#ifdef __cplusplus
}
#endif

#endif /* _EAPIEMULI2C_H_ */


