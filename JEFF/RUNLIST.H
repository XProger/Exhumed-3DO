extern short vpaircount;

enum
{
	SIGNAL_MOVE=0,
	SIGNAL_READY,
	SIGNAL_PRESS,
	SIGNAL_CONTACT,
	SIGNAL_SHOT,
	SIGNAL_VIEW,
	SIGNAL_ENTER,
	SIGNAL_EXIT,
	SIGNAL_ON,
	SIGNAL_OFF,
	SIGNAL_ACTIVATE,
	SIGNAL_DEACTIVATE,
	SIGNAL_TRIGGER,
	SIGNAL_DELETE,
	SIGNAL_BURN,
};

#define AI_IDLE	0

enum
{
	PLAYER_TYPE=0,

	ANUBIS_TYPE, 
	BASTET_TYPE, 
	HAWK_TYPE, 
	QUEEN_TYPE, 
	MANTIS_TYPE, MAGMANTIS_TYPE=MANTIS_TYPE,
	MUMMY_TYPE, 
	OMENWASP_TYPE, WASP_TYPE=OMENWASP_TYPE, 
	PIRHANA_TYPE, PIRANHA_TYPE=PIRHANA_TYPE,
	SELKIS_TYPE, 
	SENTRY_TYPE, 
	SET_TYPE, 
	SPIDER_TYPE, 
	SPINYBALL_TYPE,

	ITEM_TYPE,
	TORCH_TYPE, 
	PART_TYPE,
	ONCE_TYPE,

	FIREBALL_TYPE,
	SENTRYBALL_TYPE, 
	COBRABALL_TYPE,
	FLAMEBALL_TYPE,
	ORB_TYPE,
	RABALL_TYPE,
	CLOUD_TYPE,
	METEOR_TYPE,
	SOUL_TYPE,
	REDCOBRA_TYPE,

	OBJ_TYPE,

	XKEY_TYPE,
	BUGKEY_TYPE,
	TIMEKEY_TYPE,
	PLANTKEY_TYPE,
	SHAWL_TYPE,
	FEATHER_TYPE,
	MASK_TYPE,
	PRISM_TYPE, PYRAMID_TYPE=PRISM_TYPE,
	SANDALS_TYPE,
	SCEPTER_TYPE,
	ANKLETS_TYPE,
	HEALTH_TYPE,
	AMMO_TYPE,
	PISTOL_TYPE,
	M60_TYPE,
	GRENADE_TYPE,
	FLAMER_TYPE,
	COBRA_TYPE,
	RING_TYPE,
	MANACLE_TYPE,
	POT1_TYPE, FLAMPOT_TYPE=POT1_TYPE,
	POT2_TYPE, SNKPOT_TYPE=POT2_TYPE,
	FULLAMMO_TYPE,
	FULLHEALTH_TYPE,
	VESSEL1_TYPE,
	VESSEL2_TYPE,
	VESSEL3_TYPE,
	VESSEL4_TYPE,
	VESSEL5_TYPE,
	CAMEL_TYPE,

	WALLHIT_TYPE,
	GUTS1_TYPE,
	GUTS2_TYPE,
	BOOM_TYPE,
	REDBALL_TYPE,
	BLUEBALL_TYPE,
	GRENPOW_TYPE,

	CONTAINFIRST_TYPE,
	CONTAIN1_TYPE=CONTAINFIRST_TYPE,BLOW1_TYPE=CONTAIN1_TYPE,
	CONTAIN2_TYPE,BLOW2_TYPE=CONTAIN2_TYPE,
	CONTAIN3_TYPE,BLOW3_TYPE=CONTAIN3_TYPE,
	CONTAIN4_TYPE,BLOW4_TYPE=CONTAIN4_TYPE,
	CONTAIN5_TYPE,BLOW5_TYPE=CONTAIN5_TYPE,
	CONTAIN6_TYPE,BLOW6_TYPE=CONTAIN6_TYPE,
	CONTAIN7_TYPE,BLOW8_TYPE=CONTAIN7_TYPE,
	CONTAIN8_TYPE,BLOW7_TYPE=CONTAIN8_TYPE,
	CONTAIN9_TYPE,CBANG_TYPE=CONTAIN9_TYPE,
	CONTAIN10_TYPE,DOG_TYPE=CONTAIN10_TYPE,
	CONTAIN11_TYPE,KBANG_TYPE=CONTAIN11_TYPE,
	CONTAIN12_TYPE,MBANG_TYPE=CONTAIN12_TYPE,
	CONTAIN13_TYPE,PBANG_TYPE=CONTAIN13_TYPE,
	CONTAIN14_TYPE,SBANG_TYPE=CONTAIN14_TYPE,
	CONTAIN15_TYPE,TBANG_TYPE=CONTAIN15_TYPE,
	CONTAIN16_TYPE,TOMB1_TYPE=CONTAIN16_TYPE,
	CONTAIN17_TYPE,VAS_TYPE=CONTAIN17_TYPE,
	CONTAINLAST_TYPE=CONTAIN17_TYPE,

