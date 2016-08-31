/*
 *<KHeader>
 *+=========================================================================
 *I  Project Name: EApiDK Embedded Application Development Kit
 *+=========================================================================
 *I  $HeadURL: http://svn.code.sf.net/p/eapidk/code/trunk/lib/linux/EApiOsNIX.c $
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
 *I Description: Auto Created for EApiOsNIX.c
 *I
 *+-------------------------------------------------------------------------
 *I
 *I  File Name            : EApiOsNIX.c
 *I  File Location        : lib\linux
 *I  Last committed       : $Revision: 38 $
 *I  Last changed by      : $Author: dethrophes $
 *I  Last changed date    : $Date: 2010-01-26 07:48:22 +0100 (Tue, 26 Jan 2010) $
 *I  ID                   : $Id: EApiOsNIX.c 38 2010-01-26 06:48:22Z dethrophes $
 *I
 *+=========================================================================
 *</KHeader>
 */
  #include <EApiLib.h>
  #include <stdio.h>
  #include <time.h>
  


static void init (void) __attribute__ ((constructor));
static void fini (void) __attribute__ ((destructor));
void init (void)
{ 
  printf("initializing\n");
	return ;
}

void fini (void)
{ 
  printf("finishing\n");
	return ;
}


void EApiSleepns(unsigned long ns)  
{ 
#if _POSIX_C_SOURCE >= 199309L
  struct timespec Request;
  struct timespec Remain;
  Request.tv_sec=ns/1000000000;
  Request.tv_nsec=ns%1000000000;
  nanosleep(&Request, &Remain);
#else
  usleep(ns/1000000);
#endif
  return ;
}


