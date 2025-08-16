/***************************************************************************
 SONYFIX.H - Header file for prototyping library functions which Sony
             somehow forgot

 Author:  Daniel Berke
 Date:    4/20/95

 $Header: /PSX_CMN/PSX_CMN_H/SONYFIX.H 8     2/20/96 4:32p Borden $

 Modification History:
 $Log: /PSX_CMN/PSX_CMN_H/SONYFIX.H $
 
 8     2/20/96 4:32p Borden
 removed prototypes that appear to be included in version 3.4
 
 7     2/08/96 9:40a Joe
 added extern "C"
 
 6     8/21/95 9:53a Borden
 removed cdInit prototype, it is in libcd.h
 
 5     8/11/95 4:41p Joe
 Added prototype GsSetFarClip
 
 4     8/10/95 6:31p Joe
 Added protoype for DecDCTinCallback (spelling is different than Sony
 docs)
 
 3     6/14/95 10:00a Dan
 Modified prototypes for lib 3.1

 2     6/07/95 10:16a Dan
 Fixed headers

 1     6/06/95 6:19p Dan
 Header file with prototypes the morons at Sony forgot

 (C) Copyright 1995 Visual Integration, Inc.
	 All Rights Reserved
	 This document is proprietary and confidential
***************************************************************************/
#ifndef SONYFIX_H_INCLUDED
#define SONYFIX_H_INCLUDED

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "psx_gfx.h"

/* Task/TCB functions */
u_long OpenTh(u_long (*func)(), u_long sp, u_long gp);
u_long CloseTh(u_long thread);
u_long ChangeTh(u_long thread);
void EnterCriticalSection(void);
void ExitCriticalSection(void);
void GetConf(u_long *num_ev, u_long *num_tcbs, u_long *sp);
void SetConf(u_long num_ev, u_long num_tcbs, u_long sp);


/* System functions */
void SystemError(char c, u_long n);
//long PadInit(u_long mode);
//long PadRead(u_short id);
//long PadStop(void);
//void StopPAD(void);
long EnableEvent(u_long event);
long OpenEvent(u_long desc, long spec, long mode, long (*func)());
long SetRCnt(u_long spec, u_short target, long mode);
long GetRCnt(u_long spec);
long ResetRCnt(u_long spec);
long StartRCnt(u_long spec);
//void ResetCallback(void);
//void StopCallback(void);

/* Basic C library functions */
//int printf(char *format, ...);

/* Graphics functions */
//unsigned int VSync(int mode);
//void VSyncCallback(void (*func)()); /* does this have 1 or 2 params?? */
void GsInit3D(void);
void GsSetLsMatrix(MATRIX *mp);
void GsSetLightMode(int mode);
void GsSetNearClip(long clip_near);
void GsSetFarClip(long clip_far);
//int FntPrint(char *fmt, ...);

/* Data Processing functions */
long DecDCToutCallback(long (*func)());
long DecDCTinCallback(long (*func)());

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif

