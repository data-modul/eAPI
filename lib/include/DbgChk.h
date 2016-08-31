/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/include/DbgChk.h $
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
 *I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN '
 *I              AS IS' BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF
 *I              ANY KIND, EITHER EXPRESS OR IMPLIED.
 *I
 *I Description: Auto Created for DbgPrt.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : DbgChk.h
 *I  File Location        : lib\include
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010)$
 *I  ID                   : $Id: DbgChk.h 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
#ifndef _DBGCHK_H_
#define _DBGCHK_H_


#ifdef __cplusplus
extern "C" {
#endif



signed int
__cdecl
siFormattedMessage_M2(
		char          cType,
		const char *cszFileName,
		const char *cszFuncName,
		unsigned int  uiLineNum,
		const char *csz2ndValue,
		const char *cszFormat,
		...
		)
EAPI_PRINTF_ARG(6, 7);


signed int
__cdecl
siFormattedMessage_SC(
		char          cType,
		const char *cszFileName,
		const char *cszFuncName,
		unsigned int  uiLineNum,
		EApiStatus_t  StatusCode,
		const char *cszFormat,
		...
		)
EAPI_PRINTF_ARG(6, 7);


#ifdef __cplusplus
}
#endif



#endif /* _DBGCHK_H_ */


