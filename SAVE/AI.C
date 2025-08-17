#include <sega_mth.h>
#include <sega_scl.h>

#include "slevel.h"
#include "level.h"
#include "sprite.h"
#include "ai.h"
#include "util.h"
#include "sequence.h"
#include "hitscan.h"
#include "sound.h"
#include "sruins.h"
#include "route.h"
#include "walls.h"
#include "pic.h"
#include "file.h"
#include "gamestat.h"

#include "jasonpal.h"

#define GRAVITY (6<<12)
#define HB 0x8000

extern void suckSpriteParams(Sprite *s);

#define AISLOT(x) ((this->aiSlot&(x))==(aicount&(x)))


static int aicount=0,nextAiSlot=0;

int getFacingAngle(Sprite *from,Sprite *to)
{int angle;
 angle=getAngle(from->pos.x-to->pos.x,
		from->pos.z-to->pos.z);
 angle-=from->angle+F(180);
 if (angle>F(180)) angle-=F(360);
 if (angle<F(-180)) angle+=F(360);

 if (angle<0)
    {if (angle>F(-23))
	return 0;
     if (angle>F(-23-45))
	return 7;
     if (angle>F(-23-90))
	return 6;
     if (angle>F(-23-135))
	return 5;
     return 4;
    }
 if (angle<F(23))
    return 0;
 if (angle<F(23+45))
    return 1;
 if (angle<F(23+90))
    return 2;
 if (angle<F(23+135))
    return 3;
 return 4;
}

int PlotCourseToObject(SpriteObject *from,SpriteObject *to)
{int xdiff,zdiff;
 assert(from);
 assert(to);
 assert(from!=to);

 xdiff = to->sprite->pos.x-from->sprite->pos.x;
 zdiff = to->sprite->pos.z-from->sprite->pos.z;

 from->sprite->angle = getAngle(xdiff,zdiff);
 /*  return(ksqrt((xdiff*xdiff)+(ydiff*ydiff))); */
 return 0;
}

Fixed32 spriteDist2(Sprite *s1,Sprite *s2)
{Fixed32 dx,dy,dz;
 dx=s1->pos.x-s2->pos.x;
 dy=s1->pos.y-s2->pos.y;
 dz=s1->pos.z-s2->pos.z;
 return f(dx)*dx+f(dy)*dy+f(dz)*dz;
}

Fixed32 spriteDistApprox(Sprite *s1,Sprite *s2)
{return approxDist(s1->pos.x-s2->pos.x,
		   s1->pos.y-s2->pos.y,
		   s1->pos.z-s2->pos.z);
}


MonsterObject *findPlayer(Sprite *sprite,int dist)
{if (spriteDistApprox(sprite,player->sprite)>dist)
    return NULL;
 if (canSee(sprite,player->sprite))
    return player;
 return NULL;
}

int randomAngle(int spreadlog2)
{return ((int)(getNextRand()<<16))>>(16-spreadlog2);
}


void setSequence(SpriteObject *this)
{unsigned short seqBase=this->sequenceMap[this->state];
 assert(this->type>=0);
 assert(this->type<OT_NMTYPES);
 assert(level_sequenceMap[this->type]>=0);
 if (seqBase & 0x8000)
    this->sprite->sequence=
       level_sequenceMap[this->type]+(seqBase & 0x7fff)+
	  getFacingAngle(this->sprite,camera);
 else
    this->sprite->sequence=
       level_sequenceMap[this->type]+seqBase;
}


int monsterObject_signalHurt(MonsterObject *this,int param1,int param2)
{Object *hurter=(Object *)param2;
 assert(this->class==CLASS_MONSTER);
 this->sprite->flags|=SPRITEFLAG_FLASH;
 this->health-=param1;
 if (hurter->class==CLASS_MONSTER && hurter!=(Object *)this)
    this->enemy=(MonsterObject *)hurter;
 if (hurter->class==CLASS_PROJECTILE)
    {ProjectileObject *po=(ProjectileObject *)hurter;
     if (po->owner->class==CLASS_MONSTER && po->owner!=(Object *)this)
	this->enemy=(MonsterObject *)(po->owner);
    }
 if (this->health<=0)
    {if (getNextRand()&1)
	constructZorch(this->sprite->s,
		       this->sprite->pos.x,
		       this->sprite->pos.y+F(32),
		       this->sprite->pos.z,
		       (getNextRand()&0x40)?OT_REDZORCH:OT_BLUEZORCH);
     return 1;
    }
 return 0;
}

void monsterObject_signalDestroyed(MonsterObject *this,int param1)
{Object *killed=(Object *)param1;
 if (killed==(Object *)this)
    freeSprite(this->sprite);
 if (killed==(Object *)this->enemy)
    this->enemy=NULL;
}

MthXyz followRoute(MonsterObject *this)
{int v,w;
 Fixed32 planeDist;
 sWallType *targetWall;
 MthXyz doorPos,wallP;

 assert(this->class==CLASS_MONSTER);
 if (this->route[this->routePos]==-1)
    {/* end of route */
     this->routePos=-1;
     doorPos.x=0;
     return doorPos;
    }

 /* see if we've passed the doorway we were heading for */
 targetWall=level_wall+this->route[this->routePos];
 getVertex(targetWall->v[0],&wallP);
 planeDist=
    (f(this->sprite->pos.x-wallP.x))*targetWall->normal[0]+
    (f(this->sprite->pos.y-wallP.y))*targetWall->normal[1]+
    (f(this->sprite->pos.z-wallP.z))*targetWall->normal[2];
 if (planeDist<=F(1))
    /* we have passed the wall */
    {this->routePos++;
     doorPos=followRoute(this);
     /* make monster move perp to doorway to try to get him to
	go around corners better */
     this->sprite->angle=getAngle(-targetWall->normal[0],
				  -targetWall->normal[2]);
     return doorPos;
    }
 /* we haven't passed the wall */

 /* find centroid of door we are going towards */
 doorPos.x=0; doorPos.y=0; doorPos.z=0;
 w=this->route[this->routePos];
 for (v=0;v<4;v++)
    {getVertex(targetWall->v[v],&wallP);
     doorPos.x+=f(wallP.x);
     doorPos.y+=f(wallP.y);
     doorPos.z+=f(wallP.z);
    }
 doorPos.x=doorPos.x<<(16-2);
 doorPos.y=doorPos.y<<(16-2);
 doorPos.z=doorPos.z<<(16-2);

 this->sprite->angle=getAngle(doorPos.x-this->sprite->pos.x,
			      doorPos.z-this->sprite->pos.z);

 return doorPos;
}


static void spriteObject_makeSound(SpriteObject *this,int sndNm)
{assert(this->type<OT_NMTYPES);
 assert(sndNm>=0);
 spriteMakeSound(this->sprite,level_objectSoundMap[this->type]+sndNm);
}

static void spriteHome(SpriteObject *this,SpriteObject *enemy,
		       int fudge,int step)
{int angle=getAngle(enemy->sprite->pos.x-
		    this->sprite->pos.x,
		    enemy->sprite->pos.z-
		    this->sprite->pos.z);
 angle=normalizeAngle(angle-this->sprite->angle);

 if (angle<-fudge)
    this->sprite->angle=
       normalizeAngle(this->sprite->angle-step);
 if (angle>fudge)
    this->sprite->angle=
       normalizeAngle(this->sprite->angle+step);
}

static void setState(SpriteObject *this,int state)
{assert(level_sequenceMap[this->type]!=-2);
 this->state=state;
 this->sprite->sequence=level_sequenceMap[this->type]+
    (this->sequenceMap[state] & 0x7fff);
 this->sprite->frame=0;
}

/* returns 1 if found enemy, 0 if seeking */
int monster_seekEnemy(MonsterObject *this,int floater,int speed2)
{assert(this->class==CLASS_MONSTER);
 if (!this->enemy)
    {setState((SpriteObject *)this,0);
     this->routePos=-1;
     return 0;
    }
 if (canSee(this->sprite,this->enemy->sprite))
    {this->routePos=-1;
     return 1;
    }
 else
    {if (this->routePos==-1)
	if (!plotRouteToObject(this,
			       (SpriteObject *)this->enemy,
			       floater))
	   {setState((SpriteObject *)this,0);
	    return 0;
	   }
     {MthXyz p=followRoute(this);
      if (this->routePos==-1)
	 return 0;
      this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<speed2;
      this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<speed2;
      if (floater)
	 {if (this->sprite->pos.y<p.y-F(10))
	     this->sprite->vel.y=F(1);
	  else
	    if (p.y>this->enemy->sprite->pos.y+F(10))
	       this->sprite->vel.y=-F(1);
	    else
	       this->sprite->vel.y=0;
	 }
     }
    }
 return 0;
}

enum {STATE_IDLE,STATE_WALK,STATE_SRA,STATE_LRA,
	 STATE_HIT,STATE_SEEK,STATE_NMBASICSTATES};

void normalMonster_idle(MonsterObject *this,int speed,int icuSound)
{if ((this->aiSlot&0x1f)==(aicount&0x1f))
    {this->sprite->vel.x=0;
     this->sprite->vel.z=0;
     this->enemy=findPlayer(this->sprite,F(1600));
     if (this->enemy)
	{if (icuSound>=0)
	    spriteObject_makeSound((SpriteObject *)this,icuSound);
	 setState((SpriteObject *)this,STATE_WALK);
	 PlotCourseToObject((SpriteObject *)this,
			    (SpriteObject *)this->enemy);
	 this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<speed;
	 this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<speed;
	}
    }
}

enum {DO_SEEK,DO_RANDOMWANDER,DO_CHARGE,DO_FOLLOWROUTE,DO_GOIDLE};
int decideWhatToDo(MonsterObject *this,int route,int floater)
{if (canSee(this->sprite,this->enemy->sprite))
    {if ((this->sprite->flags ^ this->enemy->sprite->flags) &
	 SPRITEFLAG_UNDERWATER)
	/* we and our enemy are on different sides of the water */
	return DO_RANDOMWANDER;
     return DO_CHARGE;
    }
 if (route)
    {if (plotRouteToObject(this,
			   (SpriteObject *)this->enemy,
			   floater))
	return DO_FOLLOWROUTE;
    }
 if (spriteDistApprox(this->sprite,this->enemy->sprite)>1000)
    return DO_GOIDLE;
 return DO_RANDOMWANDER;
}

void normalMonster_walking(MonsterObject *this,int collide,int fflags,
			   int speed)
{assert(this->enemy);
 if ((collide & COLLIDE_SPRITE) &&
     sprites[collide&0xffff].owner==(Object *)this->enemy)
    {/* bite them! */
     setState((SpriteObject *)this,STATE_SRA);
     this->sprite->vel.x=0;
     this->sprite->vel.z=0;
    }
 if (collide & COLLIDE_WALL)
    {this->sprite->angle=
	normalizeAngle(this->sprite->angle+/*F(180)+*/
		       randomAngle(8));
     this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<speed;
     this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<speed;
    }
 if ((this->aiSlot&0x0f)==(aicount&0x0f))
    {if (!canSee(this->sprite,this->enemy->sprite))
	{this->sprite->vel.x=0;
	 this->sprite->vel.z=0;
	 setState((SpriteObject *)this,STATE_SEEK);
	 return;
	}
     PlotCourseToObject((SpriteObject *)this,
			(SpriteObject *)this->enemy);
     /* if we get here, we know we can see the player */
     if (getNextRand()&0x20)
	{/* shoot player */
	 setState((SpriteObject *)this,STATE_LRA);
	 this->sprite->vel.x=0;
	 this->sprite->vel.z=0;
	}
     else
	{this->sprite->angle=
	    normalizeAngle(this->sprite->angle+randomAngle(6));
	 this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<speed;
	 this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<speed;
	}
    }
}


static void initProjectile(MthXyz *from,MthXyz *to,
			   MthXyz *outPos,MthXyz *outVel,
			   int height,
			   int speed2)
{Fixed32 d;
 d=approxDist(from->x-to->x,
	      from->y-to->y,
	      from->z-to->z);
 d=d>>speed2;
 *outPos=*from;
 outPos->y+=height;
 outVel->x=MTH_Div(to->x-outPos->x,d);
 outVel->y=MTH_Div(to->y-outPos->y,d);
 outVel->z=MTH_Div(to->z-outPos->z,d);
 outPos->x+=outVel->x<<3;
 outPos->y+=outVel->y<<3;
 outPos->z+=outVel->z<<3;
}


/********************************\
 *          PLAYER STUFF        *
\********************************/

void player_func(Object *o,int message,int param1,int param2)
{switch (message)
    {case SIGNAL_HURT:
	playerHurt(param1);
	break;
       }
}

unsigned short playerSeqList[]={-1};
PlayerObject *constructPlayer(int sector)
{PlayerObject *this=(PlayerObject *)getFreeObject(player_func,OT_PLAYER,
						  CLASS_MONSTER);
 moveObject((Object *)this,objectIdleList);

 this->sprite=newSprite(sector,F(47),0.90*65536.0,GRAVITY,
			-1,0,
			(Object *)this);
/* suckSpriteParams(this->sprite); */
 this->health=700;
 this->sequenceMap=playerSeqList;
 this->state=0;
 return this;
}


/********************************\
 *          GENPROJ STUFF       *
\********************************/
void genproj_func(Object *_this,int msg,int param1,int param2)
{GenericProjectileObject *this=(GenericProjectileObject *)_this;
 int collide,fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    {if (this->light)
		removeLight(this->sprite);
	     freeSprite(this->sprite);
	    }
	 break;
	}
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	fflags=spriteAdvanceFrame(this->sprite);
	collide=moveSprite(this->sprite);
	if ((collide & COLLIDE_SPRITE) &&
	    sprites[collide&0xffff].owner!=this->owner)
	   {signalObject(sprites[collide&0xffff].owner,
			 SIGNAL_HURT,this->damage,
			 (int)this);
	   }
	if (collide & (COLLIDE_WALL|COLLIDE_FLOOR|COLLIDE_CEILING|
		       COLLIDE_SPRITE))
	   {if (this->light)
	       removeLight(this->sprite);
	    constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_POOF,F(1),this->explosionColor,0);
	    delayKill(_this);
	   }
	break;
       }
}

unsigned short genprojTurnSequenceMap[]={HB|0};
unsigned short genprojFlatSequenceMap[]={0};

