#ifndef __INCLUDEDlocalh
#define __INCLUDEDlocalh

enum
{
    LB_PROMPTS,
    LB_BUP,
    LB_LEVELNAMES,
    LB_ITEMMESSAGE,
    LB_MAINMENU,
    LB_OPTIONMENU,
    LB_INVBUTTONS,
    LB_INVTEXT,
    LB_ACTIONNAMES,
    LB_CREDITS
};

char* getText(sint32 block, sint32 string);
void loadLocalText(sint32 fd);

sint32 getLanguageNumber(void);

#endif
