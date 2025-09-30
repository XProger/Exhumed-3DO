#include "app.h"
#include "util.h"
#include "spr.h"
#include "file.h"
#include "v_blank.h"
#include "print.h"
#include "local.h"

static sint32 progressTotalSize, progressTotalRead;
static sint32 progressOn = 0; /* 1 if on, 2 if swirly */
static uint32 progressNextUpdate;

static FILE* openCDFile; /* only one cd file may be open at a time */

#define CDHANDLE 8000

void fs_init(void)
{
    //
}

sint32 fs_open(const char* filename)
{
    if (filename[0] == '+')
        filename++;

    assert(!openCDFile);
    openCDFile = fopen(filename, "rb");
    assert(openCDFile); /* do retry processing here */

    return CDHANDLE;
}

void fs_close(sint32 handle)
{
    assert(handle == CDHANDLE);
    assert(openCDFile);
    fclose(openCDFile);
    openCDFile = NULL;
}

sint32 fs_getFileSize(sint32 fd)
{
    sint32 pos, size;
    assert(fd == CDHANDLE);
    assert(openCDFile);
    pos = ftell(openCDFile);
    fseek(openCDFile, 0, SEEK_END);
    size = ftell(openCDFile);
    fseek(openCDFile, pos, SEEK_SET);
    return size;
}

void fs_read(sint32 fd, void* buf, sint32 n)
{
    assert(fd == CDHANDLE);
    assert(openCDFile);
    fread(buf, 1, n, openCDFile);
}

void fs_skip(sint32 fd, sint32 n)
{
    fseek(openCDFile, n, SEEK_CUR);
}

void playWholeCD(void)
{
    //
}

void playCDTrack(sint32 track, sint32 repeat)
{
    //
}

void stopCD(void)
{
    //
}

void fs_startProgress(sint32 swirly)
{
    progressTotalSize = 1; /* prevent div by 0 */
    progressTotalRead = 0;
    progressOn = 1;
    progressNextUpdate = vtimer;
}

void fs_addToProgress(const char* filename)
{
    //
}

void fs_closeProgress(void)
{
    progressOn = 0;
}

void link(char* filename)
{
#ifdef TODO // overlays
    sint8* data;
    void (*code)(void* data, sint32 size);
    sint32 fd;
    sint32 size;
    mem_init();
    fd = fs_open(filename);
    size = fs_getFileSize(fd);
    dPrint("size=%d\n", size);
    size = (size + 3) & (~3);
    data = mem_malloc(0, size);
    fs_read(fd, data, size);
    fs_close(fd);
    code = mem_malloc(0, 256);
    memcpy(code, executeLink, 256);
    dPrint("linking\n");
    INT_SetMsk(INT_MSK_ALL);
    INT_SetScuFunc(INT_SCU_HBLK_IN, NULL);
    INT_SetScuFunc(INT_SCU_VBLK_IN, NULL);
    INT_SetScuFunc(INT_SCU_VBLK_OUT, NULL);
    /* set_imask(0xffffffff); */
    (*code)(data, size >> 2);
#endif
}
