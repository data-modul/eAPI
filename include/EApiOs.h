/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/include/linux/EApiOs.h $
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
 *I Description: Auto Created for EApiOs.h
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiOs.h
 *I  File Location        : include\linux
 *I  Last committed       : $Revision: 72 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-20 04:28:21 +0200 (Sun, 20 Jun 2010) $
 *I  ID                   : $Id: EApiOs.h 72 2010-06-20 02:28:21Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
/* Windows NT Common Header */
#ifndef _EAPIOS_H_        
#define _EAPIOS_H_

#ifndef __IN
#  define __IN
#endif

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <ctype.h>



#if defined _WIN32 || defined __CYGWIN__
  #ifdef EAPI_BUILDING_LIB
    #ifdef __GNUC__
      #define EAPI_CALLTYPE __attribute__((dllexport))
    #else
      #define EAPI_CALLTYPE __declspec(dllexport) /* Note: actually gcc seems to also supports this syntax. */
    #endif
  #else
    #ifdef __GNUC__
      #define EAPI_CALLTYPE __attribute__((dllimport))
    #else
      #define EAPI_CALLTYPE __declspec(dllimport) /* Note: actually gcc seems to also supports this syntax. */
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define EAPI_CALLTYPE __attribute__ ((visibility("default")))
  #else
    #define EAPI_CALLTYPE
  #endif
#endif
#ifdef __GNUC__ 
#ifdef __i386__
#    ifndef __fastcall
#      define __fastcall __attribute__((fastcall))
#    endif
#    ifndef __stdcall
#      define __stdcall __attribute__((stdcall))
#    endif
#    ifndef __cdecl
#      define __cdecl __attribute__((cdecl))
#    endif
#else
#    ifndef __fastcall
#      define __fastcall
#    endif
#    ifndef __stdcall
#      define __stdcall
#    endif
#    ifndef __cdecl
#      define __cdecl
#    endif
#endif
#  ifndef _fastcall
#    define _fastcall __fastcall
#  endif
#  ifndef _stdcall
#    define _stdcall __stdcall
#  endif
#  ifndef _cdecl
#    define _cdecl __cdecl
#  endif

#endif

#define EAPI_PRINTF_ARG(Format, Arg) __attribute__((format(printf, Format, Arg)))
void EApiSleepns(unsigned long ns);
#define EAPI_EMUL_DELAY_NS(x) EApiSleepns(x)

#ifndef EApiSleep
#  define EApiSleep(x) usleep(x*1000)
#endif



#ifdef UNICODE
#  define TEXT(x) Lx
#  define EAPI_strlen         wcslen
#  define EAPI_strnlen        wcsnlen
#  define EAPI_strncpy        wcsncpy
#  define EAPI_vsnprintf      vsnwprintf
#  define EAPI_fprintf        fwprintf
#  define EAPI_printf         wprintf
#  define EAPI_fopen          _wfopen
#else
#  define TEXT(x) x
#  define EAPI_strlen         strlen
#  define EAPI_strnlen        strnlen
#  define EAPI_strncpy        strncpy
#  define EAPI_vsnprintf      vsnprintf
#  define EAPI_fprintf        fprintf
#  define EAPI_fopen          fopen
#  define EAPI_printf         printf
#endif

#define EAPI_vsnprintfA     vsnprintf
#define EAPI_strdup         strdup
#define EAPI_strncpyA       strncpy

#define _strdup strdup

#endif /* _EAPIOS_H_ */


