#include <machine.h>
#include <sega_int.h>
#include <sega_per.h>
#include <sega_bup.h>
#include <stdio.h>

#include "v_blank.h"
#include "menu.h"
#include "util.h"
#include "local.h"
#include "gamestat.h"
#include "weapon.h"

void *bupSpace=NULL;
void *bupWork=NULL;

static BupConfig config[3];
typedef struct
{SaveState state;
 short valid;
} SaveRec;

#define NMSAVEGAMES 6

#define CHEATKEY "POWERCHEAT!"

#ifndef PAL

#ifdef JAPAN
/* #define FILENAME "_1999_ROTF_" */
#define FILENAME "BMG_1999___"
#else
#define FILENAME "POWERSLAVE1"
#endif

#else
#define FILENAME "__EXHUMED__"
#endif

#define SPACENEEDED ((NMSAVEGAMES*sizeof(SaveRec)+63)/64)

SaveRec saveGames[NMSAVEGAMES+4 /* extra space to absorb block fragment */];
static int saveDevice;
static int openGame=-1;

SaveState *bup_getGameData(int slot)
{if (saveDevice==-1)
    return NULL;
 assert(slot>=0);
 assert(slot<NMSAVEGAMES);
 if (saveGames[slot].valid)
    return &saveGames[slot].state;
 else
    return NULL;
}

static void loadBUP(void)
{assert(!bupSpace);
 assert(!bupWork);
 bupSpace=mem_malloc(0,16*1024);
 assert(bupSpace);
 bupWork=mem_malloc(0,8*1024);
 assert(bupWork);
 assert(config);
 resetDisable();
 BUP_Init(bupSpace,bupWork,config);
 resetEnable();
}

static void unloadBup(void)
{assert(bupWork);
 assert(bupSpace);
 mem_free(bupWork);
 mem_free(bupSpace);
 bupWork=NULL;
 bupSpace=NULL;
}

static void checkForCheatKey(void)
{int ret;
 ret=BUP_Read(0,CHEATKEY,(Uint8 *)saveGames);
 if (ret==0)
    cheatsEnabled=1;
 else
    cheatsEnabled=0;
}

static int loadGameFile(void)
{int device,i,j;
 device=-1;
 for (i=0;i<2;i++)
    {j=BUP_Read(i,FILENAME,(Uint8 *)saveGames);
     if (j==BUP_BROKEN)
	{if (dlg_runYesNo(getText(LB_BUP,12),200))
	    BUP_Delete(i,FILENAME);
	 else
	    SYS_EXECDMP();
	}
     if (j==0)
	{device=i;
	 break;
	}
    }
 return device;
}

static int saveGameFile(int device,int savingGame)
{int ret;
 BupDir writetb;
 BupDate datetb;
 assert(device>=0);
 strcpy((char *)writetb.filename,FILENAME);
 strcpy((char *)writetb.comment,"save games");
 writetb.language=BUP_ENGLISH;
 {int year,month,day,hour,min;
  getDateTime(&year,&month,&day,&hour,&min);
  datetb.year=year;
  datetb.month=month;
  datetb.day=day;
  datetb.time=hour;
  datetb.min=min;
 }
 writetb.date=BUP_SetDate(&datetb);
 writetb.datasize=SPACENEEDED*64;
 writetb.blocksize=SPACENEEDED;

#if 0
 if (savingGame)
    dlg_flashMessage(getText(LB_BUP,14),getText(LB_BUP,1),
		     300,50);
 else
    dlg_flashMessage(getText(LB_BUP,0),getText(LB_BUP,1),
		     300,50);
 vtimer=0;
 while (vtimer<120) ;
#endif

 resetDisable();
 ret=BUP_Write(device,&writetb,(Uint8 *)saveGames,OFF); 
 resetEnable();
 return ret;
}

int bup_canLoadGame(void)
{int i;
 if (saveDevice<0)
    return 0;
 for (i=0;i<NMSAVEGAMES;i++)
    if (saveGames[i].valid)
       return 1;
 return 0;
}

int bup_canSaveGame(void)
{return saveDevice>=0;
}

int bup_loadGame(int slot)
{assert(slot>=0 && slot<NMSAVEGAMES);
 currentState=saveGames[slot].state;
 openGame=slot;
 return 0;
}

