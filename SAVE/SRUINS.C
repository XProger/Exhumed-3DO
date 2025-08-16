/* stuff to do:

     poison effect
     
     problem: water surface walls can be near clipped out when still visible,
     and never get to the water specific routines

     fix triangle problem

*/


#define STATUSTEXT


#include <machine.h>

#include <libsn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sega_spr.h>
#include <sega_scl.h>
#include <sega_int.h>
#include <sega_mth.h>
#include <sega_sys.h>
#include <sega_dbg.h>
#include <sega_per.h>
#include <sega_cdc.h>
#include <sega_gfs.h>
#include <sega_snd.h>

#include "util.h"
#include "spr.h"
#include "file.h"
#include "v_blank.h"
#include "level.h"
#include "art.h"
#include "print.h"
#include "sprite.h"
#include "map.h"
#include "pic.h"
#include "walls.h"
#include "sequence.h"
#include "sound.h"
#include "object.h"
#include "ai.h"
#include "hitscan.h"
#include "route.h"
#include "menu.h"
#include "bup.h"
#include "logo.h"
#include "megainit.h"
#include "weapon.h"
#include "dma.h"
#include "local.h"
#include "bigmap.h"
#include "profile.h"
#include "gamestat.h"
#include "intro.h"
#include "mov.h"
#include "plax.h"
#include "airbub.c"

extern int end;

SaveState currentState;

PlayerObject *player;
Sprite *camera;

Orient playerAngle;

int mapOn;
int quitRequest;

int debugFlag=0;

Fixed32 xavel = 0, yavel = 0;

#define MAXTVELOCITY 130000
#define MAXRTVELOCITY 200000
#define VELOCITY 48
#define RUNVELOCITY (VELOCITY*2)
#define TURNVELOCITY 10000
#define RTURNVELOCITY 12000
#define RTURNFRICTION 15000
#define TURNFRICTION 65000
#define SANDALJUMPVEL (60<<13)
#define NORMALJUMPVEL (39<<13)

/* game status variables */
static int nmFullBowls=0;
static int keyMask=0;

static int playerIsDead;
static int playerMotionEnable;
static int hitCamel,hitPyramid,hitTeleport;
static int stunCounter=0;

static int deathTimer;
static char screaming=0;

void redrawBowlDots(void);

static int colorOffset[3]={0,0,0};
static int colorCenter[3]={0,0,0};
static int colorStepRate=3;

void stepColorOffset(void)
{int i;
 for (i=0;i<3;i++)
    {if (colorOffset[i]-colorStepRate>colorCenter[i])
	colorOffset[i]-=colorStepRate;
     else
	if (colorOffset[i]+colorStepRate<colorCenter[i])
	   colorOffset[i]+=colorStepRate;
	else
	   {colorOffset[i]=colorCenter[i];
	    colorStepRate=3;
	   }
    }
 SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0|SCL_RBG0,
		  colorOffset[0],colorOffset[1],colorOffset[2]);
}

void changeColorOffset(int r,int g,int b,int rate)
{colorOffset[0]=r;
 colorOffset[1]=g;
 colorOffset[2]=b; 
 colorStepRate=rate;
}

void addColorOffset(int r,int g,int b)
{colorOffset[0]+=r;
 colorOffset[1]+=g;
 colorOffset[2]+=b; 
}

static int ouchTime=0;
void playerHurt(int hpLost)
{int i;
 if (playerIsDead)
    return;
 currentState.health-=hpLost;
 colorOffset[0]=63;
 colorOffset[1]=-63;
 colorOffset[2]=-63;
 if (ouchTime<=0)
    {do
	i=getNextRand()&0x3;
     while (i>2);	 
     playStaticSound(ST_JOHN,3+i);
     ouchTime=60;
    }
}

static int ltHurtAmount=0;
static int ltHurtTime=0;
void playerLongHurt(int amount,int time,int lava)
{ltHurtTime=time;
 ltHurtAmount=amount;
 if (currentState.inventory & INV_ANKLET)
    {if (lava)
	ltHurtAmount>>=1;
     else
	ltHurtAmount=0;
    }
}

static void greenFlash(void)
{colorOffset[1]+=32;
}

void switchPlayerMotion(int state)
{playerMotionEnable=state;
}

int playerHeightOffset=0,playerHeightVel=0;

static void stepPlayerHeight(void)
{if (playerIsDead)
    {if (playerHeightOffset>F(31) &&
	 abs(playerHeightVel)<1<<15)
	return;
     playerHeightOffset+=playerHeightVel;
     playerHeightVel+=1<<14;
     if (playerHeightOffset>F(32))
	{playerHeightOffset=F(32);
	 playerHeightVel=-playerHeightVel>>2;
	}
     return;
    }
 playerHeightOffset+=playerHeightVel;
 playerHeightVel-=playerHeightOffset>>3;
 weaponForce(0,playerHeightOffset>>4);
 playerHeightVel=MTH_Mul(playerHeightVel,65536*0.6);
 if (abs(playerHeightVel)<1<<13 &&
     abs(playerHeightOffset)<1<<13)
    {playerHeightVel=0;
     playerHeightOffset=0;
    }
}

void playerDYChange(Fixed32 dvel)
{int ouch;
 if (dvel<F(1))
    return;
 if (dvel>F(15))
    {if (screaming)
	stopAllSound(0);
     screaming=0;
     ouch=currentState.nmBowls*
	f(MTH_Mul(dvel-F(12),dvel-F(12)));
     playerHurt(ouch);
    }
 playerHeightVel+=dvel;
/* weaponForce(0,dvel>>1);*/
}