	TORCHFIRST_TYPE,
	TORCH1_TYPE=TORCHFIRST_TYPE,BOWL_TYPE=TORCH1_TYPE,
	TORCH2_TYPE,CAMP1_TYPE=TORCH2_TYPE,
	TORCH3_TYPE,CAMP2_TYPE=TORCH3_TYPE,
	TORCH4_TYPE,CAMP3_TYPE=TORCH4_TYPE,
	TORCH5_TYPE,CHAOS1_TYPE=TORCH5_TYPE,
	TORCH6_TYPE,CHAOS2_TYPE=TORCH6_TYPE,
	TORCH7_TYPE,COL1_TYPE=TORCH7_TYPE,
	TORCH8_TYPE,COL2_TYPE=TORCH8_TYPE,
	TORCH9_TYPE,COL3_TYPE=TORCH9_TYPE,
	TORCH10_TYPE,FLAME_TYPE=TORCH10_TYPE,
	TORCH11_TYPE,GLOW_TYPE=TORCH11_TYPE,
	TORCH12_TYPE,HANT1_TYPE=TORCH12_TYPE,
	TORCH13_TYPE,HANT2_TYPE=TORCH13_TYPE,
	TORCH14_TYPE,HANT3_TYPE=TORCH14_TYPE,
	TORCH15_TYPE,HANT4_TYPE=TORCH15_TYPE,
	TORCH16_TYPE,MAGMA1_TYPE=TORCH16_TYPE,
	TORCH17_TYPE,MAGMA2_TYPE=TORCH17_TYPE,
	TORCH18_TYPE,MARSH1_TYPE=TORCH18_TYPE,
	TORCH19_TYPE,MUMY_TYPE=TORCH19_TYPE,
	TORCH20_TYPE,PERIL1_TYPE=TORCH20_TYPE,
	TORCH21_TYPE,QUAR1_TYPE=TORCH21_TYPE,
	TORCH22_TYPE,SELKIS1_TYPE=TORCH22_TYPE,
	TORCH23_TYPE,SELKIS2_TYPE=TORCH23_TYPE,
	TORCH24_TYPE,SET1_TYPE=TORCH24_TYPE,
	TORCH25_TYPE,SET2_TYPE=TORCH25_TYPE,
	TORCH26_TYPE,SET3_TYPE=TORCH26_TYPE,
	TORCH27_TYPE,SET4_TYPE=TORCH27_TYPE,
	TORCH28_TYPE,THOTH1_TYPE=TORCH28_TYPE,
	TORCH29_TYPE,THOTH2_TYPE=TORCH29_TYPE,
	TORCH30_TYPE,THOTH3_TYPE=TORCH30_TYPE,
	TORCH31_TYPE,TOM1_TYPE=TORCH31_TYPE,
	TORCH32_TYPE,TOM2_TYPE=TORCH32_TYPE,
	TORCH33_TYPE,
	TORCH34_TYPE,TOWN1_TYPE=TORCH34_TYPE,
	TORCH35_TYPE,WALTRCH_TYPE=TORCH35_TYPE,
	TORCH36_TYPE,WALTRCH1_TYPE=TORCH36_TYPE,
	TORCH37_TYPE,WALTRCH2_TYPE=TORCH37_TYPE,
	TORCH38_TYPE,WTRLITE_TYPE=TORCH38_TYPE,
	TORCHLAST_TYPE=TORCH38_TYPE,

