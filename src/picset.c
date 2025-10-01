#include "app.h"
#include "mth.h"
#include "util.h"
#include "picset.h"

sint32 loadPicSet(uint16** palletes, uint32** datas, sint32 maxNmPics)
{
    sint8* data;
    sint8* d;
    sint8* lastPallete;
    sint32 chunkSize;
    sint32 size;
    sint32 pic, w, h;
    fs_read(&size, 4);

    size = FS_INT(&size);

    data = mem_malloc(0, size);
    fs_read(data, size);
    pic = 0;
    d = data;
    lastPallete = NULL;
    while (d < data + size)
    {
        assert(!(((sint32)d) & 3));
        datas[pic] = ((uint32*)d) + 1;
        chunkSize = FS_INT((sint32*)d);
        w = ((sint32*)d)[1] = FS_INT(((sint32*)d) + 1);
        h = ((sint32*)d)[2] = FS_INT(((sint32*)d) + 2);
        d += chunkSize + 12;
        if (FS_INT((sint32*)d) & 1)
        {
            d += 4;
            palletes[pic] = (uint16*)d;
            lastPallete = d;
            d += 256 * 2;
        }
        else
        {
            d += 4;
            assert(lastPallete);
            palletes[pic] = (uint16*)lastPallete;
        }
        pic++;
        assert(pic < maxNmPics);
    }
    return pic;
}
