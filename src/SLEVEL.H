
#ifndef __INCLUDEDslevelh
#define __INCLUDEDslevelh

#include <sega_xpt.h>

struct sLevelHeader
{
    sint32 nmSectors;
    sint32 nmWalls;
    sint32 nmVerticies;
    sint32 nmFaces;
    sint32 nmTextureIndexes;
    sint32 nmLightValues;
    sint32 nmObjects;
    sint32 nmObjectParams;
    sint32 nmPushBlocks;
    sint32 nmPBWalls;
    sint32 nmPBVert;
    sint32 nmWaveVert;
    sint32 nmWaveFace;
    sint32 nmCutSectors;
};

enum ObjectType
{
    OT_PISTOL,
    OT_M60,
    OT_GRENADEAMMO,
    OT_FLAMER,
    OT_COBRASTAFF,
    OT_RING,
    OT_MANACLE,
    OT_SANDALS,
    OT_MASK,
    OT_CAPE,
    OT_ANKLETS,
    OT_SCEPTER,
    OT_FEATHER,

    OT_PLAYER,
    OT_SPIDER,
    OT_ANUBIS,
    OT_MUMMY,
    OT_BASTET,
    OT_ANUBALL,
    OT_KAPOW,
    OT_BIT,
    OT_BOUNCYBIT,
    OT_WASP,
    OT_FISH,
    OT_BUGKEY,
    OT_TIMEKEY,
    OT_XKEY,
    OT_PLANTKEY,
    OT_HEALTHBALL,
    OT_AMMOBALL,
    OT_LANDGUTS,
    OT_AIRGUTS,
    OT_BLUEZORCH,
    OT_REDZORCH,
    OT_FLAMEBALL,
    OT_FLAMEWALL,
    OT_GRENPOW,
    OT_HAWK,
    OT_LIGHT,
    OT_POOF,
    OT_RINGO,
    OT_COBRA,
    OT_GRENADE,
    OT_HEALTHORB,
    OT_AMMOORB,
    OT_HEALTHSPHERE,
    OT_AMMOSPHERE,
    OT_PYRAMID,
    OT_NORMALDOOR,
    OT_NORMALELEVATOR,
    OT_BOBBINGBLOCK,
    OT_SINKINGBLOCK,
    OT_EARTHQUAKEBLOCK,
    OT_BUGDOOR,
    OT_TIMEDOOR,
    OT_XDOOR,
    OT_PLANTDOOR,
    OT_STUCKUPDOOR,
    OT_STUCKDOWNDOOR,
    OT_FLOORSWITCH,
    OT_RAMSESLID,
    OT_STUCKDOWNELEVATOR,
    OT_STARTSDOWNELEVATOR,
    OT_DOWNTHENUPELEVATOR,
    OT_RACLOUD,
    OT_FORCEFIELD,
    OT_RAMSESTRIGGER,
    OT_CAMEL,
    OT_BLOODBOWL,
    OT_SENTRY,
    OT_SBALL,
    OT_MAGMANTIS,
    OT_SET,
    OT_SELKIS,
    OT_SETBALL,
    OT_MUMBALL,
    OT_MAGBALL,
    OT_BUBBLES,
    OT_1BUBBLE,
    OT_SPLASH,
    OT_BLOB,
    OT_GRENBUBB,
    OT_LIGHTNING,
    OT_QUEEN,
    OT_QUEENEGG,
    OT_HARDBIT,
    OT_QHEAD,
    OT_TRANSMITTER,
    OT_BOOMPOT1,
    OT_BOOMPOT2,
    OT_GLOWWORM,
    OT_SECTORSWITCH,
    OT_INVISIBLEBALL,
    OT_WEAPONPOWERBALL,
    OT_EYEBALL,
    OT_BRICK,
    OT_BOINGROCK,
    OT_CHOPPER,
    OT_RAMMUMMY,
    OT_RAMSIDEMUMMY,

    OT_COMM_BATTERY,
    OT_COMM_BOTTOM,
    OT_COMM_DISH,
    OT_COMM_HEAD,
    OT_COMM_KEYBOARD,
    OT_COMM_MOUSE,
    OT_COMM_SCREEN,
    OT_COMM_TOP,

