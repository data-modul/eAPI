/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/EApiEmulGPIO.c $
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
 *I Description: Auto Created for EApiEmulGPIO.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiEmulGPIO.c
 *I  File Location        : lib
 *I  Last committed       : $Revision: 74 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-06-23 21:26:50 +0200 (Wed, 23 Jun 2010) $
 *I  ID                   : $Id: EApiEmulGPIO.c 74 2010-06-23 19:26:50Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */

#include <EApiLib.h>



#define USE_DIRECTION_EMUL 1
#define END_OF_LIST_MARK ((uint32_t)-1)
/*
 *
 *  
 *
 *      PSEUDO Hardware Emulation
 *
 *
 *
 */
/* GPIO Port Bits
 * To emulate a loop back connector set the GPIO's to the same bits
 * I.E.
 * GPI0_PORT_BIT 23
 * GPI1_PORT_BIT 23
 * GPO0_PORT_BIT 23
 * means GPO0 drives both GPI0 and GPI1
 */
#define GPI0_PORT_BIT 23
#define GPI1_PORT_BIT 15
#define GPI2_PORT_BIT 13
#define GPI3_PORT_BIT 17
#define GPO0_PORT_BIT 4
#define GPO1_PORT_BIT 6
#define GPO2_PORT_BIT 31
#define GPO3_PORT_BIT 0
typedef uint32_t ReadPortFunction (uint32_t);
typedef void     WritePortFunction (uint32_t, uint32_t);

///* Psuedo IO Ports & Defaults */
#define GPIO_LEVEL_DFLT ((EAPI_GPIO_HIGH<<GPI0_PORT_BIT)| (EAPI_GPIO_LOW <<GPI1_PORT_BIT)|\
                         (EAPI_GPIO_LOW <<GPI2_PORT_BIT)| (EAPI_GPIO_HIGH<<GPI3_PORT_BIT)|\
                         (EAPI_GPIO_LOW <<GPO0_PORT_BIT)| (EAPI_GPIO_LOW <<GPO1_PORT_BIT)|\
                         (EAPI_GPIO_LOW <<GPO2_PORT_BIT)| (EAPI_GPIO_LOW <<GPO3_PORT_BIT) \
    )

#define GPIO_DIR_DFLT   ((EAPI_GPIO_INPUT <<GPI0_PORT_BIT)| (EAPI_GPIO_INPUT <<GPI1_PORT_BIT)|\
                         (EAPI_GPIO_INPUT <<GPI2_PORT_BIT)| (EAPI_GPIO_INPUT <<GPI3_PORT_BIT)|\
                         (EAPI_GPIO_OUTPUT<<GPO0_PORT_BIT)| (EAPI_GPIO_OUTPUT<<GPO1_PORT_BIT)|\
                         (EAPI_GPIO_OUTPUT<<GPO2_PORT_BIT)| (EAPI_GPIO_OUTPUT<<GPO3_PORT_BIT) \
    )

#define GET_U8(BitOffset,Value)  ((uint8_t )(((Value)>>(BitOffset))&0xFF))
#define GET_U16(BitOffset,Value) ((uint16_t)(((Value)>>(BitOffset))&0xFFFF))
#define GET_U32(BitOffset,Value) ((uint32_t )(((Value)>>(BitOffset))&0xFFFFFFFF))
#define BIT(x) ((uint32_t)(1<<(x)))

static uint32_t EmulatedIoBlock[]={
#define EAPI_EMUL_GPIO_DIRECTION_REG 0
  GPIO_DIR_DFLT,
#define EAPI_EMUL_GPIO_LEVEL_REG     4
  GPIO_LEVEL_DFLT,
#define EAPI_EMUL_GPIO_DIRECTION_REG2 8
  GPIO_DIR_DFLT ,
#define EAPI_EMUL_GPIO_LEVEL_REG2     12
  GPIO_LEVEL_DFLT,
};

WritePortFunction EApiWriteIO32Delay;
ReadPortFunction  EApiReadIO32Delay;

uint32_t EApiReadIO32(uint32_t ByteOffset)
{
  if(sizeof(EmulatedIoBlock)<ByteOffset+4)
  {
      return 0;
  }
  return *(uint32_t *)&((uint8_t*)EmulatedIoBlock)[ByteOffset];
}

