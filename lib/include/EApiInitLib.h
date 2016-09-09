/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/include/
 *I		EApiInitLib.h $
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
 *I Description: Auto Created for EApiInitLib.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiInitLib.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision: 56 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-02-02 05:11:41 +0100 (Tue, 02 Feb 2010)$
 *I  ID                   : $Id: EApiInitLib.h 56 2010-02-02 04:11:41Z
 *I				dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
/* EAPI(Embedded Application Interface) */

#ifndef _EAPIINITLIB_H_
#define _EAPIINITLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EAPI_DBG_USE_OUTPUT_FILE
#  define EAPI_DBG_USE_OUTPUT_FILE 1
#endif
void
__cdecl
DebugMsg(
	__IN const char *const fmt,
	...
	);

uint32_t
EApiInitLib();
uint32_t
EApiUninitLib();
void find_hwmon(char **result);
EApiStatus_t read_eeprom(int eeprom_bus,char **pBuf);


#ifndef EAPI_LIB_MSG_OUT
#  if 0
#    define EAPI_LIB_MSG_OUT(...) DebugMsg("E " ## __VA_ARGS__)
#  else
#    define EAPI_LIB_MSG_OUT(x)   DebugMsg(x)
#  endif
#endif

#ifndef EAPI_LIB_ERR_OUT
#  if 0
#    define EAPI_LIB_ERR_OUT(...) DebugMsg("O " ## __VA_ARGS__)
#  else
#    define EAPI_LIB_ERR_OUT(x)   DebugMsg(x)
#  endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* _EAPIINITLIB_H_ */



