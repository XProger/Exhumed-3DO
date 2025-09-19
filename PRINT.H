int initFonts(int spriteNm,int fontMask);
void drawString(int x,int y,int font,unsigned char *text);
void drawStringFixedPitch(int x,int y,int font,unsigned char *text,
			  int pitch);
void drawCharShadow(int x,int y,int font,unsigned char text);
void drawChar(int x,int y,int font,unsigned char text);
void drawStringGouro(int x,int y,int font,unsigned short gourTop,
		     unsigned short gourBot,
		     unsigned char *text);
int getStringWidth(int font,const unsigned char *c);
int getCharWidth(int font,unsigned char c);
int getFontHeight(int font);
void drawStringBulge(int x,int y,int font,int buldgeCenter,
		     unsigned char *text);
void drawStringN(int x,int y,int font,unsigned char *text,int n);

#ifdef TODO
#define drawStringf(x,y,font,format,args...) \
   {char buff[160]; \
    sprintf(buff,format, ## args); \
    drawString(x,y,font,buff);}
#else
#define drawStringf(x,y,font,format,...)
#endif

#ifdef JAPAN
void loadJapanFontPics(void);
void loadJapanFontData(int fd);
#endif
