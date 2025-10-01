#include "app.h"
#include "vid.h"

#include <io.h>
#include <mem.h>
#include <event.h>
#include <timerutils.h>
#include <filestream.h>
#include <filestreamfunctions.h>

Item irqTimer;

extern uint8 *DRAM;
extern uint8 *VRAM;

static uint16 pad = 0xFFFF;

static Stream *app_file;

sint32 app_time(void)
{
    return (sint32)GetMSecTime(irqTimer);
}

uint16 app_input(void)
{
    return pad;
}

extern void UsrVblankEnd(void);

sint32 app_poll(void)
{
    uint32 mask;
    ControlPadEventData event;

    if (GetControlPad(1, 0, &event))
    {
        pad = 0xFFFF;
        mask = event.cped_ButtonBits;

        if (mask & ControlDown)         pad &= ~PER_DGT_D;
        if (mask & ControlUp)           pad &= ~PER_DGT_U;
        if (mask & ControlRight)        pad &= ~PER_DGT_R;
        if (mask & ControlLeft)         pad &= ~PER_DGT_L;
        if (mask & ControlA)            pad &= ~PER_DGT_A;
        if (mask & ControlB)            pad &= ~PER_DGT_B;
        if (mask & ControlC)            pad &= ~PER_DGT_C;
        if (mask & ControlStart)        pad &= ~PER_DGT_S;
        if (mask & ControlX)            pad &= ~PER_DGT_X;
        if (mask & ControlLeftShift)    pad &= ~PER_DGT_TL;
        if (mask & ControlRightShift)   pad &= ~PER_DGT_TR;
    }

    UsrVblankEnd();

    return 0;
}

void memCheck(void)
{
    MemInfo memInfoVRAM;
    availMem(&memInfoVRAM, MEMTYPE_DRAM);
    printf("DRAM: %d kb\n", (sint32)memInfoVRAM.minfo_SysFree / 1024);
    availMem(&memInfoVRAM, MEMTYPE_VRAM);
    printf("VRAM: %d kb\n", (sint32)memInfoVRAM.minfo_SysFree / 1024);
}

void app_init(void)
{
    printf("Hello, 3DO!\n");

    InitEventUtility(1, 0, LC_Observer);

    vid_init();

    memCheck();
    DRAM = (uint8*)AllocMem(DRAM_SIZE, MEMTYPE_DRAM);
    VRAM = (uint8*)AllocMem(VRAM_SIZE, MEMTYPE_VRAM);
    memCheck();

    irqTimer = GetTimerIOReq();
    ScavengeMem();
}

void fs_open(const char *name)
{
    char path[128];
    
    strcpy(path, "bin/");
    strcat(path, name);

    printf("open: %s\n", path);
    app_file = OpenDiskStream((char*)path, 2048);
    if (!app_file)
    {
        printf("failed to open file\n");
    }
}

void fs_read(void *buffer, sint32 count)
{
    if (ReadDiskStream(app_file, buffer, count) < 0)
    {
        printf("failed to read file\n");
    }
}

void fs_skip(sint32 offset)
{
    SeekDiskStream(app_file, offset, SEEK_CUR);
}

void fs_close(void)
{
    printf("close\n");
    CloseDiskStream(app_file);
    app_file = NULL;
}
