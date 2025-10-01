#include "app.h"
#include "util.h"

static sint32 textData[7 * 1024 / 4];

sint32 getLanguageNumber(void)
{
    sint32 skip;
#ifdef JAPAN
    return 0;
#endif
    skip = 0;
#ifdef TODO // language
    switch (systemMemory & PER_MSK_LANGU)
    {
        case PER_ENGLISH:
            skip = 0;
            break;
        case PER_ESPNOL:
            skip = 1;
            break;
        case PER_FRANCAIS:
            skip = 2;
            break;
        case PER_DEUTSCH:
            skip = 3;
            break;
    }
#endif
    return skip;
}

void loadLocalText(void)
{
    sint32 size, skip;
    mem_lock();
    skip = getLanguageNumber();
    for (; skip >= 0; skip--)
    {
        fs_read(&size, 4);

        size = FS_INT(&size);

        assert(size < 7 * 1024);
        fs_read(textData, size);
    }
}

char* getText(sint32 block, sint32 string)
{
    block = FS_INT(textData + block);
    block = FS_INT(textData + ((block >> 2) + string));
    return ((char*)textData) + block;
}