#if 0
void underWaterControl(unsigned short input)
{Fixed32 dir = 0, vel, tvel;
 Fixed32 maxTVel,walking;
 MthXyz ray;
 static int swimTime=0;
 int run,turningLeft=0,turningRight=0,turningUp=0,turningDown=0;
 MthXyz force;
 force.x=0; force.y=0; force.z=0;
 dir=playerAngle.yaw;
 vel=VELOCITY;
 tvel=TURNVELOCITY;
 maxTVel=MAXTVELOCITY;
 run=0;
 walking=0;
 if ((input & IMASK(ACTION_RUN)) == 0)
    {vel = RUNVELOCITY;
     tvel=RTURNVELOCITY;
     maxTVel=MAXRTVELOCITY;
     run=1;
    }
 vel=vel>>1;
 
 if ((input & PER_DGT_D) == 0)
    {xavel += TURNVELOCITY;
     if (xavel>maxTVel) xavel=maxTVel;
     turningUp=1;
    }
 else
    if (xavel>0)
       {if (xavel>=TURNFRICTION) 
	   xavel-=TURNFRICTION;
       else
	  xavel=0;
       }
 if ((input & PER_DGT_U) == 0)
    {xavel -= TURNVELOCITY;
     if (xavel<-maxTVel) xavel=-maxTVel;
     turningDown=1;
    }
 else
    {if (xavel<0)
	{if (xavel<=TURNFRICTION) 
	    xavel+=TURNFRICTION;
	else
	   xavel=0;
	}
    }


 if ((input & PER_DGT_R) == 0)
    if (input & IMASK(ACTION_STRAFE))
       {yavel -= tvel;
	if (run)
	   weaponForce(-1<<15,0);
	if( yavel < -maxTVel) yavel = -maxTVel;
	turningRight=1;
       }
    else
       {force.x+=(MTH_Cos(dir) * vel)>>2;
	force.z+=(MTH_Sin(dir) * vel)>>2;
       }
 if (!turningRight)
    if (yavel<0)
       if (run)
	  {if (yavel<=-RTURNFRICTION) 
	      yavel+=RTURNFRICTION;
	  else
	     yavel=0;
	  }
       else
	  {if (yavel<=-RTURNFRICTION) 
	      yavel+=RTURNFRICTION;
	  else
	     yavel=0;
	  }
 
 if ((input & PER_DGT_L) == 0)
    if ((input & IMASK(ACTION_STRAFE)) == 0)
       {force.x += (MTH_Cos( dir ) * -vel)>>2;
	force.z += (MTH_Sin( dir ) * -vel)>>2;
       }
    else
       {yavel += tvel;
	if (run)
	   weaponForce(1<<15,0);
	if( yavel > maxTVel) yavel = maxTVel;
	turningLeft=1;
       }

 if (!turningLeft)
    {if (yavel>0)
	if (run)
	   {if (yavel>=RTURNFRICTION) 
	       yavel-=RTURNFRICTION;
	   else
	      yavel=0;
	   }
	else
	   {if (yavel>=RTURNFRICTION) 
	       yavel-=RTURNFRICTION;
	   else
	      yavel=0;
	   }
    }

 if ((input & IMASK(ACTION_JUMP)) == 0)
    {ray.x=-MTH_Sin(playerAngle.yaw); ray.y=0; ray.z=MTH_Cos(playerAngle.yaw);
     ray.x=MTH_Mul(ray.x,MTH_Cos(playerAngle.pitch));
     ray.z=MTH_Mul(ray.z,MTH_Cos(playerAngle.pitch));
     ray.y=MTH_Sin(playerAngle.pitch);     
     force.x=ray.x<<4;
     force.y=ray.y<<4;
     force.z=ray.z<<4;

     swimTime++;
#if 1
     if (swimTime>30)
	{force.x=ray.x<<2;
	 force.y=ray.y<<2;
	 force.z=ray.z<<2;
	}
#endif
     if (swimTime>50)
	swimTime=0;
    }
 else
    swimTime=0;

 if (force.x || force.y || force.z)
    {camera->vel.x=(31*camera->vel.x+force.x)>>5;
     camera->vel.y=(31*camera->vel.y+force.y)>>5;
     camera->vel.z=(31*camera->vel.z+force.z)>>5;
    }

 if (camera->vel.y<-160<<15)
    camera->vel.y=-160<<15;

 playerAngle.roll=-((Fixed32)yavel)<<2;

 if (!(level_sector[camera->s].flags & SECFLAG_WATER))
    {int d;
     playerAngle.roll=0;
     d=findFloorDistance(camera->s,&camera->pos);
     if (d<F(14))
	camera->pos.y+=F(1);
     if (camera->vel.y>0)
	camera->vel.y=0;

     if (playerAngle.pitch>0)
	{if (xavel>0)
	    xavel=0;
	 playerAngle.pitch-=F(4);
	 if (playerAngle.pitch<0)
	    playerAngle.pitch=0;
	}
    }

}
#else
void underWaterControl(unsigned short input)
{Fixed32 dir = 0, vel, tvel;
 Fixed32 maxTVel,walking;
 MthXyz ray;
 static int swimTime=0;
 int run,turningLeft=0,turningRight=0,turningUp=0,turningDown=0;
 MthXyz force;
 force.x=0; force.y=0; force.z=0;
 dir=playerAngle.yaw;
 walking=0;
 vel = VELOCITY;
 tvel=TURNVELOCITY;
 maxTVel=MAXTVELOCITY;
 run=0;
 vel=vel>>1;
 
 if ((input & PER_DGT_D) == 0)
    {xavel += TURNVELOCITY;
     if (xavel>maxTVel) xavel=maxTVel;
     turningUp=1;
    }
 else
    if (xavel>0)
       {if (xavel>=TURNFRICTION) 
	   xavel-=TURNFRICTION;
       else
	  xavel=0;
       }
 if ((input & PER_DGT_U) == 0)
    {xavel -= TURNVELOCITY;
     if (xavel<-maxTVel) xavel=-maxTVel;
     turningDown=1;
    }
 else
    {if (xavel<0)
	{if (xavel<=TURNFRICTION) 
	    xavel+=TURNFRICTION;
	else
	   xavel=0;
	}
    }

 if (input & IMASK(ACTION_STRAFE))
    {force.x += (MTH_Cos( dir ) * 60)>>2;
     force.z += (MTH_Sin( dir ) * 60)>>2;
    }
 if (input & IMASK(ACTION_RUN))
    {force.x += (MTH_Cos( dir ) * -60)>>2;
     force.z += (MTH_Sin( dir ) * -60)>>2;
    }

 if ((input & PER_DGT_R) == 0)
    {yavel -= tvel;
     if (run)
	weaponForce(-1<<15,0);
     if( yavel < -maxTVel) yavel = -maxTVel;
     turningRight=1;
    }
 if (!turningRight)
    if (yavel<0)
       if (run)
	  {if (yavel<=-RTURNFRICTION) 
	      yavel+=RTURNFRICTION;
	  else
	     yavel=0;
	  }
       else
	  {if (yavel<=-RTURNFRICTION) 
	      yavel+=RTURNFRICTION;
	  else
	     yavel=0;
	  }
 
 if ((input & PER_DGT_L) == 0)
    {yavel += tvel;
     if (run)
	weaponForce(1<<15,0);
     if( yavel > maxTVel) yavel = maxTVel;
     turningLeft=1;
    }

 if (!turningLeft)
    {if (yavel>0)
	if (run)
	   {if (yavel>=RTURNFRICTION) 
	       yavel-=RTURNFRICTION;
	   else
	      yavel=0;
	   }
	else
	   {if (yavel>=RTURNFRICTION) 
	       yavel-=RTURNFRICTION;
	   else
	      yavel=0;
	   }
    }

 if ((input & IMASK(ACTION_JUMP)) == 0)
    {ray.x=-MTH_Sin(playerAngle.yaw); ray.y=0; ray.z=MTH_Cos(playerAngle.yaw);
     ray.x=MTH_Mul(ray.x,MTH_Cos(playerAngle.pitch));
     ray.z=MTH_Mul(ray.z,MTH_Cos(playerAngle.pitch));
     ray.y=MTH_Sin(playerAngle.pitch);     
     force.x=ray.x<<4;
     force.y=ray.y<<4;
     force.z=ray.z<<4;

     swimTime++;
#if 1
     if (swimTime>30)
	{force.x=ray.x<<2;
	 force.y=ray.y<<2;
	 force.z=ray.z<<2;
	}
#endif
     if (swimTime>50)
	swimTime=0;
    }
 else
    swimTime=0;

 if (force.x || force.y || force.z)
    {camera->vel.x=(31*camera->vel.x+force.x)>>5;
     camera->vel.y=(31*camera->vel.y+force.y)>>5;
     camera->vel.z=(31*camera->vel.z+force.z)>>5;
    }

 if (camera->vel.y<-160<<15)
    camera->vel.y=-160<<15;

 playerAngle.roll=-((Fixed32)yavel)<<2;

 if (!(level_sector[camera->s].flags & SECFLAG_WATER))
    {int d;
     playerAngle.roll=0;
     d=findFloorDistance(camera->s,&camera->pos);
     if (d<F(14))
	camera->pos.y+=F(1);
     if (camera->vel.y>0)
	camera->vel.y=0;

     if (playerAngle.pitch>0)
	{if (xavel>0)
	    xavel=0;
	 playerAngle.pitch-=F(4);
	 if (playerAngle.pitch<0)
	    playerAngle.pitch=0;
	}
    }

}
#endif