Object *constructGenproj(int sector,MthXyz *pos,MthXyz *vel,
			 SpriteObject *owner,int heading,
			 int flat,int type,int damage,
			 int explosionColor,
			 char r,char g,char b)
{GenericProjectileObject *this=(GenericProjectileObject *)
    getFreeObject(genproj_func,type,CLASS_PROJECTILE);
 assert(this);
 if (!this)
    return NULL;
 assert(level_sequenceMap[type]>=0);
 assert(level_sequenceMap[type]<1000);
 this->sprite=newSprite(sector,F(16),F(1),0,
			level_sequenceMap[type],
			SPRITEFLAG_IMATERIAL,(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
 this->sprite->angle=heading;
 if (flat)
    this->sequenceMap=genprojFlatSequenceMap;
 else
    this->sequenceMap=genprojTurnSequenceMap;
 setState((SpriteObject *)this,0);
 this->owner=(Object *)owner;
 this->damage=damage;
 this->explosionColor=explosionColor;
 if (r || g || b)
    {addLight(this->sprite,16-r,16-g,16-b);
     this->light=1;
    }
 else
    this->light=0;
 return (Object *)this;
}

/********************************\
 *           BIT STUFF          *
\********************************/

void bit_func(Object *_this,int msg,int param1,int param2)
{BitObject *this=(BitObject *)_this;
 int collide;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	collide=moveSprite(this->sprite);
	if (collide & COLLIDE_FLOOR)
	   {this->sprite->vel.x=0;
	    this->sprite->vel.y=0;
	    this->sprite->vel.z=0;
	    this->sprite->flags|=SPRITEFLAG_IMMOBILE;
	   }

	this->age++;
	if (this->age>90)
	   this->sprite->scale=32000-((this->age-90)<<11);
	if (this->age>90+15)
	   delayKill(_this);
	break;
       }
}

Object *constructBit(int sector,MthXyz *pos,int sequence,int onFloor)
{BitObject *this=(BitObject *)getFreeObject(bit_func,OT_BIT,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 level_sequenceMap[OT_BIT]=0; /* yak! */
 this->sprite=newSprite(sector,F(16),F(1),GRAVITY<<1,
			0,SPRITEFLAG_IMATERIAL,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->scale=32000;
 this->sprite->pos=*pos;
 this->sprite->vel.x=(MTH_GetRand()&0x7ffff)-F(4);
 this->sprite->vel.z=(MTH_GetRand()&0x7ffff)-F(4);
 if (onFloor)
    this->sprite->vel.y=F(8)+(MTH_GetRand()&0x3ffff);
 else
    this->sprite->vel.y=(MTH_GetRand()&0x0fffff);

 this->sequence=sequence;
 this->sequenceMap=&this->sequence;
 this->age=getNextRand()&0x1f;
 setState((SpriteObject *)this,0);
 return (Object *)this;
}

void makeExplosion(MonsterObject *this,int bitSeqStart,int nmBits)
{int seq,i;
 seq=level_sequenceMap[this->type]+bitSeqStart;
 for (i=0;i<nmBits;i++)
    constructBit(this->sprite->s,&this->sprite->pos,seq++,
		 this->sprite->floorSector!=-1);
}


/********************************\
 *        HARDBIT STUFF         *
\********************************/

void hardBit_func(Object *_this,int msg,int param1,int param2)
{HardBitObject *this=(HardBitObject *)_this;
 int collide;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	if (this->stuck)
	   break;
	collide=moveSprite(this->sprite);
	if (collide & COLLIDE_FLOOR)
	   {this->sprite->vel.x=0;
	    this->sprite->vel.y=0;
	    this->sprite->vel.z=0;
	    this->stuck=1;
	    this->sprite->flags|=SPRITEFLAG_IMMOBILE;
	   }
	break;
       }
}

Object *constructHardBit(int sector,MthXyz *pos,int sequence,int onFloor)
{HardBitObject *this=(HardBitObject *)
    getFreeObject(hardBit_func,OT_HARDBIT,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 level_sequenceMap[OT_HARDBIT]=0; /* yak! */
 this->sprite=newSprite(sector,F(16),F(1),GRAVITY<<1,
			0,SPRITEFLAG_IMATERIAL,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->scale=32000;
 this->sprite->pos=*pos;
 this->sprite->vel.x=(MTH_GetRand()&0x7ffff)-F(4);
 this->sprite->vel.z=(MTH_GetRand()&0x7ffff)-F(4);
 if (onFloor)
    this->sprite->vel.y=F(8)+(MTH_GetRand()&0x3ffff);
 else
    this->sprite->vel.y=(MTH_GetRand()&0x0fffff);

 this->sequence=sequence;
 this->sequenceMap=&this->sequence;
 this->stuck=0;
 setState((SpriteObject *)this,0);
 return (Object *)this;
}

/********************************\
 *           ZORCH STUFF        *
\********************************/

void zorch_func(Object *_this,int msg,int param1,int param2)
{ZorchObject *this=(ZorchObject *)_this;
 int fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	this->age++;
	if (this->age<30)
	   break;
	if (this->age==30)
	   this->sprite->flags&=~SPRITEFLAG_INVISIBLE;

	fflags=spriteAdvanceFrame(this->sprite);
	if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
	   {int big=(getNextRand()&0x40);
	    if (this->type==OT_BLUEZORCH)
	       constructThing(this->sprite->s,
			      0,
			      this->sprite->pos.x,
			      this->sprite->pos.y,
			      this->sprite->pos.z,
			      big?OT_AMMOORB:OT_AMMOBALL);
	    if (this->type==OT_REDZORCH)
	       constructThing(this->sprite->s,
			      0,
			      this->sprite->pos.x,
			      this->sprite->pos.y,
			      this->sprite->pos.z,
			      big?OT_HEALTHORB:OT_HEALTHBALL);
	    delayKill(_this);
	   }
	break;
       }
}

unsigned short zorchSequenceMap[]={0};

Object *constructZorch(int sector,int x,int y,int z,int zorchType)
{ZorchObject *this=(ZorchObject *)
    getFreeObject(zorch_func,zorchType,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMATERIAL|
			SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos.x=x;
 this->sprite->pos.y=y;
 this->sprite->pos.z=z;
 this->sprite->scale=48000;
 this->sequenceMap=zorchSequenceMap;
 this->age=0;
 setState((SpriteObject *)this,0);
 return (Object *)this;
}



/********************************\
 *          SPIDER STUFF        *
\********************************/

enum {AI_SPIDER_IDLE,AI_SPIDER_WALK,AI_SPIDER_BITE,AI_SPIDER_JUMPUP,
	 AI_SPIDER_JUMPDOWN,AI_SPIDER_HIT,AI_SPIDER_DEATH,AI_SPIDER_NMSEQ};

unsigned short spiderSeqMap[]={HB|0, HB|0, HB|0, HB|8,
				  HB|0, HB|0, 16};

void spider_func(Object *_this,int message,int param1,int param2)
{SpiderObject *this=(SpiderObject *)_this;
 int collide=0,fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {setState((SpriteObject *)this,AI_SPIDER_DEATH);
	   }
	else
	   spriteObject_makeSound((SpriteObject *)this,0);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_SPIDER_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_SPIDER_IDLE);
	   }
	switch (this->state)
	   {case AI_SPIDER_DEATH:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {constructOneShot(this->sprite->s,
				    this->sprite->pos.x,
				    this->sprite->pos.y,
				    this->sprite->pos.z,
				    this->sprite->floorSector==-1?
				        OT_AIRGUTS:OT_LANDGUTS,
				    48000,0,0);
		   spriteObject_makeSound((SpriteObject *)this,1);
		   makeExplosion((MonsterObject *)this,17,4);
		   makeExplosion((MonsterObject *)this,17,4);
		   delayKill(_this);
		  }
	       break;
	    case (AI_SPIDER_IDLE) :
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {if (!this->enemy)
		      this->enemy=findPlayer(this->sprite,F(900));
		   if (this->enemy)
		      {setState((SpriteObject *)this,AI_SPIDER_WALK);
		       break;
		      }
		  }
	       break;

	    case AI_SPIDER_JUMPUP:
	       if (this->sprite->vel.y<=0)
		  setState((SpriteObject *)this,AI_SPIDER_JUMPDOWN);
	       break;
	    case AI_SPIDER_JUMPDOWN:
	       if (this->sprite->floorSector!=-1)
		  setState((SpriteObject *)this,AI_SPIDER_WALK);
	       break;
	    case AI_SPIDER_WALK:
	       if (collide & COLLIDE_SPRITE)
		  if (sprites[collide&0xffff].owner==(Object *)this->enemy)
		     {/* bite them! */
		      setState((SpriteObject *)this,AI_SPIDER_BITE);
		      this->sprite->vel.x=0;
		      this->sprite->vel.z=0;
		     }
	       if (collide & COLLIDE_WALL)
		  {this->sprite->angle=
		      normalizeAngle(this->sprite->angle+/*F(180)+*/
				     randomAngle(8));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		  }
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		   if (getNextRand()&1)
		      {this->sprite->vel.y = F(16);
		       spriteObject_makeSound((SpriteObject *)this,2);
		       setState((SpriteObject *)this,AI_SPIDER_JUMPUP);
		      }
		  }
	       break;
	    case AI_SPIDER_BITE:
	       if (this->sprite->frame==2)
		  {spriteObject_makeSound((SpriteObject *)this,3);
		   assert(this->enemy);
		   signalObject((Object *)this->enemy,SIGNAL_HURT,10,
				(int)this);
		   setState((SpriteObject *)this,AI_SPIDER_WALK);
		  }
	       break;
	      }
	break;
       }
}

Object *constructSpider(int sector)
{SpiderObject *this=(SpiderObject *)getFreeObject(spider_func,OT_SPIDER,
						  CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(16),F(1),GRAVITY<<2,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=40000;
 this->sequenceMap=spiderSeqMap;
 setState((SpriteObject *)this,AI_SPIDER_IDLE);
 this->health=20;
 this->enemy=NULL;
 this->aiSlot=nextAiSlot++;
 return (Object *)this;
}


/********************************\
 *           FISH STUFF         *
\********************************/

enum {AI_FISH_IDLE,AI_FISH_WALK,AI_FISH_BITE};

unsigned short fishSeqMap[]={HB|8, HB|0, HB|19};

void fish_func(Object *_this,int message,int param1,int param2)
{FishObject *this=(FishObject *)_this;
 int collide=0,fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_AIRGUTS,48000,0,0);
	    spriteObject_makeSound((SpriteObject *)this,1);
	    makeExplosion(this,16,3);
	    makeExplosion(this,16,3);
	    delayKill(_this);
	   }
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_FISH_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_FISH_IDLE);
	   }
	if (this->state==AI_FISH_WALK && this->routePos==-1 &&
	    (this->aiSlot&0x07)==(aicount&0x07))
	   if (this->enemy)
	      {if (this->sprite->pos.y<this->enemy->sprite->pos.y-F(10))
		  this->sprite->vel.y=F(1);
	       else
		  if (this->sprite->pos.y>this->enemy->sprite->pos.y+F(10))
		     this->sprite->vel.y=-F(1);
		  else
		     this->sprite->vel.y=0;
	      }
	   else
	      this->sprite->vel.y=0;
	switch (this->state)
	   {case (AI_FISH_IDLE) :
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {this->sprite->vel.x=0;
		   this->sprite->vel.y=0;
		   if (!this->enemy)
		      this->enemy=findPlayer(this->sprite,F(900));
		   if (this->enemy)
		      {spriteObject_makeSound((SpriteObject *)this,0);
		       setState((SpriteObject *)this,AI_FISH_WALK);
		       break;
		      }
		  }
	       break;
	    case AI_FISH_WALK:
	       if (collide & COLLIDE_SPRITE)
		  if (sprites[collide&0xffff].owner==(Object *)this->enemy)
		     {/* bite them! */
		      setState((SpriteObject *)this,AI_FISH_BITE);
		      this->sprite->vel.x=0;
		      this->sprite->vel.y=0;
		      this->sprite->vel.z=0;
		     }
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  if (monster_seekEnemy(this,1,2))
		     {PlotCourseToObject((SpriteObject *)this,
					 (SpriteObject *)this->enemy);
		      this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		      this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		     }
	       break;
	    case AI_FISH_BITE:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (spriteDistApprox(this->sprite,
					this->enemy->sprite)<F(150))
		      {PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       signalObject((Object *)this->enemy,SIGNAL_HURT,70,
				    (int)this);
		       setState((SpriteObject *)this,AI_FISH_WALK);
		      }
		   else
		      setState((SpriteObject *)this,AI_FISH_WALK);
		  }
	       break;
	      }
	break;
       }
}

Object *constructFish(int sector)
{FishObject *this=(FishObject *)getFreeObject(fish_func,OT_FISH,CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_BWATER,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=40000;
 this->sequenceMap=fishSeqMap;
 setState((SpriteObject *)this,AI_FISH_IDLE);
 this->health=40;
 this->routePos=-1;
 this->enemy=NULL;
 this->aiSlot=nextAiSlot++;
 return (Object *)this;
}


/********************************\
 *          COBRA  STUFF        *
\********************************/

enum {AI_COBRA_SEEK,AI_COBRA_HOME};

unsigned short cobraSeqMap[]={HB|0,HB|0};

void cobra_func(Object *_this,int message,int param1,int param2)
{CobraObject *this=(CobraObject *)_this;
 int collide,fflags,i;
 static int lightP[]={F(5),F(25),-F(1),0,
		      F(0),F(25),0,0,
		      F(5),F(25),-F(1),0};
 int c,s,wave;
 switch (message)
    {case SIGNAL_HURT:
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==(Object *)this->enemy)
	    this->enemy=NULL;
	 if (killed==_this)
	    {freeSprite(this->sprite);
	     for (i=0;i<NMCOBRABALLS;i++)
		freeSprite(this->balls[i]);
	    }
	 break;
	}
     case SIGNAL_MOVE:
	this->age++;
	for (i=NMCOBRABALLS-1;i;i--)
	   {moveSpriteTo(this->balls[i],
			 this->balls[i-1]->s,
			 &this->balls[i-1]->pos);
	    this->balls[i]->flags=
	       this->balls[i-1]->flags;
	   }
	moveSpriteTo(this->balls[0],
		     this->sprite->s,
		     &this->sprite->pos);

	collide=moveSprite(this->sprite);
	fflags=spriteAdvanceFrame(this->sprite);
	/* do motion */
	c=MTH_Cos(this->sprite->angle);
	s=MTH_Sin(this->sprite->angle);
	this->sprite->vel.x=c<<4;
	this->sprite->vel.z=s<<4;

	wave=MTH_Cos(this->wave);
	this->wave+=F(20);
	if (this->wave>F(180))
	   this->wave-=F(360);
	this->sprite->vel.x+=MTH_Mul(s,wave)<<2;
	this->sprite->vel.z+=MTH_Mul(-c,wave)<<2;

	switch (this->state)
	   {case AI_COBRA_HOME:
	       if (!this->enemy)
		  {setState((SpriteObject *)this,AI_COBRA_SEEK);
		   break;
		  }
	       {spriteHome((SpriteObject *)this,(SpriteObject *)this->enemy,
			   F(10),F(5));
		if (this->sprite->pos.y<this->enemy->sprite->pos.y+
		    this->enemy->sprite->radius-F(20))
		   this->sprite->vel.y=F(4);
		else
		   if (this->sprite->pos.y>this->enemy->sprite->pos.y+
		       this->enemy->sprite->radius+F(20))
		      this->sprite->vel.y=-F(4);
		   else
		      this->sprite->vel.y=0;
		break;
	       }
	    case AI_COBRA_SEEK:
	       {if (this->age<10)
		   break;
		if ((this->aiSlot&0x3)!=(aicount&0x3))
		   break;
		/* look for enemy */
		{Object *o;
		 Object *list;
		 MonsterObject *m;
		 MonsterObject *best;
		 int rating,bestRating;
		 best=NULL;
		 bestRating=F(10000);
		 for (list=o=objectRunList;;o=o->next)
		    {if (!o)
			if (list==objectRunList)
			   {list=objectIdleList;
			    o=list;
			    continue;
			   }
			else
			   break;
		     if (o->class!=CLASS_MONSTER || o==this->owner)
			continue;
		     m=(MonsterObject *)o;
		     rating=approxDist(m->sprite->pos.x-this->sprite->pos.x,
				       m->sprite->pos.y-this->sprite->pos.y,
				       m->sprite->pos.z-this->sprite->pos.z);
		     if (rating>F(1000))
			continue;
		     rating=(rating>>2)+
			abs(normalizeAngle(getAngle(m->sprite->pos.x-
						    this->sprite->pos.x,
						    m->sprite->pos.z-
						    this->sprite->pos.z)-
					   this->sprite->angle));
		     if (rating>bestRating)
			continue;
		     if (!canSee(this->sprite,m->sprite))
			continue;
		     rating=bestRating;
		     best=m;
		    }
		 if (best)
		    {this->enemy=best;
		     setState((SpriteObject *)this,AI_COBRA_HOME);
		    }
		 break;
		}
	       }
	      }
	if (collide & (COLLIDE_SPRITE|COLLIDE_WALL))
	   {if ((collide & COLLIDE_SPRITE) &&
		(sprites[collide&0xffff].owner==this->owner))
	       break;
	    for (i=0;i<3;i++)
	       constructOneShot(this->sprite->s,
				this->sprite->pos.x+(((int)MTH_GetRand())>>10),
				this->sprite->pos.y+(((int)MTH_GetRand())>>10)
				         +F(10),
				this->sprite->pos.z+(((int)MTH_GetRand())>>10),
				OT_POOF,F(1),RGB(5,20,5),i*3);
	    constructLight(this->sprite->s,
			   this->sprite->pos.x,
			   this->sprite->pos.y+F(30),
			   this->sprite->pos.z,
			   lightP,F(1)/16);
	    if (this->type==OT_MUMBALL)
	       radialDamage((Object *)this,&this->sprite->pos,50,F(200));
	    else
	       radialDamage((Object *)this,&this->sprite->pos,100,F(300));
	    delayKill(_this);
	   }
	break;
       }
}

