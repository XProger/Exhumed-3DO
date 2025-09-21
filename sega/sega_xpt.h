#ifndef SEGA_XPT_H
#define SEGA_XPT_H

#ifdef TODO
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef signed char Sint8;
typedef signed short Sint16;
typedef signed int Sint32;

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;

typedef signed int Fixed32;

typedef Sint32 Bool;

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

#define FIXED(a) ((Fixed32)((a) * 65536.0))
#define DIV_FIXED(a, b) (((a) / ((b) >> 8)) << 8)

#endif
