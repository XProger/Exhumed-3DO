#ifndef SEGA_INT_H
#define SEGA_INT_H

#include <sega_xpt.h>

#define INT_ChgMsk(a,b)

#define INT_MSK_ALL     0
#define INT_MSK_NULL    0
#define INT_MSK_DMA0    0
#define INT_MSK_SPR     0

#define INT_SCU_VBLK_IN     0x40
#define INT_SCU_VBLK_OUT    0x41
#define INT_SCU_HBLK_IN     0x42

#define INT_SetMsk(msk_bit)
#define INT_SetScuFunc(num, hdr)

#define set_imask(i)
#define get_imask() 0

#endif