Object *constructCobra(int sector,int x,int y,int z,int heading,Fixed32 yvel,
		       SpriteObject *owner,int type)
{int i;
 CobraObject *this=(CobraObject *)getFreeObject(cobra_func,type,
						CLASS_PROJECTILE);
 assert(this);
 if (!this)
    return NULL;

 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL,(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos.x=x;
 this->sprite->pos.y=y;
 this->sprite->pos.z=z;
 this->sprite->scale=40000;
 this->sequenceMap=cobraSeqMap;
 this->sprite->angle=heading;
 this->sprite->vel.y=yvel;
 setState((SpriteObject *)this,AI_COBRA_SEEK);

 for (i=0;i<NMCOBRABALLS;i++)
    {this->balls[i]=newSprite(sector,F(16),F(1),0,
			      level_sequenceMap[type]+8,
			      SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMMOBILE|
			      SPRITEFLAG_IMATERIAL,
			      NULL);
     if (!this->balls[i])
	{for (i--;i>=0;i--)
	    freeSprite(this->balls[i]);
	 freeSprite(this->sprite);
	 moveObject((Object *)this,objectFreeList);
	 return NULL;
	}
     this->balls[i]->pos.x=x;
     this->balls[i]->pos.y=y;
     this->balls[i]->pos.z=z;
     this->balls[i]->scale=40000-i*5000;
    }
 this->balls[0]->flags&=~SPRITEFLAG_INVISIBLE;
 this->wave=0;
 this->owner=(Object *)owner;
 this->age=0;
 this->enemy=NULL;
 this->aiSlot=nextAiSlot++;
 return (Object *)this;
}

/********************************\
 *          ZAP  STUFF        *
\********************************/

void zap_func(Object *_this,int message,int param1,int param2)
{ZapObject *this=(ZapObject *)_this;
 int i,collide;
 switch (message)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==(Object *)this->enemy)
	    this->enemy=NULL;
	 if (killed==_this)
	    {freeSprite(this->sprite);
	     for (i=0;i<NMZAPBALLS;i++)
		freeSprite(this->balls[i]);
	    }
	 break;
	}
     case SIGNAL_MOVE:
	if (!this->enemy)
	   {delayKill((Object *)this);
	    break;
	   }
	assert(this->enemy->class==CLASS_MONSTER);
	for (i=0;i<NMZAPBALLS;i++)
	   {this->balls[i]->angle=this->balls[i]->pos.x;
	    this->balls[i]->scale=this->balls[i]->pos.y;
	    this->balls[i]->frame=this->balls[i]->pos.z;
	   }
	this->sprite->angle=this->sprite->pos.x;
	this->sprite->scale=this->sprite->pos.y;
	this->sprite->frame=this->sprite->pos.z;
	for (i=NMZAPBALLS-1;i;i--)
	   {moveSpriteTo(this->balls[i],
			 this->balls[i-1]->s,
			 &this->balls[i-1]->pos);
	    this->balls[i]->flags=
	       this->balls[i-1]->flags;
	   }
	moveSpriteTo(this->balls[0],
		     this->sprite->s,
		     &this->sprite->pos);
	{Fixed32 tDist;
	 MthXyz del;
	 del.x=this->enemy->sprite->pos.x-this->sprite->pos.x;
	 del.y=this->enemy->sprite->pos.y-this->sprite->pos.y;
	 del.z=this->enemy->sprite->pos.z-this->sprite->pos.z;
	 tDist=approxDist(del.x,del.y,del.z);
	 tDist/=32; /* distance to move */
	 del.x=MTH_Div(del.x,tDist)+(((short)getNextRand())<<4);
	 del.y=MTH_Div(del.y,tDist)+(((short)getNextRand())<<4);
	 del.z=MTH_Div(del.z,tDist)+(((short)getNextRand())<<4);
	 /* del is now a length 64 vector pointing in the right direction */
	 this->sprite->vel=del;
	}
	collide=moveSprite(this->sprite);
	if ((collide & COLLIDE_SPRITE) &&
	    sprites[collide&0xffff].owner!=this->owner)
	   {if (this->red)
	       {int i;
		MthXyz vel;
		for (i=0;i<5;i++)
		   {vel.x=(MTH_GetRand()&0xfffff)-F(8);
		    vel.z=(MTH_GetRand()&0xfffff)-F(8);
		    vel.y=F(10)+(MTH_GetRand()&0x1ffff);
		    constructRingo(this->sprite->s,&this->sprite->pos,
				   &vel,(SpriteObject *)player);
		   }
	       }
	    signalObject(sprites[collide&0xffff].owner,
			 SIGNAL_HURT,50,
			 (int)this);
	    delayKill((Object *)this);
	   }
       }
}

Object *constructZap(int sector,MthXyz *pos,SpriteObject *owner,
		     MonsterObject *enemy,int red,
		     MthXyz *hitPos /* if enemy=NULL */)
{int i;
 ZapObject *this=(ZapObject *)getFreeObject(zap_func,OT_LIGHTNING,
					    CLASS_PROJECTILE);
 assert(this);
 if (!this)
    return NULL;
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_LINE,(Object *)this);
 if (!this->sprite)
    return NULL;
 this->sprite->pos=*pos;
 this->sprite->angle=pos->x;
 this->sprite->scale=pos->y;
 this->sprite->frame=pos->z;
 this->sprite->color=RGB(31,31,31);
 for (i=0;i<NMZAPBALLS;i++)
    {this->balls[i]=newSprite(sector,F(16),F(1),0,
			      level_sequenceMap[OT_HEALTHBALL],
			      SPRITEFLAG_LINE|
			      SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMMOBILE|
			      SPRITEFLAG_IMATERIAL,
			      NULL);
     this->balls[i]->pos=*pos;
     this->balls[i]->angle=pos->x;
     this->balls[i]->scale=pos->y;
     this->balls[i]->frame=pos->z;
     this->balls[i]->color=
	red?RGB(31-(i<<2),16-i,16-i):
	   RGB(31-(i<<2),31-(i<<2),31-(i<<2));
    }
 this->balls[0]->flags&=~SPRITEFLAG_INVISIBLE;
 this->owner=(Object *)owner;
 this->enemy=enemy;
 assert(this->enemy->class==CLASS_MONSTER);
 this->aiSlot=nextAiSlot++;
 this->red=red;
 this->hitPos=*hitPos;
 return (Object *)this;
}


/********************************\
 *          HAWK STUFF          *
\********************************/
enum {AI_HAWK_SLEEP,AI_HAWK_GLIDE,AI_HAWK_DIVE,AI_HAWK_CHASE,
	 AI_HAWK_RETREAT,AI_HAWK_NMSTATES};

unsigned short hawkSeqMap[]={HB|8,HB|8,HB|8,HB|0,
				HB|16};

void hawk_func(Object *_this,int message,int param1,int param2)
{HawkObject *this=(HawkObject *)_this;
 int collide=0,fflags=0,i;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_AIRGUTS,48000,0,0);
	    for (i=0;i<10;i++)
	       constructBit(this->sprite->s,
			    &this->sprite->pos,
			    level_sequenceMap[OT_HAWK]+24,0);
	    delayKill(_this);
	   }
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	if (this->state==AI_HAWK_SLEEP)
	   setState((SpriteObject *)this,AI_HAWK_GLIDE);
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_HAWK_SLEEP)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (this->state!=AI_HAWK_GLIDE && !this->enemy)
	       setState((SpriteObject *)this,AI_HAWK_GLIDE);
	   }
	if ((aicount&0x3f)==(this->aiSlot&0x3f) &&
	    (this->state==AI_HAWK_GLIDE))
	   {if (!this->enemy)
	       this->enemy=findPlayer(this->sprite,F(1800));
	    if (level_sector[this->enemy->sprite->s].flags & SECFLAG_WATER)
	       this->enemy=NULL;
	    if (this->enemy)
	       {setState((SpriteObject *)this,AI_HAWK_DIVE);
		spriteObject_makeSound((SpriteObject *)this,0);
		this->diveTimer=0;
		PlotCourseToObject((SpriteObject *)this,
				   (SpriteObject *)this->enemy);
		this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		break;
	       }
	   }
	switch (this->state)
	   {case AI_HAWK_DIVE:
	       if (this->diveTimer++>90)
		  setState((SpriteObject *)this,AI_HAWK_CHASE);

	    case AI_HAWK_CHASE:
	       if ((aicount&0x07)==(this->aiSlot&0x07))
		  {if (level_sector[this->enemy->sprite->s].flags &
		       SECFLAG_WATER)
		      setState((SpriteObject *)this,AI_HAWK_SLEEP);
		   this->jinkTimer+=F(2);
		   if (this->jinkTimer>F(10))
		      this->jinkTimer-=F(20);
		   PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   this->sprite->angle=normalizeAngle(this->sprite->angle+
						      this->jinkTimer);
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<4;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<4;
		  }
	       this->sprite->vel.y=
		  (F(10)+this->enemy->sprite->pos.y-this->sprite->pos.y)>>4;
	       if ((collide & COLLIDE_SPRITE) &&
		   sprites[collide&0xffff].owner==(Object *)this->enemy)
		  {signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				(int)this);
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+F(180));
		   setState((SpriteObject *)this,AI_HAWK_RETREAT);
		   this->diveTimer=0;
		  }
	       break;
	    case AI_HAWK_GLIDE:
	       if ((aicount&0x07)==(this->aiSlot&0x07))
		  {this->sprite->angle=
		      normalizeAngle(this->sprite->angle+F(12));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<3;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<3;
		  }
	       break;
	    case AI_HAWK_RETREAT:
	       if ((aicount&0x03)==(this->aiSlot&0x03))
		  {this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<3;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<3;
		   this->sprite->vel.y=6<<15;
		  }
	       if (this->diveTimer++>90)
		  setState((SpriteObject *)this,AI_HAWK_GLIDE);
	       break;
	      }
	break;
       }
}

Object *constructHawk(int sector)
{HawkObject *this=(HawkObject *)getFreeObject(hawk_func,OT_HAWK,CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(24),65536*0.95,0,
			0,SPRITEFLAG_BWATER,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=65000;
 this->sequenceMap=hawkSeqMap;
 setState((SpriteObject *)this,AI_HAWK_SLEEP);
 this->health=20;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->diveTimer=0;
 this->jinkTimer=0;
 return (Object *)this;
}


/********************************\
 *          WASP STUFF          *
\********************************/

enum {AI_WASP_IDLE,AI_WASP_HOVER,AI_WASP_DART,AI_WASP_BITE,AI_WASP_SEEK,
	 AI_WASP_NMSTATES};

unsigned short waspSeqMap[]={HB|0,HB|0,HB|0,HB|9,HB|0};

void wasp_func(Object *_this,int message,int param1,int param2)
{WaspObject *this=(WaspObject *)_this;
 int collide=0,fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_AIRGUTS,48000,0,0);
	    spriteObject_makeSound((SpriteObject *)this,0);
	    makeExplosion((MonsterObject *)this,17,5);
	    makeExplosion((MonsterObject *)this,17,5);
	    delayKill(_this);
	   }
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_WASP_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_WASP_IDLE);
	   }
	if (this->enemy && this->state!=AI_WASP_SEEK &&
	    (this->aiSlot&0x07)==(aicount&0x07))
	   if (this->sprite->pos.y<this->enemy->sprite->pos.y)
	      this->sprite->vel.y+=(1<<16)-(this->sprite->vel.y>>4);
	   else
	      this->sprite->vel.y+=(-1<<16)-(this->sprite->vel.y>>4);

	switch (this->state)
	   {case AI_WASP_IDLE:
	       if (!this->enemy && (this->aiSlot&0x1f)==(aicount&0x1f))
		  this->enemy=findPlayer(this->sprite,F(800));
	       if (this->enemy)
		  {setState((SpriteObject *)this,AI_WASP_HOVER);
		   this->hoverCounter=3;
		  }
	       break;
	    case AI_WASP_HOVER:
	       this->hoverCounter--;
	       if (this->hoverCounter>0)
		  break;
	       this->hoverCounter=10;
	       assert(this->enemy);
	       if (canSee(this->sprite,this->enemy->sprite))
		  {setState((SpriteObject *)this,AI_WASP_DART);
		   PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+randomAngle(5));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<4;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<4;
		   this->dartCounter=20;
		   break;
		  }
	       else
		  setState((SpriteObject *)this,AI_WASP_SEEK);
	       break;
	    case AI_WASP_DART:
	       if (collide & COLLIDE_SPRITE)
		  if (sprites[collide&0xffff].owner==(Object *)this->enemy)
		     {/* bite them! */
		      setState((SpriteObject *)this,AI_WASP_BITE);
		      PlotCourseToObject((SpriteObject *)this,
					 (SpriteObject *)this->enemy);
		      this->sprite->vel.x=0;
		      this->sprite->vel.z=0;
		     }
	       if (collide & COLLIDE_WALL)
		  {this->sprite->angle=
		      normalizeAngle(this->sprite->angle+/*F(180)+*/
				     randomAngle(8));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		  }
	       this->dartCounter--;
	       if (this->dartCounter>0)
		  break;
	       setState((SpriteObject *)this,AI_WASP_HOVER);
	       this->sprite->vel.x=0;
	       this->sprite->vel.z=0;
	       this->hoverCounter=getNextRand()&0x1f;
	       break;
	    case AI_WASP_BITE:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (this->enemy)
		      {signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				    (int)this);
		       this->sprite->vel.x=-MTH_Cos(this->sprite->angle)<<4;
		       this->sprite->vel.z=-MTH_Sin(this->sprite->angle)<<4;
		       setState((SpriteObject *)this,AI_WASP_DART);
		       this->dartCounter=6;
		      }
		  else
		      setState((SpriteObject *)this,AI_WASP_HOVER);
		  }
	       break;
	    case AI_WASP_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,1,4))
		  {setState((SpriteObject *)this,AI_WASP_HOVER);
		   this->sprite->vel.x=0;
		   this->sprite->vel.y=0;
		   this->sprite->vel.z=0;
		  }
	       break;
	      }
	break;
       }
}

Object *constructWasp(int sector)
{WaspObject *this=(WaspObject *)getFreeObject(wasp_func,OT_WASP,CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(24),65536*0.95,0,
			0,SPRITEFLAG_BWATER,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=50000;
 this->sequenceMap=waspSeqMap;
 setState((SpriteObject *)this,AI_WASP_IDLE);
 this->health=80;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->hoverCounter=10;
 return (Object *)this;
}


/********************************\
 *          ANUBALL STUFF       *
\********************************/
void anuball_func(Object *_this,int msg,int param1,int param2)
{ProjectileObject *this=(ProjectileObject *)_this;
 int collide,fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    {removeLight(this->sprite);
	     freeSprite(this->sprite);
	    }
	 break;
	}
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	fflags=spriteAdvanceFrame(this->sprite);
	if (!this->state)
	   {collide=moveSprite(this->sprite);
	    if (collide & COLLIDE_SPRITE &&
		sprites[collide&0xffff].owner!=this->owner)
	       {signalObject(sprites[collide&0xffff].owner,
			     SIGNAL_HURT,20,
			     (int)this);
		setState((SpriteObject *)this,1);
		changeLightColor(this->sprite,5,5,0);
	       }
	    if (collide & (COLLIDE_WALL|COLLIDE_FLOOR|COLLIDE_CEILING))
	       setState((SpriteObject *)this,1);
	   }
	else
	   {changeLightColor(this->sprite,
			     5+this->sprite->frame*2,
			     5+this->sprite->frame*2,
			     0+this->sprite->frame*2);
	    if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
	       delayKill(_this);
	   }
	break;
       }
}



unsigned short anuballSequenceMap[]={HB|0,8};