#if 0
void controlInput(unsigned short input,unsigned short changeInput)
{Fixed32 dir = 0, vel, tvel;
 Fixed32 maxTVel,walking;
 static int walkTime=0,hoverTime=0;
 static int shawlActive=0;
 static int dirChange=0;
 int run,turningLeft=0,turningRight=0,turningUp=0,turningDown=0;
 MthXyz force;

 force.x=0; force.y=0; force.z=0;
 dir=playerAngle.yaw;
 vel=VELOCITY;
 tvel=TURNVELOCITY;
 maxTVel=MAXTVELOCITY;
 run=0;
 walking=0;
 if ((input & IMASK(ACTION_RUN)) == 0)
    {vel=RUNVELOCITY;
     tvel=RTURNVELOCITY;
     maxTVel=MAXRTVELOCITY;
     run=1;
    }
 if (dirChange==3)
    {if (abs(playerAngle.pitch)<F(1))
	{playerAngle.pitch=0;
	 dirChange=0;
	}
     else
	{playerAngle.pitch-=playerAngle.pitch>>3;
	 if (playerAngle.pitch<0)
	    playerAngle.pitch+=1<<14;
	 else
	    playerAngle.pitch-=1<<14;
	}
    }
 if ((input & IMASK(ACTION_FREELOC)) == 0)
    {if (!dirChange || dirChange==3)
	dirChange=1;
     if ((input & PER_DGT_D) == 0)
	{xavel += TURNVELOCITY;
	 if( xavel > maxTVel) xavel = maxTVel;
	 dirChange=2;
	 turningUp=1;
	}
     if ((input & PER_DGT_U) == 0)
	{xavel -= TURNVELOCITY;
	 if( xavel < -maxTVel) xavel = -maxTVel;
	 dirChange=2;
	 turningDown=1;
	}
    }
 else
    {if (dirChange && dirChange!=3)
	{if (dirChange==1)
	   dirChange=3;
	 else
	   dirChange=0;
	}
     if ((input & PER_DGT_U) == 0)
	{force.x += (MTH_Sin( dir ) * -vel)>>2;
	 force.z += (MTH_Cos( dir ) * vel)>>2;
	 walking=1;
	}
     if ((input & PER_DGT_D) == 0)
	{force.x += (MTH_Sin( dir ) * vel)>>2;
	 force.z += (MTH_Cos( dir ) * -vel)>>2;
	 walking=1;
	}
    }

 if (!turningUp)
    {if (xavel>0)
	{if (xavel>=TURNFRICTION) 
	    xavel-=TURNFRICTION;
	else
	   xavel=0;
	}
    }
 if (!turningDown)
    {if (xavel<0)
	{if (xavel<=TURNFRICTION) 
	    xavel+=TURNFRICTION;
	else
	   xavel=0;
	}
    }


 if ((input & PER_DGT_R) == 0)
    if (input & IMASK(ACTION_STRAFE))
       {yavel -= tvel;
	if (run)
	   weaponForce(-1<<15,0);
	if( yavel < -maxTVel) yavel = -maxTVel;
	turningRight=1;
       }
    else
       {force.x += (MTH_Cos( dir ) * vel)>>2;
	force.z += (MTH_Sin( dir ) * vel)>>2;
       }
 if (!turningRight)
    if (yavel<0)
       if (run)
	  {if (yavel<=-RTURNFRICTION) 
	      yavel+=RTURNFRICTION;
	  else
	     yavel=0;
	  }
       else
	  {if (yavel<=-TURNFRICTION) 
	      yavel+=TURNFRICTION;
	  else
	     yavel=0;
	  }
 
 if ((input & PER_DGT_L) == 0)
    if ((input & IMASK(ACTION_STRAFE)) == 0)
       {force.x += (MTH_Cos( dir ) * -vel)>>2;
	force.z += (MTH_Sin( dir ) * -vel)>>2;
       }
    else
       {yavel += tvel;
	if (run)
	   weaponForce(1<<15,0);
	if( yavel > maxTVel) yavel = maxTVel;
	turningLeft=1;
       }

 if (!turningLeft)
    {if (yavel>0)
	if (run)
	   {if (yavel>=RTURNFRICTION) 
	       yavel-=RTURNFRICTION;
	   else
	      yavel=0;
	   }
	else
	   {if (yavel>=TURNFRICTION) 
	       yavel-=TURNFRICTION;
	   else
	      yavel=0;
	   }
    }

 if (((input & IMASK(ACTION_JUMP)) == 0) && 
     !(camera->flags & SPRITEFLAG_ONSLIPPERYSLOPE))
    {camera->vel.y+=3<<12;
     if (changeInput & IMASK(ACTION_JUMP))
	if (camera->floorSector!=-1)
	   {if (currentState.inventory & INV_SANDALS)
	       camera->vel.y+=62<<13; /* these used to be ='s */
	    else
	       camera->vel.y+=41<<13;
	    playStaticSound(ST_JOHN,0);
	   }
	else
	   {if (currentState.inventory & (INV_SHAWL|INV_FEATHER))
	       {if (screaming)
		   {screaming=0;
		    stopAllSound(0);
		    playStaticSound(ST_JOHN,0);
		   }
		shawlActive=1;
		hoverTime=0;
	       }
	   }
    }
 else
    shawlActive=0;

 if (!(camera->flags & SPRITEFLAG_ONSLIPPERYSLOPE))
    if (force.x || force.z)
       {camera->vel.x=(15*camera->vel.x+force.x)>>4;
	camera->vel.z=(15*camera->vel.z+force.z)>>4;
       }
 
 if (shawlActive)
    {if (currentState.inventory & INV_FEATHER)
	{if (camera->vel.y<camera->gravity)
	    {playerHeightVel+=camera->gravity-camera->vel.y;
	     camera->vel.y=camera->gravity;
	     hoverTime+=3;
	     if (hoverTime>360)
		hoverTime=0;
	     playerHeightVel+=MTH_Sin(F(hoverTime-180))>>2;
	    }
	}
     else
	if (camera->vel.y<-4<<15)
	   camera->vel.y=-4<<15;
    }
 else
    if (camera->vel.y<F(-80))
       {camera->vel.y=F(-80);
       }

 if (camera->vel.y<F(-15))
    {if (!screaming)
	playStaticSound(ST_JOHN,6);
     screaming++;
    }

 if (walking)
    {walkTime++;
     if (walkTime>20)
	weaponForce(0,1<<14);
     if (walkTime>40)
	walkTime=0;
    }
 else
    walkTime=0;
 if (run)
    playerAngle.roll=-((Fixed32)yavel);
 else
    playerAngle.roll=0;

}
#else
void controlInput(unsigned short input,unsigned short changeInput)
{Fixed32 dir = 0, vel, tvel;
 Fixed32 maxTVel,walking;
 static int walkTime=0,hoverTime=0;
 static int shawlActive=0;
 static int dirChange=0;
 int turningLeft=0,turningRight=0,turningUp=0,turningDown=0;
 MthXyz force;

 force.x=0; force.y=0; force.z=0;
 dir=playerAngle.yaw;
 walking=0;
 vel=RUNVELOCITY;
 tvel=RTURNVELOCITY;
 maxTVel=MAXRTVELOCITY;

 if (dirChange==3)
    {if (abs(playerAngle.pitch)<F(1))
	{playerAngle.pitch=0;
	 dirChange=0;
	}
     else
	{playerAngle.pitch-=playerAngle.pitch>>3;
	 if (playerAngle.pitch<0)
	    playerAngle.pitch+=1<<14;
	 else
	    playerAngle.pitch-=1<<14;
	}
    }
 if ((input & IMASK(ACTION_FREELOC)) == 0)
    {if (!dirChange || dirChange==3)
	dirChange=1;
     if ((input & PER_DGT_D) == 0)
	{xavel += TURNVELOCITY;
	 if( xavel > maxTVel) xavel = maxTVel;
	 dirChange=2;
	 turningUp=1;
	}
     if ((input & PER_DGT_U) == 0)
	{xavel -= TURNVELOCITY;
	 if( xavel < -maxTVel) xavel = -maxTVel;
	 dirChange=2;
	 turningDown=1;
	}
    }
 else
    {if (dirChange && dirChange!=3)
	{if (dirChange==1)
	   dirChange=3;
	 else
	   dirChange=0;
	}
     if ((input & PER_DGT_U) == 0)
	{force.x += (MTH_Sin( dir ) * -vel)>>2;
	 force.z += (MTH_Cos( dir ) * vel)>>2;
	 walking=1;
	}
     if ((input & PER_DGT_D) == 0)
	{force.x += (MTH_Sin( dir ) * vel)>>2;
	 force.z += (MTH_Cos( dir ) * -vel)>>2;
	 walking=1;
	}
    }

 if (!turningUp)
    {if (xavel>0)
	{if (xavel>=TURNFRICTION) 
	    xavel-=TURNFRICTION;
	else
	   xavel=0;
	}
    }
 if (!turningDown)
    {if (xavel<0)
	{if (xavel<=TURNFRICTION) 
	    xavel+=TURNFRICTION;
	else
	   xavel=0;
	}
    }

 if (input & IMASK(ACTION_STRAFE))
    {force.x += (MTH_Cos( dir ) * 60)>>2;
     force.z += (MTH_Sin( dir ) * 60)>>2;
    }
 if (input & IMASK(ACTION_RUN))
    {force.x += (MTH_Cos( dir ) * -60)>>2;
     force.z += (MTH_Sin( dir ) * -60)>>2;
    }

 if ((input & PER_DGT_R) == 0)
    {yavel -= tvel;
     weaponForce(-1<<15,0);
     if( yavel < -maxTVel) yavel = -maxTVel;
     turningRight=1;
    }

 if (!turningRight)
    if (yavel<0)
       {if (yavel<=-RTURNFRICTION) 
	   yavel+=RTURNFRICTION;
       else
	  yavel=0;
       }
 
 if ((input & PER_DGT_L) == 0)
    {yavel += tvel;
     weaponForce(1<<15,0);
     if( yavel > maxTVel) yavel = maxTVel;
     turningLeft=1;
    }

 if (!turningLeft)
    {if (yavel>0)
	if (yavel>=RTURNFRICTION) 
	   yavel-=RTURNFRICTION;
	else
	   yavel=0;
    }

 if (((input & IMASK(ACTION_JUMP)) == 0) && 
     !(camera->flags & SPRITEFLAG_ONSLIPPERYSLOPE))
    {camera->vel.y+=3<<12;
     if (changeInput & IMASK(ACTION_JUMP))
	if (camera->floorSector!=-1)
	   {if (currentState.inventory & INV_SANDALS)
	       camera->vel.y=SANDALJUMPVEL;
	    else
	       camera->vel.y=NORMALJUMPVEL;
	    playStaticSound(ST_JOHN,0);
	    weaponForce(0,1<<17);
	   }
	else
	   {if (currentState.inventory & (INV_SHAWL|INV_FEATHER))
	       {if (screaming)
		   {screaming=0;
		    stopAllSound(0);
		    playStaticSound(ST_JOHN,0);
		   }
		shawlActive=1;
		hoverTime=0;
	       }
	   }
    }
 else
    shawlActive=0;

 if (!(camera->flags & SPRITEFLAG_ONSLIPPERYSLOPE))
    if (force.x || force.z)
       {camera->vel.x=(15*camera->vel.x+force.x)>>4;
	camera->vel.z=(15*camera->vel.z+force.z)>>4;
       }
 
 if (shawlActive)
    {if (currentState.inventory & INV_FEATHER)
	{if (camera->vel.y<camera->gravity)
	    {playerHeightVel+=camera->gravity-camera->vel.y;
	     camera->vel.y=camera->gravity;
	     hoverTime+=3;
	     if (hoverTime>360)
		hoverTime=0;
	     playerHeightVel+=MTH_Sin(F(hoverTime-180))>>2;
	    }
	}
     else
	if (camera->vel.y<-4<<15)
	   camera->vel.y=-4<<15;
    }
 else
    if (camera->vel.y<F(-80))
       {camera->vel.y=F(-80);
       }

 if (camera->vel.y<F(-15))
    {if (!screaming)
	playStaticSound(ST_JOHN,6);
     screaming++;
    }

 if (walking)
    {walkTime++;
     if (walkTime>20)
	weaponForce(0,1<<14);
     if (walkTime>40)
	walkTime=0;
    }
 else
    walkTime=0;
 playerAngle.roll=-((Fixed32)yavel);
}
#endif

