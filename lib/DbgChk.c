/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/DbgChk.c $
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
 *I  File Name            : DbgChk.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: DbgChk.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>
typedef struct ErrorLookupTbl_S{
  const EApiStatus_t StatusCode;
  const char  *const  ErrorString;
}ErrorLookupTbl_t;
#define EAPI_INVALID_STRLEN ((size_t )-1)
extern FILE *OutputStream;

const ErrorLookupTbl_t ErrorLookupA[]={
  {EAPI_STATUS_NOT_INITIALIZED        , "NOT_INITIALIZED"          },
  {EAPI_STATUS_INITIALIZED            , "INITIALIZED"              },
  {EAPI_STATUS_ALLOC_ERROR            , "ALLOC_ERROR"              },
  {EAPI_STATUS_DRIVER_TIMEOUT         , "DRIVER_TIMEOUT"           },
  {EAPI_STATUS_INVALID_PARAMETER      , "INVALID_PARAMETER"        },
  {EAPI_STATUS_INVALID_BLOCK_ALIGNMENT, "INVALID_BLOCK_ALIGNMENT"  },
  {EAPI_STATUS_INVALID_BLOCK_LENGTH   , "INVALID_BLOCK_LENGTH"     },
  {EAPI_STATUS_INVALID_DIRECTION      , "INVALID_DIRECTION"        },
  {EAPI_STATUS_INVALID_BITMASK        , "INVALID_BITMASK"          },
  {EAPI_STATUS_RUNNING                , "RUNNING"                  },
  {EAPI_STATUS_UNSUPPORTED            , "UNSUPPORTED"              },
  {EAPI_STATUS_NOT_FOUND              , "NOT_FOUND"                },
  {EAPI_STATUS_TIMEOUT                , "TIMEOUT"                  },
  {EAPI_STATUS_BUSY_COLLISION         , "BUSY_COLLISION"           },
  {EAPI_STATUS_READ_ERROR             , "READ_ERROR"               },
  {EAPI_STATUS_WRITE_ERROR            , "WRITE_ERROR"              },
  {EAPI_STATUS_MORE_DATA              , "MORE_DATA"                },
  {EAPI_STATUS_ERROR                  , "ERROR"                    },
  {EAPI_STATUS_SUCCESS                , "SUCCESS"                  },
};
EApiStatus_t
EApiGetErrorStringA(
    __OUT const char  **const pszString     , 
    __IN EApiStatus_t StatusCode
    )
{
  unsigned i;
  const ErrorLookupTbl_t *pErrorLookup;

/*   EAPI_LIB_ASSERT_PARAMATER_NULL(EApiGetErrorStringA, pszString); */

  *pszString=NULL;
  for(i=ARRAY_SIZE(ErrorLookupA),
      pErrorLookup=ErrorLookupA; 
      i--;pErrorLookup++
    )

  {
    if(pErrorLookup->StatusCode==StatusCode){
      *pszString=pErrorLookup->ErrorString;
      return EAPI_STATUS_SUCCESS;
    }
  }
  return EAPI_STATUS_ERROR;
}

const char *GetLastOccurance(
    const char *czFilename
    )
{
  const char*cszLPos=czFilename;
  while(*czFilename!='\0'){
    switch(*czFilename++){
      case '\\':
      case '/':
        cszLPos=czFilename;
      default:
        break;
    }
  }
  return cszLPos;
}
const char cszLogFormat[]={"%c%04i %-15.15s %-30s | "};

signed int
__cdecl
siFormattedMessage_M2(
    char          cType       ,
    const char *  cszFileName ,   
    const char *  cszFuncName ,   
    unsigned int  uiLineNum   ,
    const char *  csz2ndValue ,   
    const char *  cszFormat   ,   
    ...
    )
{
  signed int Ret=0;
  va_list _ArgList;

  va_start(_ArgList, cszFormat);
  if (OutputStream) {
  Ret = fprintf(
      OutputStream,
      cszLogFormat, 
      cType       ,
      uiLineNum   ,
      GetLastOccurance(cszFileName),
      cszFuncName
    );
  fprintf(OutputStream, "%-25s | ", csz2ndValue);
  Ret += vfprintf(OutputStream, cszFormat, _ArgList);
    }
	va_end(_ArgList);
	return Ret;
}


signed int
__cdecl
siFormattedMessage_SC(
    char          cType       ,
    const char *  cszFileName ,   
    const char *  cszFuncName ,   
    unsigned int  uiLineNum   ,
    EApiStatus_t  StatusCode  ,   
    const char *  cszFormat   ,   
    ...
    )
{
  signed int Ret=0;
  va_list _ArgList;
  const char *szStatusDesc;
  va_start(_ArgList, cszFormat);
  if (OutputStream) {
  EApiGetErrorStringA(&szStatusDesc, StatusCode);

  Ret = fprintf(
      OutputStream,
      cszLogFormat, 
      cType       ,
      uiLineNum   ,
      GetLastOccurance(cszFileName),
      cszFuncName
    );
  if(szStatusDesc==NULL){
    fprintf(OutputStream, "               0x%08"PRIX32" | ", StatusCode);
  }else{
    fprintf(OutputStream, "%-25s | ", szStatusDesc);
  }
  Ret += vfprintf(OutputStream, cszFormat, _ArgList);
  }
	va_end(_ArgList);
	return Ret;
}