	PLANT1_TYPE,
	PLANT2_TYPE,
	PLANT3_TYPE,
	PLANT4_TYPE,
	BUBLE1_TYPE,
	FISH_TYPE,
	FALS_TYPE,
	WTRBUBS_TYPE,
	BRANCH1_TYPE,
	GLOW1_TYPE,
	CHOPPER_TYPE,
	XMITTER1_TYPE,
	XMITTER2_TYPE,
	XMITTER3_TYPE,
	XMITTER4_TYPE,
	XMITTER5_TYPE,
	XMITTER6_TYPE,
	XMITTER7_TYPE,
	XMITTER8_TYPE,

	DOOR_TYPE,
	LIFT_TYPE,
	SWITCH_TYPE,
	SHOOTER_TYPE,

	MAPEYE_TYPE,
	TITLE_TYPE,

	BLOCK_TYPE,

	MAX_TYPES 
};

#define MAXOBJECTS 300

typedef int (*messHandler)(short object,short message,
                            short param1,short param2);
typedef struct
{
	short type;
	short index;
  short next;  
  short prev;  
	messHandler func;
} runType;  

extern runType objectChain[];
#define runList 0
#define	idleList 1
#define	freeList 2

void initObjects();
short getFreeObject(short type, short index, messHandler func);
void moveObject(short object, short destList);
int signalObject(short object, short message, short param1, short param2);
void getObjectType(short object, short *type, short *index);
short getNextObject(short o);
void freeObject(short object);
void deleteObject(short o);
void freeDeletedObjects();
void channelSignal(int signal, short channel);
void safeMoveObject(short o, short d);
void moveObjects();

typedef struct
{
	short sprite;		//sprite index
	short enemy;		//
	short state;
	short health;
	short burntime;
	short burnz[4];
	signed char burnx[4];
}	creaturetype;

typedef struct
{
	short data[12];
} gentype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	long dz;
}	fireballtype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	long dz;
}	sentryballtype;

typedef struct
{
	short sprite[7];		//sprite index
	short owner;		//sprite owner
	short state;
	short pitch;
	short target;
}	cloudtype;

//typedef struct
//{
//	short sprite;		//sprite index
//	short owner;		//sprite owner
//	short state;
//	short target;
//}	cloudtype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	long dz;
	short time;
}	flameballtype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	short reps;
}	orbtype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	long	dz;
}	meteortype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	long	dz;
}	soultype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	short bounces;
	short pitch;
}	raballtype;

typedef struct
{
	short sprite;		//sprite index
	short owner;		//sprite owner
	short state;
	short head;
	long dz;
	short tail1;
	short tail2;
	short tail3;
	short tail4;
	short tail5;
	short time;
}	cobraballtype;

typedef struct
{
	short sprite;		//sprite index
	short type;
}	parttype;

typedef struct
{
	short sprite;		//sprite index
	short type;
	short delay;
	short r,g,b;
	short sound;
}	oncetype;

typedef struct
{
	short sprite;		//sprite index
	short type;
	short delay;
	short health;
}	itemtype;

typedef struct
{
	short sprite;		//sprite index
	short type;
}	torchtype;

typedef struct
{
	short type;
	short subtype;
	short s;
	short z;
	short	dz;
	short maxz;
	short minz;
	short time;
	short firstvertex;
	short lastvertex;
	short channel;
	short done;
} objtype;

extern gentype creatureList[];
extern short creaturecount;

typedef struct
{
	short sprite;		//sprite index
	short enemy;		//
	short state;
	short health;
	long speed;
	long turnspeed;
	short vang;
	short xang;
	short yang;
	short kneedz;
	long bobtime;
	long landtime;
	long weaponbobtime;
	long swimtime;
	short weaponx;
	short weapony;
	long levitatetime;
	short currentgun;
	short gunsequence;
	short	gunframe;
	short	gunclock;
	short	grenadetime;
	short	raballside;
	short	newgun;
	short ammo[10];
	short shawlactive;
	short featheractive;
	short vessels;
	short shakedz;
	int newpitch:1;
	int resetpitch:1;
	int key1:1;
	int key2:1;
	int key3:1;
	int key4:1;
	int hassandals:1;
	int hasshawl:1;
	int hasmask:1;
	int hasscarab:1;
	int haseye:1;
	int hasfeather:1;
	int hasvessel1:1;
	int hasvessel2:1;
	int hasvessel3:1;
	int hasvessel4:1;
	int hasvessel5:1;
}	playertype;

extern playertype playerList[];
extern short playercount;

void initSectorObjects();
extern int obj_func(short object, short message, short param1, short param2);
