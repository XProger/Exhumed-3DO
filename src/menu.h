#ifndef __INCLUDEDmenuh
#define __INCLUDEDmenuh

#include "mth.h"

typedef struct
{
    sint32 type;
    fix32 xp, yp;
    sint16 x1, y1, x2, y2, xv, yv;
    sint32 w, h;
    sint32 color, code;
    char* text;
} DlgItem;

extern DlgItem dlgItem[];
extern uint16 lastUsedSelButton;

void dlg_clear(void);
void dlg_addRect(sint32 x, sint32 y, sint32 w, sint32 h, sint32 color);
void dlg_addText(sint32 x, sint32 y, sint32 w, char* text);
void dlg_addFontText(sint32 x, sint32 y, sint32 w, sint32 font, char* text);
void dlg_addCenteredText(sint32 x, sint32 y, sint32 w, char* text);
void dlg_addButton(sint32 pressCode, sint32 x, sint32 y, sint32 w, sint32 h, char* text);
void dlg_addBigWavyButton(sint32 pressCode, sint32 x, sint32 y, char* text);
void dlg_addGameButton(sint32 x, sint32 y, sint32 gameNm);
void dlg_addFontString(sint32 x, sint32 y, sint32 font, char* text);
#ifdef JAPAN
void dlg_addBigWavyJapaneseButton(sint32 pressCode, sint32 x, sint32 y, char* text);
#endif

void dlg_selectButton(sint32 b);
void dlg_centerStuff(void);

enum
{
    MENUMOVE_FREE,
    MENUMOVE_HORZ,
    MENUMOVE_VERT
};
sint32 dlg_run(sint32 selSound, sint32 pushSound, sint32 movement);
void dlg_runMessage(char* message, sint32 w);
void dlg_flashMessage(char* message1, char* message2, sint32 w, sint32 h);
sint32 dlg_runYesNo(char* message, sint32 w);
void runInventory(sint32 inventory, sint32 keyMask, sint32* mapState, sint32 fade, sint32 fadeButton, sint32 fadeSelection);

void dlg_init(sint32 fd);

void dlg_setupSlideIn(void);
void dlg_setupSlideOut(void);
void dlg_setupSlideUp(void);
void dlg_setupNoSlide(void);
void dlg_runSlideIn(void);

sint32 runTravelQuestion(char* destination);

#endif
