#include <sega_spr.h>
#include <sega_scl.h>
#include "util.h"
#include "spr.h"
#include "print.h"

/* #include "font0.h" */
#include "font1.h"
#include "font2.h"

#ifdef JAPAN
#define MAXNMFONTS 4
#else
#define MAXNMFONTS 3
#endif

static uint8* fontList[] = { brianFont, brianFont, bigFont, NULL };

static sint16 charMap[MAXNMFONTS][256];
static sint8 widths[MAXNMFONTS][256];
static sint8 heights[MAXNMFONTS];

#ifdef JAPAN
#include "file.h"
#include "pic.h"

#define MAXNMJCHARS 240
static uint32* charData[MAXNMJCHARS];
static sint32 nmJChars;

void loadJapanFontData(sint32 fd)
{
    uint16* palData[MAXNMJCHARS];
    nmJChars = loadPicSet(fd, palData, charData, MAXNMJCHARS);
}

#define PICDATA(d) (((uint8*)d) + 8)
static sint32 firstJFontPic;
void loadJapanFontPics(void)
{
    sint32 i, res;
    for (i = 0; i < nmJChars; i++)
    {
        res = addPic(TILEJCHAR, PICDATA(charData[i]), NULL, 4);
        if (i == 0)
            firstJFontPic = res;
        widths[3][i] = 17;
    }
    heights[3] = 18;
    EZ_setLookupTbl(3, (struct sprLookupTbl*)(fontList[1] + 2));
}
#endif

sint32 initFonts(sint32 spriteNm, sint32 fontMask)
{
    sint32 f, i, x, y, c, fontc, fontHeight, widthBy8;
    uint8 buffer[32 * 32];

    fontMask &= ~1;

    for (f = 0; fontList[f]; f++)
    {
        i = fontMask & 1;
        fontMask = fontMask >> 1;
        if (!i)
            continue;
        heights[f] = (sint8)*(sint16*)fontList[f];
        fontHeight = heights[f];
        EZ_setLookupTbl(f, (struct sprLookupTbl*)(fontList[f] + 2));
        for (i = 0; i < 256; i++)
            widths[f][i] = *(fontList[f] + 2 + 32 + i);
        fontc = 0;
        for (i = 0; i < 256; i++)
        {
            if (widths[f][i] == 0)
            {
                charMap[f][i] = -1;
                continue;
            }
            c = 0;
            widthBy8 = (widths[f][i] + 7) & (~7);
            for (y = 0; y < fontHeight; y++)
            {
                for (x = 0; x < ((widths[f][i] + 1) >> 1); x++)
                    buffer[c++] = fontList[f][2 + 32 + 256 + fontc++];
                for (; x < (widthBy8 >> 1); x++)
                    buffer[c++] = 0;
            }
            EZ_setChar(spriteNm, COLOR_1, widthBy8, fontHeight, buffer);
            charMap[f][i] = spriteNm;
            spriteNm++;
        }
        /* if font does not have lower case chars, then map lowers to uppers */
        for (i = 'a'; i <= 'z'; i++)
            if (charMap[f][i] == -1)
            {
                charMap[f][i] = charMap[f][i + 'A' - 'a'];
                widths[f][i] = widths[f][i + 'A' - 'a'];
            }
        /* map accented chars */
        {
            static uint8* charEquiv[] = { "\355\315", "\372\332", "\351\311", NULL };
            for (i = 0; charEquiv[i]; i++)
            {
                sint32 one = charEquiv[i][0];
                sint32 two = charEquiv[i][1];
                if (charMap[f][one] == -1 && charMap[f][two] != -1)
                {
                    charMap[f][one] = charMap[f][two];
                    widths[f][one] = widths[f][two];
                }
                if (charMap[f][two] == -1 && charMap[f][one] != -1)
                {
                    charMap[f][two] = charMap[f][one];
                    widths[f][two] = widths[f][one];
                }
            }
        }
    }

    for (i = 0; i < 256; i++)
    {
        widths[0][i] = widths[1][i];
        charMap[0][i] = charMap[1][i];
    }
    heights[0] = heights[1];

    return spriteNm;
}

void drawString(sint32 x, sint32 y, sint32 font, uint8* text)
{
    XyInt pos;
    pos.x = x;
    pos.y = y;
    for (; *text; text++)
    {
        if (!widths[font][(sint32)*text])
        { /* assert(0); */
            continue;
        }
#ifdef JAPAN
        if (font == 3)
        {
            EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_REP, 3, mapPic((*text) + firstJFontPic), &pos, NULL);
        }
        else
#endif
            EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_REP, font, charMap[font][(sint32)*text], &pos, NULL);
        pos.x += widths[font][(sint32)*text] + 1;
    }
}

