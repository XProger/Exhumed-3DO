#include <windows.h>

#include "app.h"
#include "vid.h"

HWND hWnd;

static sint32 is_quit;
static sint32 wnd_width = 320 * 3;
static sint32 wnd_height = 240 * 3;

static LARGE_INTEGER gTimerFreq;
static LARGE_INTEGER gTimerStart;

static LRESULT CALLBACK WndProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
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
    return (sint32)((count.QuadPart - gTimerStart.QuadPart) * 1000L / gTimerFreq.QuadPart);
}

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

    QueryPerformanceFrequency(&gTimerFreq);
    QueryPerformanceCounter(&gTimerStart);
}
