#<KHeader>
#+=========================================================================
#I  Project Name: EApiDK Embedded Application Development Kit
#+=========================================================================
#I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/linux/makefile $
#+=========================================================================
#I   Copyright: Copyright (c) 2002-2009, Kontron Embedded Modules GmbH
#I      Author: John Kearney,                  John.Kearney@kontron.com
#I
#I     License: All rights reserved. This program and the accompanying 
#I              materials are licensed and made available under the 
#I              terms and conditions of the BSD License which 
#I              accompanies this distribution. The full text of the 
#I              license may be found at 
#I              http://opensource.org/licenses/bsd-license.php
#I              
#I              THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "
#I              AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF 
#I              ANY KIND, EITHER EXPRESS OR IMPLIED.
#I
#I Description: Auto Created for makefile
#I
#+-------------------------------------------------------------------------
#I
#I  File Name            : makefile
#I  File Location        : lib\linux
#I  Last committed       : $Revision: 60 $
#I  Last changed by      : $Author: dethrophes $
#I  Last changed date    : $Date: 2010-02-28 21:53:20 +0100 (Sun, 28 Feb 2010) $
#I  ID                   : $Id: makefile 60 2010-02-28 20:53:20Z dethrophes $
#I
#+=========================================================================
#</KHeader>
ARCH = $(shell uname -s)
ifndef MACH
MACH = $(shell uname -m)
endif
ifeq ($(ARCH), HP-UX)
ARCH=HPUX
endif
# Note: for cross compilation
# This script does allow for CROSS Compilation
# to achieve this specify the following
# variables
# CFLAGS
# MACH
# INTERPRETER  (Not used for PE Format/Windows/Cygwin)
.SUFFIXES:
.SUFFIXES: .c .s .o .exe .H

# static library
AR = ar
RANLIB = ranlib
ARFLAGS = -ruv

BinFolder=./bin/$(ARCH)/$(MACH)/rel
DBinFolder=./bin/$(ARCH)/$(MACH)/dbg
BuildDir=./build/$(MACH)/rel
DBuildDir=./build/$(MACH)/dbg
PrgFldr=$(join $(shell pwd)/, ../lib)
OalFldr=.
EAPI_INC=../include/EApi.h

ifeq (,$(PREFIX))
	PREFIX=/usr/local
endif
LIBDIR=$(PREFIX)/lib
INCDIR=$(PREFIX)/include
Install_Inc=../include/*

GET_VAR_FROM_CFILE=$(shell grep "$(2)" $(1) |sed -rn "s/^\#\s*define\s+$2\s+([0-9]+).*/\1/p")
EAPI_VERSION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_VER)
EAPI_REVISION=$(call GET_VAR_FROM_CFILE,$(EAPI_INC),EAPI_REVISION)

LibName:=EApi
LibFName:=$(LibName).$(EAPI_VERSION).$(EAPI_REVISION).o
SymLinkName:=lib$(LibName).so
SoName:=lib$(LibName).so.$(EAPI_VERSION)
SoFName:=lib$(LibName).so.$(EAPI_VERSION).$(EAPI_REVISION)
StaticLibFName:=$(LibName).a

#DBGOPT:=-gxcoff3
#DBGOPT:=-gxcoff+
#DBGOPT:=-gcoff3
#DBGOPT:=-gdwarf-2 -O0
#DBGOPT:=-g3
#DBGOPT:=-gstabs
#DBGOPT:=-gstabs+
DBGOPT:=-g
RELOPT:=-O3
INCLUDS=-I../include/linux -I../lib/include -I../include
CFLAGS:=$(CFLAGS) -Wall -W -pedantic $(INCLUDS)
ifneq ($(OS), Windows_NT)
ifndef INTERPRETER
INTERPRETER:=$(shell readelf -l $(shell which ld)|grep "\[.*:.*\]"| sed -nr "s/\s*\[.*:\s+(.*)\]/\1/p")
endif
InterpFName:=interp.o Message.o
CFLAGS:=$(CFLAGS) -fvisibility=hidden -fPIC -Wl,-e,EApiMain -D EAPI_INTERPRETER=\"$(INTERPRETER)\"
else
InterpFName:=
endif

define FLD_BUILD_RULE
@mkdir -p $(@)
endef

define LIB_BUILD_RULE
$(LD) -o $(@) -r $^
endef

define STATIC_BUILD_RULE
$(AR) $(ARFLAGS) $(@) $^
$(RANLIB) $(@)
endef

define SHARED_BUILD_RULE
$(CC) $(CFLAGS) $(1) -v -shared -L. -o $(@) -Wl,-soname,$(SoName),-Map,$(basename $<).map  $^
objcopy --only-keep-debug  "$(@)" "$(@).debug" 
strip --strip-debug --strip-unneeded "$(@)"
objcopy --add-gnu-debuglink="$(@).debug" "$(@)"
endef

