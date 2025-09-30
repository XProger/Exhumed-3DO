#include "app.h"
#include "mth.h"

#define PI          3.14159265358979323846f
#define DEG2RAD     (PI / 180.0f)
#define RAD2DEG     (180.0f / PI)

enum {
    M00, M01, M02, M03,
    M10, M11, M12, M13,
    M20, M21, M22, M23
};

static uint32 seed;

uint32 MTH_GetRand(void)
{
    return (seed = seed * 5 + 0x3573);
}

fix32 MTH_Mul(fix32 a, fix32 b)
{
    return (fix32)((long long)a * b >> 16);
}

fix32 MTH_Div(fix32 a, fix32 b)
{
    return (fix32)(((long long)a << 16) / b);
}

fix32 MTH_Product(fix32 *a, fix32 *b)
{
    return MTH_Mul(a[0], b[0]) + MTH_Mul(a[1], b[1]) + MTH_Mul(a[2], b[2]);
}

fix32 MTH_Sin(fix32 degree) // TODO tables
{
    return (fix32)(sinf(degree * (DEG2RAD / 65536.0f)) * 65536.0f);
}

fix32 MTH_Cos(fix32 degree) // TODO tables
{
    return (fix32)(cosf(degree * (DEG2RAD / 65536.0f)) * 65536.0f);
}

fix32 MTH_Atan(fix32 y, fix32 x) // TODO tables
{
    return (fix32)(atan2f(y / 65536.0f, x / 65536.0f) * RAD2DEG * 65536.0f);
}

void MTH_InitialMatrix(MthMatrixTbl *matrixTbl, uint16 stackSize, MthMatrix *matrix)
{
    matrixTbl->stackSize = stackSize;
    matrixTbl->current = matrix;
    matrixTbl->stack = matrix;
}

void MTH_ClearMatrix(MthMatrixTbl *matrixTbl)
{
    fix32* m = (fix32*)matrixTbl->current;

    memset(m, 0, sizeof(MthMatrix));
    m[M00] = 1 << 16;
    m[M11] = 1 << 16;
    m[M22] = 1 << 16;
}

void MTH_PushMatrix(MthMatrixTbl *matrixTbl)
{
    MthMatrix *next = matrixTbl->current + 1;

    memcpy(next, matrixTbl->current, sizeof(MthMatrix));
    matrixTbl->current = next;
}

void MTH_PopMatrix(MthMatrixTbl *matrixTbl)
{
    matrixTbl->current--;
}

void MTH_RotateMatrixX(MthMatrixTbl *matrixTbl, fix32 xDegree)
{
    fix32 s, c, r0, r1;
    fix32* m = (fix32*)matrixTbl->current;

    s = MTH_Sin(xDegree);
    c = MTH_Cos(xDegree);

    r0 = MTH_Mul(m[M01], c) + MTH_Mul(m[M02], s);
    r1 = MTH_Mul(m[M02], c) - MTH_Mul(m[M01], s);
    m[M01] = r0;
    m[M02] = r1;

    r0 = MTH_Mul(m[M11], c) + MTH_Mul(m[M12], s);
    r1 = MTH_Mul(m[M12], c) - MTH_Mul(m[M11], s);
    m[M11] = r0;
    m[M12] = r1;

    r0 = MTH_Mul(m[M21], c) + MTH_Mul(m[M22], s);
    r1 = MTH_Mul(m[M22], c) - MTH_Mul(m[M21], s);
    m[M21] = r0;
    m[M22] = r1;
}

void MTH_RotateMatrixY(MthMatrixTbl *matrixTbl, fix32 yDegree)
{
    fix32 s, c, r0, r1;
    fix32* m = (fix32*)matrixTbl->current;

    s = MTH_Sin(yDegree);
    c = MTH_Cos(yDegree);

    r0 = MTH_Mul(m[M00], c) - MTH_Mul(m[M02], s);
    r1 = MTH_Mul(m[M02], c) + MTH_Mul(m[M00], s);
    m[M00] = r0;
    m[M02] = r1;

    r0 = MTH_Mul(m[M10], c) - MTH_Mul(m[M12], s);
    r1 = MTH_Mul(m[M12], c) + MTH_Mul(m[M10], s);
    m[M10] = r0;
    m[M12] = r1;

    r0 = MTH_Mul(m[M20], c) - MTH_Mul(m[M22], s);
    r1 = MTH_Mul(m[M22], c) + MTH_Mul(m[M20], s);
    m[M20] = r0;
    m[M22] = r1;
}

void MTH_RotateMatrixZ(MthMatrixTbl *matrixTbl, fix32 zDegree)
{
    fix32 s, c, r0, r1;
    fix32* m = (fix32*)matrixTbl->current;

    s = MTH_Sin(zDegree);
    c = MTH_Cos(zDegree);

    r0 = MTH_Mul(m[M00], c) + MTH_Mul(m[M01], s);
    r1 = MTH_Mul(m[M01], c) - MTH_Mul(m[M00], s);
    m[M00] = r0;
    m[M01] = r1;

    r0 = MTH_Mul(m[M10], c) + MTH_Mul(m[M11], s);
    r1 = MTH_Mul(m[M11], c) - MTH_Mul(m[M10], s);
    m[M10] = r0;
    m[M11] = r1;

    r0 = MTH_Mul(m[M20], c) + MTH_Mul(m[M21], s);
    r1 = MTH_Mul(m[M21], c) - MTH_Mul(m[M20], s);
    m[M20] = r0;
    m[M21] = r1;
}

void MTH_MoveMatrix(MthMatrixTbl *matrixTbl, fix32 x, fix32 y, fix32 z)
{
    fix32* m = (fix32*)matrixTbl->current;

    m[M03] += MTH_Mul(m[M00], x) + MTH_Mul(m[M01], y) + MTH_Mul(m[M02], z);
    m[M13] += MTH_Mul(m[M10], x) + MTH_Mul(m[M11], y) + MTH_Mul(m[M12], z);
    m[M23] += MTH_Mul(m[M20], x) + MTH_Mul(m[M21], y) + MTH_Mul(m[M22], z);
}

void MTH_CoordTrans(MthMatrix *matrix, MthXyz *src, MthXyz *ans)
{
    fix32* m = (fix32*)matrix;

    ans->x = MTH_Mul(m[M00], src->x) + MTH_Mul(m[M01], src->y) + MTH_Mul(m[M02], src->z) + m[M03];
    ans->y = MTH_Mul(m[M10], src->x) + MTH_Mul(m[M11], src->y) + MTH_Mul(m[M12], src->z) + m[M13];
    ans->z = MTH_Mul(m[M20], src->x) + MTH_Mul(m[M21], src->y) + MTH_Mul(m[M22], src->z) + m[M23];
}
