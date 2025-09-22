#ifndef SEGA_MTH_H
#define SEGA_MTH_H

#include <sega_xpt.h>
#include <math.h>

typedef struct {
    Fixed32 x, y, z;
} MthXyz;

typedef struct {
    Fixed32 val[3][4];
} MthMatrix;

typedef struct MthMatrixTbl {
    Uint16 stackSize;
    MthMatrix* current;
    MthMatrix* stack;
} MthMatrixTbl;

typedef struct {
    Sint32 x, y;
} XyInt;

#ifndef TODO
#endif

#if 0
Uint32  MTH_GetRand(void);
Fixed32 MTH_Div(Fixed32 a, Fixed32 b);
Fixed32 MTH_Product(Fixed32* a, Fixed32* b);
Fixed32 MTH_Sin(Fixed32 degree);
Fixed32 MTH_Cos(Fixed32 degree);
Fixed32 MTH_Atan(Fixed32 y, Fixed32 x);
void    MTH_InitialMatrix(MthMatrixTbl *matrixTbl, Uint16 stackSize, MthMatrix *matrix);
void    MTH_ClearMatrix(MthMatrixTbl *matrixTbl);
void    MTH_PushMatrix(MthMatrixTbl *matrixTbl);
void    MTH_PopMatrix(MthMatrixTbl *matrixTbl);
void    MTH_RotateMatrixX(MthMatrixTbl *matrixTbl, Fixed32 xDegree);
void    MTH_RotateMatrixY(MthMatrixTbl *matrixTbl, Fixed32 yDegree);
void    MTH_RotateMatrixZ(MthMatrixTbl *matrixTbl, Fixed32 zDegree);
void    MTH_MoveMatrix(MthMatrixTbl *matrixTbl, Fixed32 x, Fixed32 y, Fixed32 z);
void    MTH_CoordTrans(MthMatrix *matrix, MthXyz *src, MthXyz *ans);
#else
#define MTH_GetRand()       4
#define MTH_Div(a,b)        0
#define MTH_Product(a,b)    0
#define MTH_Sin(degree)     0
#define MTH_Cos(degree)     0
#define MTH_Atan(y,x)       0
#define MTH_InitialMatrix(matrixTbl,stackSize,matrix) memset(matrix, 0, sizeof(matrix))
#define MTH_ClearMatrix(matrixTbl)
#define MTH_PushMatrix(matrixTbl)
#define MTH_PopMatrix(matrixTbl)
#define MTH_RotateMatrixX(matrixTbl,xDegree)
#define MTH_RotateMatrixY(matrixTbl,yDegree)
#define MTH_RotateMatrixZ(matrixTbl,zDegree)
#define MTH_MoveMatrix(matrixTbl,x,y,z) (matrixTbl)->stackSize = 0; (matrixTbl)->current = (matrixTbl)->stack = NULL
#define MTH_CoordTrans(matrix,src,ans) (ans)->x = (ans)->y = (ans)->z = 0
#endif

#endif
