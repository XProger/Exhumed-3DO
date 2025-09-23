#ifndef __INCLUDEDaicommonh
#define __INCLUDEDaicommonh

#define GRAVITY (6 << 12)
#define HB 0x8000

#define AISLOT(x) ((this->aiSlot & (x)) == (aicount & (x)))

extern sint32 aicount, nextAiSlot;

void markSectorFloor(sint32 sec, Object* this);

void setSectorBrightness(sint32 s, sint32 level);

sint32 getFacingAngle(Sprite* from, Sprite* to);

MonsterObject* findPlayer(Sprite* sprite, sint32 dist);

fix32 spriteDistApprox(Sprite* s1, Sprite* s2);

fix32 spriteDist2(Sprite* s1, Sprite* s2);

sint32 PlotCourseToObject(SpriteObject* from, SpriteObject* to);

sint32 randomAngle(sint32 spreadlog2);

void spriteHome(SpriteObject* this, SpriteObject* enemy, sint32 fudge, sint32 step);

void spriteObject_makeSound(SpriteObject* this, sint32 sndNm);

MthXyz followRoute(MonsterObject* this);

void monsterObject_signalDestroyed(MonsterObject* this, sint32 param1);

sint32 monsterObject_signalHurt(MonsterObject* this, sint32 param1, sint32 param2);

void setSequence(SpriteObject* this);

void initProjectile(MthXyz* from, MthXyz* to, MthXyz* outPos, MthXyz* outVel, sint32 height, sint32 speed2);

void normalMonster_walking(MonsterObject* this, sint32 collide, sint32 fflags, sint32 speed);

enum
{
    DO_RANDOMWANDER,
    DO_CHARGE,
    DO_FOLLOWROUTE,
    DO_GOIDLE
};
sint32 decideWhatToDo(MonsterObject* this, sint32 route, sint32 floater);

enum
{
    STATE_IDLE,
    STATE_WALK,
    STATE_SRA,
    STATE_LRA,
    STATE_HIT,
    STATE_SEEK,
    STATE_NMBASICSTATES
};

void normalMonster_idle(MonsterObject* this, sint32 speed, sint32 icuSound);
sint32 monster_seekEnemy(MonsterObject* this, sint32 floater, sint32 speed2);
void setState(SpriteObject* this, sint32 state);

void movePlayerToSector(sint32 playerSec);

void signalAllObjects(sint32 signal, sint32 param1, sint32 param2);

void pbObject_move(PushBlockObject* pbo, fix32 dy);

void pbObject_moveTo(PushBlockObject* pbo, sint32 y);

void spriteObject_makeZorch(SpriteObject* this);

void explodeMaskedWall(sint32 wallNm);
void explodeAllMaskedWallsInSector(sint32 s);

#endif
