#ifndef SEGA_XPT_H
#define SEGA_XPT_H

#ifdef TODO
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef sint32 fix32;

enum BooleanLogic
{
    FALSE = 0,
    TRUE = 1
};

enum BooleanSwitch
{
    OFF = 0,
    ON = 1
};

enum Judgement
{
    OK = 0,
    NG = -1
};

#define FIXED(a) ((fix32)((a) * 65536.0))
#define DIV_FIXED(a, b) (((a) / ((b) >> 8)) << 8)

#endif