    OT_TORCH1,
    OT_TORCH2,
    OT_TORCH3,
    OT_TORCH4,
    OT_TORCH5,
    OT_TORCH6,
    OT_TORCH7,
    OT_TORCH8,
    OT_TORCH9,
    OT_TORCH10,
    OT_TORCH11,
    OT_TORCH12,
    OT_TORCH13,
    OT_TORCH14,
    OT_TORCH15,
    OT_TORCH16,
    OT_TORCH17,
    OT_TORCH18,
    OT_TORCH19,
    OT_TORCH20,
    OT_TORCH21,
    OT_TORCH22,
    OT_TORCH23,
    OT_TORCH24,
    OT_TORCH25,
    OT_TORCH26,
    OT_TORCH27,
    OT_TORCH28,
    OT_TORCH29,
    OT_TORCH30,
    OT_TORCH31,
    OT_TORCH32,
    OT_TORCH33,
    OT_TORCH34,
    OT_TORCH35,
    OT_TORCH36,
    OT_TORCH37,
    OT_TORCH38,

    OT_CONTAIN1,
    OT_CONTAIN2,
    OT_CONTAIN3,
    OT_CONTAIN4,
    OT_CONTAIN5,
    OT_CONTAIN6,
    OT_CONTAIN7,
    OT_CONTAIN8,
    OT_CONTAIN9,
    OT_CONTAIN10,
    OT_CONTAIN11,
    OT_CONTAIN12,
    OT_CONTAIN13,
    OT_CONTAIN14,
    OT_CONTAIN15,
    OT_CONTAIN16,
    OT_CONTAIN17,

    OT_TELEPSECTOR,
    OT_TELEPRETURN,
    OT_SHOOTER1,
    OT_SHOOTER2,
    OT_SHOOTER3,
    OT_SW1,
    OT_SW2,
    OT_SW3,
    OT_SW4,

    OT_ANIM_CHAOS1,
    OT_ANIM_CHAOS2,
    OT_ANIM_CHAOS3,
    OT_ANIM_LAVA1,
    OT_ANIM_LAVA2,
    OT_ANIM_LAVA3,
    OT_ANIM_LAVAFALL,
    OT_ANIM_LAVAPO1,
    OT_ANIM_LAVAPO2,
    OT_ANIM_LAVAFAL1,
    OT_ANIM_TELEP1,
    OT_ANIM_TELEP2,
    OT_ANIM_TELEP3,
    OT_ANIM_TELEP4,
    OT_ANIM_TELEP5,
    OT_ANIM_LAVAHEAD,
    OT_ANIM_FORCEFIELD,
    OT_ANIM_WSAND,
    OT_ANIM_WBRICK,
    OT_ANIM_SWAMP,
    OT_ANM1,
    OT_ANM2,
    OT_ANM3,
    OT_ANM4,
    OT_ANM5,
    OT_ANM6,
    OT_ANM7,
    OT_ANM8,
    OT_ANM9,
    OT_ANM10,
    OT_ANM11,
    OT_ANM12,

    OT_DOLL1,
    OT_DOLL2,
    OT_DOLL3,
    OT_DOLL4,
    OT_DOLL5,
    OT_DOLL6,
    OT_DOLL7,
    OT_DOLL8,
    OT_DOLL9,
    OT_DOLL10,
    OT_DOLL11,
    OT_DOLL12,
    OT_DOLL13,
    OT_DOLL14,
    OT_DOLL15,
    OT_DOLL16,
    OT_DOLL17,
    OT_DOLL18,
    OT_DOLL19,
    OT_DOLL20,
    OT_DOLL21,
    OT_DOLL22,
    OT_DOLL23,

    OT_NMTYPES,
    OT_DEAD = 8196,
};

typedef struct
{
    sint32 pos, vel;
    sint16 connect[4]; /* -1 means no connection */
} WaveVert;

typedef struct
{
    sint16 connect[4];
} WaveFace;

typedef struct
{
    sint16 type;
    sint16 firstParam;
} sObjectType;