Object *constructAnuball(int sector,MthXyz *pos,MthXyz *vel,
			 SpriteObject *owner,int heading)
{ProjectileObject *this=(ProjectileObject *)
    getFreeObject(anuball_func,OT_ANUBALL,CLASS_PROJECTILE);
 if (!this)
    return NULL;
 assert(this);
 assert(level_sequenceMap[OT_ANUBALL]>=0);
 assert(level_sequenceMap[OT_ANUBALL]<1000);
 this->sprite=newSprite(sector,F(16),F(1),0,
			level_sequenceMap[OT_ANUBALL],
			SPRITEFLAG_IMATERIAL,(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
 this->sprite->angle=heading;
 this->sequenceMap=anuballSequenceMap;
 setState((SpriteObject *)this,0);
 this->owner=(Object *)owner;
 addLight(this->sprite,16,16,0);
 return (Object *)this;
}


/********************************\
 *           RINGO STUFF        *
\********************************/
void ringo_func(Object *_this,int msg,int param1,int param2)
{ProjectileObject *this=(ProjectileObject *)_this;
 int collide,fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	fflags=spriteAdvanceFrame(this->sprite);
	collide=moveSprite(this->sprite);
	if (collide & (COLLIDE_WALL|COLLIDE_FLOOR|COLLIDE_CEILING))
	   {this->age++;
	    if (this->age>6)
	       delayKill(_this);
	   }
	if (collide & COLLIDE_SPRITE &&
	    sprites[collide&0xffff].owner!=this->owner)
	   {signalObject(sprites[collide&0xffff].owner,
			 SIGNAL_HURT,20,
			 (int)this);
	    delayKill(_this);
	   }
	break;
       }
}

unsigned short ringoSequenceMap[]={0};

Object *constructRingo(int sector,MthXyz *pos,MthXyz *vel,
		       SpriteObject *owner)
{ProjectileObject *this=(ProjectileObject *)
    getFreeObject(ringo_func,OT_RINGO,CLASS_PROJECTILE);
 assert(this);
 if (!this)
    return NULL;
 assert(level_sequenceMap[OT_RINGO]>=0);
 assert(level_sequenceMap[OT_RINGO]<1000);
 this->sprite=newSprite(sector,F(16),F(1),GRAVITY<<2,
			level_sequenceMap[OT_RINGO],
			SPRITEFLAG_BOUNCY|SPRITEFLAG_IMATERIAL,(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
#if SPEEDOPTIMIZE
 this->sprite->maxSpeed=10;
#endif
 this->sequenceMap=ringoSequenceMap;
 setState((SpriteObject *)this,0);
 this->owner=(Object *)owner;
 this->age=0;
 return (Object *)this;
}


/********************************\
 *        FLAMEBALL STUFF       *
\********************************/
#define MAXFLAMEBALLAGE 50
static ProjectileObject *flameList[MAXFLAMEBALLAGE];
static int flameHead;
void initFlames(void)
{int i;
 for (i=0;i<MAXFLAMEBALLAGE;i++)
    flameList[i]=NULL;
 flameHead=0;
}

void translateFlames(int dx,int dy,int dz)
{int i;
 for (i=0;i<MAXFLAMEBALLAGE;i++)
    if (flameList[i] && flameList[i]->state<2)
       {flameList[i]->sprite->pos.x+=dx;
	flameList[i]->sprite->pos.y+=dy;
	flameList[i]->sprite->pos.z+=dz;
       }
}

void rotateFlames(int angle,int cx,int cz)
{int i,x,z;
 int C,S;
 for (i=0;i<MAXFLAMEBALLAGE;i++)
    if (flameList[i] && flameList[i]->state<2)
       {C=MTH_Cos(angle);
	S=MTH_Sin(angle);

	x=flameList[i]->sprite->pos.x-cx;
	z=flameList[i]->sprite->pos.z-cz;

	flameList[i]->sprite->pos.x=
	   MTH_Mul(C,x)-MTH_Mul(S,z)+cx;
	flameList[i]->sprite->pos.z=
	   MTH_Mul(C,z)+MTH_Mul(S,x)+cz;

	x=flameList[i]->sprite->vel.x;
	z=flameList[i]->sprite->vel.z;
	flameList[i]->sprite->vel.x=
	   MTH_Mul(C,x)-MTH_Mul(S,z);
	flameList[i]->sprite->vel.z=
	   MTH_Mul(C,z)+MTH_Mul(S,x);

       }
}


#define FLAMELEN 30
static unsigned short flameColor[FLAMELEN];
static int flameInit=0;

static void initFlameColor(void)
{const int colors[3][3]=
    {{15,15,31},
     {31,31,0},
     {15,0,0}};
 const int slideLen[]={5,20,0};
 int i,c,count;
 int r,g,b;
 Fixed32 frac1,frac2;

 flameInit=1;
 count=0;
 for (c=0;slideLen[c];c++)
    {for (i=0;i<slideLen[c];i++)
	{frac1=MTH_Div(i,slideLen[c]);
	 frac2=F(1)-frac1;
	 r=MTH_Mul(colors[c][0],frac2)+MTH_Mul(colors[c+1][0],frac1);
	 g=MTH_Mul(colors[c][1],frac2)+MTH_Mul(colors[c+1][1],frac1);
	 b=MTH_Mul(colors[c][2],frac2)+MTH_Mul(colors[c+1][2],frac1);
	 flameColor[count++]=RGB(r,g,b);
	}
    }
}

void flameball_func(Object *_this,int msg,int param1,int param2)
{ProjectileObject *this=(ProjectileObject *)_this;
 int collide,i;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    {freeSprite(this->sprite);
	     for (i=0;i<MAXFLAMEBALLAGE;i++)
		if (flameList[i]==this)
		   break;
	     assert(i<MAXFLAMEBALLAGE-1);
	     flameList[i]=NULL;
	    }
	 break;
	}
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	this->sprite->color=flameColor[this->age];
	break;
     case SIGNAL_MOVE:
	if (this->state!=2)
	   {this->age++;
	    if (this->age==2)
	       this->sprite->flags&=~SPRITEFLAG_INVISIBLE;
	    if (this->age>=2)
	       this->sprite->scale+=5000-((this->age-2)*100);
	    if (this->age==7)
	       {setState((SpriteObject *)this,1);
		this->sprite->frame=aicount%2;
	       }
	    collide=moveSprite(this->sprite);
	    if ((collide & COLLIDE_SPRITE) &&
		sprites[collide&0xffff].owner!=this->owner)
	       {assert((collide&0xffff)<500);
		assert(sprites[collide&0xffff].owner);
		this->sprite->pos.x+=(MTH_GetRand()&0xfffff)-F(8);
		this->sprite->pos.y+=(MTH_GetRand()&0xfffff)-F(8);
		this->sprite->pos.z+=(MTH_GetRand()&0xfffff)-F(8);

		signalObject(sprites[collide&0xffff].owner,
			     SIGNAL_HURT,2,
			     (int)this);
		this->sprite->flags&=~SPRITEFLAG_INVISIBLE;
		setState((SpriteObject *)this,2);
	       }
	    if (collide & (COLLIDE_WALL|COLLIDE_FLOOR|COLLIDE_CEILING))
	       {this->sprite->flags&=~SPRITEFLAG_INVISIBLE;
		setState((SpriteObject *)this,2);
		break;
	       }
	    if (this->age>20 /* 10 */)
	       delayKill(_this);
	   }
	else
	   {if (spriteAdvanceFrame(this->sprite) & FRAMEFLAG_ENDOFSEQUENCE)
	       delayKill(_this);
	   }
	break;
       }
}

unsigned short flameballSequenceMap[]={1,2,0};

Object *constructFlameball(int sector,MthXyz *pos,MthXyz *vel,
			   SpriteObject *owner,int heading,int frame)
{ProjectileObject *this=(ProjectileObject *)
    getFreeObject(flameball_func,OT_FLAMEBALL,CLASS_PROJECTILE);
 int i;
 assert(this);
 if (!this)
    return NULL;
 assert(level_sequenceMap[OT_FLAMEBALL]>=0);
 assert(level_sequenceMap[OT_FLAMEBALL]<1000);
 if (!flameInit)
    initFlameColor();
 this->sprite=newSprite(sector,F(4),F(1),0,
			level_sequenceMap[OT_FLAMEBALL],
			SPRITEFLAG_COLORED|SPRITEFLAG_IMATERIAL|
			SPRITEFLAG_INVISIBLE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->color=flameColor[0];
 this->sprite->scale=10000;
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
 this->sprite->angle=heading;
 this->sequenceMap=flameballSequenceMap;
 this->age=0;
 setState((SpriteObject *)this,0);
 this->owner=(Object *)owner;

 for (i=0;i<MAXFLAMEBALLAGE;i++)
    if (!flameList[i])
       break;
 assert(i<MAXFLAMEBALLAGE-1);
 flameList[i]=this;

 return (Object *)this;
}

/********************************\
 *         GRENADE STUFF        *
\********************************/

void grenade_func(Object *_this,int msg,int param1,int param2)
{ProjectileObject *this=(ProjectileObject *)_this;
 int collide,fflags;
 static int lightP[]={0,F(25),0,0,
		      F(5),F(25),0,0,
		      F(25),F(25),0,0};
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	this->age++;
	fflags=spriteAdvanceFrame(this->sprite);
	collide=moveSprite(this->sprite);
	if (collide)
	   {if ((collide & COLLIDE_SPRITE) &&
		(sprites[collide&0xffff].owner==this->owner))
	       break;
	    constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y+F(30),
			     this->sprite->pos.z,
			     OT_GRENPOW,F(2),0,0);
	    constructLight(this->sprite->s,
			   this->sprite->pos.x,
			   this->sprite->pos.y+F(30),
			   this->sprite->pos.z,
			   lightP,F(1)/16);
	    radialDamage((Object *)this,&this->sprite->pos,100,F(300));
	    delayKill(_this);
	   }
	break;
       }
}

unsigned short grenadeSequenceMap[]={0,1};

Object *constructGrenade(int sector,MthXyz *pos,MthXyz *vel,
			 SpriteObject *owner,int heading,
			 int age)
{ProjectileObject *this=(ProjectileObject *)
    getFreeObject(grenade_func,OT_GRENADE,CLASS_PROJECTILE);
 assert(this);
 if (!this)
    return NULL;
 assert(level_sequenceMap[OT_GRENADE]>=0);
 assert(level_sequenceMap[OT_GRENADE]<1000);
 this->sprite=newSprite(sector,F(4),F(1),GRAVITY<<2,
			level_sequenceMap[OT_GRENADE],
			SPRITEFLAG_IMATERIAL,(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->scale=15000;
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
 this->sprite->angle=heading;
 this->sequenceMap=grenadeSequenceMap;
 this->age=age;
 setState((SpriteObject *)this,0);
 this->owner=(Object *)owner;
 return (Object *)this;
}

/********************************\
 *       MAGMANTIS STUFF        *
\********************************/

enum  {AI_MAGMANTIS_IDLE,AI_MAGMANTIS_LURK,AI_MAGMANTIS_TAIL,
	  AI_MAGMANTIS_UP,AI_MAGMANTIS_FIRE,
	  AI_MAGMANTIS_DOWN,AI_MAGMANTIS_SPLASH,
	  AI_MAGMANTIS_NMSEQ};

unsigned short magmantisSeqMap[]={0,0,0,
				  HB|1,HB|9,
				  HB|25,33};

void magmantis_func(Object *_this,int message,int param1,int param2)
{MagmantisObject *this=(MagmantisObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	this->sprite->flags|=SPRITEFLAG_FLASH;
	this->health-=param1;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_MAGMANTIS_IDLE)
	   {collide=moveSprite(this->sprite);
	    if (!(this->sprite->flags & SPRITEFLAG_INVISIBLE))
	       fflags=spriteAdvanceFrame(this->sprite);
	   }
	switch (this->state)
	   {case AI_MAGMANTIS_IDLE:
	       if ((this->aiSlot&0x1f)==(aicount&0x1f))
		  {this->sprite->vel.x=0;
		   this->sprite->vel.z=0;
		   if (!this->enemy)
		      this->enemy=findPlayer(this->sprite,F(800));
		   if (this->enemy)
		      {setState((SpriteObject *)this,AI_MAGMANTIS_LURK);
		       PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		       this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		      }
		  }
	       break;
	    case AI_MAGMANTIS_LURK:

	       if ((this->aiSlot&0x1f)==(aicount&0x1f))
		  {PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   if (this->rarCount>0)
		      {this->rarCount--;
		       this->sprite->vel.x=0;
		       this->sprite->vel.z=0;
		       this->sprite->flags&=
			  ~(SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMATERIAL);
		       setState((SpriteObject *)this,AI_MAGMANTIS_UP);
		       this->tailInhibit=2;
		       break;
		      }
		   this->tailInhibit--;
		   if (this->tailInhibit<=0)
		      {this->tailInhibit=4;
		       this->sprite->vel.x=0;
		       this->sprite->vel.z=0;
		       this->sprite->flags&=
			  ~(SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMATERIAL);
		       setState((SpriteObject *)this,AI_MAGMANTIS_TAIL);
		       break;
		      }
		   this->angry--;
		   if (this->angry<=0)
		      {this->angry=5;
		       this->rarCount=(getNextRand()&7)+1;
		      }
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+randomAngle(7));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;

		  }
	       break;
	    case AI_MAGMANTIS_SPLASH:
	    case AI_MAGMANTIS_TAIL:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {setState((SpriteObject *)this,AI_MAGMANTIS_LURK);
		   this->sprite->flags|=
		      SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMATERIAL;
		   PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+randomAngle(7));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		   if (this->health<=0)
		      {spriteObject_makeSound((SpriteObject *)this,0);
		       {int i;
			for (i=0;i<20;i++)
			   {MthXyz vel;
			    vel.x=(MTH_GetRand()&0xfffff)-F(8);
			    vel.z=(MTH_GetRand()&0xfffff)-F(8);
			    vel.y=F(20)+(MTH_GetRand()&0x3ffff);
			    constructRingo(this->sprite->s,&this->sprite->pos,
					   &vel,(SpriteObject *)this);
			   }
		       }
		       delayKill(_this);
		       break;
		      }
		  }
	       break;
	    case AI_MAGMANTIS_UP:
	       spriteHome((SpriteObject *)this,
			  (SpriteObject *)this->enemy,
			  F(10),F(2));
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {setState((SpriteObject *)this,AI_MAGMANTIS_FIRE);
		  }
	       break;
	    case AI_MAGMANTIS_FIRE:
	       spriteHome((SpriteObject *)this,
			  (SpriteObject *)this->enemy,
			  F(10),F(2));
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (this->enemy && canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(90),4);
		       constructGenproj(this->sprite->s,&ballPos,&ballVel,
					(SpriteObject *)this,
					getAngle(ballVel.x,ballVel.z),
					1,OT_MAGBALL,10,RGB(15,5,5),
					0,0,0);
		      }
		   if (this->rarCount-->0)
		      setState((SpriteObject *)this,AI_MAGMANTIS_FIRE);
		   else
		      setState((SpriteObject *)this,AI_MAGMANTIS_DOWN);
		  }
	       break;
	    case AI_MAGMANTIS_DOWN:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_MAGMANTIS_SPLASH);
	       break;
	      }
	break;
       }
}

Object *constructMagmantis(int sector)
{MagmantisObject *this=(MagmantisObject *)getFreeObject(magmantis_func,
							OT_MAGMANTIS,
							CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(32),F(1),GRAVITY<<1,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_INVISIBLE|
			SPRITEFLAG_FOOTCLIP,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=80000;
 this->sequenceMap=magmantisSeqMap;
 setState((SpriteObject *)this,AI_MAGMANTIS_IDLE);
 this->health=100;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->tailInhibit=0;
 this->rarCount=0;
 this->angry=5;
 return (Object *)this;
}

/********************************\
 *          ANUBIS STUFF        *
\********************************/

enum  {AI_ANUBIS_IDLE=STATE_IDLE,
	  AI_ANUBIS_WALK=STATE_WALK,
	  AI_ANUBIS_CLAW=STATE_SRA,
	  AI_ANUBIS_THROW=STATE_LRA,
	  AI_ANUBIS_HIT=STATE_HIT,
	  AI_ANUBIS_SEEK=STATE_SEEK,
	  AI_ANUBIS_NMSEQ};

unsigned short anubisSeqMap[]={HB|0, HB|8, HB|16, HB|24,
				  HB|32, HB|8};

void anubis_func(Object *_this,int message,int param1,int param2)
{AnubisObject *this=(AnubisObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_LANDGUTS,48000,0,0);
	    makeExplosion((MonsterObject *)this,40,4);
	    makeExplosion((MonsterObject *)this,40,4);
	    spriteObject_makeSound((SpriteObject *)this,1);
	    delayKill(_this);
	   }
	else
	   if (!this->stunCounter)
	      {setState((SpriteObject *)this,AI_ANUBIS_HIT);
	       this->stunCounter=20;
	      }
	this->sprite->vel.x=0;
	this->sprite->vel.z=0;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_ANUBIS_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_ANUBIS_IDLE);
	   }
	if (this->stunCounter)
	   {this->stunCounter--;
	    /*break;*/
	   }
	switch (this->state)
	   {case AI_ANUBIS_HIT:
	       if (fflags && FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_ANUBIS_WALK);
	       break;
	    case AI_ANUBIS_IDLE:
	       normalMonster_idle((MonsterObject *)this,2,0);
	       break;
	    case AI_ANUBIS_CLAW:
	       this->sprite->vel.x=0;
	       this->sprite->vel.z=0;
	       if (this->sprite->frame==5 || this->sprite->frame==18)
		  {if (spriteDistApprox(this->sprite,
					this->enemy->sprite)<F(150))
		      {PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				    (int)this);
		      }
		   else
		      {setState((SpriteObject *)this,AI_ANUBIS_WALK);
		      }
		  }
	       break;
	    case AI_ANUBIS_THROW:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_ANUBIS_WALK);
	       if (this->sprite->frame==9)
		  {if (canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(46),4);
		       constructAnuball(this->sprite->s,&ballPos,&ballVel,
					(SpriteObject *)this,
					getAngle(ballVel.x,ballVel.z));
		      }
		  }
	       break;
	    case AI_ANUBIS_WALK:
	       normalMonster_walking((MonsterObject *)this,
				     collide,fflags,2);
	       break;
	    case AI_ANUBIS_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,0,2))
		  setState((SpriteObject *)this,AI_ANUBIS_WALK);
	       break;
	      }
	break;
       }
}

