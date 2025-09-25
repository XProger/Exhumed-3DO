#ifdef GAPI_GL

#include <stdio.h>
#include <assert.h>
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

typedef struct {
    float e00, e10, e20, e30,
          e01, e11, e21, e31,
          e02, e12, e22, e32,
          e03, e13, e23, e33;
} mat4;

typedef struct {
    float x, y, u, v;
    uint32 color;
} VERTEX;

enum {
    aCoord,
    aColor
};

typedef uint16 INDEX;

#define MAX_QUADS   1024

static VERTEX vertices[MAX_QUADS * 4];
static INDEX  indices[MAX_QUADS * 6];
static sint32 num_quads;

static GLuint vao;
static GLuint vbo[2];
static GLuint shader;
static GLint shader_uProj;

static mat4 mProj;
static sint32 vid_cx, vid_cy;

//#define GetProcOGL(x) x=(decltype(x))GetProc(#x)
#define GetProcOGL(x) x=(void*)wglGetProcAddress(#x)

// shader
PFNGLCREATEPROGRAMPROC              glCreateProgram;
PFNGLDELETEPROGRAMPROC              glDeleteProgram;
PFNGLLINKPROGRAMPROC                glLinkProgram;
PFNGLUSEPROGRAMPROC                 glUseProgram;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog;
PFNGLCREATESHADERPROC               glCreateShader;
PFNGLDELETESHADERPROC               glDeleteShader;
PFNGLSHADERSOURCEPROC               glShaderSource;
PFNGLATTACHSHADERPROC               glAttachShader;
PFNGLCOMPILESHADERPROC              glCompileShader;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog;
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation;
PFNGLUNIFORM1IVPROC                 glUniform1iv;
PFNGLUNIFORM1FVPROC                 glUniform1fv;
PFNGLUNIFORM2FVPROC                 glUniform2fv;
PFNGLUNIFORM3FVPROC                 glUniform3fv;
PFNGLUNIFORM4FVPROC                 glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv;
PFNGLBINDATTRIBLOCATIONPROC         glBindAttribLocation;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
PFNGLGETPROGRAMIVPROC               glGetProgramiv;
// buffer
PFNGLGENBUFFERSARBPROC              glGenBuffers;
PFNGLDELETEBUFFERSARBPROC           glDeleteBuffers;
PFNGLBINDBUFFERARBPROC              glBindBuffer;
PFNGLBUFFERDATAARBPROC              glBufferData;
PFNGLBUFFERSUBDATAARBPROC           glBufferSubData;
// vertex array
PFNGLGENVERTEXARRAYSPROC            glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC         glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC            glBindVertexArray;

static void mat4_identity(mat4 *m) {
    m->e10 = m->e20 = m->e30 = m->e01 = m->e21 = m->e31 = m->e02 = m->e12 = m->e32 = m->e03 = m->e13 = m->e23 = 0.0f;
    m->e00 = m->e11 = m->e22 = m->e33 = 1.0f;
}

static void mat4_ortho(mat4 *m, float l, float r, float b, float t, float znear, float zfar)
{
    mat4_identity(m);
    m->e00 = 2.0f / (r - l);
    m->e11 = 2.0f / (t - b);
    m->e22 = 2.0f / (znear - zfar);
    m->e33 = 1.0f;
    m->e03 = (l + r) / (l - r);
    m->e13 = (t + b) / (b - t);
    m->e23 = (znear + zfar) / (znear - zfar);
}


