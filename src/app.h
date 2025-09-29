#ifndef APP_H
#define APP_H

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef sint32 fix32;

#ifndef NULL
    #define NULL ((void*)0)
#endif

#define FIXED(a) ((fix32)((a) * 65536.0))
#define DIV_FIXED(a, b) MTH_Div(a, b)

#define FS_INT(p)   (sint32)( ((uint8*)(p))[3] | (((uint8*)(p))[2] << 8) | (((uint8*)(p))[1] << 16) | (((uint8*)(p))[0] << 24) )
#define FS_SHORT(p) (sint32)( ((uint8*)(p))[1] | (((uint8*)(p))[0] << 8) )

sint32 app_time(void);
uint16 app_input(void);
sint32 app_poll(void);
void app_init(void);

#endif
