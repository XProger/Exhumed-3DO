#ifndef SEGA_MTH_H
#define SEGA_MTH_H

#include <sega_int.h>

typedef struct {
    Sint32 x, y, z, _padding;
} MthXyz;

typedef struct {
    Sint32 m00;
} MthMatrix;

typedef struct {
    Sint32 x, y;
} XyInt;

#ifndef TODO
#endif

#define MTH_Div(a,b)    ((a) / (b))
#define MTH_Atan(y,x)   (0)
#define MTH_GetRand()   42

#endif
