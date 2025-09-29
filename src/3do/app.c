#include <sega_per.h>

#include "app.h"
#include "vid.h"

Item irqTimer;

static uint16 pad = 0xFFFF;

sint32 app_time(void)
{
    return GetMSecTime(irqTimer);
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

void app_init(void)
{
    printf("Hello, 3DO!\n");

    InitEventUtility(1, 0, LC_Observer);

    vid_init();

    irqTimer = GetTimerIOReq();
    ScavengeMem();
}

float sinf(float x)
{
    return 0.0f;
}

float cosf(float x)
{
    return 0.0f;
}

float atan2f(float y, float x)
{
    return 0.0f;
}