Object *constructAnubis(int sector)
{AnubisObject *this=(AnubisObject *)getFreeObject(anubis_func,OT_ANUBIS,
						  CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(32),F(1),GRAVITY<<1,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sequenceMap=anubisSeqMap;
 setState((SpriteObject *)this,AI_ANUBIS_IDLE);
 this->health=100;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->stunCounter=0;
 return (Object *)this;
}

/********************************\
 *          SELKIS STUFF        *
\********************************/

enum  {AI_SELKIS_IDLE=STATE_IDLE,AI_SELKIS_WALK=STATE_WALK,
	  AI_SELKIS_CLAW=STATE_SRA,AI_SELKIS_THROW=STATE_LRA,
	  AI_SELKIS_HIT=STATE_HIT,AI_SELKIS_SEEK=STATE_SEEK,

	  AI_SELKIS_SPARK1,AI_SELKIS_SPARK2,
	  AI_SELKIS_DIEING,AI_SELKIS_DEAD,
	  AI_SELKIS_ICU,
	  AI_SELKIS_NMSEQ};

unsigned short selkisSeqMap[]={HB|0, HB|8,
				  HB|25, HB|16,
				  HB|33, HB|8,

				  42,43,
				  44,45,
				  41
				 };

void selkis_func(Object *_this,int message,int param1,int param2)
{SelkisObject *this=(SelkisObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 assert(AI_SELKIS_IDLE==0 &&
	AI_SELKIS_WALK==1 &&
	AI_SELKIS_CLAW==2 &&
	AI_SELKIS_THROW==3 &&
	AI_SELKIS_HIT==4);
 switch (message)
    {case SIGNAL_HURT:
	if (this->state==AI_SELKIS_DIEING || this->state==AI_SELKIS_DEAD)
	   break;
	this->health-=param1;
	this->sprite->flags|=SPRITEFLAG_FLASH;
	if (this->health<=0)
	   {setState((SpriteObject *)this,AI_SELKIS_DIEING);
	    this->sparkTimer=0;
	   }
	else
	   if (!this->stunCounter && this->state==AI_SELKIS_WALK)
	      {setState((SpriteObject *)this,AI_SELKIS_HIT);
	       this->stunCounter=30*4;
	      }
	this->sprite->vel.x=0;
	this->sprite->vel.z=0;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_SELKIS_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_SELKIS_IDLE);
	   }
	if (this->stunCounter)
	   this->stunCounter--;
	switch (this->state)
	   {case AI_SELKIS_HIT:
	       if (fflags && FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SELKIS_WALK);
	       break;
	    case AI_SELKIS_ICU:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SELKIS_WALK);
	       break;
	    case AI_SELKIS_IDLE:
	       normalMonster_idle((MonsterObject *)this,2,-1);
	       if (this->enemy)
		  {setState((SpriteObject *)this,AI_SELKIS_ICU);
		   this->sprite->vel.x=0;
		   this->sprite->vel.z=0;
		  }
	       break;
	    case AI_SELKIS_CLAW:
	       this->sprite->vel.x=0;
	       this->sprite->vel.z=0;
	       if (this->sprite->frame==5 || this->sprite->frame==18)
		  {if (spriteDistApprox(this->sprite,
					this->enemy->sprite)<F(150))
		      {PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				    (int)this);
		      }
		   else
		      {setState((SpriteObject *)this,AI_SELKIS_WALK);
		      }
		  }
	       break;
	    case AI_SELKIS_THROW:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SELKIS_WALK);
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(46),4);
		       constructAnuball(this->sprite->s,&ballPos,&ballVel,
					(SpriteObject *)this,
					getAngle(ballVel.x,ballVel.z));
		      }
		  }
	       break;
	    case AI_SELKIS_WALK:
	       if ((this->aiSlot&0x1f)==(aicount&0x1f))
		  {this->sparkTimer=0;
		   if (this->health<500 && getNextRand()<5000)
		      {setState((SpriteObject *)this,AI_SELKIS_SPARK1);
		       this->sprite->vel.x=0; this->sprite->vel.z=0;
		       break;
		      }
		   if (this->health<250 && getNextRand()<13000)
		      {setState((SpriteObject *)this,AI_SELKIS_SPARK2);
		       this->sprite->vel.x=0; this->sprite->vel.z=0;
		       break;
		      }
		  }
	       normalMonster_walking((MonsterObject *)this,
				     collide,fflags,2);
	       break;
	    case AI_SELKIS_SPARK1:
	       if (this->sparkTimer++>30)
		  setState((SpriteObject *)this,AI_SELKIS_WALK);
	       break;
	    case AI_SELKIS_SPARK2:
	       if (this->sparkTimer++>60)
		  setState((SpriteObject *)this,AI_SELKIS_WALK);
	       break;
	    case AI_SELKIS_DIEING:
	       if (this->sparkTimer++>120)
		  setState((SpriteObject *)this,AI_SELKIS_DEAD);
	       break;
	    case AI_SELKIS_DEAD:
	       break;
	    case AI_SELKIS_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,0,2))
		  setState((SpriteObject *)this,AI_SELKIS_WALK);
	       break;
	      }
	break;
       }
}

Object *constructSelkis(int sector)
{SelkisObject *this=(SelkisObject *)getFreeObject(selkis_func,OT_SELKIS,
						  CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(32),F(1),GRAVITY<<1,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=65536*1.6;
 this->sequenceMap=selkisSeqMap;
 setState((SpriteObject *)this,AI_SELKIS_IDLE);
 this->health=1000;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->stunCounter=0;
 this->sparkTimer=0;
 return (Object *)this;
}

/********************************\
 *           SET STUFF          *
\********************************/

enum  {AI_SET_IDLE=STATE_IDLE,AI_SET_WALK=STATE_WALK,AI_SET_CLAW=STATE_SRA,
	  AI_SET_THROW=STATE_LRA,AI_SET_HIT=STATE_HIT,AI_SET_SEEK=STATE_SEEK,
	  AI_SET_DYING,AI_SET_DEAD,AI_SET_JUMP1,AI_SET_JUMP2,AI_SET_JUMP3,
	  AI_SET_NMSEQ};

unsigned short setSeqMap[]={HB|0, HB|0, HB|8,
			    HB|40, HB|48, HB|0 ,
			    56,57,HB|16,HB|24,HB|32};

void set_func(Object *_this,int message,int param1,int param2)
{SetObject *this=(SetObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	this->health-=param1;
	this->sprite->flags|=SPRITEFLAG_FLASH;
	if (this->health<=0 && this->state!=AI_SET_DYING &&
	    this->state!=AI_SET_DEAD)
	   {setState((SpriteObject *)this,AI_SET_DYING);
	   }
	else
	   if (!this->stunCounter && this->state==AI_SET_WALK)
	      {setState((SpriteObject *)this,AI_SET_HIT);
	       this->stunCounter=40;
	      }
	this->sprite->vel.x=0;
	this->sprite->vel.z=0;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_SET_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_SET_IDLE);
	   }
	if (this->stunCounter)
	   this->stunCounter--;
	switch (this->state)
	   {case AI_SET_HIT:
	       if (fflags && FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SET_WALK);
	       break;
	    case AI_SET_IDLE:
	       normalMonster_idle((MonsterObject *)this,3,-1);
	       break;
	    case AI_SET_CLAW:
	       this->sprite->vel.x=0;
	       this->sprite->vel.z=0;
	       if (this->sprite->frame==5 || this->sprite->frame==18)
		  {if (spriteDistApprox(this->sprite,
					this->enemy->sprite)<F(150))
		      {PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				    (int)this);
		      }
		   else
		      {setState((SpriteObject *)this,AI_SET_WALK);
		      }
		  }
	       break;
	    case AI_SET_THROW:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SET_WALK);
	       if (this->sprite->frame==9)
		  {if (canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(46),4);
		       constructGenproj(this->sprite->s,&ballPos,&ballVel,
					(SpriteObject *)this,
					getAngle(ballVel.x,ballVel.z),
					0,OT_SETBALL,10,RGB(15,15,15),
					0,0,0);
		      }
		  }
	       break;
	    case AI_SET_WALK:
	       if ((this->aiSlot&0x7f)==(aicount&0x7f) &&
		   spriteDistApprox(this->sprite,this->enemy->sprite)>F(300))
		  {PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+randomAngle(6));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<3;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<3;

		   setState((SpriteObject *)this,AI_SET_JUMP1);
		   break;
		  }
	       normalMonster_walking((MonsterObject *)this,
				     collide,fflags,3);
	       break;
	    case AI_SET_JUMP1:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {setState((SpriteObject *)this,AI_SET_JUMP2);
		   this->sprite->vel.y=F(30);
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<4;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<4;
		  }
	       break;
	    case AI_SET_JUMP2:
	       if (this->sprite->floorSector!=-1)
		  {setState((SpriteObject *)this,AI_SET_JUMP3);
		   this->sprite->vel.x=0;
		   this->sprite->vel.z=0;
		   setEarthQuake(30);
		   if (camera->floorSector!=-1)
		      stunPlayer(120);
		  }
	       break;
	    case AI_SET_JUMP3:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SET_WALK);
	       break;
	    case AI_SET_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,0,2))
		  setState((SpriteObject *)this,AI_SET_WALK);
	       break;
	    case AI_SET_DYING:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SET_DEAD);
	       break;
	    case AI_SET_DEAD:
	       break;
	      }
	break;
       }
}

Object *constructSet(int sector)
{SetObject *this=(SetObject *)getFreeObject(set_func,OT_SET,
					    CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(64),F(1),GRAVITY<<2,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=80000;
 this->sequenceMap=setSeqMap;
 setState((SpriteObject *)this,AI_SET_IDLE);
 this->health=1000;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->stunCounter=0;
 return (Object *)this;
}

/********************************\
 *          SENTRY BALL         *
\********************************/

void sball_func(Object *_this,int msg,int param1,int param2)
{ProjectileObject *this=(ProjectileObject *)_this;
 int collide;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	collide=moveSprite(this->sprite);
	if (collide)
	   {if (collide & COLLIDE_SPRITE)
	       {if (sprites[collide&0xffff].owner==this->owner)
		   break;
		signalObject(sprites[collide&0xffff].owner,
			     SIGNAL_HURT,10,
			     (int)this);
	       }
	    delayKill(_this);
	    constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_POOF,F(1),RGB(20,20,0),0);
	   }
	break;
       }
}

unsigned short sballSequenceMap[]={24};

Object *constructSball(int sector,MthXyz *pos,MthXyz *vel,
		       SpriteObject *owner,int heading,int seq,
		       int scale)
{ProjectileObject *this=(ProjectileObject *)
    getFreeObject(sball_func,OT_SBALL,CLASS_PROJECTILE);
 assert(this);
 sballSequenceMap[0]=seq;
 assert(level_sequenceMap[OT_SBALL]>=0);
 assert(level_sequenceMap[OT_SBALL]<1000);
 moveObject((Object *)this,objectRunList);
 this->sprite=newSprite(sector,F(4),F(1),0,
			level_sequenceMap[OT_SBALL],
			SPRITEFLAG_IMATERIAL,(Object *)this);
 this->sprite->scale=scale;
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
 this->sprite->angle=heading;
 this->sequenceMap=sballSequenceMap;
 setState((SpriteObject *)this,0);
 this->owner=(Object *)owner;
 return (Object *)this;
}


/********************************\
 *          SENTRY STUFF        *
\********************************/

enum  {AI_SENTRY_IDLE=STATE_IDLE,
	  AI_SENTRY_WALK=STATE_WALK,
	  AI_SENTRY_CLAW=STATE_SRA,
	  AI_SENTRY_THROW=STATE_LRA,
	  AI_SENTRY_HIT=STATE_HIT,
	  AI_SENTRY_SEEK=STATE_SEEK,
	  AI_SENTRY_NMSEQ};

unsigned short sentrySeqMap[]={HB|0, HB|0, HB|16, HB|16,
				  HB|8, HB|0};

void sentry_func(Object *_this,int message,int param1,int param2)
{SentryObject *this=(SentryObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_LANDGUTS,48000,0,0);
	    makeExplosion((MonsterObject *)this,40,4);
	    makeExplosion((MonsterObject *)this,40,4);
	    delayKill(_this);
	   }
	else
	   if (!this->stunCounter)
	      {setState((SpriteObject *)this,AI_SENTRY_HIT);
	       this->stunCounter=20;
	      }
	this->sprite->vel.x=0;
	this->sprite->vel.z=0;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_SENTRY_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_SENTRY_IDLE);
	   }
	if (this->stunCounter)
	   this->stunCounter--;
	switch (this->state)
	   {case AI_SENTRY_HIT:
	       if (fflags && FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SENTRY_WALK);
	       break;
	    case AI_SENTRY_IDLE:
	       normalMonster_idle((MonsterObject *)this,2,-1);
	       break;
	    case AI_SENTRY_CLAW:
	    case AI_SENTRY_THROW:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_SENTRY_WALK);
	       if (this->sprite->frame==9)
		  {if (canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(46),4);
		       constructSball(this->sprite->s,&ballPos,&ballVel,
				      (SpriteObject *)this,
				      getAngle(ballVel.x,ballVel.z),24,
				      15000);
		      }
		  }
	       break;
	    case AI_SENTRY_WALK:
	       normalMonster_walking((MonsterObject *)this,
				     collide,fflags,2);
	       break;
	    case AI_SENTRY_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,0,2))
		  setState((SpriteObject *)this,AI_SENTRY_WALK);
	       break;
	      }
	break;
       }
}

Object *constructSentry(int sector)
{SentryObject *this=(SentryObject *)getFreeObject(sentry_func,OT_SENTRY,
						  CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(32),F(1),GRAVITY<<1,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sequenceMap=sentrySeqMap;
 setState((SpriteObject *)this,AI_SENTRY_IDLE);
 this->health=100;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->stunCounter=0;
 return (Object *)this;
}


/********************************\
 *          MUMMY STUFF        *
\********************************/
enum  {AI_MUMMY_IDLE=STATE_IDLE,
	  AI_MUMMY_WALK=STATE_WALK,
	  AI_MUMMY_CLAW=STATE_SRA,
	  AI_MUMMY_THROW=STATE_LRA,
	  AI_MUMMY_STUNNED=STATE_HIT,
	  AI_MUMMY_SEEK=STATE_SEEK,
	  AI_MUMMY_NMSEQ};

unsigned short mummySeqMap[]={HB|8, HB|0, HB|20, HB|28,
				 HB|8,HB|0};

void mummy_func(Object *_this,int message,int param1,int param2)
{MummyObject *this=(MummyObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_LANDGUTS,48000,0,0);
	    spriteObject_makeSound((SpriteObject *)this,0);
	    makeExplosion((MonsterObject *)this,16,4);
	    makeExplosion((MonsterObject *)this,16,4);
	    delayKill(_this);
	   }
	else
	   if (this->state!=AI_MUMMY_STUNNED)
	      {setState((SpriteObject *)this,AI_MUMMY_STUNNED);
	       this->stunCounter=20;
	      }
	this->sprite->vel.x=0;
	this->sprite->vel.z=0;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state!=AI_MUMMY_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_MUMMY_IDLE);
	   }
	switch (this->state)
	   {case AI_MUMMY_IDLE:
	       normalMonster_idle((MonsterObject *)this,2,-1);
	       break;
	    case AI_MUMMY_STUNNED:
	       if (this->stunCounter>0)
		  this->stunCounter--;
	       else
		  setState((SpriteObject *)this,AI_MUMMY_WALK);
	       break;
	    case AI_MUMMY_CLAW:
	       this->sprite->vel.x=0;
	       this->sprite->vel.z=0;
	       if (this->sprite->frame==5 || this->sprite->frame==18)
		  {if (spriteDistApprox(this->sprite,
					this->enemy->sprite)<F(150))
		      {PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				    (int)this);
		      }
		   else
		      setState((SpriteObject *)this,AI_MUMMY_WALK);
		  }
	       break;
	    case AI_MUMMY_THROW:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_MUMMY_IDLE);
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(46),4);
		       constructCobra(this->sprite->s,ballPos.x,ballPos.y,
				      ballPos.z,getAngle(ballVel.x,ballVel.z),
				      0,(SpriteObject *)this,OT_MUMBALL);
		      }
		  }
	       break;
	    case AI_MUMMY_WALK:
	       normalMonster_walking((MonsterObject*)this,
				     collide,fflags,2);
	       break;
	    case AI_MUMMY_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,0,2))
		  setState((SpriteObject *)this,AI_MUMMY_WALK);
	       break;
	      }
	break;
       }
}

