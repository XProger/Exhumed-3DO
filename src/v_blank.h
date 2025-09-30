#ifndef __INCLUDEDv_blankh
#define __INCLUDEDv_blankh

#include "app.h"

void SetVblank(void);

#define INPUTQSIZE 16 /* dont change this */
extern uint16 inputQ[INPUTQSIZE];
extern sint8 inputQHead;
/* interrupt only changes QHead, main thread only changes
   QTail */
extern sint8 controlerPresent;
extern uint16 lastInputSample;
extern uint16 inputAccum;
extern sint32 fadeDir, fadePos, fadeEnd;
extern uint8 PadWorkArea[];
extern sint8 analogControlerPresent;
extern sint16 analogX;
extern sint16 analogY;
extern sint8 analogIndexButtonsPresent;
extern sint16 analogTR;
extern sint16 analogTL;
extern sint8 enableDoorReset;
void initInput(void);

extern uint32 vtimer, htimer;

#ifndef NDEBUG
extern sint32 errorQ[];
extern sint32 prQ[];
#endif

#if 0
#define PAD_U PER_DGT_U
#define PAD_D PER_DGT_D
#define PAD_R PER_DGT_R
#define PAD_L PER_DGT_L
#define PAD_A PER_DGT_A
#define PAD_B PER_DGT_B
#define PAD_C PER_DGT_C
#define PAD_S PER_DGT_S
#define PAD_X PER_DGT_X
#define PAD_Y PER_DGT_Y
#define PAD_Z PER_DGT_Z
#define PAD_RB PER_DGT_TR
#define PAD_LB PER_DGT_TL

/*-----------keys----------------*/
#define KJUMP PER_DGT_B
#define KFIRE PER_DGT_A
#define KRUN PER_DGT_TR
#define KSTRAFE PER_DGT_TL
#define KPUSH PER_DGT_C
#define KPITCH PER_DGT_X
#define KWEAPDOWN PER_DGT_Y
#define KWEAPUP PER_DGT_Z
#define KFLYUP PER_DGT_A
#define KUP PER_DGT_U
#define KDOWN PER_DGT_D
#define KLEFT PER_DGT_L
#define KRIGHT PER_DGT_R
#endif

#endif
