#ifndef SEGA_SYS_H
#define SEGA_SYS_H

#define SYS_SETUINT(_Num_, _Hdr_)
#define SYS_CHGSCUIM(_AndMask_, _OrMask_)

typedef void (*SysFuncPtr)(void);

#define SYS_SETSINT(num,func) ((SysFuncPtr)func)()

#endif