Object *constructMummy(int sector)
{MummyObject *this=(MummyObject *)getFreeObject(mummy_func,OT_MUMMY,
						CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(32),F(1),GRAVITY<<1,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sequenceMap=mummySeqMap;
 setState((SpriteObject *)this,AI_MUMMY_IDLE);
 this->health=100;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->stunCounter=0;
 return (Object *)this;
}


/********************************\
 *          BASTET STUFF        *
\********************************/
enum  {AI_BASTET_IDLE,AI_BASTET_WALK,AI_BASTET_CLAW,
	  AI_BASTET_STUNNED,AI_BASTET_SEEK,
	  AI_BASTET_PORTDOWN,AI_BASTET_PORTING,AI_BASTET_PORTUP,
	  AI_BASTET_NMSEQ};

unsigned short bastetSeqMap[]={31, HB|0, HB|10,
				  HB|18,HB|0,
				  8,31,9};

void bastet_func(Object *_this,int message,int param1,int param2)
{BastetObject *this=(BastetObject *)_this;
 int collide;
 int fflags;
 collide=0;
 fflags=0;
 switch (message)
    {case SIGNAL_HURT:
	if (monsterObject_signalHurt((MonsterObject *)this,param1,param2))
	   {constructOneShot(this->sprite->s,
			     this->sprite->pos.x,
			     this->sprite->pos.y,
			     this->sprite->pos.z,
			     OT_LANDGUTS,48000,0,0);
	    spriteObject_makeSound((SpriteObject *)this,1);
	    makeExplosion((MonsterObject *)this,26,4);
	    delayKill(_this);
	   }
	else
	   if (this->state!=AI_BASTET_STUNNED)
	      {setState((SpriteObject *)this,AI_BASTET_STUNNED);
	       this->stunCounter=20;
	      }
	this->sprite->vel.x=0;
	this->sprite->vel.z=0;
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	return;
	if (this->state!=AI_BASTET_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_BASTET_IDLE);
	   }
	switch (this->state)
	   {case AI_BASTET_IDLE:
	       normalMonster_idle((MonsterObject *)this,2,0);
	       break;
	    case AI_BASTET_STUNNED:
	       if (this->stunCounter>0)
		  this->stunCounter--;
	       else
		  setState((SpriteObject *)this,AI_BASTET_WALK);
	       break;
	    case AI_BASTET_CLAW:
	       this->sprite->vel.x=0;
	       this->sprite->vel.z=0;
	       if (this->sprite->frame==5 || this->sprite->frame==18)
		  {if (spriteDistApprox(this->sprite,
					this->enemy->sprite)<F(150))
		      {PlotCourseToObject((SpriteObject *)this,
					  (SpriteObject *)this->enemy);
		       signalObject((Object *)this->enemy,SIGNAL_HURT,20,
				    (int)this);
		      }
		   else
		      setState((SpriteObject *)this,AI_BASTET_WALK);
		  }
	       break;
	    case AI_BASTET_PORTDOWN:
	       if (this->sprite->frame==23)
		  addLight(this->sprite,0,0,0);
	       if (this->sprite->frame>23)
		  changeLightColor(this->sprite,
				   this->sprite->frame-23,
				   this->sprite->frame-23,
				   this->sprite->frame-23);
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {removeLight(this->sprite);
		   setState((SpriteObject *)this,AI_BASTET_PORTING);
		   this->sprite->flags|=SPRITEFLAG_INVISIBLE;
		   this->portTimer=0;
		  }
	       break;
	    case AI_BASTET_PORTING:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   /* if we get here, we know we can see the player */
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+randomAngle(3));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<4;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<4;
		  }
	       this->portTimer++;
	       if (this->portTimer>60)
		  {addLight(this->sprite,0,0,0);
		   setState((SpriteObject *)this,AI_BASTET_PORTUP);
		   this->sprite->flags&=~SPRITEFLAG_INVISIBLE;
		   this->sprite->vel.x=0;
		   this->sprite->vel.z=0;
		  }
	       break;
	    case AI_BASTET_PORTUP:
	       changeLightColor(this->sprite,
				this->sprite->frame,
				this->sprite->frame,
				this->sprite->frame);
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {setState((SpriteObject *)this,AI_BASTET_WALK);
		   this->sprite->flags&=~(SPRITEFLAG_FOOTCLIP|
					  SPRITEFLAG_IMATERIAL);
		   removeLight(this->sprite);
		  }
	       break;
	    case AI_BASTET_WALK:
	       if ((collide & COLLIDE_SPRITE) &&
		   sprites[collide&0xffff].owner==(Object *)this->enemy)
		  {/* bite them! */
		   setState((SpriteObject *)this,AI_BASTET_CLAW);
		   this->sprite->vel.x=0;
		   this->sprite->vel.z=0;
		  }
	       if (collide & COLLIDE_WALL)
		  {this->sprite->angle=
		      normalizeAngle(this->sprite->angle+/*F(180)+*/
				     randomAngle(8));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		  }
	       if ((this->aiSlot&0x1f)==(aicount&0x1f))
		  {if (getNextRand()&0x80)
		      {setState((SpriteObject *)this,AI_BASTET_PORTDOWN);
		       this->sprite->vel.x=0;
		       this->sprite->vel.z=0;
		       this->sprite->flags|=SPRITEFLAG_FOOTCLIP|
			  SPRITEFLAG_IMATERIAL;

		       break;
		      }
		  }
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {if (!canSee(this->sprite,this->enemy->sprite))
		      {this->sprite->vel.x=0;
		       this->sprite->vel.z=0;
		       setState((SpriteObject *)this,AI_BASTET_SEEK);
		       break;
		      }
		   PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   /* if we get here, we know we can see the player */
		   this->sprite->angle=
		      normalizeAngle(this->sprite->angle+randomAngle(7));
		   this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<4;
		   this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<4;
		  }
	       break;
	    case AI_BASTET_SEEK:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f) &&
		   monster_seekEnemy((MonsterObject *)this,0,2))
		  setState((SpriteObject *)this,AI_BASTET_WALK);
	       break;
	      }
	break;
       }
}


Object *constructBastet(int sector)
{BastetObject *this=(BastetObject *)getFreeObject(bastet_func,OT_BASTET,
						  CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(32),F(1),GRAVITY<<1,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sequenceMap=bastetSeqMap;
 setState((SpriteObject *)this,AI_BASTET_IDLE);
 this->health=100;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 this->stunCounter=0;
 return (Object *)this;
}


/********************************\
 *          KAPOW   STUFF       *
\********************************/

unsigned short kapowSequenceMap[]={0};

void kapow_func(Object *_this,int msg,int param1,int param2)
{SpriteObject *this=(SpriteObject *)_this;
 int fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	fflags=spriteAdvanceFrame(this->sprite);
	if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
	   delayKill(_this);
	break;
       }
}

Object *constructKapow(int sector,int x,int y,int z)
{SpriteObject *this=(SpriteObject *)
    getFreeObject(kapow_func,OT_KAPOW,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos.x=x;
 this->sprite->pos.y=y;
 this->sprite->pos.z=z;
 this->sequenceMap=kapowSequenceMap;
 setState(this,0);
 return (Object *)this;
}


/********************************\
 *          THING STUFF         *
\********************************/

#define BEATLEN 12
static int beatScale[]=
  {255,230,190,180,190,200,210,220,230,240,250,256};

unsigned short thingSequenceMap[]={0};

void thing_func(Object *_this,int msg,int param1,int param2)
{ThingObject *this=(ThingObject *)_this;
 int collide;
 int fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	collide=moveSprite(this->sprite);
	fflags=spriteAdvanceFrame(this->sprite);
	this->sin+=F(8);
	if (this->sin>F(180))
	   this->sin-=F(360);
	this->sprite->pos.y=this->baseY+6*MTH_Sin(this->sin);

	if (this->type==OT_HEALTHBALL)
	   {this->frame++;
	    if (this->frame>=32)
	       this->frame=0;
	    if (this->frame<BEATLEN)
	       {this->sprite->scale=
		   ((48000*beatScale[this->frame])>>8);
		this->sprite->radius=(16*beatScale[this->frame])<<8;
	       }
	   }
	if (collide & COLLIDE_SPRITE)
	   {if (&(sprites[collide&0xffff])==camera)
	       {/* player picked us up */
		if (playerGetObject(this->type))
		   delayKill(_this);
	       }
	   }
	break;
       }
}

Object *constructThing(int sector,int center,int x,int y,int z,int thingType)
{ThingObject *this;
 /* decide if we should not place this thing */
 if (thingType>=OT_PISTOL && thingType<=OT_RING)
    if (currentState.inventory & (INV_PISTOL<<(thingType-OT_PISTOL)))
       return NULL;
 if (thingType>=OT_SANDALS && thingType<=OT_FEATHER)
    if (currentState.inventory & (INV_SANDALS<<(thingType-OT_SANDALS)))
       return NULL;
 if (thingType==OT_PYRAMID)
    if (currentState.levFlags[(int)currentState.currentLevel] &
	LEVFLAG_GOTPYRAMID)
       return NULL;
 if (thingType==OT_BLOODBOWL)
    if (currentState.levFlags[(int)currentState.currentLevel] &
	LEVFLAG_GOTVESSEL)
       return NULL;

 this=(ThingObject *)
    getFreeObject(thing_func,thingType,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 if (thingType==OT_AMMOBALL || thingType==OT_HEALTHBALL)
    this->sprite->scale=20000;
 if (thingType==OT_AMMOORB || thingType==OT_HEALTHORB)
    this->sprite->scale=48000;
 if (thingType==OT_AMMOSPHERE || thingType==OT_HEALTHSPHERE)
    this->sprite->scale=65000;
 this->sequenceMap=thingSequenceMap;
 if (!center)
    {this->sprite->pos.x=x;
     this->sprite->pos.y=y;
     this->sprite->pos.z=z;
    }
 this->sin=0;
 this->baseY=this->sprite->pos.y;
 this->frame=0;
 setState((SpriteObject *)this,0);
 if (getPicClass(level_chunk[
		   level_frame[
                     level_sequence[this->sprite->sequence]].chunkIndex].tile)
     ==TILESMALL16BPP)
    this->sprite->flags|=SPRITEFLAG_32x32;
 return (Object *)this;
}

/********************************\
 *          TORCH STUFF         *
\********************************/

unsigned short torchSequenceMap[]={0};

void torch_func(Object *_this,int msg,int param1,int param2)
{TorchObject *this=(TorchObject *)_this;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	spriteAdvanceFrame(this->sprite);
	break;
       }
}

Object *constructTorch(int sector,int torchType)
{TorchObject *this;
 this=(TorchObject *)
    getFreeObject(torch_func,torchType,CLASS_SPRITE);
 assert(this);
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_IMMOBILE,
			(Object *)this);
 suckSpriteParams(this->sprite);
 moveObject((Object *)this,objectRunList);
 this->sequenceMap=torchSequenceMap;
 setState((SpriteObject *)this,0);
 return (Object *)this;
}

/********************************\
 *           BLOB STUFF         *
\********************************/

enum  {AI_BLOB_IDLE,AI_BLOB_RILED,AI_BLOB_EXPLODE};
unsigned short blobSequenceMap[]={0,1,2};

void blob_func(Object *_this,int msg,int param1,int param2)
{BlobObject *this=(BlobObject *)_this;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	spriteAdvanceFrame(this->sprite);
	if (this->state)
	   {this->anger++;
	    if (this->anger==30)
	       setState((SpriteObject *)this,2);
	    if (this->anger==50)
	       {constructOneShot(this->sprite->s,
				 this->sprite->pos.x,
				 this->sprite->pos.y+F(30),
				 this->sprite->pos.z,
				 OT_GRENBUBB,F(2),0,0);
		radialDamage((Object *)this,&this->sprite->pos,100,F(300));
		delayKill(_this);
	       }
	   }
	if (!this->anger &&
	    (aicount & 0xf)==(this->aiSlot & 0xf) &&
	    spriteDistApprox(this->sprite,player->sprite)<F(128))
	   {this->anger=1;
	    setState((SpriteObject *)this,1);
	   }
	break;
       }
}

Object *constructBlob(int sector)
{BlobObject *this;
 this=(BlobObject *)
    getFreeObject(blob_func,OT_BLOB,CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMMOBILE|SPRITEFLAG_IMATERIAL,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sequenceMap=blobSequenceMap;
 this->anger=0;
 this->health=20;
 this->enemy=NULL;
 this->aiSlot=nextAiSlot++;
 setState((SpriteObject *)this,0);
 return (Object *)this;
}


/********************************\
 *           LIGHT STUFF        *
\********************************/
unsigned short lightSequenceMap[]={0};

void light_func(Object *_this,int msg,int param1,int param2)
{LightObject *this=(LightObject *)_this;
 int r,g,b;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    {removeLight(this->sprite);
	     freeSprite(this->sprite);
	    }
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	r=f(evalHermite(this->age,
			this->parameters[0],
			this->parameters[1],
			this->parameters[2],
			this->parameters[3]));
	g=f(evalHermite(this->age,
			this->parameters[4],
			this->parameters[5],
			this->parameters[6],
			this->parameters[7]));
	b=f(evalHermite(this->age,
			this->parameters[8],
			this->parameters[9],
			this->parameters[10],
			this->parameters[11]));
	changeLightColor(this->sprite,r,g,b);
	this->age+=this->ageInc;
	if (this->age>F(1))
	   {delayKill(_this);
	   }
	break;
       }
}

/* parameters are start level, end level, start slope, end slope for
   each of red, green and blue */
Object *constructLight(int sector,int x,int y,int z,
		       int *parameters,Fixed32 ageInc)
{LightObject *this=(LightObject *)
    getFreeObject(light_func,OT_LIGHT,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMATERIAL|
			SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos.x=x;
 this->sprite->pos.y=y;
 this->sprite->pos.z=z;
 this->sequenceMap=lightSequenceMap;
 this->parameters=parameters;
 this->ageInc=ageInc;
 addLight(this->sprite,f(parameters[0]),f(parameters[4]),f(parameters[8]));
 this->age=0;
 return (Object *)this;
}


/********************************\
 *          ONESHOT STUFF       *
\********************************/

unsigned short oneShotSequenceMap[]={0};

void oneshot_func(Object *_this,int msg,int param1,int param2)
{OneShotObject *this=(OneShotObject *)_this;
 int fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	if (this->waitTime)
	   {this->waitTime--;
	    if (this->waitTime)
	       break;
	    this->sprite->flags&=~SPRITEFLAG_INVISIBLE;
	   }
	fflags=spriteAdvanceFrame(this->sprite);
	if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
	   delayKill(_this);
	break;
       }
}

Object *constructOneShot(int sector,int x,int y,int z,int oneShotType,
			 int scale,unsigned short colored,int waitTime)
{OneShotObject *this=(OneShotObject *)
    getFreeObject(oneshot_func,oneShotType,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_INVISIBLE|SPRITEFLAG_IMATERIAL|
			  SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 if (colored)
    {this->sprite->color=colored;
     this->sprite->flags|=SPRITEFLAG_COLORED;
    }
 this->sprite->pos.x=x;
 this->sprite->pos.y=y;
 this->sprite->pos.z=z;
 this->sprite->scale=scale;
 this->sequenceMap=oneShotSequenceMap;
 this->waitTime=waitTime+1;
 assert(waitTime>=0);
 setState((SpriteObject *)this,0);
 return (Object *)this;
}


/********************************\
 *          RACLOUD STUFF       *
\********************************/

unsigned short cloudSequenceMap[]={1};

void cloud_func(Object *_this,int msg,int param1,int param2)
{CloudObject *this=(CloudObject *)_this;
 int fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==(Object *)this->target)
	    this->target=NULL;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	fflags=spriteAdvanceFrame(this->sprite);
	if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
	   {if (this->target)
	       signalObject((Object *)this->target,SIGNAL_HURT,this->damage,
			    (int)this);
	    delayKill(_this);
	   }
	break;
       }
}

