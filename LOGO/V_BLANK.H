#ifndef __INCLUDEDv_blankh
#define __INCLUDEDv_blankh

#include <sega_per.h>

void SetVblank(void);

#define INPUTQSIZE 16 /* dont change this */
extern volatile PerDgtInfo inputQ[INPUTQSIZE];
extern volatile char inputQHead,inputQTail;
/* interrupt only changes QHead, main thread only changes
   QTail */
extern volatile PerDgtInfo lastInputSample;

void initInput(void);
void flushInput(void);

extern volatile Uint32 vtimer, htimer;

#define PAD_U   PER_DGT_U
#define PAD_D	PER_DGT_D
#define PAD_R	PER_DGT_R
#define PAD_L	PER_DGT_L
#define PAD_A	PER_DGT_A
#define PAD_B	PER_DGT_B
#define PAD_C	PER_DGT_C
#define PAD_S	PER_DGT_S
#define PAD_X	PER_DGT_X
#define PAD_Y	PER_DGT_Y
#define PAD_Z	PER_DGT_Z
#define PAD_RB	PER_DGT_TR
#define PAD_LB	PER_DGT_TL

#endif