typedef struct
{
    sint16 enclosingSector;
    sint16 startWall, endWall; /* indexes into the pbwall array */
    sint16 startVertex, endVertex; /* indexes into the pbvertex array */
    sint16 floorSector; /* a sector's floor that moves with the block,
                          or -1 if none */
    sint16 dx, dy, dz;
} sPBType;

#define PBVFLAG_XLOCK 1
#define PBVFLAG_YLOCK 2
#define PBVFLAG_ZLOCK 4
typedef struct
{
    uint16 vStart;
    uint8 vNm;
    sint8 flags;
} sPBVertex;

typedef struct
{
    sint16 x, y, z;
    sint8 light, pad;
} sVertexType;

typedef struct
{
    uint16 v[4];
    uint8 tile;
    sint8 pad;
} sFaceType;

#define TILESIZE 64

#define WALLFLAG_PARALLELOGRAM 0x01
#define WALLFLAG_INVISIBLE 0x02
#define WALLFLAG_SLIPPERY 0x04
#define WALLFLAG_BLOCKSSIGHT 0x08
#define WALLFLAG_NOTSTEPABLE 0x10
#define WALLFLAG_DOORWALL 0x20
#define WALLFLAG_PARALLAX 0x40
#define WALLFLAG_LAVA 0x80

/* blocking bits */
#define WALLFLAG_BLOCKBITS 0x1f00
#define WALLFLAG_BLOCKED 0x100
#define WALLFLAG_WATERSURFACE 0x200
#define WALLFLAG_WATERBNDRY 0x400
#define WALLFLAG_CLIFFBNDRY 0x800
#define WALLFLAG_SHORTOPENING 0x1000

#define WALLFLAG_SWAMP 0x2000
#define WALLFLAG_COLLIDEASFLOOR 0x4000

#define WALLFLAG_EXPLODABLE 0x8000

typedef struct
{ /* plane equation */
    sint32 normal[3];
    sint32 d;
    void* object;

    sint16 flags;
    /* if RECTANGLE then */
    uint16 textures; /* index into texture array */
    /* else */
    sint16 firstFace, lastFace;
    uint16 firstVertex, lastVertex;
    /* endif */
    uint16 v[4];
    sint16 nextSector; /* or -1 */
    uint16 firstLight;
    sint16 pixelLength;
    uint8 tileLength, tileHeight;
    /* length is number of tiles along v0 -> v1,
       height is number along v1->v2 */
} sWallType;

#define MAXCUTSECTORS 128

#define SECFLAG_WATER 1
#define SECFLAG_CUTSORT 2
#define SECFLAG_SEEN 4
#define SECFLAG_EXPLODINGWALLS 8
#define SECFLAG_LASERSECTOR 16
#define SECFLAG_NOMAP 32

typedef struct
{
    void* object;
    sint16 center[3];
    sint16 floorLevel; /* avg floor level if sloped */
    sint16 firstWall, lastWall;
    sint16 light; /*lighting level (-16 to 16)*/
    sint16 flags;
    sint8 cutIndex, cutChannel;
    sint16 pad;
} sSectorType;

#define TILEFLAG_VDP2 0x01
#define TILEFLAG_64x64 0x02
#define TILEFLAG_32x32 0x04
#define TILEFLAG_8BPP 0x08
#define TILEFLAG_16BPP 0x10
#define TILEFLAG_PALLETE 0x20
#define TILEFLAG_RLE 0x40
struct seqHeader
{
    sint32 nmSequences;
    sint32 nmFrames;
    sint32 nmChunks;
};

/* sequence data */
/*sint16 *sequenceList; declared elsewhere */
/*index into the frame array for start of each sequence*/
/* there is always an extra entry on the end which points to the element past
   the end of the frameArray */

#define FRAMEFLAG_ENDOFSEQUENCE 0x4000
#define FRAMEFLAG_FIRE 0x80

typedef struct
{
    sint16 chunkIndex; /* index into the chunk array for each frame */
    sint16 flags;
    sint16 sound;
    sint8 pad[2];
} sFrameType;

typedef struct
{
    sint16 chunkx, chunky;
    sint16 tile;
    sint8 flags;
    sint8 pad;
} sChunkType;

#endif
