#ifndef SEGA_PER_H
#define SEGA_PER_H

#include <sega_xpt.h>

#ifdef TODO
#endif

#define PER_DGT_A   (1 << 0)
#define PER_DGT_B   (1 << 1)
#define PER_DGT_C   (1 << 2)
#define PER_DGT_X   (1 << 3)
#define PER_DGT_Y   (1 << 4)
#define PER_DGT_Z   (1 << 5)
#define PER_DGT_TL  (1 << 6)
#define PER_DGT_TR  (1 << 7)
#define PER_DGT_U   (1 << 8)
#define PER_DGT_D   (1 << 9)
#define PER_DGT_L   (1 << 10)
#define PER_DGT_R   (1 << 11)
#define PER_DGT_S   (1 << 12)


#define PER_KD_SYS      0
#define PER_SIZE_DGT    2

typedef struct  {
    Uint8   cc;
    Uint8   ac;
    Uint16  ss;
    Uint32  sm;
    Uint8   stat;
} PerGetSys;

#define PER_SIZE_NCON_15    0x0f
#define PER_ID_DGT          0x00
#define PER_ID_ANL          0x10

#define PER_KD_PERTIM       2

typedef Uint8   PerMulId;
typedef Uint8   PerMulCon;

typedef struct  {
    PerMulId    id;
    PerMulCon   con;
}PerMulInfo; 

typedef void PerGetPer;

#if 0
Uint32 PER_LInit(Sint32, Sint32, Sint32, Uint8*, Uint8);
Uint32 PER_LGetPer(PerGetPer **, PerMulInfo **);
#else
#define PER_LInit(a, b, c, d, e) 0
#define PER_LGetPer(a, b) 0
#endif

#define PER_MSK_STEREO  (0x1 << 9) 

#define PER_GET_SYS() 0
#define PER_SMPC_SET_SM(ireg)

#define PER_MSK_LANGU   (0xf << 0)

#define PER_ENGLISH     0x0
#define PER_DEUTSCH     0x1
#define PER_FRANCAIS    0x2
#define PER_ESPNOL      0x3
#define PER_ITALIANO    0x4
#define PER_JAPAN       0x5

#define PER_GET_TIM()   0 // TODO GetTick

#endif
