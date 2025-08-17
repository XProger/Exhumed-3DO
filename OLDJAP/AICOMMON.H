#ifndef __INCLUDEDaicommonh
#define __INCLUDEDaicommonh

#define GRAVITY (6<<12)
#define HB 0x8000

#define AISLOT(x) ((this->aiSlot&(x))==(aicount&(x)))

extern int aicount,nextAiSlot;

void markSectorFloor(int sec,Object *this);

void setSectorBrightness(int s,int level);

int getFacingAngle(Sprite *from,Sprite *to);

MonsterObject *findPlayer(Sprite *sprite,int dist);

Fixed32 spriteDistApprox(Sprite *s1,Sprite *s2);

Fixed32 spriteDist2(Sprite *s1,Sprite *s2);

int PlotCourseToObject(SpriteObject *from,SpriteObject *to);

int randomAngle(int spreadlog2);

void spriteHome(SpriteObject *this,SpriteObject *enemy,
		       int fudge,int step);

void spriteObject_makeSound(SpriteObject *this,int sndNm);

MthXyz followRoute(MonsterObject *this);

void monsterObject_signalDestroyed(MonsterObject *this,int param1);

int monsterObject_signalHurt(MonsterObject *this,int param1,int param2);

void setSequence(SpriteObject *this);

void initProjectile(MthXyz *from,MthXyz *to,
		    MthXyz *outPos,MthXyz *outVel,
		    int height,
		    int speed2);

void normalMonster_walking(MonsterObject *this,int collide,int fflags,
			   int speed);

enum {DO_RANDOMWANDER,DO_CHARGE,DO_FOLLOWROUTE,DO_GOIDLE};
int decideWhatToDo(MonsterObject *this,int route,int floater);

enum {STATE_IDLE,STATE_WALK,STATE_SRA,STATE_LRA,
	 STATE_HIT,STATE_SEEK,STATE_NMBASICSTATES};

void normalMonster_idle(MonsterObject *this,int speed,int icuSound);
int monster_seekEnemy(MonsterObject *this,int floater,int speed2);
void setState(SpriteObject *this,int state);

void movePlayerToSector(int playerSec);

void signalAllObjects(int signal,int param1,int param2);

void pbObject_move(PushBlockObject *pbo,Fixed32 dy);

void pbObject_moveTo(PushBlockObject *pbo,int y);

void spriteObject_makeZorch(SpriteObject *this);

void explodeMaskedWall(int wallNm);
void explodeAllMaskedWallsInSector(int s);

#endif