static void shader_compile(void)
{
    static char* GLSL_HEADER_VERT_GL3 = 
        "#version 140\n"
        "#define VERT\n"
        "#define varying out\n"
        "#define attribute in\n"
        "#define texture2D texture\n";

    static char* GLSL_HEADER_FRAG_GL3 = 
        "#version 140\n"
        "#define FRAG\n"
        "#define varying in\n"
        "#define texture2D texture\n"
        "out vec4 fragColor;\n";

    static char* text =
        "varying vec2 vTexCoord;\n"
        "varying vec4 vColor;\n"

        "#ifdef VERT\n"
            "uniform mat4 uProj;\n"

            "attribute vec4 aCoord;\n"
            "attribute vec4 aColor;\n"

            "void main() {\n"
                "vTexCoord = aCoord.zw;\n"
                "vColor = aColor;\n"
                "gl_Position = uProj * vec4(aCoord.xy, 0.0, 1.0);\n"
            "}\n"

        "#else\n"

            "//uniform sampler2D sDiffuse;\n"

            "void main() {\n"
                "//fragColor = texture2D(sDiffuse, vTexCoord);\n"
                "fragColor = vColor;\n"
            "}\n"

        "#endif\n";

    sint32 i;
    GLchar info[1024];
    GLuint obj;
    const GLenum type[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

    GLchar *code[2][3] = {
        { GLSL_HEADER_VERT_GL3, "#line 0\n", text },
        { GLSL_HEADER_FRAG_GL3, "#line 0\n", text }
    };

    shader = glCreateProgram();

    for (i = 0; i < 2; i++)
    {
        obj = glCreateShader(type[i]);
        glShaderSource(obj, 3, code[i], NULL);
        glCompileShader(obj);

        glGetShaderInfoLog(obj, sizeof(info), NULL, info);
        if (info[0] && strlen(info) > 8)
        {
            printf("%s\n", info);
            assert(0);
        }

        glAttachShader(shader, obj);
        glDeleteShader(obj);
    }

    glBindAttribLocation(shader, aCoord, "aCoord");
    glBindAttribLocation(shader, aColor, "aColor");

    glLinkProgram(shader);

    glGetProgramInfoLog(shader, sizeof(info), NULL, info);
    if (info[0] && strlen(info) > 8)
    {
        printf("%s\n", info);
        assert(0);
    }

    i = 0;
    glUseProgram(shader);
    glUniform1iv(glGetUniformLocation(shader, "sDiffuse"), 1, &i);
    shader_uProj = glGetUniformLocation(shader, "uProj");
    glUseProgram(0);
}

static void vb_init(void)
{
    sint32 i, j;

    for (i = 0, j = 0; i < MAX_QUADS * 6; i += 6, j += 4)
    {
        indices[i + 0] = j + 0;
        indices[i + 1] = j + 1;
        indices[i + 2] = j + 2;
        indices[i + 3] = j + 2;
        indices[i + 4] = j + 3;
        indices[i + 5] = j + 0;
    }

    num_quads = 0;

    glGenVertexArrays(1, &vao);

    glGenBuffers(2, vbo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void vb_flush(void)
{
    VERTEX *v = NULL;

    mat4_ortho(&mProj, 0, 320, 240, 0, 0, 1);

    glUseProgram(shader);
    glUniformMatrix4fv(shader_uProj, 1, GL_FALSE, (GLfloat*)&mProj);

    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX) * num_quads * 4, vertices);

    glEnableVertexAttribArray(aCoord);
    glEnableVertexAttribArray(aColor);

    glVertexAttribPointer(aCoord, 4, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(*v), &v->color);

    glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);

    glUseProgram(0);

    num_quads = 0;
}

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

    GetProcOGL(glCreateProgram);
    GetProcOGL(glDeleteProgram);
    GetProcOGL(glLinkProgram);
    GetProcOGL(glUseProgram);
    GetProcOGL(glGetProgramInfoLog);
    GetProcOGL(glCreateShader);
    GetProcOGL(glDeleteShader);
    GetProcOGL(glShaderSource);
    GetProcOGL(glAttachShader);
    GetProcOGL(glCompileShader);
    GetProcOGL(glGetShaderInfoLog);
    GetProcOGL(glGetUniformLocation);
    GetProcOGL(glUniform1iv);
    GetProcOGL(glUniform1fv);
    GetProcOGL(glUniform2fv);
    GetProcOGL(glUniform3fv);
    GetProcOGL(glUniform4fv);
    GetProcOGL(glUniformMatrix4fv);
    GetProcOGL(glBindAttribLocation);
    GetProcOGL(glEnableVertexAttribArray);
    GetProcOGL(glDisableVertexAttribArray);
    GetProcOGL(glVertexAttribPointer);
    GetProcOGL(glGetProgramiv);

    GetProcOGL(glGenBuffers);
    GetProcOGL(glDeleteBuffers);
    GetProcOGL(glBindBuffer);
    GetProcOGL(glBufferData);
    GetProcOGL(glBufferSubData);

    GetProcOGL(glGenVertexArrays);
    GetProcOGL(glDeleteVertexArrays);
    GetProcOGL(glBindVertexArray);

    vb_init();
    shader_compile();

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void vid_resize(sint32 width, sint32 height)
{
    glViewport(0, 0, width, height);
}

void vid_clear(void)
{
    glClearColor(0.3f, 0.6f, 0.8f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void vid_blit(void)
{
    vb_flush();

#if AP_WIN
    SwapBuffers(hDC);
#elif AP_NIX
    glXSwapBuffers(dpy, wnd);
#endif
}

void vid_center(sint32 x, sint32 y)
{
    vid_cx = x;
    vid_cy = y;
}

#define SET_VERTEX(vert,_x,_y,_u,_v,_color)\
    (vert)->x = (float)((_x) + vid_cx);\
    (vert)->y = (float)((_y) + vid_cy);\
    (vert)->u = (float)(_u);\
    (vert)->v = (float)(_v);\
    (vert)->color = _color;

static const uint32 colors[] = {
    0xFF0000FF,
    0xFF00A5FF,
    0xFF00FFFF,
    0xFF00FF00,
    0xFFFF0000,
    0xFFFF0066,
    0xFFFF008B,
    0xFF00FFFF
};

void vid_spr(sint32 *a, sint32 *c)
{
    sint32 ax = a[0];
    sint32 ay = a[1];
    sint32 cx = ax + c[0];
    sint32 cy = ay + c[1];
    VERTEX *v = vertices + num_quads * 4;
    uint32 color = colors[num_quads & 7];

    assert(num_quads < MAX_QUADS);
    num_quads++;

    SET_VERTEX(v + 0, ax, ay, 0.0f, 0.0f, color);
    SET_VERTEX(v + 1, cx, ay, 0.0f, 0.0f, color);
    SET_VERTEX(v + 2, cx, cy, 0.0f, 0.0f, color);
    SET_VERTEX(v + 3, ax, cy, 0.0f, 0.0f, color);
}

void vid_poly(sint32 *p)
{
    VERTEX *v = vertices + num_quads * 4;
    uint32 color = colors[num_quads & 7];

    assert(num_quads < MAX_QUADS);
    num_quads++;

    SET_VERTEX(v + 0, p[0], p[1], 0.0f, 0.0f, color);
    SET_VERTEX(v + 1, p[2], p[3], 0.0f, 0.0f, color);
    SET_VERTEX(v + 2, p[4], p[5], 0.0f, 0.0f, color);
    SET_VERTEX(v + 3, p[6], p[7], 0.0f, 0.0f, color);
}

#endif // GAPI_GL