void EApiWriteIO32(uint32_t ByteOffset, uint32_t Data)
{
  if(sizeof(EmulatedIoBlock)<ByteOffset+4)
  {
      return ;
  }
  *(uint32_t *)&((uint8_t*)&EmulatedIoBlock)[ByteOffset]=Data;
  return ;
}
void EApiWriteIO32Delay(uint32_t ByteOffset, uint32_t Data)
{
  EAPI_EMUL_DELAY_NS(1400);
  EApiWriteIO32(ByteOffset, Data);
  return ;
}
uint32_t EApiReadIO32Delay(uint32_t ByteOffset)
{
  EAPI_EMUL_DELAY_NS(1400);
  return EApiReadIO32(ByteOffset);
}
/*
 *
 *  
 *
 *  G P I O
 *
 *
 *
 */
typedef struct PortDescriptor_s{
    uint32_t Dirty;
    uint32_t Value;
#if USE_DIRECTION_EMUL
    uint32_t Value2;
#endif
    uint32_t LevelArg1;
    uint32_t DirectionArg1;
    WritePortFunction *Write;
    ReadPortFunction *Read;
}PortDescriptor_t;
typedef struct GPIOMappingsTbl_s{
  uint32_t PortMask; /* Bitmask For Port (Arbitrary Values Used Here) */
  PortDescriptor_t *Port;
}GPIOMappingsTbl_t;
PortDescriptor_t Port1Desc={
  0x00, 0,
#if USE_DIRECTION_EMUL
   0,
#endif
  EAPI_EMUL_GPIO_LEVEL_REG    ,
  EAPI_EMUL_GPIO_DIRECTION_REG,
  &EApiWriteIO32Delay,
  &EApiReadIO32Delay,
};
PortDescriptor_t Port2Desc={
  0x00, 0,
#if USE_DIRECTION_EMUL
   0,
#endif
  EAPI_EMUL_GPIO_LEVEL_REG2    ,
  EAPI_EMUL_GPIO_DIRECTION_REG2,
  &EApiWriteIO32Delay,
  &EApiReadIO32Delay,
};
const GPIOMappingsTbl_t GpioMappingID0[]={
#if (GPI0_PORT_BIT>31)
  {BIT(GPI0_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI0_PORT_BIT) , &Port1Desc},
#endif
#if (GPI1_PORT_BIT>31)
  {BIT(GPI1_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI1_PORT_BIT) , &Port1Desc},
#endif
#if (GPI2_PORT_BIT>31)
  {BIT(GPI2_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI2_PORT_BIT) , &Port1Desc},
#endif
#if (GPI3_PORT_BIT>31)
  {BIT(GPI3_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI3_PORT_BIT) , &Port1Desc},
#endif
#if (GPO0_PORT_BIT>31)
  {BIT(GPO0_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO0_PORT_BIT) , &Port1Desc},
#endif
#if (GPO1_PORT_BIT>31)
  {BIT(GPO1_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO1_PORT_BIT) , &Port1Desc},
#endif
#if (GPO2_PORT_BIT>31)
  {BIT(GPO2_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO2_PORT_BIT) , &Port1Desc},
#endif
#if (GPO3_PORT_BIT>31)
  {BIT(GPO3_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO3_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID1[]={
#if (GPI0_PORT_BIT>31)
  {BIT(GPI0_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI0_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID2[]={
#if (GPI1_PORT_BIT>31)
  {BIT(GPI1_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI1_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID3[]={
#if (GPI2_PORT_BIT>31)
  {BIT(GPI2_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI2_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID4[]={
#if (GPI3_PORT_BIT>31)
  {BIT(GPI3_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPI3_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID5[]={
#if (GPO0_PORT_BIT>31)
  {BIT(GPO0_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO0_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID6[]={
#if (GPO1_PORT_BIT>31)
  {BIT(GPO1_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO1_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID7[]={
#if (GPO2_PORT_BIT>31)
  {BIT(GPO2_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO2_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
const GPIOMappingsTbl_t GpioMappingID8[]={
#if (GPO3_PORT_BIT>31)
  {BIT(GPO3_PORT_BIT - 32) , &Port2Desc},
#else
  {BIT(GPO3_PORT_BIT) , &Port1Desc},
#endif
  {END_OF_LIST_MARK, NULL}
};
typedef struct GPIOIDMappingsTbl_s{
  const EApiId_t Id          ; /* EAPI Temperature Id */
  const GPIOMappingsTbl_t *const MapTbl   ; /* Gpio Mapping Table API Interface Bitmask -> Port Bitmask */
  const uint32_t OutputMask  ; /* Bitmask Supported Bits For Output  */
  const uint32_t InputMask   ; /* Bitmask Supported Bits for Input */
}GPIOIDMappingsTbl_t;
const GPIOIDMappingsTbl_t GPIOIDMapping[]={
#if (EAPI_PLATFORM==EAPI_PLATFORM_COM0)
  {EAPI_COM0_ID_GPIO_BANK   , GpioMappingID0, 0x000000F0, 0x0000000F},
  {EAPI_COM0_ID_GPIO_GPO0   , GpioMappingID1, 0x00000000, 0x00000001},
  {EAPI_COM0_ID_GPIO_GPO1   , GpioMappingID2, 0x00000000, 0x00000001},
  {EAPI_COM0_ID_GPIO_GPO2   , GpioMappingID3, 0x00000000, 0x00000001},
  {EAPI_COM0_ID_GPIO_GPO3   , GpioMappingID4, 0x00000000, 0x00000001},
  {EAPI_COM0_ID_GPIO_GPI0   , GpioMappingID5, 0x00000001, 0x00000000},
  {EAPI_COM0_ID_GPIO_GPI1   , GpioMappingID6, 0x00000001, 0x00000000},
  {EAPI_COM0_ID_GPIO_GPI2   , GpioMappingID7, 0x00000001, 0x00000000},
  {EAPI_COM0_ID_GPIO_GPI3   , GpioMappingID8, 0x00000001, 0x00000000},
#elif (EAPI_PLATFORM==EAPI_PLATFORM_ETX) || (EAPI_PLATFORM==EAPI_PLATFORM_Q7)
  {0xFFFFFFFF              , NULL          ,          0,          0},
#else
  {0xFFFFFFFF              , NULL          ,          0,          0},
#endif
};

EApiStatus_t 
EApiGPIOGetLevelEmul( 
    __IN  EApiId_t  Id      , 
    __IN  uint32_t  BitMask , 
    __OUT uint32_t *pLevel
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i,i2;
  uint32_t APIBitMask=1;

  for(i=0;i<ARRAY_SIZE(GPIOIDMapping);i++)
  {
      if(GPIOIDMapping[i].Id==Id)
      {
        EAPI_LIB_RETURN_ERROR_IF(
          EApiGPIOGetLevelEmul,
          (BitMask&~(GPIOIDMapping[i].OutputMask|GPIOIDMapping[i].InputMask)) ,
          EAPI_STATUS_INVALID_BITMASK,
          "Bit-mask Selects Invalid Bits"
          );
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          GPIOIDMapping[i].MapTbl[i2].Port->Dirty=0;
        }
        *pLevel=0;
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          if((BitMask&APIBitMask))
          {
            if(GPIOIDMapping[i].MapTbl[i2].Port->Dirty==0){
              GPIOIDMapping[i].MapTbl[i2].Port->Value=GPIOIDMapping[i].MapTbl[i2].Port->Read(
                GPIOIDMapping[i].MapTbl[i2].Port->LevelArg1
                );
              GPIOIDMapping[i].MapTbl[i2].Port->Dirty=1;
            }
            if(GPIOIDMapping[i].MapTbl[i2].Port->Value&GPIOIDMapping[i].MapTbl[i2].PortMask){
              *pLevel|=APIBitMask;
            }
          }
          APIBitMask<<=1;
        }
        EAPI_LIB_RETURN_SUCCESS(EApiGPIOGetLevel, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiGPIOGetLevel,
    EAPI_STATUS_UNSUPPORTED  ,
    "Unrecognised GPIO ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}

EApiStatus_t 
EApiGPIOSetLevelEmul( 
  __IN EApiId_t Id      , 
  __IN uint32_t BitMask , 
  __IN uint32_t Level
  )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i,i2;
  uint32_t APIBitMask=1;
  for(i=0;i<ARRAY_SIZE(GPIOIDMapping);i++)
  {
      if(GPIOIDMapping[i].Id==Id)
      {
        EAPI_LIB_RETURN_ERROR_IF(
          EApiGPIOSetLevelEmul,
          (BitMask&~(GPIOIDMapping[i].OutputMask|GPIOIDMapping[i].InputMask)) ,
          EAPI_STATUS_INVALID_BITMASK,
          "Bit-mask Selects Invalid Bits"
          );
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          GPIOIDMapping[i].MapTbl[i2].Port->Dirty=0;
        }
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++)
        {
          if((BitMask&APIBitMask))
          {
            if(GPIOIDMapping[i].MapTbl[i2].Port->Dirty==0){
              GPIOIDMapping[i].MapTbl[i2].Port->Value=GPIOIDMapping[i].MapTbl[i2].Port->Read(
                GPIOIDMapping[i].MapTbl[i2].Port->LevelArg1
                );
#if USE_DIRECTION_EMUL
              GPIOIDMapping[i].MapTbl[i2].Port->Value2=GPIOIDMapping[i].MapTbl[i2].Port->Read(
                GPIOIDMapping[i].MapTbl[i2].Port->DirectionArg1
                );
#endif
              GPIOIDMapping[i].MapTbl[i2].Port->Dirty=1;
            }
#if USE_DIRECTION_EMUL
            EAPI_LIB_RETURN_ERROR_IF(
              EApiGPIOSetLevel,
              (GPIOIDMapping[i].MapTbl[i2].Port->Value2&GPIOIDMapping[i].MapTbl[i2].PortMask) ,
              EAPI_STATUS_INVALID_PARAMETER,
              "Cant Set Level on Pin that is set to input"
              );
#endif
            if(Level&APIBitMask)
              GPIOIDMapping[i].MapTbl[i2].Port->Value|=GPIOIDMapping[i].MapTbl[i2].PortMask;
            else
              GPIOIDMapping[i].MapTbl[i2].Port->Value&=~GPIOIDMapping[i].MapTbl[i2].PortMask;
          }
          APIBitMask<<=1;
        }
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          if(GPIOIDMapping[i].MapTbl[i2].Port->Dirty==1){
            GPIOIDMapping[i].MapTbl[i2].Port->Write(
              GPIOIDMapping[i].MapTbl[i2].Port->LevelArg1,
              GPIOIDMapping[i].MapTbl[i2].Port->Value
              );
            GPIOIDMapping[i].MapTbl[i2].Port->Dirty=0;
          }
        }
        EAPI_LIB_RETURN_SUCCESS(EApiGPIOSetLevel, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiGPIOSetLevel,
    EAPI_STATUS_UNSUPPORTED,
    "Unrecognised GPIO ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}

EApiStatus_t 
EApiGPIOGetDirectionEmul( 
  __IN  EApiId_t  Id        ,
  __IN  uint32_t  BitMask   ,
  __OUT uint32_t *pDirection
  )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i,i2;
  uint32_t APIBitMask=1;

  for(i=0;i<ARRAY_SIZE(GPIOIDMapping);i++)
  {
      if(GPIOIDMapping[i].Id==Id)
      {
        EAPI_LIB_RETURN_ERROR_IF(
          EApiGPIOGetDirectionEmul,
          (BitMask&~(GPIOIDMapping[i].OutputMask|GPIOIDMapping[i].InputMask)) ,
          EAPI_STATUS_INVALID_BITMASK,
          "Bit-mask Selects Invalid Bits"
          );
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          GPIOIDMapping[i].MapTbl[i2].Port->Dirty=0;
        }
        *pDirection=0;
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          if((BitMask&APIBitMask))
          {
            if(GPIOIDMapping[i].MapTbl[i2].Port->Dirty==0){
              GPIOIDMapping[i].MapTbl[i2].Port->Value=GPIOIDMapping[i].MapTbl[i2].Port->Read(
                GPIOIDMapping[i].MapTbl[i2].Port->DirectionArg1
                );
              GPIOIDMapping[i].MapTbl[i2].Port->Dirty=1;
            }
            if(GPIOIDMapping[i].MapTbl[i2].Port->Value&GPIOIDMapping[i].MapTbl[i2].PortMask)
              *pDirection|=APIBitMask;
          }
          APIBitMask<<=1;
        }
        EAPI_LIB_RETURN_SUCCESS(EApiGPIOGetDirection, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiGPIOGetDirection,
    EAPI_STATUS_UNSUPPORTED  ,
    "Unrecognised GPIO ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}

EApiStatus_t 
EApiGPIOSetDirectionEmul( 
    __IN EApiId_t Id        , 
    __IN uint32_t BitMask   , 
    __IN uint32_t Direction
    )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i,i2;
  uint32_t APIBitMask=1;
  for(i=0;i<ARRAY_SIZE(GPIOIDMapping);i++)
  {
      if(GPIOIDMapping[i].Id==Id)
      {
        EAPI_LIB_RETURN_ERROR_IF(
          EApiGPIOSetDirectionEmul,
          (BitMask&~(GPIOIDMapping[i].OutputMask|GPIOIDMapping[i].InputMask)) ,
          EAPI_STATUS_INVALID_BITMASK,
          "Bit-mask Selects Invalid Bits"
          );
        EAPI_LIB_RETURN_ERROR_IF(
          EApiGPIOSetDirectionEmul,
          (BitMask&Direction&~GPIOIDMapping[i].InputMask) ,
          EAPI_STATUS_INVALID_DIRECTION,
          "Unsupported Input Bits"
          );
        EAPI_LIB_RETURN_ERROR_IF(
          EApiGPIOSetDirectionEmul,
          ((BitMask&~Direction)&~GPIOIDMapping[i].OutputMask) ,
          EAPI_STATUS_INVALID_DIRECTION,
          "Unsupported Output Bits"
          );
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          GPIOIDMapping[i].MapTbl[i2].Port->Dirty=0;
        }
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++)
        {
          if((BitMask&APIBitMask))
          {
            if(GPIOIDMapping[i].MapTbl[i2].Port->Dirty==0){
              GPIOIDMapping[i].MapTbl[i2].Port->Value=GPIOIDMapping[i].MapTbl[i2].Port->Read(
                GPIOIDMapping[i].MapTbl[i2].Port->DirectionArg1
                );
              GPIOIDMapping[i].MapTbl[i2].Port->Dirty=1;
            }
            if(Direction&APIBitMask)
            {
              GPIOIDMapping[i].MapTbl[i2].Port->Value|=GPIOIDMapping[i].MapTbl[i2].PortMask;
            }
            else
            {
              GPIOIDMapping[i].MapTbl[i2].Port->Value&=~GPIOIDMapping[i].MapTbl[i2].PortMask;
            }
          }
          APIBitMask<<=1;
        }
        for(i2=0;GPIOIDMapping[i].MapTbl[i2].PortMask!=END_OF_LIST_MARK;i2++){
          if(GPIOIDMapping[i].MapTbl[i2].Port->Dirty==1){
            GPIOIDMapping[i].MapTbl[i2].Port->Write(
              GPIOIDMapping[i].MapTbl[i2].Port->DirectionArg1,
              GPIOIDMapping[i].MapTbl[i2].Port->Value
              );
            GPIOIDMapping[i].MapTbl[i2].Port->Dirty=0;
          }
        }
        EAPI_LIB_RETURN_SUCCESS(EApiGPIOSetDirection, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiGPIOSetDirection,
    EAPI_STATUS_UNSUPPORTED,
    "Unrecognised GPIO ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}

EApiStatus_t 
EApiGPIOGetDirectionCapsEmul( 
  __IN      EApiId_t  Id      , 
  __OUTOPT  uint32_t *pInputs , 
  __OUTOPT  uint32_t *pOutputs
  )
{
  EApiStatus_t StatusCode=EAPI_STATUS_SUCCESS;
  unsigned i;

  for(i=0;i<ARRAY_SIZE(GPIOIDMapping);i++)
  {
      if(GPIOIDMapping[i].Id==Id)
      {
        *pInputs=GPIOIDMapping[i].InputMask;
        *pOutputs=GPIOIDMapping[i].OutputMask;
        EAPI_LIB_RETURN_SUCCESS(EApiGPIOGetDirectionCaps, "");
      }
  }
  EAPI_LIB_RETURN_ERROR(
    EApiGPIOGetDirectionCaps,
    EAPI_STATUS_UNSUPPORTED  ,
    "Unrecognised GPIO ID"
    );
EAPI_LIB_ASSERT_EXIT

  return StatusCode;
}


