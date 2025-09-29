#ifndef SEGA_MTH_H
#define SEGA_MTH_H

#include <sega_xpt.h>

typedef struct {
    fix32 x, y, z;
} MthXyz;

typedef struct {
    fix32 val[3][4];
} MthMatrix;

typedef struct MthMatrixTbl {
    uint16 stackSize;
    MthMatrix* current;
    MthMatrix* stack;
} MthMatrixTbl;

typedef struct {
    sint32 x, y;
} XyInt;

uint32  MTH_GetRand(void);
fix32   MTH_Mul(fix32 a, fix32 b);
fix32   MTH_Div(fix32 a, fix32 b);
fix32   MTH_Product(fix32 *a, fix32 *b);
fix32   MTH_Sin(fix32 degree);
fix32   MTH_Cos(fix32 degree);
fix32   MTH_Atan(fix32 y, fix32 x);
void    MTH_InitialMatrix(MthMatrixTbl *matrixTbl, uint16 stackSize, MthMatrix *matrix);
void    MTH_ClearMatrix(MthMatrixTbl *matrixTbl);
void    MTH_PushMatrix(MthMatrixTbl *matrixTbl);
void    MTH_PopMatrix(MthMatrixTbl *matrixTbl);
void    MTH_RotateMatrixX(MthMatrixTbl *matrixTbl, fix32 xDegree);
void    MTH_RotateMatrixY(MthMatrixTbl *matrixTbl, fix32 yDegree);
void    MTH_RotateMatrixZ(MthMatrixTbl *matrixTbl, fix32 zDegree);
void    MTH_MoveMatrix(MthMatrixTbl *matrixTbl, fix32 x, fix32 y, fix32 z);
void    MTH_CoordTrans(MthMatrix *matrix, MthXyz *src, MthXyz *ans);

#endif
