/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulStorage.c $
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
 *I Description: Auto Created for EApiEmulStorage.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulStorage.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulStorage.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>
#include <stdio.h>




/*
 *
 *  
 *
 *  S T O R A G E 
 *
 *
 *
 */
typedef struct StorageAreaMapping_s{
  EApiId_t Id          ; /* EAPI Storage Area Id */
  uint32_t StorageSize ; /* Storage Area Size */
  uint32_t WBlockLength; /* Block Lenght/Alignment */
  uint8_t *pu8Buffer   ; /* Pointer to Storage pu8Buffer */
  const char *const cszFilename;
  FILE     *FStream;
}StorageAreaMapping_t;
static uint8_t StorageID0[32]={0};
static uint8_t StorageID1[1024*4]={0};
StorageAreaMapping_t StorageAreaTbl[]={
  {
    EAPI_ID_STORAGE_STD        , 
    sizeof(StorageID0)         , 
    ELEMENT_SIZE(StorageID0)   , 
    StorageID0                 ,
    "EApiStorage0.EMUL"        ,
    NULL
  },
  {
    EAPI_PMG_ID_STORAGE_SAMPLE , 
    sizeof(StorageID1)         , 
    ELEMENT_SIZE(StorageID1)*1024 , 
    StorageID1                 ,
    "EApiStorage1.EMUL"        ,
    NULL
  },
};

EApiStatus_t 
EApiStorageCapEmul (
    __IN      EApiId_t  Id          , 
    __OUTOPT  uint32_t *pStorageSize, 
    __OUTOPT  uint32_t *pBlockLength
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  StorageAreaMapping_t *pCurStorageDesc;
  unsigned i;

  for(
      i=ARRAY_SIZE(StorageAreaTbl),
      pCurStorageDesc=StorageAreaTbl;
      i--;
      pCurStorageDesc++)
  {
      if(pCurStorageDesc->Id==Id)
      {
        *pStorageSize=pCurStorageDesc->StorageSize;
        *pBlockLength=pCurStorageDesc->WBlockLength;
        EAPI_LIB_RETURN_SUCCESS(EApiStorageCap, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiStorageCapEmul        , 
    EAPI_STATUS_UNSUPPORTED   , 
    "Unrecognised Storage ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}
EApiStatus_t 
EApiStorageAreaReadEmul(
  __IN  EApiId_t Id     , 
  __IN  uint32_t Offset , 
  __OUT    void *pvBuffer, 
  __IN  uint32_t ByteCnt
  )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  StorageAreaMapping_t *pCurStorageDesc;
  unsigned i;

  for(
      i=ARRAY_SIZE(StorageAreaTbl),
      pCurStorageDesc=StorageAreaTbl;
      i--;
      pCurStorageDesc++)
  {
      if(pCurStorageDesc->Id==Id)
      {
        EAPI_LIB_RETURN_ERROR_IF(
          EApiStorageAreaReadEmul                         , 
          (Offset+ByteCnt)>pCurStorageDesc->StorageSize  , 
          EAPI_STATUS_INVALID_BLOCK_LENGTH                , 
          "Read Len extends beyond End of Storage Area"
          );
        
#if 0
        memcpy(pvBuffer, pCurStorageDesc->pu8Buffer+Offset, ByteCnt);
#else
        fseek(pCurStorageDesc->FStream, Offset, SEEK_SET);
        if(ByteCnt!=fread(pvBuffer, sizeof(uint8_t), ByteCnt, pCurStorageDesc->FStream)){
	}
#endif

        EAPI_LIB_RETURN_SUCCESS(EApiStorageAreaRead, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
      EApiStorageAreaReadEmul  , 
      EAPI_STATUS_UNSUPPORTED  , 
      "Unrecognised Storage ID"
      );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}

EApiStatus_t 
EApiStorageAreaWriteEmul(
    __IN EApiId_t Id, 
    __IN uint32_t Offset, 
    __IN void *pvBuffer, 
    __IN uint32_t ByteCnt
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  StorageAreaMapping_t *pCurStorageDesc;
  unsigned i;
  for(
      i=ARRAY_SIZE(StorageAreaTbl),
      pCurStorageDesc=StorageAreaTbl;
      i--;
      pCurStorageDesc++)
  {
      if(pCurStorageDesc->Id==Id)
      {
        EAPI_LIB_RETURN_ERROR_IF(
          EApiStorageAreaWriteEmul, 
          (ByteCnt%pCurStorageDesc->WBlockLength)       , 
          EAPI_STATUS_INVALID_BLOCK_ALIGNMENT, 
          "Write length Not Aligned"        
          );
        EAPI_LIB_RETURN_ERROR_IF(
          EApiStorageAreaWriteEmul, 
          (Offset%pCurStorageDesc->WBlockLength)        , 
          EAPI_STATUS_INVALID_BLOCK_ALIGNMENT, 
          "Write Base Address Not Aligned"        
          );
        EAPI_LIB_RETURN_ERROR_IF(
            EApiStorageAreaWriteEmul, 
            (Offset+ByteCnt)>pCurStorageDesc->StorageSize , 
            EAPI_STATUS_INVALID_BLOCK_LENGTH, 
            "Write Len extends beyond End of Storage Area"
            );

#if 0
        memcpy(pCurStorageDesc->pu8Buffer, pvBuffer+Offset, ByteCnt);
#else
        fseek(pCurStorageDesc->FStream, Offset, SEEK_SET);
        fwrite(pvBuffer, sizeof(uint8_t), ByteCnt, pCurStorageDesc->FStream);
#endif
        EAPI_LIB_RETURN_SUCCESS(EApiStorageAreaWriteEmul, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
      EApiStorageAreaWrite, 
      EAPI_STATUS_UNSUPPORTED  , 
      "Unrecognised Storage ID"
      );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}







EApiStatus_t 
OpenEepromFiles(void)
{
  StorageAreaMapping_t *pCurStorageDesc;
  unsigned i;
  for(
      i=ARRAY_SIZE(StorageAreaTbl),
      pCurStorageDesc=StorageAreaTbl;
      i--;
      pCurStorageDesc++)
  {
    pCurStorageDesc->FStream=fopen(pCurStorageDesc->cszFilename, "rb+");
    if(pCurStorageDesc->FStream==NULL){
      pCurStorageDesc->FStream=fopen(pCurStorageDesc->cszFilename, "wb+");
    }
    if(pCurStorageDesc->FStream!=NULL){
        fseek(pCurStorageDesc->FStream, 0, SEEK_END);
        if(ftell(pCurStorageDesc->FStream)<(signed)pCurStorageDesc->StorageSize){
          rewind(pCurStorageDesc->FStream);
          fwrite(pCurStorageDesc->pu8Buffer, sizeof(uint8_t), pCurStorageDesc->StorageSize, pCurStorageDesc->FStream);
      }
    }
  }
  return EAPI_STATUS_SUCCESS;
}
EApiStatus_t 
CloseEepromFiles(void)
{
  StorageAreaMapping_t *pCurStorageDesc;
  unsigned i;
  for(
      i=ARRAY_SIZE(StorageAreaTbl),
      pCurStorageDesc=StorageAreaTbl;
      i--;
      pCurStorageDesc++)
  {
    if(pCurStorageDesc->FStream!=NULL){
      fclose(pCurStorageDesc->FStream);
      pCurStorageDesc->FStream=NULL;
    }
  }
  return EAPI_STATUS_SUCCESS;
}

