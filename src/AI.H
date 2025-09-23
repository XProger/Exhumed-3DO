#ifndef __INCLUDEDaih
#define __INCLUDEDaih
/* Sequence definitions */

#define MAX_VISUAL_RANGE 4000

typedef MonsterObject SpiderObject;
typedef MonsterObject PlayerObject;
typedef MonsterObject FishObject;

typedef SpriteObject OneBubbleObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, seeCounter;
} TorchObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, toLevel, disableTime;
} CamelObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint16 speedDiv;
} BobBlockObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint32 vel, distanceToFall, fallAccum;
    sint32 channel, switchChannel;
} SinkBlockObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint32 vel;
} EarthQuakeBlockObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint32 distanceToRise, vel;
} RamsesLidObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;

    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad;
    sint32 distLeft;
} BubbleObject;

typedef struct
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
    sint32 anger;
} BlobObject;

typedef struct
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
    sint32 age;
} QeggObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 sectorNm, state, aiSlot;

    sint32 timer, disabled;
    struct __sprite* ramses;
    sint32 baseY;
    sint32 homeSector;
    sint32 fd, soundRingBase, soundRingHead;
#ifdef JAPAN
    sint32 soundRingBase2;
#endif
    sint32 nmFrames, framePos;
    sint8* frames;
    sint8 sndSlot, lastSndPos;
} RamsesTriggerObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 sectorNm, state, aiSlot;

    sint32 toLevel, powerLevel, angle, countDown, enable, endOfPuzzle;
    sSectorType* pSector;
} TeleportObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 sectorNm, state, aiSlot;

    SpriteObject* artifact;
    sint32 age, artifactNm, playerSec, playerAngle, artifactPlace;
    MthXyz apos;
} TeleportReturnObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 wallNm, state, aiSlot;

    MthXyz center;
} FFieldObject;

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
    sint16 damage;
    uint16 explosionColor;
    sint8 light;
    sint32 armTime;
} GenericProjectileObject;

typedef struct
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

    sint32 jinkTimer;
    sint16 diveTimer;
} HawkObject;

typedef struct
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

    sint32 boomTimer, iAmMapHolder;
} BlowPotObject;

typedef struct
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

    sint16 hoverCounter, dartCounter;
} WaspObject;

#define NMCOBRABALLS 5
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

    Sprite* balls[NMCOBRABALLS];
    sint32 wave;
    MonsterObject* enemy;
    sint32 aiSlot, glow;
} CobraObject;

#define NMQHEADBALLS 5
typedef struct
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

    Sprite* balls[NMQHEADBALLS];
    sint32 wave, waitTime, nmBalls;
} QheadObject;

#define NMZAPBALLS 5
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

    Sprite* balls[NMZAPBALLS];
    sint32 wave;
    MonsterObject* enemy;
    sint32 aiSlot, red;
    MthXyz hitPos;
} ZapObject;

typedef struct
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

    sint16 stunCounter;
} AnubisObject;

typedef struct
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

    sint32 tailInhibit;
    sint16 rarCount, angry;
} MagmantisObject;

typedef AnubisObject MummyObject;
typedef AnubisObject SetObject;

typedef struct
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

    sint16 stunCounter;
    sint16 aimCounter, sprintCounter, decideNow;
} SentryObject;

typedef struct
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

    sint16 stunCounter;
    sint16 chargeCounter;
    sint32 sparkTimer;
} SelkisObject;

typedef struct
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

    sint16 fireCount;
    sint16 chargeCounter;
    sint16 multiCharge;
} QueenObject;

typedef struct
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

    sint16 stunCounter;
    sint16 portTimer;
    sint16 hasLight;
} BastetObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    uint16 sequence;
    sint32 age, soft;
} BitObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    uint16 sequence;
    sint32 stuck;
} HardBitObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    sint32 age, big;
} ZorchObject;

#define THINGFLAG_WAVE 1
#define THINGFLAG_PULSE 2
#define THINGFLAG_ANIMATE 4
#define THINGFLAG_THROB 8
#define THINGFLAG_MYSTICAL 16
#define THINGFLAG_BLUEMYSTICAL 32
#define THINGFLAG_NOISY 64
typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    sint32 baseY, sin, frame;
    sint32 flags, light;
} ThingObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    sint32* parameters;
    sint32 ageInc;
    sint32 age;
} LightObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    struct __sprite* sprite;
    uint16* sequenceMap;
    sint16 state, pad1;

    sint32 waitTime;
} OneShotObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint32 wallNm;
    sint16 aiSlot, state;

    sint32 firstTile, lastTile, speed;
} AnimWallObject;

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

    MonsterObject* target;
    sint32 damage;
} CloudObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint16 throw;
    sint16 stepEnable;
    sint16 channel;
    sint16 direction;
} ElevatorObject;

typedef ElevatorObject UpDownElevatorObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint16 channel;
    sint16 doorHeight;
} DoorObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 pbNum, state;
    sint32 counter, waitCounter;
    fix32 offset;

    sint32 channel, upperLevel, lowerLevel;
} FloorSwitchObject;

