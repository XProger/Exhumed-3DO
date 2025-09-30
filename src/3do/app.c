#include "app.h"
#include "vid.h"

Item irqTimer;

extern uint8 *DRAM;
extern uint8 *VRAM;

static uint16 pad = 0xFFFF;

sint32 app_time(void)
{
    return (sint32)GetMSecTime(irqTimer);
}

uint16 app_input(void)
{
    return pad;
}

sint32 app_poll(void)
{
    uint32 mask;
    ControlPadEventData event;

    if (!GetControlPad(1, 0, &event))
        return;

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

float sinf(float x)
{
    return 0.0f;
}

float cosf(float x)
{
    return 1.0f;
}

float atan2f(float y, float x)
{
    return 0.0f;
}
