/*----------------------------------------------------------------------------
 *  V_Blank.c -- V-Blank割り込み処理内ルーチンサンプル
 *  Copyright(c) 1994 SEGA
 *  Written by K.M on 1994-05-16 Ver.1.00
 *  Updated by K.M on 1994-09-21 Ver.1.00
 *
 *  UsrVblankStart()	：V-Blank開始割り込み処理サンプル
 *  UsrVblankEnd()	：V-Blank終了割り込み処理サンプル
 *
 *----------------------------------------------------------------------------
 */

#include <machine.h>
#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_per.h>
#include "v_blank.h"

#define INPUTQSIZE 16 /* dont change this */
volatile PerDgtInfo inputQ[INPUTQSIZE];
volatile PerDgtInfo lastInputSample;
volatile char inputQHead,inputQTail;

volatile Uint16	PadData1  = 0x0000;
volatile Uint16	PadData1E = 0x0000;
volatile Uint16	PadData2  = 0x0000;
volatile Uint16	PadData2E = 0x0000;
volatile PerDgtInfo *Pad;
volatile Uint32 vtimer = 0, htimer = 0, secs = 0;

Uint32  PadWorkArea[7];

void UsrHblankIn(void)
{htimer++;
}

void   UsrVblankStart(void)
{SCL_VblankStart();
 vtimer++;
}

void initInput(void)
{inputQHead=0;
 inputQTail=0;
}

void flushInput(void)
{inputQTail=inputQHead;
}

void UsrVblankEnd(void)
{SCL_VblankEnd();
 PER_GetPer((PerGetPer **)&Pad);
 if (Pad)
    {lastInputSample=Pad[0];
     /* add input sample to Q if it is not already full */
     if (((inputQHead+1)&0xf)!=inputQTail)
	{inputQ[(int)inputQHead]=Pad[0];
	 inputQHead=(inputQHead+1)&0xf;
	}
    }
/* if( Pad != NULL )
    {PadData1   = Pad[0].data ^ 0xffff;
     PadData1E |= Pad[0].push ^ 0xffff;
     PadData2   = Pad[1].data ^ 0xffff;
     PadData2E |= Pad[1].push ^ 0xffff;
    }*/
}