define C_BUILD_RULE
$(CC) $(CFLAGS) $(1) -x c -c -Wa,-a,-ad -o  $(@) $< > $(basename $(@)).lst
$(CC) $(CFLAGS) $(1) -x c -S -o  $(basename $(@)).s $<
endef

define CLEAN_BUILD_FOLDER
@rm -f $(1)/*.log
@rm -f $(1)/*.o
@rm -f $(1)/*.a
@rm -f $(1)/*
@rm -f $(1)/*.lst
@rm -f $(1)/*.map
@rm -f $(1)/*.exeendef
endef

define REMOVE_BIN_FILES
@rm -f $(1)/*
endef

OBJECTS=EApiEmulGPIO.o \
      EApiEmulI2C.o \
      EApiInitLib.o \
      EApiEmulBacklight.o \
      EApiEmulBoardInfo.o \
      EApiEmulStorage.o \
      EApiEmulWDT.o \
      EApiOsNIX.o \
      EApiEmulEeprom.o \
      EApiAPI.o DbgChk.o

DefaultTgt: buildall

buildall: clean quick

quick:$(BinFolder) \
      $(BuildDir) \
      $(DBinFolder) \
      $(DBuildDir) \
      $(DBinFolder)/$(StaticLibFName) \
      $(BinFolder)/$(StaticLibFName) \
      $(DBinFolder)/$(SoFName) \
      $(BinFolder)/$(SoFName)
	@echo SYSTEM ARCHITECTURE : $(ARCH)
	@echo CPU ARCHITECTURE .. : $(MACH)
	@echo Library Name ...... : $(LibName)
	@echo Shared Lib Name ... : $(SoName)
	@echo Static Lib Filename : $(StaticLibFName)
	@echo Shared Lib Filename : $(SoFName)
	@echo Interpreter ....... : $(INTERPRETER)

  

$(BinFolder):
	$(FLD_BUILD_RULE)

$(BuildDir):
	$(FLD_BUILD_RULE)

$(DBinFolder):
	$(FLD_BUILD_RULE)

$(DBuildDir):
	$(FLD_BUILD_RULE)

$(DBuildDir)/$(LibFName): $(addprefix $(DBuildDir)/,$(OBJECTS)) 
	$(call LIB_BUILD_RULE,$(DBGOPT))

$(BuildDir)/$(LibFName): $(addprefix $(BuildDir)/,$(OBJECTS))
	$(call LIB_BUILD_RULE,$(RELOPT))

$(DBinFolder)/$(StaticLibFName): $(addprefix $(DBuildDir)/,$(LibFName)) 
	$(call STATIC_BUILD_RULE,$(DBGOPT))

$(BinFolder)/$(StaticLibFName): $(addprefix $(BuildDir)/,$(LibFName))
	$(call STATIC_BUILD_RULE,$(RELOPT))

$(DBinFolder)/$(SoFName): $(addprefix $(DBuildDir)/,$(LibFName) $(InterpFName)) 
	$(call SHARED_BUILD_RULE,$(DBGOPT))

$(BinFolder)/$(SoFName): $(addprefix $(BuildDir)/,$(LibFName) $(InterpFName))
	$(call SHARED_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(OalFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(OalFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(PrgFldr)/%.c
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(PrgFldr)/%.c
	$(call C_BUILD_RULE,$(RELOPT))

$(DBuildDir)/%.o:$(PrgFldr)/%.cpp
	$(call C_BUILD_RULE,$(DBGOPT))

$(BuildDir)/%.o:$(PrgFldr)/%.cpp
	$(call C_BUILD_RULE,$(RELOPT))

$(LIBDIR)/$(SoFName): $(DBinFolder)/$(SoFName)
	$(info Default Install Directory is $(PREFIX). For changing please set\
		PREFIX and run again.)
	cp --force $< $(@)
	@chmod 755 $(@)
	@ln --force --symbolic $(SoFName) $(dir $(@))$(SoName)
	@ln --force --symbolic $(SoFName) $(dir $(@))$(SymLinkName)
	@ldconfig -n $(dir $(@))$(SoName)
	@mkdir -p $(INCDIR)
	cp -a $(Install_Inc) $(INCDIR)/

install: $(LIBDIR)/$(SoFName)


.PHONY: clean
.PHONY: help
.PHONY: uninstall

uninstall::
	@rm -f $(LIBDIR)/$(SymLinkName)
	@rm -f $(LIBDIR)/$(SoFName)
	@rm -f $(LIBDIR)/$(SoName)
	@rm -f $(INCDIR)/EApi*

clean::
	$(call REMOVE_BIN_FILES,$(DBinFolder))
	$(call REMOVE_BIN_FILES,$(BinFolder))
	$(call CLEAN_BUILD_FOLDER,$(DBuildDir))
	$(call CLEAN_BUILD_FOLDER,$(BuildDir))


help::
	$(info ###############################################################################)
	$(info #        Build Make Files)
	$(info ###############################################################################)
	$(info # help)
	$(info # clean)
	$(info # quick)
	$(info # buildall)
	$(info #)