void drawStringN(sint32 x, sint32 y, sint32 font, uint8* text, sint32 n)
{
    XyInt pos;
    pos.x = x;
    pos.y = y;
    for (; n; n--, text++)
    {
        if (!widths[font][(sint32)*text])
        {
            dPrint("unknown character %d\n", (sint32)*text);
            assert(0);
            continue;
        }
        EZ_normSpr(DIR_NOREV, UCLPIN_ENABLE | ECD_DISABLE | COLOR_1 | COMPO_REP, font, charMap[font][(sint32)*text], &pos, NULL);
        pos.x += widths[font][(sint32)*text] + 1;
    }
}

void drawStringFixedPitch(sint32 x, sint32 y, sint32 font, uint8* text, sint32 pitch)
{
    XyInt pos;
    pos.x = x;
    pos.y = y;
    for (; *text; text++)
    {
        if (!widths[font][(sint32)*text])
        {
            assert(0);
            continue;
        }
        EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_REP, font, charMap[font][(sint32)*text], &pos, NULL);
        pos.x += pitch + 1;
    }
}

void drawStringBulge(sint32 x, sint32 y, sint32 font, sint32 buldgeCenter, uint8* text)
{
    XyInt pos;
    sint32 cnm, bright;
    struct gourTable gtable;
    pos.x = x;
    pos.y = y;
    for (cnm = 0; *text; text++, cnm++)
    {
        if (!widths[font][(sint32)*text])
        {
            assert(0);
            continue;
        }
        bright = 31 - 3 * abs(buldgeCenter - cnm);
        if (bright < 16)
            bright = 16;
        gtable.entry[0] = greyTable[bright];
        gtable.entry[1] = greyTable[bright];
        gtable.entry[2] = greyTable[bright];
        gtable.entry[3] = greyTable[bright];
        EZ_normSpr(DIR_NOREV, DRAW_GOURAU | ECD_DISABLE | COLOR_1 | COMPO_REP, font, charMap[font][(sint32)*text], &pos, &gtable);
        pos.x += widths[font][(sint32)*text] + 1;
    }
}

void drawStringGouro(sint32 x, sint32 y, sint32 font, uint16 gourTop, uint16 gourBot, uint8* text)
{
    XyInt pos;
    struct gourTable gtable;
    gtable.entry[0] = gourTop;
    gtable.entry[1] = gourTop;
    gtable.entry[2] = gourBot;
    gtable.entry[3] = gourBot;
    pos.x = x;
    pos.y = y;
    for (; *text; text++)
    {
        if (!widths[font][(sint32)*text])
        { /*assert(0);*/
            continue;
        }
#ifdef JAPAN
        if (font == 3)
        {
            EZ_normSpr(DIR_NOREV, DRAW_GOURAU | ECD_DISABLE | COLOR_1 | COMPO_REP, 3, mapPic((*text) + firstJFontPic), &pos, &gtable);
        }
        else
#endif
            EZ_normSpr(DIR_NOREV, DRAW_GOURAU | ECD_DISABLE | COLOR_1 | COMPO_REP, font, charMap[font][(sint32)*text], &pos, &gtable);
        pos.x += widths[font][(sint32)*text] + 1;
    }
}

void drawChar(sint32 x, sint32 y, sint32 font, uint8 text)
{
    XyInt pos;
    pos.x = x;
    pos.y = y;
    if (!widths[font][(sint32)text])
    { /*assert(0);*/
        return;
    }
#ifdef JAPAN
    if (font == 3)
    {
        EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_REP, 3, mapPic(text + firstJFontPic), &pos, NULL);
    }
    else
#endif
        EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_REP, font, charMap[font][(sint32)text], &pos, NULL);
}

void drawCharShadow(sint32 x, sint32 y, sint32 font, uint8 text)
{
    XyInt pos;
    pos.x = x;
    pos.y = y;
    if (!widths[font][(sint32)text])
    { /*assert(0);*/
        return;
    }
#ifdef JAPAN
    if (font == 3)
    {
        EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_REP | COMPO_SHADOW, 3, mapPic(text + firstJFontPic), &pos, NULL);
    }
    else
#endif
        EZ_normSpr(DIR_NOREV, ECD_DISABLE | COLOR_1 | COMPO_SHADOW, font, charMap[font][(sint32)text], &pos, NULL);
}

sint32 getStringWidth(sint32 font, const uint8* c)
{
    sint32 tot;
    for (tot = 0; *c; c++)
        tot += widths[font][(sint32)*c] + 1;
    return tot - 1;
}

sint32 getCharWidth(sint32 font, uint8 c)
{
    return widths[font][(sint32)c];
}

sint32 getFontHeight(sint32 font)
{
    return heights[font];
}