Object *constructCloud(MonsterObject *target,int damage)
{CloudObject *this=(CloudObject *)
    getFreeObject(cloud_func,OT_RACLOUD,CLASS_PROJECTILE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(target->sprite->s,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos.x=target->sprite->pos.x;
 this->sprite->pos.y=target->sprite->pos.y+F(64);
 this->sprite->pos.z=target->sprite->pos.z;

 this->sequenceMap=cloudSequenceMap;
 this->target=target;
 this->owner=(Object *)player;
 this->damage=damage;
 setState((SpriteObject *)this,0);
 return (Object *)this;
}


/********************************\
 *          DOOR STUFF          *
\********************************/

enum  {AI_DOOR_IDLE,AI_DOOR_UP,AI_DOOR_WAIT,AI_DOOR_DOWN};

void door_func(Object *_this,int msg,int param1,int param2)
{PushBlockObject *this=(PushBlockObject *)_this;
 switch (msg)
    {case SIGNAL_PRESS:
     case SIGNAL_CEILCONTACT:
	this->state=AI_DOOR_UP;
	pushBlockMakeSound(this,level_staticSoundMap[ST_PUSHBLOCK]);
	delay_moveObject((Object *)this,objectRunList);
	break;
     case SIGNAL_MOVE:
	switch (this->state)
	   {case AI_DOOR_UP:
	       if (this->counter<128)
		  {movePushBlock(this->pbNum,0,1,0);
		   this->counter++;
		  }
	       else
		  {stopAllSound((int)this);
		   this->state=AI_DOOR_WAIT;
		   this->waitCounter=0;
		  }
	       break;
	    case AI_DOOR_WAIT:
	       if (this->waitCounter++>128)
		  {this->state=AI_DOOR_DOWN;
		   pushBlockMakeSound(this,
				      level_staticSoundMap[ST_PUSHBLOCK]);
		  }
	       break;
	    case AI_DOOR_DOWN:
	       movePushBlock(this->pbNum,0,-1,0);
	       if (--this->counter<=0)
		  {this->state=AI_DOOR_IDLE;
		   stopAllSound((int)this);
		   pushBlockMakeSound(this,
				      level_staticSoundMap[ST_PUSHBLOCK]+1);
		   delay_moveObject((Object *)this,objectIdleList);
		  }
	       break;
	      }
       }
}

Object *constructDoor(int pb)
{PushBlockObject *this=(PushBlockObject *)
    getFreeObject(door_func,OT_NORMALDOOR,CLASS_PUSHBLOCK);
 assert(this);
 moveObject((Object *)this,objectIdleList);
 registerPBObject(pb,(Object *)this);
 this->pbNum=pb;
 this->state=AI_DOOR_IDLE;
 this->counter=0;
 this->waitCounter=0;
 return (Object *)this;
}


/********************************\
 *        ELEVATOR STUFF        *
\********************************/

enum  {AI_ELEVATOR_SLEEP,
	  AI_ELEVATOR_WAITUP,
	  AI_ELEVATOR_GODOWN,
	  AI_ELEVATOR_WAITDOWN,
	  AI_ELEVATOR_GOUP};

void elevator_func(Object *_this,int msg,int param1,int param2)
{ElevatorObject *this=(ElevatorObject *)_this;
 switch (msg)
    {case SIGNAL_PRESS:
	this->state=AI_ELEVATOR_GODOWN;
	pushBlockMakeSound((PushBlockObject *)this,
			   level_staticSoundMap[ST_PUSHBLOCK]+2);
	pushBlockMakeSound((PushBlockObject *)this,
			   level_staticSoundMap[ST_PUSHBLOCK]);
	delay_moveObject((Object *)this,objectRunList);
	break;
     case SIGNAL_FLOORCONTACT:
	this->stepAccum+=3;
	if (this->state==AI_ELEVATOR_SLEEP)
	   {delay_moveObject((Object *)this,objectRunList);
	    this->state=AI_ELEVATOR_WAITUP;
	    this->counter=0;
	    this->stepAccum=0;
	   }
	if (this->stepAccum>5*30)
	   {this->stepAccum=5*30;
	    if (this->state==AI_ELEVATOR_WAITUP)
	       {this->state=AI_ELEVATOR_GODOWN;
		pushBlockMakeSound((PushBlockObject *)this,
			   level_staticSoundMap[ST_PUSHBLOCK]+2);
		pushBlockMakeSound((PushBlockObject *)this,
				   level_staticSoundMap[ST_PUSHBLOCK]);
	       }
	    if (this->state==AI_ELEVATOR_WAITDOWN)
	       {this->state=AI_ELEVATOR_GOUP;
		pushBlockMakeSound((PushBlockObject *)this,
			   level_staticSoundMap[ST_PUSHBLOCK]+2);
		pushBlockMakeSound((PushBlockObject *)this,
				   level_staticSoundMap[ST_PUSHBLOCK]);
	       }
	   }
	break;
     case SIGNAL_MOVE:
	if (this->stepAccum>0)
	   this->stepAccum--;
	this->counter++;
	switch (this->state)
	   {case AI_ELEVATOR_WAITUP:
	       if (this->counter>30*5)
		  {this->state=AI_ELEVATOR_SLEEP;
		   delay_moveObject((Object *)this,objectIdleList);
		   break;
		  }
	       break;
	    case AI_ELEVATOR_GODOWN:
	       movePushBlock(this->pbNum,0,-5,0);
	       this->pos-=5;
	       if (this->pos<=this->lowerLevel)
		  {movePushBlock(this->pbNum,0,this->lowerLevel-this->pos,0);
		   this->pos=this->lowerLevel;
		   this->state=AI_ELEVATOR_WAITDOWN;
		   stopAllSound((int)this);
		   this->counter=0;
		   this->stepAccum=0;
		  }
	       break;
	    case AI_ELEVATOR_WAITDOWN:
	       if (this->counter>30*10)
		  {this->state=AI_ELEVATOR_GOUP;
		   pushBlockMakeSound((PushBlockObject *)this,
				      level_staticSoundMap[ST_PUSHBLOCK]);
		  }
	       break;
	    case AI_ELEVATOR_GOUP:
	       movePushBlock(this->pbNum,0,5,0);
	       this->pos+=5;
	       if (this->pos>=this->upperLevel)
		  {movePushBlock(this->pbNum,0,this->upperLevel-this->pos,0);
		   this->pos=this->upperLevel;
		   this->counter=0;
		   this->stepAccum=0;
		   this->state=AI_ELEVATOR_WAITUP;
		   stopAllSound((int)this);
		  }
	       break;
	      }
       }
}


Object *constructElevator(int pb,short lowerLevel,short upperLevel)
{ElevatorObject *this=(ElevatorObject *)
    getFreeObject(elevator_func,OT_NORMALELEVATOR,CLASS_PUSHBLOCK);
 assert(this);
 moveObject((Object *)this,objectIdleList);
 registerPBObject(pb,(Object *)this);
 this->pbNum=pb;
 this->state=AI_ELEVATOR_SLEEP;
 this->counter=0;
 this->lowerLevel=lowerLevel;
 this->upperLevel=upperLevel;
 dPrint("low=%d up=%d\n",lowerLevel,upperLevel);
 this->pos=upperLevel;
 this->stepAccum=0;
 return (Object *)this;
}


/********************************\
 *       FORCEFIELD STUFF       *
\********************************/
enum  {AI_FFIELD_ON,AI_FFIELD_OFF};

void ffield_func(Object *_this,int msg,int param1,int param2)
{FFieldObject *this=(FFieldObject *)_this;
 switch (msg)
    {case SIGNAL_MOVE:
	switch (this->state)
	   {case AI_FFIELD_ON:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {sWallType *w=level_wall+this->wallNm;
		   if (approxDist(camera->pos.x-this->center.x,
				  camera->pos.y-this->center.y,
				  camera->pos.z-this->center.z)<F(600))
		      {this->state=AI_FFIELD_OFF;
		       w->flags|=WALLFLAG_INVISIBLE;
		       w->flags&=~WALLFLAG_BLOCKED;
		      }
		  }
	       break;
	    case AI_FFIELD_OFF:
	       if ((this->aiSlot&0x0f)==(aicount&0x0f))
		  {sWallType *w=level_wall+this->wallNm;
		   if (approxDist(camera->pos.x-this->center.x,
				  camera->pos.y-this->center.y,
				  camera->pos.z-this->center.z)>F(700))
		      {this->state=AI_FFIELD_ON;
		       w->flags&=~WALLFLAG_INVISIBLE;
		       w->flags|=WALLFLAG_BLOCKED;
		      }
		  }
	       break;
	      }
       }
}

Object *constructForceField(int wallNm)
{MthXyz center,v;
 int i;
 FFieldObject *this=(FFieldObject *)
    getFreeObject(ffield_func,OT_FORCEFIELD,CLASS_WALL);
 assert(this);
 moveObject((Object *)this,objectRunList);
 assert(level_wall[wallNm].flags & WALLFLAG_PARALLELOGRAM);
 this->aiSlot=nextAiSlot++;
 this->startTile=level_texture[level_wall[wallNm].textures];
 this->wallNm=wallNm;
 this->state=AI_FFIELD_ON;
 center.x=0; center.y=0; center.z=0;
 for (i=0;i<4;i++)
    {getVertex(level_wall[wallNm].v[i],&v);
     center.x+=v.x>>2;
     center.y+=v.y>>2;
     center.z+=v.z>>2;
    }
 this->center=center;

 return (Object *)this;
}


/********************************\
 *        RTRIGGER STUFF        *
\********************************/
enum  {AI_RTRIGGER_WAIT,AI_RTRIGGER_ALIGNPLAYER,AI_RTRIGGER_RISE,
	  AI_RTRIGGER_WAITFORAUDIO,AI_RTRIGGER_TALK,AI_RTRIGGER_FADE,
	  AI_RTRIGGER_WAITFORCDEND};

void ramsesTrigger_func(Object *_this,int msg,int param1,int param2)
{RamsesTriggerObject *this=(RamsesTriggerObject *)_this;
 int fflag,i;
 switch (msg)
    {case SIGNAL_FLOORCONTACT:
	if (!this->enabled)
	   break;
	delay_moveObject((Object *)this,objectRunList);
	this->state=AI_RTRIGGER_ALIGNPLAYER;
	this->fad=getTrackStartFAD(3);
	playCDTrack(3,0);
	switchPlayerMotion(0);
	this->timer=0;
	this->enabled=0;
	{unsigned short *colorRam=(unsigned short *)SCL_COLRAM_ADDR;
	 for (i=0;i<256;i++)
	    colorRam[NMOBJECTPALLETES*256+i]=
	       ((unsigned short *)jasonPallete)[i];
	}
	break;
     case SIGNAL_VIEW:
	this->ramses->flags|=SPRITEFLAG_FLASH;
	break;
     case SIGNAL_MOVE:
	switch (this->state)
	   {case AI_RTRIGGER_ALIGNPLAYER:
	       camera->vel.x+=(F(level_sector[this->sectorNm].center[0])-
			       camera->pos.x)>>5;
	       camera->vel.z+=(F(level_sector[this->sectorNm].center[2])-
			       camera->pos.z)>>5;
	       camera->vel.x-=camera->vel.x>>3;
	       camera->vel.z-=camera->vel.z>>3;

	       playerAngle.yaw-=playerAngle.yaw>>3;
	       playerAngle.pitch-=playerAngle.pitch>>3;

	       if (this->timer++>30)
		  {this->state=AI_RTRIGGER_RISE;
#define RAMSEC 57
		   this->ramses=newSprite(RAMSEC,F(16),F(1),0,
					  level_sequenceMap[OT_RAMSESTRIGGER],
					  SPRITEFLAG_INVISIBLE|
					  SPRITEFLAG_IMATERIAL|
					  SPRITEFLAG_IMMOBILE|
					  SPRITEFLAG_NOSCALE,
					  (Object *)this);
		   this->ramses->pos.x=F(level_sector[RAMSEC].center[0])+F(25);
		   this->ramses->pos.y=F(level_sector[RAMSEC].center[1])-F(32);
		   this->ramses->pos.z=F(level_sector[RAMSEC].center[2]);
		   this->ramses->scale=65536+16000;

		   addLight(this->ramses,0,0,0);
		   this->timer=0;
		  }
	       break;
	    case AI_RTRIGGER_RISE:
	       if (this->timer>20)
		  {this->ramses->pos.y+=F(1);
		   playerAngle.pitch+=F(1)>>1;
		   this->ramses->flags&=~SPRITEFLAG_INVISIBLE;
		   fflag=spriteAdvanceFrame(this->ramses);
		   if (fflag & FRAMEFLAG_ENDOFSEQUENCE)
		      {this->state=AI_RTRIGGER_WAITFORAUDIO;
		       this->ramses->sequence=
			  level_sequenceMap[OT_RAMSESTRIGGER]+1;
		       removeLight(this->ramses);
		       this->baseY=this->ramses->pos.y;
		       this->timer=0;
		       break;
		      }
		  }
	       if (this->timer<64)
		  {int color;
		   color=f(evalHermite(F(this->timer)>>6,0,F(25),F(0),F(0)));
		   changeLightColor(this->ramses,color,color,color);
		  }
	       this->timer++;
	       break;
	    case AI_RTRIGGER_WAITFORAUDIO:
	       if (getCurrentFAD()>this->fad+577)
		  {this->ramses->sequence=
		      level_sequenceMap[OT_RAMSESTRIGGER]+3;
		   this->state=AI_RTRIGGER_TALK;
		  }
	       break;
	    case AI_RTRIGGER_TALK:
	       fflag=spriteAdvanceFrame(this->ramses);
	       if (fflag & FRAMEFLAG_ENDOFSEQUENCE)
		  {this->state=AI_RTRIGGER_WAITFORCDEND;
		   this->ramses->sequence=
		      level_sequenceMap[OT_RAMSESTRIGGER]+1;
		  }
	       break;
	    case AI_RTRIGGER_WAITFORCDEND:
	       if ((getCurrentStatus()&0xf)==1)
		  {this->state=AI_RTRIGGER_FADE;
		   this->ramses->sequence=
		      level_sequenceMap[OT_RAMSESTRIGGER]+2;
		  }
	       break;
	    case AI_RTRIGGER_FADE:
	       fflag=spriteAdvanceFrame(this->ramses);
	       if (fflag & FRAMEFLAG_ENDOFSEQUENCE)
		  {freeSprite(this->ramses);
		   delay_moveObject((Object *)this,objectIdleList);
		   switchPlayerMotion(1);
		  }
	       break;
	      }
       }
}

Object *constructRamsesTrigger(int sectorNm)
{int i;
 RamsesTriggerObject *this=(RamsesTriggerObject *)
    getFreeObject(ramsesTrigger_func,OT_RAMSESTRIGGER,CLASS_SECTOR);
 assert(this);
 moveObject((Object *)this,objectIdleList);
 this->aiSlot=nextAiSlot++;
 this->sectorNm=sectorNm;
 this->state=AI_RTRIGGER_WAIT;
 this->enabled=1;
 for (i=level_sector[sectorNm].firstWall;i<=level_sector[sectorNm].lastWall;
      i++)
    {if (level_wall[i].normal[1]>0)
	{level_wall[i].object=this;
	}
    }
 return (Object *)this;
}


/******************\
 * BUBBLES STUFF  *
\******************/

unsigned short bubbleSequenceMap[]={0,1,2};

void bubble_func(Object *_this,int msg,int param1,int param2)
{BubbleObject *this=(BubbleObject *)_this;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	spriteAdvanceFrame(this->sprite);
	this->sprite->pos.y+=F(2);
	this->distLeft-=F(2);
	if (this->distLeft<=0)
	   delayKill(_this);
	break;
       }
}

Object *constructBubble(int sector,MthXyz *pos,int distToCiel)
{static int type=0;
 BubbleObject *this=(BubbleObject *)
    getFreeObject(bubble_func,OT_BUBBLES,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(16),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_IMMOBILE,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos=*pos;
 this->sequenceMap=bubbleSequenceMap;

 setState((SpriteObject *)this,type++);
 if (type>2)
    type=0;
 this->distLeft=abs(distToCiel);
 return (Object *)this;
}

/******************\
 * DROPLET STUFF  *
\******************/

unsigned short oneBubbleSequenceMap[]={0};

void oneBubble_func(Object *_this,int msg,int param1,int param2)
{OneBubbleObject *this=(OneBubbleObject *)_this;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	this->sprite->pos.x+=this->sprite->vel.x;
	this->sprite->pos.y+=this->sprite->vel.y;
	this->sprite->pos.z+=this->sprite->vel.z;
	this->sprite->vel.y+=GRAVITY<<1;
	this->sprite->scale-=600;
	if (this->sprite->scale<0)
	   {this->sprite->scale=1;
	    delayKill(_this);
	   }
	break;
       }
}

Object *constructOneBubble(int sector,MthXyz *pos,MthXyz *vel)
{OneBubbleObject *this=(OneBubbleObject *)
    getFreeObject(oneBubble_func,OT_1BUBBLE,CLASS_SPRITE);
 assert(this);
 if (!this)
    return NULL;
 this->sprite=newSprite(sector,F(1),F(1),-GRAVITY<<1,
			0,SPRITEFLAG_IMATERIAL,
			(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos=*pos;
 this->sprite->vel=*vel;
 this->sprite->scale=64000;
 this->sequenceMap=oneBubbleSequenceMap;

 setState((SpriteObject *)this,0);
 return (Object *)this;
}



/******************\
 *   CAMEL STUFF  *
\******************/

unsigned short camelSequenceMap[]={0};

void camel_func(Object *_this,int msg,int param1,int param2)
{CamelObject *this=(CamelObject *)_this;
 int collide;
 int fflags;
 switch (msg)
    {case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    freeSprite(this->sprite);
	 break;
	}
     case SIGNAL_VIEW:
	break;
     case SIGNAL_MOVE:
	collide=moveSprite(this->sprite);
	fflags=spriteAdvanceFrame(this->sprite);
	if (collide & COLLIDE_SPRITE)
	   {if (&(sprites[collide&0xffff])==camera)
	       {/* player picked us up */
		playSound((int)this,level_objectSoundMap[OT_CAMEL]);
		dPrint("camel to level %d\n",this->toLevel);
		playerGetCamel(this->toLevel);
	       }
	   }
	break;
       }
}

Object *constructCamel(int sector)
{CamelObject *this=(CamelObject *)
    getFreeObject(camel_func,OT_CAMEL,CLASS_SPRITE);
 assert(this);
 moveObject((Object *)this,objectRunList);
 this->sprite=newSprite(sector,F(48),F(1),0,
			0,SPRITEFLAG_IMATERIAL|SPRITEFLAG_IMMOBILE,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sequenceMap=camelSequenceMap;
 this->toLevel=0;

 setState((SpriteObject *)this,0);
 return (Object *)this;
}

/********************************\
 *          QUEEN STUFF        *
\********************************/

enum  {AI_QUEEN_IDLE,
	  AI_QUEEN_WALK,
	  AI_QUEEN_SHOOTTAIL,
	  AI_QUEEN_LOSETAIL,
	  AI_QUEEN_WALKNOTAIL,

	  AI_QUEEN_SHOOTMOUTH,
	  AI_QUEEN_SRA,
	  AI_QUEEN_HURTBLOOD,
	  AI_QUEEN_DEAD,

	  AI_QUEEN_NMSEQ};

unsigned short queenSeqMap[]=
{
 HB|0,
 HB|0,
 HB|16,
 HB|40,
 HB|8,

 HB|24,
 HB|32,
 HB|49,
 50,
};

#define LOSETAIL 95
void queen_func(Object *_this,int message,int param1,int param2)
{QueenObject *this=(QueenObject *)_this;
 int collide,i;
 int fflags;
 collide=0;
 fflags=0;

 switch (message)
    {case SIGNAL_HURT:
	if (this->state>=AI_QUEEN_HURTBLOOD &&
	    this->state<=AI_QUEEN_DEAD)
	   break;
	if (this->health>LOSETAIL &&
	    this->health-param1<=LOSETAIL)
	   {setState((SpriteObject *)this,AI_QUEEN_LOSETAIL);
	    this->sprite->vel.x=0;
	    this->sprite->vel.z=0;
	    {int x,z,angle;
	     angle=normalizeAngle(this->sprite->angle+F(180));
	     x=this->sprite->pos.x+(MTH_Cos(angle)<<4);
	     z=this->sprite->pos.z+(MTH_Sin(angle)<<4);
	     constructOneShot(this->sprite->s,
			      x,
			      this->sprite->pos.y+F(30),
			      z,
			      OT_GRENPOW,F(2),0,0);
	    }
	   }
	this->health-=param1;
	this->sprite->flags|=SPRITEFLAG_FLASH;
	if (this->health<=0)
	   {setState((SpriteObject *)this,AI_QUEEN_HURTBLOOD);
	    this->sprite->vel.x=0;
	    this->sprite->vel.z=0;
	   }
	break;
     case SIGNAL_OBJECTDESTROYED:
	monsterObject_signalDestroyed((MonsterObject *)this,param1);
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_MOVE:
	if (this->state==AI_QUEEN_DEAD)
	   break;
	if (this->state!=AI_QUEEN_IDLE)
	   {collide=moveSprite(this->sprite);
	    fflags=spriteAdvanceFrame(this->sprite);
	    if (!this->enemy)
	       setState((SpriteObject *)this,AI_QUEEN_IDLE);
	   }
	switch (this->state)
	   {case AI_QUEEN_IDLE:
	       normalMonster_idle((MonsterObject *)this,2,-1);
	       break;
	    case AI_QUEEN_HURTBLOOD:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {setState((SpriteObject *)this,AI_QUEEN_DEAD);
		   this->sprite->flags|=SPRITEFLAG_IMATERIAL;
		   constructOneShot(this->sprite->s,
				    this->sprite->pos.x,
				    this->sprite->pos.y,
				    this->sprite->pos.z,
				    OT_LANDGUTS,
				    48000,0,0);
		   for (i=0;i<5;i++)
		      constructHardBit(this->sprite->s,&this->sprite->pos,
				       level_sequenceMap[OT_QUEEN]+51,0);
		   constructQhead(this->sprite->s,
				  &this->sprite->pos);
		  }
	       break;
	    case AI_QUEEN_LOSETAIL:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  setState((SpriteObject *)this,AI_QUEEN_WALKNOTAIL);
	       break;
	    case AI_QUEEN_SHOOTTAIL:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (this->fireCount--<=0)
		      setState((SpriteObject *)this,AI_QUEEN_WALK);
		  }
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {if (canSee(this->sprite,this->enemy->sprite))
		      {MthXyz ballPos;
		       MthXyz ballVel;
		       initProjectile(&this->sprite->pos,
				      &this->enemy->sprite->pos,
				      &ballPos,&ballVel,F(50),4);
		       constructSball(this->sprite->s,&ballPos,&ballVel,
				      (SpriteObject *)this,
				      getAngle(ballVel.x,ballVel.z),52,
				      45000);
		      }
		  }
	       break;
	    case AI_QUEEN_SHOOTMOUTH:
	       if (fflags & FRAMEFLAG_ENDOFSEQUENCE)
		  {setState((SpriteObject *)this,AI_QUEEN_WALKNOTAIL);
		  }
	       break;
	    case AI_QUEEN_WALKNOTAIL:
	    case AI_QUEEN_WALK:
	       if (AISLOT(0x0f))
		  {PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)this->enemy);
		   if ((getNextRand()&0xff)<190)
		      {this->sprite->angle=
			  normalizeAngle(this->sprite->angle+randomAngle(6));
		       this->sprite->vel.x=MTH_Cos(this->sprite->angle)<<2;
		       this->sprite->vel.z=MTH_Sin(this->sprite->angle)<<2;
		       break;
		      }
		   switch (this->state)
		      {case AI_QUEEN_WALK:
			  /* shoot player */
			  setState((SpriteObject *)this,AI_QUEEN_SHOOTTAIL);
			  this->fireCount=4;
			  this->sprite->vel.x=0;
			  this->sprite->vel.z=0;
			  break;
		       case AI_QUEEN_WALKNOTAIL:
			  /* shoot player */
			  setState((SpriteObject *)this,AI_QUEEN_SHOOTMOUTH);
			  this->sprite->vel.x=0;
			  this->sprite->vel.z=0;
			  break;
			 }
		  }
	       break;
	      }
	break;
       }
}