static void push(void)
{int hscan;
 MthXyz pos,ray,collidePos;
 Fixed32 yaw,pitch;
 int sector;

 pos=camera->pos;
 yaw=playerAngle.yaw;
 pitch=playerAngle.pitch;
 ray.x=-MTH_Sin(yaw); ray.y=0; ray.z=MTH_Cos(yaw);
 ray.x=MTH_Mul(ray.x,MTH_Cos(pitch));
 ray.z=MTH_Mul(ray.z,MTH_Cos(pitch));
 ray.y=MTH_Sin(pitch);     
 hscan=hitScan(camera,&ray,&pos,camera->s,&collidePos,&sector);

 if ((hscan & COLLIDE_WALL) &&
     approxDist(camera->pos.x-collidePos.x,
		camera->pos.y-collidePos.y,
		camera->pos.z-collidePos.z)<F(120))
    {/* we hit a wall */
     dPrint("hit wall %d\n",hscan&0xffff);
     if (level_wall[hscan & 0xffff].object)
	{signalObject((Object *)(level_wall[hscan & 0xffff].object),
		      SIGNAL_PRESS,(int)(&collidePos),0);
	}
    }
}
   
void movePlayer(int inputEnd,int nmFrames)
{unsigned short input;
 unsigned short changeInput=0;
 unsigned short pushed;
 static unsigned short lastInput=0;
 static char wasUnderWater=0;
 int inputPos;
 inputPos=inputEnd-nmFrames;
 if (inputPos<0) inputPos+=INPUTQSIZE;
 while (inputPos!=inputEnd)
    {/* control input */
     input=inputQ[inputPos];
     changeInput=lastInput ^ input;
     pushed=changeInput&~input;
     if (!playerMotionEnable || stunCounter)
	{if (stunCounter)
	    stunCounter--;
	 if (playerIsDead)
	    {currentState.health=0;
	     if (playerAngle.pitch<F(80))
		{xavel+=1<<12;
		 playerAngle.pitch += xavel;
		 if (playerAngle.pitch>F(90)) playerAngle.pitch=F(90);
		 if (playerAngle.pitch<F(-90)) playerAngle.pitch=F(-90);
		}
	     else
		xavel=0;
	    }
	}
     else
	{if (camera->flags & SPRITEFLAG_UNDERWATER)
	    {underWaterControl(input);
	     wasUnderWater=5;
	     if ((level_sector[camera->s].flags & SECFLAG_WATER) &&
		 (getNextRand())<1500)
		{/* decide which sector to put bubble in */
		 int s=camera->s;
		 int i,j,k;
		 /* start in the player's sector and random walk away */
		 for (i=0;i<10;i++)
		    {k=level_sector[s].firstWall;
		     for (j=getNextRand()&0xf;j>=0;j--)
			{k++;
			 if (k>level_sector[s].lastWall ||
			     level_wall[k].nextSector==-1)
			    k=level_sector[s].firstWall;
			}
		     if (level_sector[level_wall[k].nextSector].flags &
			 SECFLAG_WATER)
			s=level_wall[k].nextSector;
		     assert(s>=0);
		    }		 
		 /* now s=sector to put bubble in */
		 {MthXyz pos,v;
		  pos.x=F(level_sector[s].center[0]);
		  pos.y=F(level_sector[s].center[1]);
		  pos.z=F(level_sector[s].center[2]);
		  k=level_sector[s].firstWall;
		  for (j=getNextRand()&0xf;j>=0;j--)
		     {k++;
		      if (k>level_sector[s].lastWall)
			 k=level_sector[s].firstWall;
		     }
		  getVertex(level_wall[k].v[0],&v);		  
		  pos.x=(pos.x+v.x)>>1;
		  pos.z=(pos.z+v.z)>>1;
		  pos.y-=findFloorDistance(s,&pos);
		  constructBubble(s,&pos,findCeilDistance(s,&pos));
		 }
		}
	    }
	else
	   {if (wasUnderWater)
	       {wasUnderWater--;
		input&=~PER_DGT_U;
		camera->vel.y+=(int)(0.45*65536); 
	       }		
	    controlInput(input,changeInput);
	   }
	 if (currentState.health<=0)
	    {playerIsDead=1;	     
	     colorCenter[0]=-255;
	     colorCenter[1]=-255;
	     colorCenter[2]=-255;
	     
	     playStaticSound(ST_JOHN,1);
	     switchPlayerMotion(0);
	     switchWeapons(0);
	     xavel=0;
	     yavel=0;
	     camera->vel.y+=F(6);
	     camera->vel.x+=MTH_Sin(playerAngle.yaw)*5;	     
	     camera->vel.z-=MTH_Cos(playerAngle.yaw)*5;
	     deathTimer=0;
	    }
	 
	 if (!(input & IMASK(ACTION_FIRE)))
	    {if (weaponSequenceQEmpty())
		fireWeapon();
	    }
	 
	 if (pushed&IMASK(ACTION_WEPUP))
	    {weaponUp(WEAPONINV(currentState.inventory));
	     redrawBowlDots();
	    }
	 
	 if (pushed&IMASK(ACTION_WEPDN))
	    {weaponDown(WEAPONINV(currentState.inventory));
	     redrawBowlDots();
	    }
	 
	 if (pushed&IMASK(ACTION_PUSH))
	    {push();
	     debugFlag=1;
	     dumpProfileData(); 
	    }
	 
#ifndef NOCHEATS
	 if (!(input&(PER_DGT_A|PER_DGT_C)))
	    camera->vel.y=F(3);
#endif
	 
	 /* player angle update */
	 playerAngle.yaw += yavel;
	 playerAngle.pitch += xavel;
	 if( playerAngle.yaw > F( 180 )) playerAngle.yaw -= F( 360 );
	 if( playerAngle.yaw < F( -180 )) playerAngle.yaw += F( 360 );
	 if( playerAngle.pitch > F(90)) playerAngle.pitch = F( 90 );
	 if( playerAngle.pitch < F( -90 )) playerAngle.pitch = F( -90 );
	}

     /* player position update */
     moveCamera();
     weaponPlayerMove(yavel);
     lastInput=input;
     inputPos++;
     if (inputPos==INPUTQSIZE)
	inputPos=0;
    }
}

void setVDP2(void)
{static Uint16 cycle[]=
   {0xeeee, 0xeeee,
    0xeeee, 0xeeee,
    0x44ff, 0xeeee,
    0x44ff, 0xeeee};

 SclVramConfig vcfg;
 SCL_InitVramConfigTb(&vcfg);
 vcfg.vramModeA=ON;
 vcfg.vramModeB=ON;
 vcfg.vramA0=SCL_RBG0_K;
 vcfg.vramA1=SCL_RBG0_CHAR;

 SCL_SetVramConfig(&vcfg);

 SCL_SetColRamMode(SCL_CRM15_2048);

 SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|SCL_SP4|
		 SCL_SP5|SCL_SP6|SCL_SP7, 7);
 SCL_SetPriority(SCL_NBG0,6);
 SCL_SetPriority(SCL_RBG1,7);
 SCL_SetPriority(SCL_SP0,4);
 SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
 SCL_SetCycleTable(cycle);
}


void loadVDP2Sprites(int fd)
{unsigned char *vram;
 SclConfig scfg;
 vram=(unsigned char *)SCL_VDP2_VRAM;
 fs_read(fd,vram+1024*256,1024*256);
 /* setup VDP2Sprite screen */
 SCL_InitConfigTb(&scfg);
 scfg.dispenbl=ON;
 scfg.bmpsize=SCL_BMP_SIZE_512X512;
 scfg.coltype=SCL_COL_TYPE_256;
 scfg.datatype= SCL_BITMAP;
 scfg.mapover=SCL_OVER_0;
 scfg.plate_addr[0]=1024*256;
 scfg.patnamecontrl=0;
 SCL_SetConfig(SCL_NBG0, &scfg);
 dontDisplayVDP2Pic();
}

static void plotBowl(unsigned char *pos)
{int y,x;
 unsigned char c;
 for (y=0;y<*(((int *)stat_bowl)+1);y++)
    {for (x=0;x<*(((int *)stat_bowl));x++)
	{c=stat_bowl[y*(*(int *)stat_bowl)+x+8];
	 if (c!=0)
	    *(pos+x)=c;
	}
     pos+=320;
    }
}

static void plotDot(unsigned char *pos,int blue)
{int y,x;
 unsigned char *art;
 unsigned char c;
 if (blue)
    art=stat_bluedot;
 else
    art=stat_reddot;
 for (y=0;y<*(((int *)art)+1);y++)
    {for (x=0;x<*(((int *)art));x++)
	{c=art[y*(*(int *)art)+x+8];
	 if (c!=0)
	    *(pos+x)=c;
	}
     pos+=320;
    }
}

static void plotNoDot(unsigned char *pos)
{int y,x;
 unsigned char c;
 for (y=0;y<*(((int *)stat_bluedot)+1);y++)
    {for (x=0;x<*(((int *)stat_bluedot));x++)
	{c=stat_bluedot[y*(*(int *)stat_bluedot)+x+8];
	 if (c!=0)
	    *(pos+x)=96; /* black in ruins pallete */
	}
     pos+=320;
    }
}

