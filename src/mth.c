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

static const uint16 MTH_TBL_SIN[] = { // 0..90 deg [value, step]
    0x0477, 0x0000, 0x0478, 0x0477, 0x0476, 0x08EF, 0x0476, 0x0D65, 0x0474, 0x11DB, 0x0473, 0x164F, 0x0470, 0x1AC2, 0x046E, 0x1F32, 0x046C, 0x23A0, 0x0468, 0x280C,
    0x0464, 0x2C74, 0x0461, 0x30D8, 0x045D, 0x3539, 0x0458, 0x3996, 0x0453, 0x3DEE, 0x044F, 0x4241, 0x0448, 0x4690, 0x0443, 0x4AD8, 0x043D, 0x4F1B, 0x0436, 0x5358,
    0x0430, 0x578E, 0x0428, 0x5BBE, 0x0420, 0x5FE6, 0x0419, 0x6406, 0x0411, 0x681F, 0x0409, 0x6C30, 0x03FF, 0x7039, 0x03F7, 0x7438, 0x03ED, 0x782F, 0x03E3, 0x7C1C,
    0x03DA, 0x7FFF, 0x03CF, 0x83D9, 0x03C5, 0x87A8, 0x03BA, 0x8B6D, 0x03AE, 0x8F27, 0x03A4, 0x92D5, 0x0397, 0x9679, 0x038B, 0x9A10, 0x0380, 0x9D9B, 0x0372, 0xA11B,
    0x0366, 0xA48D, 0x0359, 0xA7F3, 0x034B, 0xAB4C, 0x033E, 0xAE97, 0x032F, 0xB1D5, 0x0322, 0xB504, 0x0313, 0xB826, 0x0305, 0xBB39, 0x02F6, 0xBE3E, 0x02E7, 0xC134,
    0x02D8, 0xC41B, 0x02C8, 0xC6F3, 0x02B8, 0xC9BB, 0x02A8, 0xCC73, 0x0298, 0xCF1B, 0x0288, 0xD1B3, 0x0278, 0xD43B, 0x0266, 0xD6B3, 0x0256, 0xD919, 0x0244, 0xDB6F,
    0x0234, 0xDDB3, 0x0221, 0xDFE7, 0x0211, 0xE208, 0x01FE, 0xE419, 0x01EC, 0xE617, 0x01DB, 0xE803, 0x01C8, 0xE9DE, 0x01B5, 0xEBA6, 0x01A4, 0xED5B, 0x0190, 0xEEFF,
    0x017E, 0xF08F, 0x016B, 0xF20D, 0x0158, 0xF378, 0x0145, 0xF4D0, 0x0131, 0xF615, 0x011F, 0xF746, 0x010B, 0xF865, 0x00F7, 0xF970, 0x00E4, 0xFA67, 0x00D1, 0xFB4B,
    0x00BD, 0xFC1C, 0x00A9, 0xFCD9, 0x0095, 0xFD82, 0x0081, 0xFE17, 0x006E, 0xFE98, 0x005A, 0xFF06, 0x0046, 0xFF60, 0x0032, 0xFFA6, 0x001E, 0xFFD8, 0x000A, 0xFFF6,
    0x0000, 0xFFFF,
};

