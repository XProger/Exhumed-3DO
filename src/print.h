sint32 initFonts(sint32 spriteNm, sint32 fontMask);
void drawString(sint32 x, sint32 y, sint32 font, uint8* text);
void drawStringFixedPitch(sint32 x, sint32 y, sint32 font, uint8* text, sint32 pitch);
void drawCharShadow(sint32 x, sint32 y, sint32 font, uint8 text);
void drawChar(sint32 x, sint32 y, sint32 font, uint8 text);
void drawStringGouro(sint32 x, sint32 y, sint32 font, uint16 gourTop, uint16 gourBot, uint8* text);
sint32 getStringWidth(sint32 font, const uint8* c);
sint32 getCharWidth(sint32 font, uint8 c);
sint32 getFontHeight(sint32 font);
void drawStringBulge(sint32 x, sint32 y, sint32 font, sint32 buldgeCenter, uint8* text);
void drawStringN(sint32 x, sint32 y, sint32 font, uint8* text, sint32 n);

#ifdef TODO
#define drawStringf(x, y, font, format, args...) \
    {                                            \
        char buff[160];                          \
        sprintf(buff, format, ##args);           \
        drawString(x, y, font, buff);            \
    }
#else
#define drawStringf(x, y, font, format, ...)
#endif

#ifdef JAPAN
void loadJapanFontPics(void);
void loadJapanFontData(sint32 fd);
#endif