Object *constructQueen(int sector)
{QueenObject *this=(QueenObject *)getFreeObject(queen_func,OT_QUEEN,
						  CLASS_MONSTER);
 assert(this);
 moveObject((Object *)this,objectRunList);

 this->sprite=newSprite(sector,F(64),F(1),GRAVITY<<1,
			0,SPRITEFLAG_BWATERBNDRY,
			(Object *)this);
 suckSpriteParams(this->sprite);
 this->sprite->scale=65536*1.3;
 this->sequenceMap=queenSeqMap;
 setState((SpriteObject *)this,AI_QUEEN_IDLE);
 this->health=100;
 this->enemy=NULL;
 this->routePos=-1;
 this->aiSlot=nextAiSlot++;
 return (Object *)this;
}

/********************************\
 *          QHEAD  STUFF        *
\********************************/

enum {AI_QHEAD_WAIT,AI_QHEAD_RUN,AI_QHEAD_ATTACK};

unsigned short qheadSeqMap[]={HB|53,HB|53,HB|53};

void qhead_func(Object *_this,int message,int param1,int param2)
{QheadObject *this=(QheadObject *)_this;
 int collide,fflags,i;
 int c,s,wave;
 switch (message)
    {case SIGNAL_HURT:
	if (this->state==AI_QHEAD_WAIT)
	   break;
	this->sprite->flags|=SPRITEFLAG_FLASH;
	for (i=0;i<NMQHEADBALLS;i++)
	   this->balls[i]->flags|=SPRITEFLAG_FLASH;
	this->health-=param1;
	this->nmBalls=this->health/20;
	if (this->nmBalls<0) this->nmBalls=0;
	if (this->nmBalls>NMQHEADBALLS) this->nmBalls=NMQHEADBALLS;
	if (this->health<=0)
	   {for (i=0;i<20;i++)
	       constructOneShot(this->sprite->s,
				this->sprite->pos.x+(getNextRand()<<6)-F(32),
				this->sprite->pos.y+(getNextRand()<<5)+F(16),
				this->sprite->pos.z+(getNextRand()<<6)-F(32),
				OT_GRENPOW,F(2),0,i*5);
	    delayKill(_this);
	   }
	break;
     case SIGNAL_VIEW:
	setSequence((SpriteObject *)this);
	break;
     case SIGNAL_OBJECTDESTROYED:
	{Object *killed=(Object *)param1;
	 if (killed==_this)
	    {freeSprite(this->sprite);
	     for (i=0;i<NMQHEADBALLS;i++)
		if (this->balls[i])
		   freeSprite(this->balls[i]);
	    }
	 break;
	}
     case SIGNAL_MOVE:
	if (this->state==AI_QHEAD_WAIT)
	   {this->waitTime++;
	    if (moveSprite(this->sprite)&COLLIDE_FLOOR)
	       {this->sprite->vel.x=0;
		this->sprite->vel.z=0;
	       }
	    if (this->waitTime>170)
	       {setState((SpriteObject *)this,AI_QHEAD_ATTACK);
		this->sprite->gravity=0;
		this->balls[0]->flags&=~SPRITEFLAG_INVISIBLE;
		moveSpriteTo(this->balls[0],
			     this->sprite->s,
			     &this->sprite->pos);
	       }
	    else
	       break;
	   }
	for (i=NMQHEADBALLS-1;i>=this->nmBalls;i--)
	   {if (!this->balls[i])
	       continue;
	    collide=moveSprite(this->balls[i]);
	    if (collide & COLLIDE_FLOOR)
	       {constructOneShot(this->balls[i]->s,
				 this->balls[i]->pos.x,
				 this->balls[i]->pos.y+F(30),
				 this->balls[i]->pos.z,
				 OT_GRENPOW,F(2),0,0);
		freeSprite(this->balls[i]);
		this->balls[i]=NULL;
	       }
	   }
	for (i=this->nmBalls-1;i>0;i--)
	   {moveSpriteTo(this->balls[i],
			 this->balls[i-1]->s,
			 &this->balls[i-1]->pos);
	    this->balls[i]->flags=
	       this->balls[i-1]->flags;
	   }
	if (this->balls[0])
	   moveSpriteTo(this->balls[0],
			this->sprite->s,
			&this->sprite->pos);

	collide=moveSprite(this->sprite);
	fflags=spriteAdvanceFrame(this->sprite);
	/* do motion */
	c=MTH_Cos(this->sprite->angle);
	s=MTH_Sin(this->sprite->angle);
	this->sprite->vel.x=c<<4;
	this->sprite->vel.z=s<<4;

	wave=MTH_Cos(this->wave);
	this->wave+=F(20);
	if (this->wave>F(180))
	   this->wave-=F(360);
	this->sprite->vel.x+=MTH_Mul(s,wave)<<2;
	this->sprite->vel.z+=MTH_Mul(-c,wave)<<2;

	switch (this->state)
	   {case AI_QHEAD_ATTACK:
	       spriteHome((SpriteObject *)this,(SpriteObject *)player,
			  F(20),F(15));
	       if (this->sprite->pos.y<player->sprite->pos.y-F(3))
		  this->sprite->vel.y=F(2);
	       else
		  if (this->sprite->pos.y>player->sprite->pos.y+F(3))
		     this->sprite->vel.y=-F(2);
		  else
		     this->sprite->vel.y=0;
	       if ((collide & COLLIDE_SPRITE) &&
		   (sprites[collide&0xffff].owner==(Object *)player))
		  {signalObject((Object *)player,SIGNAL_HURT,10,
				(int)this);
		   PlotCourseToObject((SpriteObject *)this,
				      (SpriteObject *)player);
		   this->sprite->angle+=F(180-32)+F(getNextRand()&0x3f);
		   setState((SpriteObject *)this,AI_QHEAD_RUN);
		   this->sprite->vel.y=0;
		   this->waitTime=60;
		  }
	       break;
	    case AI_QHEAD_RUN:
	       if (collide & COLLIDE_WALL)
		  this->sprite->angle=
		     normalizeAngle(this->sprite->angle+
				    F(getNextRand()<<6)-F(32));
	       this->waitTime--;
	       if (this->waitTime<=0)
		  setState((SpriteObject *)this,AI_QHEAD_ATTACK);
	       break;
	      }
	break;
       }
}

Object *constructQhead(int sector,MthXyz *pos)
{int i;
 QheadObject *this=(QheadObject *)getFreeObject(qhead_func,OT_QHEAD,
						CLASS_MONSTER);
 assert(this);
 if (!this)
    return NULL;

 this->sprite=newSprite(sector,F(16),F(1),GRAVITY<<1,
			0,SPRITEFLAG_NOSPRCOLLISION,(Object *)this);
 if (!this->sprite)
    return NULL;
 moveObject((Object *)this,objectRunList);
 this->sprite->pos=*pos;
 this->sprite->scale=65536*1.3;
 this->sprite->vel.y=F(10);
 this->sprite->vel.x=F(6);
 this->sprite->vel.z=F(2);
 this->sequenceMap=qheadSeqMap;
 setState((SpriteObject *)this,AI_QHEAD_WAIT);

 for (i=0;i<NMQHEADBALLS;i++)
    {this->balls[i]=newSprite(sector,F(16),F(1),0,
			      level_sequenceMap[OT_QUEEN]+61,
			      SPRITEFLAG_INVISIBLE|SPRITEFLAG_NOSPRCOLLISION,
			      (Object *)this);
     this->balls[i]->pos=*pos;
     this->balls[i]->scale=60000-i*3000;
     /* prepare for falling off snake */
     this->balls[i]->vel.x=(MTH_GetRand()&0x7ffff)-F(4);
     this->balls[i]->vel.y=(MTH_GetRand()&0x3fffff);
     this->balls[i]->vel.z=(MTH_GetRand()&0x7ffff)-F(4);
     this->balls[i]->gravity=GRAVITY<<2;
    }
 this->wave=0;
 this->waitTime=0;
 this->enemy=NULL;
 this->aiSlot=nextAiSlot++;
 this->health=100;
 this->nmBalls=NMQHEADBALLS;
 return (Object *)this;
}

void doh_func(Object *o,int i,int j,int k)
{assert(0);
}

void runObjects(void)
{signalList(objectRunList,SIGNAL_MOVE,0,0);
 aicount++;
}
