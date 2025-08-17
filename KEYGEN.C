#include <machine.h>

#include <libsn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sega_spr.h"
#include "sega_scl.h"
#include "sega_int.h"
#include "sega_mth.h"
#include "sega_sys.h"
#include "sega_dbg.h"
#include "sega_per.h"
#include "sega_cdc.h"
#include "sega_snd.h"
#include "sega_gfs.h"
#include <sega_int.h>
#include <sega_per.h>
#include <sega_bup.h>
#include <stdio.h>

#include "picset.h"
#include "file.h"
#include "util.h"
#include "print.h"
#include "v_blank.h"
#include "sound.h"
#include "spr.h"
#include "mov.h"
#include "megainit.h"
#include "initmain.h"
#include "local.h"

void *bupSpace=NULL;
void *bupWork=NULL;

char saveGames[1024];
static BupConfig config[3];

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

void *camera;

static void bup_createKeyFile(char *filename)
{int ret;
 int device=0;
 BupStat sttb;
 BupDir writetb;
 BupDate datetb;
 loadBUP();
 BUP_Stat(0,64,&sttb);
 assert(device>=0);
 strcpy((char *)writetb.filename,filename);
 strcpy((char *)writetb.comment,"cheat key");
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
 writetb.datasize=64;
 writetb.blocksize=1;
 resetDisable();
 BUP_Write(device,&writetb,(Uint8 *)saveGames,OFF);
 resetEnable();
 unloadBup();
}

void main(void)
{mem_init();
 bup_createKeyFile("POWERCHEAT!");
}