#define NMSHOOTERBEAMS 16
typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 wallNm, state, aiSlot;

    sint16 sectorNm;
    sint8 nmLines;
    sint32 pulse;
    MthXyz normal;
    MthXyz orficePos;
    Sprite* beam[NMSHOOTERBEAMS];
    sint16 switchType, channel, counter;
} ShooterObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 wallNm, state, aiSlot;

    sint8* tilePos;
    sint16 frame, sequence;
    sint16 channel, sectorNm;
    MthXyz orficePos;
} SwitchObject;

typedef struct
{
    sint16 type, class;
    struct __object *next, *prev;
    messHandler func;
    sint16 sectorNm, state, aiSlot;

    sint32 channel;
} SectorSwitchObject;

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
} BoingRockObject;

void item_func(Object* this, sint32 message, sint32 param1, sint32 param2);
void spider_func(Object* this, sint32 message, sint32 param1, sint32 param2);
void hawk_func(Object* this, sint32 message, sint32 param1, sint32 param2);
void anubis_func(Object* this, sint32 message, sint32 param1, sint32 param2);
void torch_func(Object* this, sint32 message, sint32 param1, sint32 param2);
void player_func(Object* this, sint32 message, sint32 param1, sint32 param2);

Object* constructSpider(sint32 sector, sint32 suckParams, MthXyz* pos, MthXyz* vel);
PlayerObject* constructPlayer(sint32 sector, sint32 suckParams);
Object* constructAnubis(sint32 sector);
Object* constructSelkis(sint32 sector);
Object* constructSet(sint32 sector);
Object* constructSentry(sint32 sector);
Object* constructMummy(sint32 sector);
Object* constructKapow(sint32 sector, sint32 x, sint32 y, sint32 z);
Object* constructWasp(sint32 sector);
Object* constructFish(sint32 sector);
Object* constructThing(sint32 sector, sint32 x, sint32 y, sint32 z, sint32 thingType);
Object* constructOneShot(sint32 sector, sint32 x, sint32 y, sint32 z, sint32 oneShotType, sint32 scale, uint16 colored, sint32 waitTime);
Object* constructZorch(sint32 sector, sint32 x, sint32 y, sint32 z, sint32 zorchType);
Object* constructFlameball(sint32 sector, MthXyz* pos, MthXyz* vel, SpriteObject* owner, sint32 heading, sint32 frame);
Object* constructGrenade(sint32 sector, MthXyz* pos, MthXyz* vel, SpriteObject* owner);
Object* constructLight(sint32 sector, sint32 x, sint32 y, sint32 z, sint32* parameters, fix32 ageInc);
Object* constructCobra(sint32 sector, sint32 x, sint32 y, sint32 z, sint32 heading, fix32 yvel, SpriteObject* owner, sint32 type, sint32 glow);
Object* constructRingo(sint32 sector, MthXyz* pos, MthXyz* vel, SpriteObject* owner);
Object* constructDoor(sint32 type, sint32 pb);
Object* constructBobBlock(sint32 pb);
Object* constructSinkBlock(sint32 pb);

Object* constructCloud(MonsterObject* target, sint32 damage);

Object* constructHawk(sint32 sector);

Object* constructElevator(sint32 pb, sint32 type, sint16 lowerLevel, sint16 upperLevel);
Object* constructUpDownElevator(sint32 pb, sint32 type, sint16 lowerLevel, sint16 upperLevel);
Object* constructFloorSwitch(sint32 pb);

Object* constructBastet(sint32 sector);

Object* constructForceField(sint32 wallNm);

Object* constructRamsesTrigger(sint32 triggerSec, sint32 homeSector);

Object* constructCamel(sint32 sector);

Object* constructMagmantis(sint32 sector);
Object* constructBubble(sint32 sector, MthXyz* pos, sint32 distToCiel);
Object* constructOneBubble(sint32 sector, MthXyz* pos, MthXyz* vel);
Object* constructBlob(sint32 sector);
Object* constructZap(sint32 sector, MthXyz* pos, SpriteObject* owner, MonsterObject* enemy, sint32 red);
Object* constructQueen(sint32 sector);
Object* constructQhead(sint32 sector, MthXyz* pos);

Object* constructTorch(sint32 sector, sint32 type);
Object* constructBlowPot(sint32 sector, sint32 type);

Object* constructTeleportReturn(void);
Object* constructTeleporter(void);

Object* constructShooter(sint32 type);
Object* constructRamsesLid(sint32 pb);
Object* constructSwitch(sint32 type);

void runObjects(void);

void rotateFlames(sint32 angle, sint32 cx, sint32 cz);
void translateFlames(sint32 dx, sint32 dy, sint32 dz);
void initFlames(void);

Object* constructDownDoor(sint32 pb);
Object* constructSectorSwitch(void);
Object* constructBouncyBit(sint32 sector, MthXyz* pos, sint32 sequence, sint32 frame, sint32 soft);

Object* constructHardBit(sint32 sector, MthXyz* pos, sint32 sequence, sint32 onFloor, sint32 frame);

Object* constructBoingRock(sint32 sector, MthXyz* pos, MthXyz* vel);
Object* constructEarthQuakeBlock(sint32 pb);

void resetLimitCounters(void);

#endif