static const fix32 MTH_TBL_TAN[] = { // 0..90 deg [value, step]
    0x00000000, 0x00394A3A, 0x00000477, 0x0039414B, 0x000008F0, 0x00392F70, 0x00000D6A, 0x003914AD, 0x000011E6, 0x0038F10C, 0x00001665, 0x0038C498, 0x00001AE8, 0x00388F5F, 0x00001F6E, 0x00385170, 0x000023FA, 0x00380AE0, 0x0000288B, 0x0037BBC4,
    0x00002D23, 0x00376435, 0x000031C2, 0x0037044F, 0x0000366A, 0x00369C2F, 0x00003B1A, 0x00362BF6, 0x00003FD3, 0x0035B3C7, 0x00004498, 0x003533C7, 0x00004968, 0x0034AC1E, 0x00004E44, 0x00341CF7, 0x0000532D, 0x0033867E, 0x00005825, 0x0032E8E2,
    0x00005D2D, 0x00324454, 0x00006244, 0x00319908, 0x0000676E, 0x0030E733, 0x00006CAA, 0x00302F0C, 0x000071FA, 0x002F70CD, 0x0000775F, 0x002EACB2, 0x00007CDC, 0x002DE2F7, 0x00008270, 0x002D13DB, 0x0000881E, 0x002C3F9F, 0x00008DE7, 0x002B6686,
    0x000093CD, 0x002A88D2, 0x000099D2, 0x0029A6C9, 0x00009FF7, 0x0028C0B2, 0x0000A63F, 0x0027D6D3, 0x0000ACAC, 0x0026E978, 0x0000B340, 0x0025F8E8, 0x0000B9FE, 0x0025056F, 0x0000C0E8, 0x00240F5A, 0x0000C802, 0x002316F5, 0x0000CF4E, 0x00221C8D,
    0x0000D6CF, 0x00212071, 0x0000DE89, 0x002022EE, 0x0000E680, 0x001F2455, 0x0000EEB9, 0x001E24F5, 0x0000F737, 0x001D251D, 0x00010000, 0x001C251D, 0x00010918, 0x001B2545, 0x00011286, 0x001A25E5, 0x00011C51, 0x0019274C, 0x0001267E, 0x001829C9,
    0x00013116, 0x00172DAD, 0x00013C22, 0x00163345, 0x000147AA, 0x00153AE0, 0x000153B9, 0x001444CB, 0x0001605A, 0x00135152, 0x00016D9B, 0x001260C2, 0x00017B89, 0x00117366, 0x00018A34, 0x00108988, 0x000199AF, 0x000FA371, 0x0001AA0E, 0x000EC168,
    0x0001BB67, 0x000DE3B4, 0x0001CDD6, 0x000D0A9B, 0x0001E177, 0x000C365F, 0x0001F66D, 0x000B6743, 0x00020CE0, 0x000A9D88, 0x000224FE, 0x0009D96D, 0x00023EFC, 0x00091B2E, 0x00025B19, 0x00086307, 0x0002799F, 0x0007B132, 0x00029AE7, 0x000705E6,
    0x0002BF5A, 0x00066158, 0x0002E77A, 0x0005C3BC, 0x000313E3, 0x00052D43, 0x00034556, 0x00049E1C, 0x00037CC7, 0x00041673, 0x0003BB67, 0x00039673, 0x000402C2, 0x00031E44, 0x000454DB, 0x0002AE0B, 0x0004B462, 0x000245EB, 0x00052501, 0x0001E605,
    0x0005ABD9, 0x00018E76, 0x00065052, 0x00013F5A, 0x00071D88, 0x0000F8CA, 0x000824F3, 0x0000BADB, 0x000983AD, 0x000085A2, 0x000B6E17, 0x0000592D, 0x000E4CF8, 0x0000358D, 0x001314C5, 0x00001ACA, 0x001CA2E1, 0x000008EF, 0x00394A3A, 0x00000105,
    0x7FFFFFFF, 0x00000000
};

fix32 MTH_Sin(fix32 degree)
{
    uint32 sign, index, delta, value, ret;

    if (degree >= FIXED(0))
    {
        sign = 0;
    }
    else
    {
        sign = 1;
        degree = -degree;
    }

    if (degree >= FIXED(180))
    {
        degree = FIXED(0);
    } 
    else if (degree > FIXED(90))
    {
        degree = FIXED(180) - degree;
    }

    index = (uint16)(degree >> 16) << 1;

    delta = MTH_TBL_SIN[index + 0];
    value = MTH_TBL_SIN[index + 1];

    ret = ((uint32)(degree & 0xFFFF) * delta >> 16) + value;

    if (sign != 0)
    {
        return -(fix32)ret;
    }

    return ret;
}

fix32 MTH_Cos(fix32 degree)
{
    degree += FIXED(90);
    if (degree >= FIXED(180))
    {
        degree -= FIXED(360);
    }

    return MTH_Sin(degree);
}

fix32 MTH_Atan(fix32 y, fix32 x)
{
    sint32 ax, ay, d, M, L = 0, R = 90;
    fix32 degree, w, t;

    ax = abs(x);
    ay = abs(y);

    if (ax >= FIXED(0.001f))
    {
        w = MTH_Div(ay, ax);
    }
    else
    {
        w = 0x7FFF0000; // max fixed
    }

    while (L < R)
    {
        M = (L + R) >> 1;
        t = MTH_TBL_TAN[M * 2 + 0];

        if (t > w)
        {
            R = M;
        }
        else
        {
            L = M + 1;
            if (w >= MTH_TBL_TAN[L * 2 + 0])
                break;
        }
    }

    t = MTH_TBL_TAN[M * 2 + 0];
    d = MTH_TBL_TAN[M * 2 + 1];
    degree = MTH_Mul(w - t, d);

    if (degree > FIXED(1))
    {
        degree = FIXED(1);
    }

    degree += FIXED(M);

    if (y < 0)
    {
        degree = -degree;
    }

    if (x < 0)
    {
        if (y >= 0)
        {
            degree = FIXED(180) - degree;
        }
        else
        {
            degree = FIXED(-180) - degree;
        }
    }

    return degree;
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
