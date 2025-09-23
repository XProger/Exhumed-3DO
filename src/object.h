#ifndef __INCLUDEDobjecth
#define __INCLUDEDobjecth

#include "sprite.h"

#define SIGNAL_MOVE 0
#define SIGNAL_ENTER 1 /* sent to sector objects when player enters */
#define SIGNAL_PRESS 2
#define SIGNAL_CEILCONTACT 3 /* only sent for contact with player */
#define SIGNAL_FLOORCONTACT 4 /* only sent for contact with player */
#define SIGNAL_VIEW 5
#define SIGNAL_HURT 10 /* params: hp of damage, Object * to damager */
#define SIGNAL_OBJECTDESTROYED 11 /* params: Object * of destroyed object */
#define SIGNAL_SWITCH 12 /* params: channel */
#define SIGNAL_SWITCHRESET 13 /* params: channel */

#define CLASS_SPRITE 1
#define CLASS_SECTOR 2
#define CLASS_PUSHBLOCK 3
#define CLASS_PROJECTILE 4
#define CLASS_MONSTER 5
#define CLASS_WALL 6
#define CLASS_DEAD 100

struct __object;

typedef void (*messHandler)(struct __object* this, sint32 message, sint32 param1, sint32 param2);

typedef struct __object
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint8 pad[128];
} Object;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    sint16 wallNm, state, aiSlot;
} WallObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    sint16 sectorNm, state, aiSlot;
} SectorObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;
} PushBlockObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;
} SpriteObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    Object* owner;
    sint32 age;
} ProjectileObject;

typedef struct __monster
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    sint16 health, aiSlot;
    struct __monster* enemy;
    sint16 route[ROUTESIZE + 1];
    sint16 routePos;
} MonsterObject;

extern Object* objectRunList; /* these lists have head nodes */
extern Object* objectIdleList;
extern Object* objectFreeList;

void initObjects(void);

void delayKill(Object* o);
void processKills(void);

Object* getFreeObject(messHandler handler, sint32 type, sint32 class);
void signalObject(Object* object, sint32 message, sint32 param1, sint32 param2);
void hurtSprite(struct __sprite* sprite, Object* hurter, sint32 damage);
void signalList(Object* head, sint32 message, sint32 param1, sint32 param2);
void delayKill(Object* o);
void processDelayedMoves(void);
void delay_moveObject(Object* o, Object* toList);
void moveObject(Object* o, Object* toList);
void placeObjects(void);

void radialDamage(Object* this, MthXyz* center, sint32 sector, sint32 damage, fix32 radius);

void registerPBObject(sint32 pbNm, Object* me);

void pushBlockMakeSound(PushBlockObject* source, sint32 sndNm);
void pushBlockAdjustSound(PushBlockObject* source);

sint16 suckShort(void);
sint32 suckInt(void);

#endif
