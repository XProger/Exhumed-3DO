#ifdef GAPI_GL

#include <stdio.h>
#include <assert.h>
#include "app.h"
#include "vid.h"

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
} VID_VERTEX;

typedef struct {
    GLuint tex_index;
    sint32 count;
} VID_RANGE;

enum {
    aCoord,
    aColor
};

typedef uint16 VID_INDEX;

#define MAX_QUADS   1024

static VID_VERTEX vertices[MAX_QUADS * 4];
static VID_INDEX  indices[MAX_QUADS * 6];
static sint32 num_quads;

static GLuint vao;
static GLuint vbo[2];
static GLuint shader;
static GLint shader_uProj;

static mat4 mProj;
static sint32 vid_cx, vid_cy;

#define MAX_TEX     512 // == MAXNMCHARS
#define MAX_CLUT    256

static GLuint tex[MAX_TEX];
static VID_RANGE ranges[MAX_TEX];
static VID_RANGE *cur_range;

static GLuint clut_tex;
static uint32 cluts[MAX_CLUT][256];
static sint32 clut_updated;

//#define GetProcOGL(x) x=(decltype(x))GetProc(#x)
#define GetProcOGL(x) x=(void*)wglGetProcAddress(#x)

// texture
PFNGLACTIVETEXTUREPROC              glActiveTexture;
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

            "uniform sampler2D sTEX;\n"
            "uniform sampler2D sCLUT;\n"

            "void main() {\n"
                "vec4 color = texture2D(sTEX, vTexCoord);\n"
                "vec4 clut = texture2D(sCLUT, vec2(color.r, vColor.a));\n"
                "color = mix(color, clut, float(vColor.a < 1.0));\n"
                "if (color.a == 0.0) discard;\n"
                "fragColor = color * vec4(vColor.xyz, 1.0);\n"
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

    glUseProgram(shader);
    i = 0;
    glUniform1iv(glGetUniformLocation(shader, "sTEX"), 1, &i);
    i = 1;
    glUniform1iv(glGetUniformLocation(shader, "sCLUT"), 1, &i);
    shader_uProj = glGetUniformLocation(shader, "uProj");
    glUseProgram(0);
}

static void clut_init(void)
{
    glGenTextures(1, &clut_tex);
    glBindTexture(GL_TEXTURE_2D, clut_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 16, MAX_CLUT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    memset(&cluts[VID_NO_CLUT][0], 0xFF, 256 * sizeof(uint32));
    clut_updated = 1;
}

static void clut_validate(void)
{
    if (!clut_updated)
        return;
    clut_updated = 0;

    glActiveTexture(GL_TEXTURE1); // sCLUT
    glBindTexture(GL_TEXTURE_2D, clut_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, MAX_CLUT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cluts[0][0]);
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
    cur_range = ranges;
    cur_range->tex_index = 0;
    cur_range->count = 0;

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
    VID_VERTEX *v = NULL;
    VID_RANGE *range = ranges;
    uint16 *base_index = NULL;

    clut_validate();

    mat4_ortho(&mProj, 0, 320, 240, 0, 0, 1);

    glUseProgram(shader);
    glUniformMatrix4fv(shader_uProj, 1, GL_FALSE, (GLfloat*)&mProj);

    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VID_VERTEX) * num_quads * 4, vertices);

    glEnableVertexAttribArray(aCoord);
    glEnableVertexAttribArray(aColor);

    glVertexAttribPointer(aCoord, 4, GL_FLOAT, GL_FALSE, sizeof(*v), &v->x);
    glVertexAttribPointer(aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(*v), &v->color);

    glActiveTexture(GL_TEXTURE0); // sTEX
    while (range <= cur_range)
    {
        glBindTexture(GL_TEXTURE_2D, tex[range->tex_index]);

        if (range->count) // TODO make it nice!
        {
            glDrawElements(GL_TRIANGLES, range->count * 6, GL_UNSIGNED_SHORT, base_index);
            base_index += range->count * 6;
        }

        range++;
    }

    glBindVertexArray(0);

    glUseProgram(0);

    num_quads = 0;
    cur_range = ranges;
    cur_range->tex_index = 0;
    cur_range->count = 0;
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

    GetProcOGL(glActiveTexture);
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

    glGenTextures(MAX_TEX, tex);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    clut_init();
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

void vid_origin(sint32 x, sint32 y)
{
    vid_cx = x;
    vid_cy = y;
}

#define SET_VERTEX(vert,_x,_y,_u,_v,_clut,_color)\
    (vert)->x = (float)((_x) + vid_cx);\
    (vert)->y = (float)((_y) + vid_cy);\
    (vert)->u = (float)(_u);\
    (vert)->v = (float)(_v);\
    (vert)->color = (_color & 0x00FFFFFF) | (_clut << 24);