void redrawBowlDots(void)
{unsigned char *barPic,*pos;
 int i,w;
 barPic=(unsigned char *)((EZ_charNoToVram(0)<<3)+0x5c00000);
 for (i=0;i<currentState.nmBowls-1;i++)
    {pos=barPic+196+320*29+(10*i);
     plotBowl(pos);
    }
 for (i=0;i<nmFullBowls;i++)
    {pos=barPic+200+320*31+(10*i);
     plotDot(pos,0);
    }
 for (;i<currentState.nmBowls-1;i++)
    {pos=barPic+200+320*31+(10*i);
     plotNoDot(pos);
    }

 w=WEAPONINV(currentState.inventory);
 for (i=0;i<8;i++)
    {if (!((w>>i)&1))
	continue;
     pos=barPic+56+320*29+(10*i);
     plotBowl(pos);
     if (currentState.desiredWeapon==i)
	plotDot(barPic+59+320*31+(10*i),1);
     else
	plotNoDot(barPic+59+320*31+(10*i));
    }
}

void redrawStatBar(void)
{EZ_setChar(0,COLOR_4,*(int *)stat_bar,*(int *)(stat_bar+4),
	    (Uint8 *)stat_bar+8);
 redrawBowlDots();
}

void drawStatBar(int time)
{int weapon;
 static XyInt statusbar = {-160, 112 - 40};
 XyInt compass = {-14,93};
 static int healthMeterPos=0;
 int hmp;
 
 if (abs(healthMeterPos-F(currentState.health))<F(1))
    healthMeterPos=F(currentState.health);
 else
    healthMeterPos-=(healthMeterPos-F(currentState.health))>>3;

 hmp=f(healthMeterPos);
 /* draw blood */
 if ((hmp-1)/200!=nmFullBowls)
    {nmFullBowls=(hmp-1)/200;
     redrawBowlDots();
    }

 /* draw erase bar */
 {XyInt ammoBar[4];
  ammoBar[0].x=310; ammoBar[0].y=95;
  ammoBar[1].x=310; ammoBar[1].y=99;
  ammoBar[2].x=-105; ammoBar[2].y=99;
  ammoBar[3].x=-105; ammoBar[3].y=95;
  EZ_polygon(ECD_DISABLE|SPD_DISABLE,RGB(0,0,0),ammoBar,NULL);
 }

 weapon=currentState.desiredWeapon;
 /* draw ammo indicator */
 if (weaponMaxAmmo[weapon] && currentState.weaponAmmo[weapon])
    {XyInt ammoBar[4];
     int ammoPos;
     int i;
     ammoPos=(87*currentState.weaponAmmo[weapon])/
	weaponMaxAmmo[weapon];
     
     ammoBar[0].x=-105; ammoBar[0].y=95;
     ammoBar[1].x=-105; ammoBar[1].y=99;
     ammoBar[2].x=-105+ammoPos; ammoBar[2].y=99;
     ammoBar[3].x=-105+ammoPos; ammoBar[3].y=95;
     EZ_polygon(ECD_DISABLE|SPD_DISABLE,RGB(0,0,31),ammoBar,NULL);
     if (weaponMaxAmmo[weapon]<=10)
	{for (i=1;i<currentState.weaponAmmo[weapon];i++)
	    {ammoBar[0].x=-105+(87*i)/weaponMaxAmmo[weapon];
	     ammoBar[0].y=95;
	     ammoBar[1].x=ammoBar[0].x;
	     ammoBar[1].y=99;
	     EZ_line(ECD_DISABLE|SPD_DISABLE,RGB(0,0,11),ammoBar,NULL);
	     ammoBar[0].x++;
	     ammoBar[1].x++;
	     EZ_line(ECD_DISABLE|SPD_DISABLE,RGB(10,10,31),ammoBar,NULL);
	    }
	}
    }

 /* draw health indicator */
 {int pos=((hmp%200)*87)/200;
  XyInt rect[4];
  if (pos==0 && hmp!=0)
     pos=(199*87)/200;
  rect[0].x=35; rect[0].y=95;
  rect[1].x=35; rect[1].y=99;
  rect[2].x=35+pos; rect[2].y=99;
  rect[3].x=35+pos; rect[3].y=95;
  EZ_polygon(ECD_DISABLE|SPD_DISABLE,RGB(17,3,2),rect,NULL);
 }
 
 EZ_normSpr(DIR_NOREV,COLOR_4,0x4000,0,&statusbar,NULL);

 /* draw compass */
 {int angle,flip;
  angle=camera->angle;
  flip=0;
  if (angle<0)
     {flip|=DIR_LRREV;
      angle=-angle;
      compass.x-=5;
     }
  if (angle>F(90))
     {flip|=DIR_TBREV;
      angle=F(180)-angle;
     }
  if (angle<F(23))
     EZ_normSpr(flip,COLOR_4,0x4000,1,&compass,NULL);
  else
     if (angle<F(23+45))
	EZ_normSpr(flip,COLOR_4,0x4000,2,&compass,NULL);
     else
	EZ_normSpr(flip,COLOR_4,0x4000,3,&compass,NULL);
 }


}


static char *currentMessage=NULL;
static int messageAge,messageXPos;

#define MESSAGEFONT 1

static void changeMessage(char *message)
{currentMessage=message;
 messageAge=0;
 messageXPos=-getStringWidth(MESSAGEFONT,message)/2;
}

static void drawMessage(int nmFrames)
{int t,b;
 int s;
 if (!currentMessage)
    return;

 s=(MTH_Sin(normalizeAngle(F(messageAge)<<4))>>13);
 t=15+s;
 b=15-s;
       
 drawStringGouro(messageXPos,-100,MESSAGEFONT,RGB(t,t,t),RGB(b,b,b),
		 currentMessage);
 messageAge+=nmFrames;
 if (messageAge>60*10)
    currentMessage=0;
}


extern int slaveSize;
extern int lastHitWall;

void playerGetCamel(int toLevel)
{hitCamel=toLevel+100;
}

void playerHitTeleport(int toLevel)
{hitTeleport=toLevel+200;
}

static int delayed_fade=0;
static int delayed_fadeButton=0;
static int delayed_fadeSel=0;

int playerGetObject(int objectType)
{int i;
 switch (objectType)
    {case OT_COMM_BATTERY ... OT_COMM_TOP:
	currentState.inventory|=0x10000<<(objectType-OT_COMM_BATTERY);
	delayed_fade=1;	delayed_fadeButton=3; 
	delayed_fadeSel=objectType-OT_COMM_BATTERY;
	break;
     case OT_PYRAMID:
	hitPyramid=1;
	currentState.levFlags[(int)currentState.currentLevel]|=
	   LEVFLAG_GOTPYRAMID;
	break;
     case OT_M60:
	currentState.inventory|=INV_M60; 
	setCurrentWeapon(WP_M60); 
	redrawBowlDots();
	changeMessage(getText(LB_ITEMMESSAGE,5));
	break;
     case OT_COBRASTAFF:
	currentState.inventory|=INV_COBRA; 
	setCurrentWeapon(WP_COBRA); 
	redrawBowlDots();
	changeMessage(getText(LB_ITEMMESSAGE,8));
	break;
     case OT_FLAMER:
	currentState.inventory|=INV_FLAMER; 
	setCurrentWeapon(WP_FLAMER);
	redrawBowlDots();
	changeMessage(getText(LB_ITEMMESSAGE,6));
	break;
     case OT_GRENADEAMMO:
	currentState.inventory|=INV_GRENADE; 
	setCurrentWeapon(WP_GRENADE);
	redrawBowlDots();
	changeMessage(getText(LB_ITEMMESSAGE,7));
	break;
     case OT_MANACLE:
	currentState.inventory|=INV_MANACLE; 
	setCurrentWeapon(WP_RAVOLT); 
	redrawBowlDots();
	changeMessage(getText(LB_ITEMMESSAGE,9));
	break;
     case OT_PISTOL:
	currentState.inventory|=INV_PISTOL; 
	setCurrentWeapon(WP_PISTOL);
	redrawBowlDots();
	changeMessage(getText(LB_ITEMMESSAGE,4));
	break;
     case OT_BUGKEY: 
	keyMask|=1; 
	playStaticSound(ST_ITEM,0);
	changeMessage(getText(LB_ITEMMESSAGE,0));
	break;
     case OT_TIMEKEY: 
	keyMask|=2; 
	playStaticSound(ST_ITEM,0);
	changeMessage(getText(LB_ITEMMESSAGE,1));
	break;
     case OT_XKEY: 
	keyMask|=4; 
	playStaticSound(ST_ITEM,0);
	changeMessage(getText(LB_ITEMMESSAGE,2));
	break;
     case OT_PLANTKEY: 
	keyMask|=8; 
	playStaticSound(ST_ITEM,0);
	changeMessage(getText(LB_ITEMMESSAGE,3));
	break;
     case OT_CAPE:
	currentState.inventory|=INV_SHAWL;
	delayed_fade=1;	delayed_fadeButton=2; delayed_fadeSel=2;
	break;
     case OT_FEATHER:
	currentState.inventory|=INV_FEATHER;
	delayed_fade=1;	delayed_fadeButton=2; delayed_fadeSel=5;
	break;
     case OT_MASK:
	currentState.inventory|=INV_MASK;
	delayed_fade=1;	delayed_fadeButton=2; delayed_fadeSel=1;
	break;
     case OT_SANDALS:
	currentState.inventory|=INV_SANDALS;
	delayed_fade=1;	delayed_fadeButton=2; delayed_fadeSel=0;
	break;
     case OT_ANKLETS:
	currentState.inventory|=INV_ANKLET;
	delayed_fade=1;	delayed_fadeButton=2; delayed_fadeSel=3;
	break;	
     case OT_SCEPTER:
	currentState.inventory|=INV_SCEPTER;
	delayed_fade=1;	delayed_fadeButton=2; delayed_fadeSel=4;
	break;
     case OT_BLOODBOWL:
	currentState.nmBowls++;
	currentState.levFlags[(int)currentState.currentLevel]|=
	   LEVFLAG_GOTVESSEL;
	changeMessage(getText(LB_ITEMMESSAGE,15));
	redrawBowlDots();
	break;
     case OT_HEALTHBALL:
     case OT_HEALTHORB:
     case OT_HEALTHSPHERE:
	if (currentState.health==currentState.nmBowls*200)
	   return 0;
	if (objectType==OT_HEALTHBALL)
	   {currentState.health+=20;
	    changeMessage(getText(LB_ITEMMESSAGE,13));
	    playStaticSound(ST_ITEM,2);
	   }
	if (objectType==OT_HEALTHORB)
	   {currentState.health+=50;
	    changeMessage(getText(LB_ITEMMESSAGE,13));
	    playStaticSound(ST_ITEM,2);
	   }
	if (objectType==OT_HEALTHSPHERE)
	   {currentState.health=currentState.nmBowls*200;
	    changeMessage(getText(LB_ITEMMESSAGE,14));
	    playStaticSound(ST_ITEM,3);	    
	   }	
	if (currentState.health>currentState.nmBowls*200)
	   currentState.health=currentState.nmBowls*200;
	break;
     case OT_AMMOBALL:
     case OT_AMMOORB:
	{int diff;
	 if (currentState.weaponAmmo[currentWeapon]==
	     weaponMaxAmmo[currentWeapon])
	    return 0;
	 diff=
	    (weaponMaxAmmo[currentWeapon]*(objectType==OT_AMMOBALL?20:40))/100;
	 if (diff<1) diff=1;
	 weaponChangeAmmo(currentWeapon,diff);
	 changeMessage(getText(LB_ITEMMESSAGE,11));
	 playStaticSound(ST_ITEM,1);	    
	 if (currentState.weaponAmmo[currentWeapon]>
	     weaponMaxAmmo[currentWeapon])
	    currentState.weaponAmmo[currentWeapon]=
	       weaponMaxAmmo[currentWeapon];
	 break;
	}
     case OT_AMMOSPHERE:
	for (i=0;i<WP_NMWEAPONS;i++)
	   if (currentState.weaponAmmo[i]<weaponMaxAmmo[i])
	      break;
	if (i==WP_NMWEAPONS)
	   return 0;
	for (i=0;i<WP_NMWEAPONS;i++)
	   currentState.weaponAmmo[i]=weaponMaxAmmo[i];
	changeMessage(getText(LB_ITEMMESSAGE,12));
	playStaticSound(ST_ITEM,3);
	break;
       }
 dPrint("Got away\n");
 greenFlash();
 return 1;
}

