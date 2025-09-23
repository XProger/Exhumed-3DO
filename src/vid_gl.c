#ifdef GAPI_GL

#include "app.h"

#ifdef AP_WIN
    #include <windows.h>
    #include <gl/GL.h>
    #include <gl/glu.h>
    #include <gl/glext.h>
    #include <gl/wglext.h>

    extern HWND hWnd;

    HDC hDC;
    HGLRC hRC;
#elif AP_NIX
    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/glx.h>
#endif

//#define GetProcOGL(x) x=(decltype(x))GetProc(#x)
#define GetProcOGL(x) x=(void*)wglGetProcAddress(#x)

void vid_init(void)
{
#ifdef AP_WIN
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;

    PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = NULL;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

    {
        HWND fWnd = CreateWindow(L"static", NULL, WS_POPUP, 0, 0, 0, 0, 0, 0, 0, 0);
        HDC fDC = GetDC(fWnd);

        int format = ChoosePixelFormat(fDC, &pfd);
        SetPixelFormat(fDC, format, &pfd);
        HGLRC fRC = wglCreateContext(fDC);
        wglMakeCurrent(fDC, fRC);

        wglChoosePixelFormatARB    = GetProcOGL(wglChoosePixelFormatARB);
        wglCreateContextAttribsARB = GetProcOGL(wglCreateContextAttribsARB);

        wglMakeCurrent(0, 0);
        ReleaseDC(fWnd, fDC);
        wglDeleteContext(fRC);
        DestroyWindow(fWnd);
    }

    hDC = GetDC(hWnd);

    if (wglChoosePixelFormatARB && wglCreateContextAttribsARB)
    {
        static const int pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
			WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,     32,
			WGL_ALPHA_BITS_ARB,     8,
			WGL_DEPTH_BITS_ARB,     24,
			0
        };

        int format;
        UINT numFormats;
        int status = wglChoosePixelFormatARB(hDC, pixelAttribs, NULL, 1, &format, &numFormats);

        DescribePixelFormat(hDC, format, sizeof(pfd), &pfd);
        SetPixelFormat(hDC, format, &pfd);

        int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        hRC = wglCreateContextAttribsARB(hDC, 0, contextAttribs);
    } else {
        int format = ChoosePixelFormat(hDC, &pfd);
        SetPixelFormat(hDC, format, &pfd);
        hRC = wglCreateContext(hDC);
    }

    wglMakeCurrent(hDC, hRC);
#endif
}

void vid_resize(sint32 width, sint32 height)
{
    glViewport(0, 0, width, height);
}

void vid_clear(void)
{
    glClearColor(1.0f, 1.0, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void vid_blit(void)
{
#if AP_WIN
    SwapBuffers(hDC);
#elif AP_NIX
    glXSwapBuffers(dpy, wnd);
#endif
}

#endif // GAPI_GL