void bup_saveGame(void)
{if (saveDevice<0)
    return;
 assert(openGame>=0);
 loadBUP();
 {int year,month,day,hour,min;
  getDateTime(&year,&month,&day,&hour,&min);
  currentState.year=year;
  currentState.month=month;
  currentState.day=day;
  currentState.hour=hour;
  currentState.min=min;
  dPrint("game date=%d/%d/%d\n",year,month,day);
 }
 saveGames[openGame].state=currentState;
 saveGameFile(saveDevice,1);
 unloadBup();
}

void bup_initCurrentGame(void)
{int i;
 /* initialize game state variables */
 currentState.inventory=INV_SWORD;
 currentState.gameFlags=/*GAMEFLAG_JUSTTELEPORTED*/ GAMEFLAG_FIRSTLEVEL;
 currentState.nmBowls=1;
 currentState.health=200;
 currentState.dolls=0;
 for (i=0;i<WP_NMWEAPONS;i++)
    currentState.weaponAmmo[i]=weaponMaxAmmo[i]; 
 for (i=0;i<NMLEVELS;i++)
    currentState.levFlags[i]=0;
 currentState.currentLevel=3;
 currentState.levFlags[3]=LEVFLAG_CANENTER;
 currentState.desiredWeapon=0;
 {int year,month,day,hour,min;
  getDateTime(&year,&month,&day,&hour,&min);
  currentState.year=year;
  currentState.month=month;
  currentState.day=day;
  currentState.hour=hour;
  currentState.min=min;
  dPrint("game date=%d/%d/%d\n",year,month,day);
 }
}


int bup_newGame(int slot)
{assert(slot>=0 && slot<NMSAVEGAMES);
 bup_initCurrentGame();
 if (saveGames[slot].valid)
    {if (!dlg_runYesNo(getText(LB_BUP,3),220))
	return 1;
     if (saveGames[slot].state.dolls==ALLDOLLS &&
	 (saveGames[slot].state.inventory & (INV_MUMMY|INV_TRANSMITTER))==
	 (INV_MUMMY|INV_TRANSMITTER))
	currentState.gameFlags|=GAMEFLAG_DOLLPOWERMODE;
    }
 saveGames[slot].state=currentState;
 saveGames[slot].valid=1;
 /* save game */
 openGame=slot;
 loadBUP();
 saveGameFile(saveDevice,0);
 unloadBup();
 return 0;
}

void bup_initialProc(void)
{BupStat sttb;
 int i;
 int device; /* = backup device of save file or -1 if not found */
 
 loadBUP();
 /* check to make sure all backup devices are formatted */
 if (BUP_Stat(0,SPACENEEDED*64,&sttb)==BUP_UNFORMAT)
    {/* must format */
     i=dlg_runYesNo(getText(LB_BUP,5),240);
     if (!i)
	{dlg_runMessage(getText(LB_BUP,6),270);
#ifdef JAPAN
	 dlg_runMessage(getText(LB_BUP,14),270);
#endif
	 SYS_EXECDMP();
	}
     resetDisable();
     if (BUP_Format(0))
	{dlg_runMessage(getText(LB_BUP,7),200);
	 SYS_EXECDMP();
	}
     resetEnable();
    }
 if (BUP_Stat(1,SPACENEEDED*64,&sttb)==BUP_UNFORMAT)
    {/* must format */
     i=dlg_runYesNo(getText(LB_BUP,8),240);
     if (!i)
	{dlg_runMessage(getText(LB_BUP,9),270);
#ifdef JAPAN
	 dlg_runMessage(getText(LB_BUP,15),270);
#endif
	 SYS_EXECDMP();
	}
     resetDisable();
     if (BUP_Format(1))
	{dlg_runMessage(getText(LB_BUP,10),240);
	 SYS_EXECDMP();
	}
     resetEnable();
    }

 /* try to find our backup record on the backup system. */
 checkForCheatKey();
 device=loadGameFile();
 saveDevice=device;

 if (device==-1)
    {/* try to make our save game record */
     for (i=0;i<NMSAVEGAMES;i++)
	saveGames[i].valid=0;
     for (device=0;device<2;device++)
	{if (BUP_Stat(device,SPACENEEDED*64,&sttb))
	    continue;
	 if (sttb.freeblock>=SPACENEEDED)
	    break;
	}
     if (device==2)
	{/* couldn't find a place for our save game file */
#ifndef JAPAN
	 char buff[512];
	 sprintf(buff,getText(LB_BUP,11),SPACENEEDED);
	 dlg_runMessage(buff,240);
#else
	 dlg_runMessage(getText(LB_BUP,11),240);
#endif
	}
     else
	{/* we found a place for our game file */
	 saveGameFile(device,0);
	 saveDevice=device;
	}     
    }
 unloadBup();
}