void rotateRectangle(int width,int height,int cx,int cy,
		     Fixed32 angle,
		     XyInt *result)
{MthXyz north,east;
 int x,y;
 north.x=MTH_Cos(angle);
 north.y=MTH_Sin(angle);
 east.x=-north.y;
 east.y=north.x;

 x=-cx; y=-cy;
 result[0].x=f(x*north.x+y*north.y);
 result[0].y=f(x*east.x+y*east.y);

 x+=width;
 result[1].x=f(x*north.x+y*north.y);
 result[1].y=f(x*east.x+y*east.y);

 y+=height;
 result[2].x=f(x*north.x+y*north.y);
 result[2].y=f(x*east.x+y*east.y);

 x-=width;
 result[3].x=f(x*north.x+y*north.y);
 result[3].y=f(x*east.x+y*east.y);
 
}

#if 0
#define MAXNMDIALLINES 100
static Fixed32 dialLines[MAXNMDIALLINES*4];
static int nmDialLines;

#define DIALRADIUS 40
#define DIALNMCYCLES 6
#define TEXTSIZE 3
#define TEXTSPACE 15
void makeDial(void)
{int angle;
 int angleInc;
 int cycle;
 int s,c;
 int line;
 int i;
 angleInc=F(270)/(8*DIALNMCYCLES);
 angle=F(-90);
 line=0;

 /* add E */
 dialLines[line++]=F(+TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE*2+TEXTSPACE);
 dialLines[line++]=F(-TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE*2+TEXTSPACE);

 dialLines[line++]=F(+TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE*2+TEXTSPACE);
 dialLines[line++]=F(+TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSPACE);

 dialLines[line++]=F(+TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE+TEXTSPACE);
 dialLines[line++]=F(0);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE+TEXTSPACE);

 dialLines[line++]=F(+TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSPACE);
 dialLines[line++]=F(-TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSPACE);

 /* add lines */
 for (cycle=0;cycle<=DIALNMCYCLES*8;cycle++)
    {s=MTH_Sin(angle);
     c=MTH_Cos(angle);
     angle=normalizeAngle(angle+angleInc);
     assert(line<MAXNMDIALLINES*4);
     dialLines[line++]=c*DIALRADIUS;
     dialLines[line++]=s*DIALRADIUS;
     i=3;
     if (!(cycle & 7))
	i+=4;
     if (!(cycle & 3))
	i+=4;
     dialLines[line++]=c*(DIALRADIUS-i);
     dialLines[line++]=s*(DIALRADIUS-i);
     assert(line<MAXNMDIALLINES*4);
    }

 /* add F */
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE*2+TEXTSPACE);
 dialLines[line++]=F(-TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE*2+TEXTSPACE);
 dialLines[line++]=F(+TEXTSIZE);

 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE*2+TEXTSPACE);
 dialLines[line++]=F(-TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSPACE);
 dialLines[line++]=F(-TEXTSIZE);

 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE+TEXTSPACE);
 dialLines[line++]=F(-TEXTSIZE);
 dialLines[line++]=F(-DIALRADIUS+TEXTSIZE+TEXTSPACE);
 dialLines[line++]=F(0);

 nmDialLines=line>>2;
}
#endif
		     
static Fixed32 airStatus=0;
static int drownStatus=0;
static int meterPos=0;
static enum {METERUP,METERDOWN,TRANSITION} meterState=METERUP;
static int airBase;
void drawAirMeter(int frames)
{int i;
 static int transitionTimer=0;
 Fixed32 angle=0;
 static Fixed32 dialPos=0;
 static Fixed32 dialVel=0; 
 static int underCount=0;
 static airFrom,airTo;
 XyInt pos[4];
  
 switch (meterState)
    {case METERUP:
	if (camera->flags & SPRITEFLAG_UNDERWATER)
	   meterState=METERDOWN;
	if (meterPos==0)
	   return;
	meterPos-=frames;
	if (meterPos<0)
	   meterPos=0;
	break;
     case METERDOWN:
	if (meterPos<50)
	   {meterPos+=frames;
	    if (meterPos>50)
	       meterPos=50;
	   }
	else
	   if (!(camera->flags & SPRITEFLAG_UNDERWATER))
	      {meterState=TRANSITION;
	       transitionTimer=0;
	      }
	break;
     case TRANSITION:
	transitionTimer+=frames;
	if (transitionTimer>120)
	   meterState=METERUP;
	if (camera->flags & SPRITEFLAG_UNDERWATER)
	   meterState=METERDOWN;
	break;
       }

 if (level_sector[camera->s].flags & SECFLAG_WATER)
    {for (i=0;i<frames;i++)
	{underCount++;
	 if (underCount==220)
	    {airFrom=airStatus;
	     airStatus+=F(27);
	     if (airStatus>F(270) || !(currentState.inventory & INV_MASK))
		{playerHurt(30);
		 airStatus=F(270);	    
#if 0
		 if (currentState.health<100 && !playerIsDead)
		    {int j;
		     for (j=0;j<3;j++)
			colorCenter[j]=-128+currentState.health;
		    }
#endif
		 underCount=drownStatus;
		 drownStatus=180;
		 if (drownStatus>180)
		    drownStatus=180;
		}
	     else
		{playSoundE(0,level_staticSoundMap[ST_JOHN]+7,32,0);
		 drownStatus=0;
		}
	    }
	 airTo=airStatus;
	 
	 if (underCount==240)
	    {underCount=0;
	     dialPos=airStatus;
	    }
	}
    }
 else
    {if (underCount>0)
	{airStatus=0;
	 underCount=-1;
	 dialVel=0;
	}
    }

 if (underCount>220 && underCount<240)
    dialPos=evalHermite(F(underCount-220)/20,airFrom,airTo,0,0);

 if (underCount<0)
    {for (i=0;i<frames;i++)
	{dialPos+=dialVel;
	 dialVel+=(airStatus-dialPos)>>9;
	 dialVel-=dialVel>>5;
	 if (dialPos<0)
	    {dialPos=0;
	     dialVel=-dialVel>>4;
	    }
	}
    }

 if (!(currentState.inventory & INV_MASK))
    return;
 angle=F(90)-dialPos;
 if (angle<F(-180))
    angle+=F(360);

 EZ_localCoord(320/2,meterPos-50);

#if 1
 {pos[0].x=90;
  pos[0].y=15;
  EZ_normSpr(0,DRAW_MESH|COLOR_5|ECD_DISABLE,0,mapPic(airBase+1),pos,NULL);
  pos[0].x+=31;
  pos[0].y+=31;
  pos[1].x=64-(dialPos/276480);
  pos[1].y=pos[1].x;
  EZ_scaleSpr(ZOOM_MM,DRAW_MESH|COLOR_5|ECD_DISABLE,0,mapPic(airBase),pos,
	      NULL);
 }
#else
 pos[0].x=-67;
 pos[0].y=-130+120;
 EZ_normSpr(0,UCLPIN_ENABLE|COLOR_5|ECD_DISABLE,0,4,pos,NULL);
 {int c=0;
  unsigned short color = RGB(31,5,5);
  MthXyz north,east;
  north.x=MTH_Cos(angle);
  north.y=MTH_Sin(angle);
  east.x=-north.y;
  east.y=north.x;

  for (i=0;i<nmDialLines;i++)
     {pos[0].x=
	 f(MTH_Mul(dialLines[c],north.x)+MTH_Mul(dialLines[c+1],north.y));
      pos[0].y=
	 f(MTH_Mul(dialLines[c],east.x)+MTH_Mul(dialLines[c+1],east.y));
      pos[1].x=
	 f(MTH_Mul(dialLines[c+2],north.x)+MTH_Mul(dialLines[c+3],north.y));
      pos[1].y=
	 f(MTH_Mul(dialLines[c+2],east.x)+MTH_Mul(dialLines[c+3],east.y));
      c+=4;
      EZ_line(COMPO_REP|ECD_DISABLE|SPD_DISABLE,color,pos,NULL);
      if (i==13)
	 color=RGB(31,31,15);
      if (i==30)
	 color=RGB(31,31,31);
      
     }
 }
#endif
 EZ_localCoord(320/2,240/2);
}


