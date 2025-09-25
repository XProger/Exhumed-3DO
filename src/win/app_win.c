#include <windows.h>

#include <sega_per.h>

#include "app.h"
#include "vid.h"

HWND hWnd;

static sint32 is_quit;
static sint32 wnd_width = 320 * 3;
static sint32 wnd_height = 240 * 3;

static LARGE_INTEGER timer_freq;
static LARGE_INTEGER timer_start;

static uint16 pad = 0xFFFF;

static LRESULT CALLBACK WndProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            sint32 btn = 0;

            switch (wParam)
            {
                case VK_LEFT:
                    btn = PER_DGT_L;
                    break;
                case VK_RIGHT:
                    btn = PER_DGT_R;
                    break;
                case VK_UP:
                    btn = PER_DGT_U;
                    break;
                case VK_DOWN:
                    btn = PER_DGT_D;
                    break;
                case VK_RETURN:
                    btn = PER_DGT_S;
                    break;
                case 'Z':
                    btn = PER_DGT_A;
                    break;
                case 'X':
                    btn = PER_DGT_B;
                    break;
                case 'C':
                    btn = PER_DGT_C;
                    break;
                case 'A':
                    btn = PER_DGT_X;
                    break;
                case 'S':
                    btn = PER_DGT_Y;
                    break;
                case 'D':
                    btn = PER_DGT_Z;
                    break;
                case 'Q':
                    btn = PER_DGT_TL;
                    break;
                case 'E':
                    btn = PER_DGT_TR;
                    break;
            }

            if (btn)
            {
                if (msg == WM_KEYUP || msg == WM_SYSKEYUP)
                {
                    pad |= btn;
                }
                else
                {
                    pad &= ~btn;
                }
            }

            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            is_quit = 1;
            break;
        }

        case WM_SIZE:
        {
            wnd_width = LOWORD(lParam);
            wnd_height = HIWORD(lParam);
            vid_resize(wnd_width, wnd_height);
            break;
        }

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

sint32 app_time(void)
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return (sint32)((count.QuadPart - timer_start.QuadPart) * 1000L / timer_freq.QuadPart);
}

uint16 app_input(void)
{
    return pad;
}

extern void UsrVblankEnd(void);

sint32 app_poll(void)
{
    MSG msg;

    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (is_quit)
    {
        ExitProcess(0);
    }

    Sleep(16);

    UsrVblankEnd(); // TODO

    return is_quit;
}

void app_init(void)
{
    WNDCLASSEX wcex;
    sint32 x, y, w, h;
    RECT r = { 0, 0, wnd_width, wnd_height };

    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, 0);
    w = (r.right - r.left);
    h = (r.bottom - r.top);
    x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.hInstance      = GetModuleHandle(NULL);
    wcex.hIcon          = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszClassName  = L"SLAVE_WND";
    wcex.hIconSm        = wcex.hIcon;
    wcex.lpfnWndProc    = &WndProc;
    RegisterClassEx(&wcex);

    hWnd = CreateWindow(wcex.lpszClassName, L"PowerSlave (DBG)", WS_OVERLAPPEDWINDOW, x, y, w, h, 0, 0, wcex.hInstance, 0);
    ShowWindow(hWnd, SW_SHOWDEFAULT);

    vid_init();

    QueryPerformanceFrequency(&timer_freq);
    QueryPerformanceCounter(&timer_start);
}