static uint32 conv_rgba(uint32 rgb)
{
    uint8 r = ((31 & (rgb)) << 3);
    uint8 g = ((31 & (rgb >> 5)) << 3);
    uint8 b = ((31 & (rgb >> 10)) << 3);

    return r | (g << 8) | (b << 16) | (rgb & 0x8000 ? 0xFF000000 : 0);
}

static void add_range(sint32 tex_index)
{
    if (cur_range->tex_index != tex_index)
    {
        cur_range++;
        cur_range->tex_index = tex_index;
        cur_range->count = 1;
    }
    else
    {
        cur_range->count++;
    }
}

static uint32 tmp_img[256 * 256];

#define VID_FMT_4   0x0008  // == COLOR_1
#define VID_FMT_8   0x0020  // == COLOR_4
#define VID_FMT_16  0x0028  // == COLOR_5

void vid_tex_reset(void)
{
    //
}

void vid_tex_set(sint32 tex_index, sint32 format, const void *data, sint32 width, sint32 height)
{
    sint32 i;

    assert(width < 256 && height < 256);

    if (format == VID_FMT_4)
    {
        for (i = 0; i < width * height; i += 2)
        {
            // store CLUT indices to R
            uint8 n = ((uint8*)data)[i >> 1];
            tmp_img[i + 0] = n >> 4;
            tmp_img[i + 1] = n & 15;
        }
    }
    else if (format == VID_FMT_8)
    {
        for (i = 0; i < width * height; i++)
        {
            // store CLUT indices to R
            tmp_img[i] = ((uint8*)data)[i];
        }
    }
    else if (format == VID_FMT_16)
    {
        for (i = 0; i < width * height; i++)
        {
            tmp_img[i] = conv_rgba(FS_SHORT((uint16*)data + i));
        }
    }
    else
    {
        assert(0);
    }

    glBindTexture(GL_TEXTURE_2D, tex[tex_index]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp_img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void vid_clut_set(sint32 clut_index, const uint16 *data, sint32 length)
{
    sint32 i;

    for (i = 0; i < length; i++)
    {
        cluts[clut_index][i] = conv_rgba(FS_SHORT(data + i));
    }

    // not necessary
    for (i = length; i < 256; i++)
    {
        cluts[clut_index][i] = 0;
    }

    clut_updated = 1;
}

void vid_sprite(sint32 *a, sint32 *c, uint16 color, sint32 dir, sint32 tex_index, sint32 clut_index)
{
    sint32 ax = a[0];
    sint32 ay = a[1];
    sint32 cx = ax + c[0];
    sint32 cy = ay + c[1];
    VID_VERTEX *v = vertices + num_quads * 4;
    uint32 color32 = conv_rgba(0xFFFF - color);

    add_range(tex_index);

    if (dir & SPR_FLIP_V)
    {
        sint32 t = ay;
        ay = cy;
        cy = t;
    }

    if (dir & SPR_FLIP_H)
    {
        sint32 t = ax;
        ax = cx;
        cx = t;
    }

    assert(num_quads < MAX_QUADS);
    num_quads++;

    SET_VERTEX(v + 0, ax, ay, 0.0f, 0.0f, clut_index, color32);
    SET_VERTEX(v + 1, cx, ay, 1.0f, 0.0f, clut_index, color32);
    SET_VERTEX(v + 2, cx, cy, 1.0f, 1.0f, clut_index, color32);
    SET_VERTEX(v + 3, ax, cy, 0.0f, 1.0f, clut_index, color32);
}

void vid_poly(sint32 *points, uint16 *colors, sint32 tex_index, sint32 clut_index)
{
    VID_VERTEX *v = vertices + num_quads * 4;
    uint32 colors32[4];

    add_range(tex_index);

    if (colors)
    {
        colors32[0] = conv_rgba(colors[0]);
        colors32[1] = conv_rgba(colors[1]);
        colors32[2] = conv_rgba(colors[2]);
        colors32[3] = conv_rgba(colors[3]);
    }
    else
    {
        colors32[0] =
        colors32[1] =
        colors32[2] =
        colors32[3] = 0xFFFFFFFF;
    }

    assert(num_quads < MAX_QUADS);
    num_quads++;

    SET_VERTEX(v + 0, points[0], points[1], 0.0f, 0.0f, clut_index, colors32[0]);
    SET_VERTEX(v + 1, points[2], points[3], 1.0f, 0.0f, clut_index, colors32[1]);
    SET_VERTEX(v + 2, points[4], points[5], 1.0f, 1.0f, clut_index, colors32[2]);
    SET_VERTEX(v + 3, points[6], points[7], 0.0f, 1.0f, clut_index, colors32[3]);
}

#endif // GAPI_GL