static int earthQuake;
void setEarthQuake(int richter)
{if (richter>earthQuake)
    earthQuake=richter;
}

void stunPlayer(int ticks)
{stunCounter=ticks;
 playStaticSound(ST_JOHN,3);
 colorOffset[2]=128;
}

#ifndef NDEBUG
int extraStuff=0;
#endif

int runLevel(char *filename,int levelNm)
{XyInt parms;
 XyInt noUserClip[2]={{0,0},{320-1,240-1}};
 int i,monsterMoveCounter;
 int nmWeaponTiles,nmStaticSounds;
 int lastDraw=0,lastCalc=0;
 int framesElapsed,inputEnd;
 unsigned int smoothVTime;
 int vspeedSwitchCount;
 int lastLastCalc=0;
 int lastYaw,lastPitch,mmcSave;

 MthMatrixTbl viewTransform;
 MthMatrix matstack[4];
 
 startSlave(wallRenderSlaveMain);
 initSound();

 quitRequest=0;
 mem_init();
 /* do hardware initialization */
 setVDP2();

 SPR_SetTvMode(SPR_TV_NORMAL,SPR_TV_320X240,OFF);
 EZ_initSprSystem(1448,4,1224,
		  240,0x8000);

 SCL_SetFrameInterval(0xfffe); 

 for (i=0;i<3;i++)
    {EZ_openCommand();
     EZ_closeCommand();
     SCL_DisplayFrame();
    }
 EZ_setChar(0,COLOR_4,*(int *)stat_bar,*(int *)(stat_bar+4),
	    (Uint8 *)stat_bar+8);
 EZ_setChar(1,COLOR_4,*(int *)stat_compass0,*(int *)(stat_compass0+4),
	    (Uint8 *)stat_compass0+8);
 EZ_setChar(2,COLOR_4,*(int *)stat_compass1,*(int *)(stat_compass1+4),
	    (Uint8 *)stat_compass1+8);
 EZ_setChar(3,COLOR_4,*(int *)stat_compass2,*(int *)(stat_compass2+4),
	    (Uint8 *)stat_compass2+8);
 
 i=initFonts(4);
 initPicSystem(i,((int []){27,32,1,10,-1}));


 redrawStatBar();

 MTH_InitialMatrix(&viewTransform,4,matstack);
 MTH_ClearMatrix(&viewTransform);

 initObjects();
 initFlames();

 SCL_SetWindow(SCL_W1,0,SCL_RBG0,0xfffffff,0,0,0,0); 
 SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0|SCL_RBG0,0,0,0);
 fs_startProgress();
 fs_addToProgress("+STATIC.DAT");
 fs_addToProgress(filename); 
 /* load static data */
 {int fd;
  fd=fs_open("+STATIC.DAT");
  assert(fd>=0);
  loadVDP2Sprites(fd);
  nmStaticSounds=loadStaticSounds(fd);
  nmWeaponTiles=loadWeaponTiles(fd);
  loadWeaponSequences(fd);
  fs_close(fd);
 }
 /* load level file */
 debugPrint("Loaded static\n");
 {int fd;
  fd=fs_open(filename);
  assert(fd>=0);
  initPlax(fd);
  loadLevel(fd,nmWeaponTiles);
  loadDynamicSounds(fd);
  loadTiles(fd);
  loadSequences(fd,nmWeaponTiles,nmStaticSounds);
  fs_close(fd);
 }
 fs_closeProgress();

 /* air meter stuff */
 airBase=addPic(TILE16BPP,meter_bubble+8,NULL,0);
 addPic(TILE16BPP,meter_back+8,NULL,0);
 

 SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0|SCL_RBG0,-255,-255,-255);
 colorOffset[0]=-255; colorOffset[1]=-255; colorOffset[2]=-255; 
 EZ_setErase(240,0x0000);

 debugPrint("Loaded dynamic\n");
 initRoutePlotter();
 initWallRenderer();
 initMap();
 markAnimTiles();
 mapOn=0;
 assert(level_nmSectors<=MAXNMSECTORS);
 assert(level_nmWalls<=MAXNMWALLS);
 initSpriteSystem();

 
 switchPlayerMotion(1);
 colorCenter[0]=0;
 colorCenter[1]=0;
 colorCenter[2]=0;
 player=NULL;
 playerAngle.pitch=0;
 playerAngle.yaw=F(0);
 placeObjects(); 
 if (!player)
    player=constructPlayer(0,0);
 assert(player);
 camera=player->sprite;

 monsterMoveCounter=0;
 initWeapon();
 playerIsDead=0;
 hitCamel=0;
 hitPyramid=0;
 hitTeleport=0;
 initInput();
 playCDTrackForLevel(levelNm); 
 
 initWater();
#if 0
 makeDial();
#endif
 debugPrint("Start Loop\n");
 initProfiler();
 keyMask=0;
 earthQuake=0;
 vspeedSwitchCount=0;
 screaming=0;
 airStatus=0;
 meterPos=0;
 meterState=METERUP;
 drownStatus=0;
 framesElapsed=0;
 inputEnd=0;
 lastYaw=playerAngle.yaw;
 lastPitch=playerAngle.pitch;
 ltHurtTime=0;
 delayed_fade=0;
 switchWeapons(1);
 retryPlaxPal();
 vtimer=0;
 smoothVTime=1;
 while(1)
    {htimer=0;

     if (framesElapsed>8)
	framesElapsed=8;
     /* move paralax sky */
     setMark(sprites[1].pos.x,sprites[1].pos.z);
     MTH_PushMatrix(&viewTransform);
     MTH_RotateMatrixZ(&viewTransform, playerAngle.roll );
     MTH_RotateMatrixX(&viewTransform, playerAngle.pitch );
     MTH_RotateMatrixY(&viewTransform, playerAngle.yaw );
     if (earthQuake)
	{camera->pos.x+=(MTH_GetRand()%(earthQuake<<15))-(earthQuake<<14);
	 camera->pos.y+=(MTH_GetRand()%(earthQuake<<15))-(earthQuake<<14);
	 camera->pos.z+=(MTH_GetRand()%(earthQuake<<15))-(earthQuake<<14);
	 earthQuake--;
	}
     MTH_MoveMatrix(&viewTransform, 
		    -camera->pos.x, 
		    -camera->pos.y+playerHeightOffset,
		    -camera->pos.z);

     EZ_openCommand();
     parms.x = 320 - 1;
     parms.y = 240 - 1;
     EZ_sysClip();
     EZ_userClip(noUserClip);

     EZ_localCoord(320/2,240/2);
     pushProfile("Walls");
     drawWalls(viewTransform.current); 
     popProfile();

     EZ_userClip(noUserClip);

     pushProfile("Motion");
     movePlayer(inputEnd,framesElapsed);
     camera->angle=playerAngle.yaw;
#if 0
     if (autoTarget)
	{Fixed32 dist,pitch;
	 
	 dist=approxDist(camera->pos.x-autoTarget->pos.x,
			 camera->pos.z-autoTarget->pos.z,
			 0);
	 pitch=getAngle(dist,autoTarget->pos.y-camera->pos.y);
	 playerAngle.pitch=pitch;
	}
     else
	playerAngle.pitch=F(0); 
#endif
     if (monsterMoveCounter>8) 
	monsterMoveCounter=8;
     mmcSave=monsterMoveCounter;
     for (;monsterMoveCounter>1;monsterMoveCounter-=2)
	{if (ltHurtTime>0)
	    {ltHurtTime--;
	     playerHurt(ltHurtAmount);
	    }
	 pushProfile("Run Objects");
	 runObjects();
	 popProfile();
	 stepColorOffset();
	 stepPlayerHeight();
	 ouchTime--;
	}
     popProfile();

     pushProfile("Walls");
     drawWallsFinish();
     popProfile();

     for (;mmcSave>1;mmcSave-=2)
	{advanceWallAnimations();
	 stepWater();
	}
     updatePushBlockPositions();
     processDelayedMoves();

     if (mapOn)
	drawMap(camera->pos.x,camera->pos.z,camera->pos.y,playerAngle.yaw,
		camera->s);

     runWeapon(framesElapsed);

     MTH_PopMatrix(&viewTransform);

     drawMessage(framesElapsed);
     drawStatBar(framesElapsed);
     
     drawAirMeter(framesElapsed);

#ifndef NOSTATUSTEXT
     drawStringf(-158,-60,0,"fps:%d %d",60/framesElapsed,60/(smoothVTime+1));

     drawStringf(-158,-80,0,"sector:%d",camera->s);

#ifndef NDEBUG
     drawStringf(-158,-100,0,"extra:%d",extraStuff);

     for (i=0;i<16;i++)
	{if (errorQ[i])
	    {drawStringf(35,-50+10*i,0,"%x",errorQ[i]);
	     drawStringf(90,-50+10*i,0,"%x",prQ[i]);
	    }
	}
#endif

     drawStringf(-158,-70,0,"polys:%3d+%3d=%d",nmPolys,nmSlavePolys,
		 nmPolys+nmSlavePolys);

     drawStringf(-158,-50,0,"time:%d  %4d+%4d=%d",
		 (lastCalc+lastLastCalc)>>1,
		 lastCalc,lastDraw,
		 lastCalc+lastDraw);

     drawStringf(-158,-40,0,"mem:%dk+%dk=%dk",mem_coreleft(0)>>10,
		 mem_coreleft(1)>>10,(mem_coreleft(0)+mem_coreleft(1))>>10);

#endif

     lastLastCalc=lastCalc;
     lastCalc=htimer;
     {int nmSwaps[NMCLASSES];
      int used[NMCLASSES];
      pic_nextFrame(nmSwaps,used);
#ifndef NDEBUG
#ifndef NOSTATUSTEXT
      drawString(45,-80,0,"vswaps:");
      for (i=0;i<NMCLASSES;i++)
         drawStringf(100+17*i,-80,0,"%d",nmSwaps[i]);
      drawString(45,-70,0,"used:");
      for (i=0;i<NMCLASSES;i++)
         drawStringf(100+17*i,-70,0,"%d",used[i]);
#endif
#endif
     }

     if (hitPyramid || hitTeleport)
	{EZ_closeCommand();
	 SPR_WaitDrawEnd();
	 SCL_DisplayFrame();
	 return hitTeleport?hitTeleport:3;
	}
     /* used to be here */
     EZ_closeCommand();
     SPR_WaitDrawEnd();
     lastDraw=htimer-lastCalc;

     DISABLE;
     if (vtimer<smoothVTime)
	{vspeedSwitchCount++;
	 if (vspeedSwitchCount>10)
	    {smoothVTime=vtimer;
	     if (smoothVTime<1)
		smoothVTime=1;
	     vspeedSwitchCount=0;
	    }
	}
     else
	vspeedSwitchCount=0;
     ENABLE;
     while (vtimer<smoothVTime) ;

     /* sometimes a vtimer switch can occur in here */
     SCL_DisplayFrame(); 

     DISABLE;
     if (vtimer-1>smoothVTime)
	{smoothVTime=vtimer-1;
	 if (smoothVTime>2)
	    smoothVTime=2;
	 vspeedSwitchCount=0;
	}
     monsterMoveCounter+=vtimer;
     framesElapsed=vtimer;
     inputEnd=inputQHead;
     vtimer=0;
     ENABLE;

     movePlax(lastYaw,lastPitch);
#ifndef PAL
     SCL_SetWindow(SCL_W1,0,SCL_RBG0,0xfffffff,
		   plaxBBxmin+160,plaxBBymin+120,
		   plaxBBxmax+160,plaxBBymax+120);
#else
     SCL_SetWindow(SCL_W1,0,SCL_RBG0,0xfffffff,
		   plaxBBxmin+160,plaxBBymin+120+8,
		   plaxBBxmax+160,plaxBBymax+120+8);
#endif
		   
     lastYaw=playerAngle.yaw; lastPitch=playerAngle.pitch;

     if (playerIsDead && colorOffset[0]==-255)
	return 1;

     enablePlax(1);
     if (hitCamel)
	{enablePlax(0);
	 if (runTravelQuestion(getText(LB_LEVELNAMES,hitCamel-100)))
	    return hitCamel;
	 hitCamel=0;
	 vtimer=smoothVTime;
	}
     if (playerMotionEnable && 
	 (!(lastInputSample & PER_DGT_S) || !controlerPresent || delayed_fade))
	{enablePlax(0);
	 runInventory(currentState.inventory,keyMask,&mapOn,
		      delayed_fade,delayed_fadeButton,delayed_fadeSel);
	 delayed_fade=0;
	 vtimer=smoothVTime;
	}

     if (quitRequest)
	return 2;
#ifdef PSYQ
     pollhost();
#endif
    }
}

void logo(void)
{int i,pos;
 int r,g,b;
 POKE_W(SCL_VDP2_VRAM+0x180112,0x00); /* enable line scroll for nbg[01] */
 pos=0;
 for (i=0;i<32;i++)
    {pos+=F(1)/32;
     r=f(evalHermite(pos,0,F(-255),0,0));
     g=f(evalHermite(pos,0,F(-255),0,0));
     b=f(evalHermite(pos,0,F(-255),0,0));
     
     while (!(PEEK_W(SCL_VDP2_VRAM+0x180004) & 8)) ;
     POKE_W(SCL_VDP2_VRAM+0x180114,r & 0x1ff);
     POKE_W(SCL_VDP2_VRAM+0x180116,g & 0x1ff);
     POKE_W(SCL_VDP2_VRAM+0x180118,b & 0x1ff);
     while ((PEEK_W(SCL_VDP2_VRAM+0x180004) & 8)) ;
    }
}



void main(void)
{char *levelFile;
 int level,i;
 enable_stereo=1;
 enable_music=1;
 logo();
 megaInit();

 dPrint("Start...\n");
 fs_init();

 set_imask(0);

 /* aquire system info */
 {PerGetSys *sys_data;
  PER_LInit(PER_KD_SYS,6,PER_SIZE_DGT,PadWorkArea,0);
  while (!(sys_data=PER_GET_SYS())); 
  systemMemory=sys_data->sm;
 }

 SCL_Vdp2Init();
#ifndef PALMODE
 SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
#else
 SCL_SetDisplayMode(SCL_NON_INTER,SCL_256LINE,SCL_NORMAL_A);
#endif
 SPR_SetEraseData(RGB(0,0,0),0,0,319,239);
 setVDP2();
 SetVblank();

 mem_init();
 /* do initial load */
 {int fd=fs_open("+INITLOAD.DAT");
  dlg_init(fd); /* warning, locks memory */
  loadLocalText(fd); /* locks memory */
  fs_close(fd);
 }
 initDMA();

/* testEZ();*/

 EZ_initSprSystem(1540,8,1524,
		  240,0x8000);
 SCL_SetFrameInterval(0xfffe);
 SPR_SetTvMode(SPR_TV_NORMAL,SPR_TV_320X240,OFF);
 initFonts(1);

 POKE_W(SCL_VDP2_VRAM+0x180114,0); /* reset color offsets */
 POKE_W(SCL_VDP2_VRAM+0x180116,0);
 POKE_W(SCL_VDP2_VRAM+0x180118,0);

 dPrint("Initialized\n");
 bup_initialProc();

intro:
/* credits(); */
/* playMovie();*/
#ifndef TESTCODE
 playIntro(); 
#else
 bup_initCurrentGame();
 currentState.inventory=0x3fffff; 
#endif

SCL_Vdp2Init();
 /* this line had something to do with a null pointer reference? */

#ifndef PALMODE
 SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
#else
 SCL_SetDisplayMode(SCL_NON_INTER,SCL_256LINE,SCL_NORMAL_A);
#endif
 setVDP2();
#ifndef TESTCODE
 level=runMap(currentState.currentLevel,&levelFile); 
#else
 level=22;
 levelFile="+TEST.LEV";
#endif
 while (1)
    {int action;
     SaveState levStart=currentState;
     currentState.currentLevel=level;
     action=runLevel(levelFile,level);
     stopAllLoopedSounds();
     dontDisplayVDP2Pic();
     EZ_clearScreen();
     SCL_SetColOffset(SCL_OFFSET_A,SCL_SP0|SCL_NBG0|SCL_RBG0,0,0,0);
     switch (action)
	{case 100 ... 199: /* camel */
	    currentState.levFlags[hitCamel-100]|=LEVFLAG_CANENTER;
	    level=runMap(hitCamel-100,&levelFile);
	    if (currentState.health<200)
	       currentState.health=200;
	    break;
	 case 200 ... 399: /* teleporter */
	    level=action-200;
	    levelFile=getLevelName(level);
	    break;
	 case 1: /* restart level */
	    currentState=levStart;
	    break;
	 case 2: /* quit */
	    goto intro;
	    break;
	 case 3: /* warp to tomb */
	    dontDisplayVDP2Pic();
	    mem_init();
	    bup_saveGame();
	    level=3;
	    levelFile="+TOMB.LEV";
	    break;
	   }
    }
}






