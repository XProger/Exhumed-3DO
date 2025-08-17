#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#define FORCERECT 0
#define ELEVATOR 1

#include "..\slevel.h"

#include "dexshell.h"

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

	TEAMDOLL1_TYPE,
	TEAMDOLL2_TYPE,
	TEAMDOLL3_TYPE,
	TEAMDOLL4_TYPE,
	TEAMDOLL5_TYPE,
	TEAMDOLL6_TYPE,
	TEAMDOLL7_TYPE,
	TEAMDOLL8_TYPE,
	TEAMDOLL9_TYPE,
	TEAMDOLL10_TYPE,
	TEAMDOLL11_TYPE,
	TEAMDOLL12_TYPE,
	TEAMDOLL13_TYPE,
	TEAMDOLL14_TYPE,
	TEAMDOLL15_TYPE,
	TEAMDOLL16_TYPE,
	TEAMDOLL17_TYPE,
	TEAMDOLL18_TYPE,
	TEAMDOLL19_TYPE,
	TEAMDOLL20_TYPE,
	TEAMDOLL21_TYPE,
	TEAMDOLL22_TYPE,
	TEAMDOLL23_TYPE,
	MAX_TYPES
};

void addPB(void);
void makeDoorWay(int s,int type);
void makeElevator(int s);
void makeBobbingBlock(int s);
int addTile(char *filename,char flags);

typedef
struct {double x,y,z;} fVert;

#define MAXNMVERT 64000
#define MAXNMFACES 32000
#define MAXNMTEXTURES 64000
#define MAXNMLIGHT 64000
#define MAXNMWALLS 8000
#define MAXNMSECTORS 600
#define MAXNMOBJECTS 500
#define MAXNMSEQ 800
#define MAXNMFRAMES 3000
#define MAXNMCHUNKS 9000
#define MAXNMOBJECTPARAMS 100000

FILE *ofile;

typedef struct
{int x,y,z;
 char light;
} cVertexType;

static int levelHasWater=0;
static int levelHasExplodeWalls=0;
static int ram_triggerSector=-1;
static int ram_homeSector=-1;

#define MAXINVERT 150000
int vertexMap[MAXINVERT];
short vertexLight[MAXINVERT];
cVertexType outVert[MAXNMVERT];
int nmVertex;
sFaceType outFace[MAXNMFACES];
int nmFaces;
char outTexture[MAXNMTEXTURES];
int nmTextures;
unsigned char outLight[MAXNMLIGHT];
int nmLight;
sWallType outWalls[MAXNMWALLS];
int nmWalls;
sSectorType outSector[MAXNMSECTORS];
int nmSectors;
unsigned char outCutPlane[MAXNMSECTORS][MAXCUTSECTORS];
int nmCutSectors=0;
sObjectType outObjects[MAXNMOBJECTS];
int nmObjects;
unsigned char outObjectParams[MAXNMOBJECTPARAMS];
int nmObjectParams;

WaveVert outWaveVert[MAXNMVERT];
int nmWaveVert;
WaveFace outWaveFace[MAXNMWALLS];
int nmWaveFace;


short outSequenceMap[OT_NMTYPES];
short outSoundMap[OT_NMTYPES];

struct
{int isPB,isInternal;
 int floorClearance,thickness;
} pbList[MAXNMSECTORS];

sPBType outPBs[MAXNMSECTORS];
int nmPBs;
sPBVertex outPBVertex[MAXNMVERT];
int nmPBVert;
short outPBWalls[MAXNMWALLS];
int nmPBWalls;

void object_addObject(int type);
void object_addChar(int s);
void object_addShort(int s);
void object_addInt(int s);

short outSequenceList[MAXNMSEQ];
int nmSequences;
sFrameType outFrames[MAXNMFRAMES];
int nmFrames;
sChunkType outChunk[MAXNMCHUNKS];
int nmChunks;

char levelName[160];

int nmNonRect;
int nmInvisible;
int tilesWrote=0;

typedef struct
{int x,y,z;} MthXyz;

#define MAXNMSUBTILES 32

typedef struct
{char filename[160];
 int subx[MAXNMSUBTILES],suby[MAXNMSUBTILES];
 int subIs32x32[MAXNMSUBTILES];
 int nmSubTiles;
 int uses;
 int width,height;
 int number,palNm;
 int value;
 char flags;
} TileInfo;

#define MAXNMTILES 900
TileInfo tiles[MAXNMTILES];
int nmTiles=0;

FILE *ofile;

void writeShort(short i)
{fputc(i >> 8,ofile);
 fputc(i & 0xff,ofile);
}

void writeInt(int i)
{fputc(i >> 24,ofile);
 fputc((i >> 16)&0xff,ofile);
 fputc((i >> 8)&0xff,ofile);
 fputc(i & 0xff,ofile);
}

void writeChar(char i)
{fputc(i,ofile);
}

void findFaceCenter(int i,int *xo,int *yo,int *zo)
{int x,y,z;
 x=0; y=0; z=0;
 x+=vertex[face[i].v0].x;
 y+=vertex[face[i].v0].y;
 z+=vertex[face[i].v0].z;
 x+=vertex[face[i].v1].x;
 y+=vertex[face[i].v1].y;
 z+=vertex[face[i].v1].z;
 x+=vertex[face[i].v2].x;
 y+=vertex[face[i].v2].y;
 z+=vertex[face[i].v2].z;
 x+=vertex[face[i].v3].x;
 y+=vertex[face[i].v3].y;
 z+=vertex[face[i].v3].z;
 *xo=x>>3;
 *yo=z>>3;
 *zo=y>>3;
}

int readLight(FILE *ifile)
{double g;
 int i;
 fscanf(ifile,"%lf\n",&g);
 i=g*128.0;
 if (i>127)
    i=127;
 if (i<0)
    i=0;
 return i;
}

#if 0
void loadLightMap(char *filename)
{FILE *ifile;
 int i,s,w,f;
 ifile=fopen(filename,"r");
 if (!ifile)
    {/* light map does not exist, use random light */
     printf("Light map %s does not exist, using random light\n",filename);
     for (i=0;i<vertexcount;i++)
	vertexLight[i]=outLight[i]=80;/*rand()&0x7f*/;
     return;
    }
 printf("Reading light info from %s\n",filename);
 for (s=0;s<sectorcount;s++)
    for (w=sector[s].firstwall;w<=sector[s].lastwall+2;w++)
       {if (wall[w].firstface==-1)
	   continue;
	for (f=wall[w].firstface;f<=wall[w].lastface;f++)
	   {vertexLight[face[f].v0]=readLight(ifile);
	    vertexLight[face[f].v1]=readLight(ifile);
	    vertexLight[face[f].v2]=readLight(ifile);
	    vertexLight[face[f].v3]=readLight(ifile);
	   }
       }
}
#else
#define GUESSTILEVAL 28L

#define MAXNMMONSTERS 300
struct jeffMonsterRec
{short type,x,y,z,s,a;
} jeffMonster[MAXNMMONSTERS];
short nmJeffMonsters;


struct
{short jeff,me;
} jeffMonsterMap[]=
{
 {PLAYER_TYPE,OT_PLAYER},
 {ANUBIS_TYPE,OT_ANUBIS},
 {BASTET_TYPE,OT_BASTET},
 {HAWK_TYPE,OT_HAWK},
 {QUEEN_TYPE,OT_QUEEN},
 {MANTIS_TYPE,OT_MAGMANTIS},
 {MUMMY_TYPE,OT_MUMMY},
 {OMENWASP_TYPE,OT_WASP},
 {PIRHANA_TYPE,OT_FISH},
 {SELKIS_TYPE,OT_SELKIS},
 {SENTRY_TYPE,OT_SENTRY},
 {SET_TYPE,OT_SET},
 {SPIDER_TYPE,OT_SPIDER},
 {SPINYBALL_TYPE,OT_BLOB},

 {XKEY_TYPE,OT_XKEY},
 {BUGKEY_TYPE,OT_BUGKEY},
 {TIMEKEY_TYPE,OT_TIMEKEY},
 {PLANTKEY_TYPE,OT_PLANTKEY},
 {SHAWL_TYPE,OT_CAPE},
 {FEATHER_TYPE,OT_FEATHER},
 {MASK_TYPE,OT_MASK},
 {SANDALS_TYPE,OT_SANDALS},
 {SCEPTER_TYPE,OT_SCEPTER},
 {ANKLETS_TYPE,OT_ANKLETS},
 {HEALTH_TYPE,OT_HEALTHBALL},
 {AMMO_TYPE,OT_AMMOBALL},
 {PISTOL_TYPE,OT_PISTOL},
 {M60_TYPE,OT_M60},
 {GRENADE_TYPE,OT_GRENADEAMMO},
 {FLAMER_TYPE,OT_FLAMER},
 {COBRA_TYPE,OT_COBRASTAFF},
 {RING_TYPE,OT_RING},
 {MANACLE_TYPE,OT_MANACLE},
 {FULLAMMO_TYPE,OT_AMMOSPHERE},
 {FULLHEALTH_TYPE,OT_HEALTHSPHERE},
 {CAMEL_TYPE,OT_CAMEL},

 {TORCH1_TYPE,OT_TORCH1},
 {TORCH2_TYPE,OT_TORCH2},
 {TORCH3_TYPE,OT_TORCH3},
 {TORCH4_TYPE,OT_TORCH4},
 {TORCH5_TYPE,OT_TORCH5},
 {TORCH6_TYPE,OT_TORCH6},
 {TORCH7_TYPE,OT_TORCH7},
 {TORCH8_TYPE,OT_TORCH8},
 {TORCH9_TYPE,OT_TORCH9},
 {TORCH10_TYPE,OT_TORCH10},
 {TORCH11_TYPE,OT_TORCH11},
 {TORCH12_TYPE,OT_TORCH12},
 {TORCH13_TYPE,OT_TORCH13},
 {TORCH14_TYPE,OT_TORCH14},
 {TORCH15_TYPE,OT_TORCH15},
 {TORCH16_TYPE,OT_TORCH16},
 {TORCH17_TYPE,OT_TORCH17},
 {TORCH18_TYPE,OT_TORCH18},
 {TORCH20_TYPE,OT_TORCH20},
 {TORCH21_TYPE,OT_TORCH21},
 {TORCH22_TYPE,OT_TORCH22},
 {TORCH23_TYPE,OT_TORCH23},
 {TORCH24_TYPE,OT_TORCH24},
 {TORCH25_TYPE,OT_TORCH25},
 {TORCH26_TYPE,OT_TORCH26},
 {TORCH27_TYPE,OT_TORCH27},
 {TORCH28_TYPE,OT_TORCH28},
 {TORCH29_TYPE,OT_TORCH29},
 {TORCH30_TYPE,OT_TORCH30},
 {TORCH31_TYPE,OT_TORCH31},
 {TORCH32_TYPE,OT_TORCH32},
 {TORCH33_TYPE,OT_TORCH33},
 {TORCH34_TYPE,OT_TORCH34},
 {TORCH35_TYPE,OT_TORCH35},
 {TORCH36_TYPE,OT_TORCH36},
 {TORCH37_TYPE,OT_TORCH37},
 {TORCH38_TYPE,OT_TORCH38},

 {TOMB1_TYPE,OT_CONTAIN1},
 {TBANG_TYPE,OT_CONTAIN2},
 {BLOW3_TYPE,OT_CONTAIN3},
 {BLOW8_TYPE,OT_CONTAIN4},
 {SBANG_TYPE,OT_CONTAIN5},
 {BLOW1_TYPE,OT_CONTAIN6},
 {BLOW7_TYPE,OT_CONTAIN7},
 {VAS_TYPE,OT_CONTAIN8},
 {PBANG_TYPE,OT_CONTAIN9},
 {BLOW4_TYPE,OT_CONTAIN10},
 {MBANG_TYPE,OT_CONTAIN11},
 {BLOW5_TYPE,OT_CONTAIN12},
 {DOG_TYPE,OT_CONTAIN13},
 {BLOW6_TYPE,OT_CONTAIN14},
 {BLOW2_TYPE,OT_CONTAIN15},
 {KBANG_TYPE,OT_CONTAIN16},
 {CBANG_TYPE,OT_CONTAIN17},
 {POT1_TYPE,OT_BOOMPOT1},
 {POT2_TYPE,OT_BOOMPOT2},
 {PYRAMID_TYPE,OT_PYRAMID},
 {VESSEL1_TYPE,OT_BLOODBOWL},
 {VESSEL2_TYPE,OT_BLOODBOWL},
 {VESSEL3_TYPE,OT_BLOODBOWL},
 {VESSEL4_TYPE,OT_BLOODBOWL},
 {VESSEL5_TYPE,OT_BLOODBOWL},
 {CHOPPER_TYPE,OT_CHOPPER},
 {XMITTER1_TYPE,OT_COMM_BATTERY},
 {XMITTER2_TYPE,OT_COMM_BOTTOM},
 {XMITTER3_TYPE,OT_COMM_DISH},
 {XMITTER4_TYPE,OT_COMM_HEAD},
 {XMITTER5_TYPE,OT_COMM_KEYBOARD},
 {XMITTER6_TYPE,OT_COMM_MOUSE},
 {XMITTER7_TYPE,OT_COMM_SCREEN},
 {XMITTER8_TYPE,OT_COMM_TOP},
 {MUMY_TYPE,OT_RAMMUMMY},
 {TEAMDOLL1_TYPE,OT_DOLL1},
 {TEAMDOLL2_TYPE,OT_DOLL2},
 {TEAMDOLL3_TYPE,OT_DOLL3},
 {TEAMDOLL4_TYPE,OT_DOLL4},
 {TEAMDOLL5_TYPE,OT_DOLL5},
 {TEAMDOLL6_TYPE,OT_DOLL6},
 {TEAMDOLL7_TYPE,OT_DOLL7},
 {TEAMDOLL8_TYPE,OT_DOLL8},
 {TEAMDOLL9_TYPE,OT_DOLL9},
 {TEAMDOLL10_TYPE,OT_DOLL10},
 {TEAMDOLL11_TYPE,OT_DOLL11},
 {TEAMDOLL12_TYPE,OT_DOLL12},
 {TEAMDOLL13_TYPE,OT_DOLL13},
 {TEAMDOLL14_TYPE,OT_DOLL14},
 {TEAMDOLL15_TYPE,OT_DOLL15},
 {TEAMDOLL16_TYPE,OT_DOLL16},
 {TEAMDOLL17_TYPE,OT_DOLL17},
 {TEAMDOLL18_TYPE,OT_DOLL18},
 {TEAMDOLL19_TYPE,OT_DOLL19},
 {TEAMDOLL20_TYPE,OT_DOLL20},
 {TEAMDOLL21_TYPE,OT_DOLL21},
 {TEAMDOLL22_TYPE,OT_DOLL22},
 {TEAMDOLL23_TYPE,OT_DOLL23},
 {-1,-1}};

void loadLightMap(char *filename)
{FILE *ifile;
 char buff[80];
 unsigned short light;
 int v,i;
 nmJeffMonsters=0;
 ifile=fopen(filename,"rb");
 if (!ifile)
    {/* light map does not exist, use random light */
     printf("Light map %s does not exist, using random light\n",filename);
     for (i=0;i<vertexcount;i++)
	vertexLight[i]=outLight[i]=80;/*rand()&0x7f; */
     return;
    }
 printf("Reading light & monster info from %s\n",filename);
 fread(&nmJeffMonsters,2,1,ifile);
 printf("%d monsters\n",nmJeffMonsters);
 fread(jeffMonster,12,nmJeffMonsters,ifile);
 fread(buff,8,1,ifile);
 fread(&light,2,1,ifile);
 printf("%d vertices\n",light);
 for (v=0;v<vertexcount;v++)
    {/*int psxtileval;
       int sat; */
     fread(&light,2,1,ifile);
     if (light>128)
	light=128;
     /* psxtileval=(light*GUESSTILEVAL)/128;
	sat=psxtileval-GUESSTILEVAL+16;
	if (sat<0) sat=0;
	if (sat>31) sat=31;
	vertexLight[v]=sat<<3;*/
     vertexLight[v]=light;
    }
}
#endif

#define MAXNMTILEMAPS 127
char wallTileMap[MAXNMTILEMAPS][80];
enum {WT_NORM,WT_WATER,WT_PLAX,WT_LAVA,WT_SWAMP,WT_SHOOTER1,WT_SHOOTER2,
	 WT_SHOOTER3,WT_SW1,WT_SW2,WT_SW3,WT_SW4,WT_FORCE,
	 WT_NOTEXPLODABLE,WT_NMTYPES};
int wallTileType[MAXNMTILEMAPS];
int brewTileToOurTile[MAXNMTILEMAPS];

int nmWallTileMaps;

char *translateWallTileNm(int n)
{static int yell=0;
 assert(n>=0);
 if (n>=nmWallTileMaps)
    {if (!yell)
	{printf("Warning: wall tile out of range\n");
	 yell=1;
	}
     return wallTileMap[0];
    }
 return wallTileMap[n];
}

void loadWallTileMap(char *filename)
{FILE *ifile;
 char *c;
 short nmMapEntries;
 int i;
 int nmUsed;
 int used[MAXNMTILEMAPS],useCount[MAXNMTILEMAPS];
 char buff[160];
 nmUsed=0;
 for (i=0;i<MAXNMTILEMAPS;i++)
    {used[i]=0;
     useCount[i]=0;
    }
 {int s,w,f;
  for (s=0;s<sectorcount;s++)
     for (w=sector[s].firstwall;w<=sector[s].lastwall+2;w++)
	{if (wall[w].firstface<0)
	    continue;
	 for (f=wall[w].firstface;f<=wall[w].lastface;f++)
	    {assert(face[f].tile>=0);
	     assert(face[f].tile<MAXNMTILEMAPS);
	     if (!used[face[f].tile])
		{used[face[f].tile]=s+1;
		 nmUsed++;
		}
	    }
	}
  /* just a check */
  for (i=0;i<facecount;i++)
     {assert(used[face[i].tile]);
      useCount[face[i].tile]++;
     }
 }

 ifile=fopen(filename,"rb");
 if (!ifile)
    {printf("Can't read tile map file %s.\n",filename);
     printf("Trying default.\n");
     ifile=fopen("default.til","rb");
     if (!ifile)
	{printf("Can't read default tile file.\n");
	 exit(-1);
	}
    }
 fread(buff,2,1,ifile);
 if (buff[0]!='B' || buff[1]!='R')
    {printf("Tile map file bad!\n");
     exit(-1);
    }
 fread(buff,2,1,ifile);
 fread(&nmMapEntries,2,1,ifile);
 assert(nmMapEntries<MAXNMTILEMAPS);
 for (i=0;i<nmMapEntries;i++)
    {fgets(wallTileMap[i],80,ifile);
     for (c=wallTileMap[i];*c && *c!='\r' && *c!='\n';c++) ;
     if (*c)
	*c=0;
     {char *(*triggerTiles[])[]=
	 {&((char *[])
	    {
	     NULL}),
	  &((char *[])
	    {
	     "1default\\def5_s.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "1default\\def2_c.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "animated\\lava\\k17.bmp",
	     "animated\\lava\\k20.bmp",
	     "animated\\lava\\k23.bmp",
	     "animated\\lava\\m1.bmp",
	     "animated\\lava\\m5.bmp",
	     "animated\\lava\\m10.bmp",
	     "animated\\lava\\m22.bmp",
	     "animated\\lava\\m16.bmp",
	     "animated\\lava\\m19.bmp",
	     "animated\\lava\\m13.bmp",
	     "animated\\lava\\lavfal1.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "stages\\marsh\\grnrk.bmp",
	     "animated\\swamp\\mrsh23.bmp",
	     "animated\\swamp\\goop1.bmp",
	     NULL}),

	  /* shooters */
	  &((char *[])
	    {
	     "stages\\camp\\camshoot.bmp",
	     "stages\\sanctuar\\sancshot.bmp",
	     "stages\\pctiles\\shoot17.bmp",
	     "stages\\magma\\magshoot.bmp",
	     "stages\\marsh\\shoot7.bmp",
	     "stages\\selkis\\shoot6.bmp",
	     "stages\\peril\\cavshoot.bmp",
	     "stages\\thoth\\t24.bmp",
	     "stages\\set\\s3.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "stages\\camp\\camrock.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "stages\\karnak\\shoot20.bmp",
	     "stages\\thoth\\thoshoot.bmp",
	     "stages\\canyons\\canshoot.bmp",
	     NULL}),

	  /* switches */
	  &((char *[])
	    {
	     "switches\\0280.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "switches\\0282.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "switches\\0285.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "switches\\0295.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "animated\\fcefield\\force1a.bmp",
	     NULL}),
	  &((char *[])
	    {
	     "stages\\pctiles\\capt0049.bmp",
	     NULL})
	    };
      int type,j;
      wallTileType[i]=WT_NORM;
      for (type=WT_NORM;type<WT_NMTYPES;type++)
	 {for (j=0;(*(triggerTiles[type]))[j];j++)
	     {if (!stricmp(wallTileMap[i],(*(triggerTiles[type]))[j]))
		 {wallTileType[i]=type;
		  printf("Special!\n");
		 }
	     }
	 }
     }
    }
 nmWallTileMaps=nmMapEntries;
 fclose(ifile);
 for (i=0;i<nmWallTileMaps;i++)
    if (used[i] && wallTileType[i]!=WT_PLAX && wallTileType[i]!=WT_WATER)
       brewTileToOurTile[i]=
	  addTile(translateWallTileNm(i),
		  TILEFLAG_64x64|TILEFLAG_16BPP|TILEFLAG_PALLETE);
 for (i=0;i<nmWallTileMaps;i++)
    if (used[i])
       printf("Tile %s   1st use:%d  # use:%d\n",translateWallTileNm(i),
	      used[i]-1,useCount[i]);
 printf("%d tiles used of %d in map.\n",nmUsed,nmWallTileMaps);
}


#define MAXNMPAL 50
unsigned short palletes[MAXNMPAL][256];
int nmPals=0;

unsigned char picdata[512][512];
unsigned short picPal[256];
int picWidth,picHeight;

int mapPal(void)
{int p;
 for (p=0;p<nmPals;p++)
    if (!memcmp(picPal,palletes[p],512))
       break;
 if (p<nmPals)
    return p;
 memcpy(palletes[nmPals],picPal,512);
 nmPals++;
 return nmPals-1;
}

int swap;
void readBmp(char *filename)
{FILE *ifile;
 char buff[160];
 int width,height,i,j;
 {char *c=filename+strlen(filename);
  c--;
  while (*c=='\n' || *c=='\r') c--;
  c++;
  *c=0;
 }
 ifile=fopen(filename,"rb");
 if (!ifile)
    {printf("Error: can't read file %s.\n",filename);
     exit(-1);
    }
 fread(buff,14+4,1,ifile);
 fread(&width,4,1,ifile);
 fread(&height,4,1,ifile);
 picWidth=width;
 picHeight=height;
 fseek(ifile,14+40,0);
 if (picWidth>512 || picHeight>512)
    {printf("Pic toooo big\n");
     exit(-1);
    }
 /* read pallete */
 for (i=0;i<256;i++)
    {unsigned char r,g,b,foo;
     fread(&b,1,1,ifile);
     fread(&g,1,1,ifile);
     fread(&r,1,1,ifile);
     fread(&foo,1,1,ifile);
     b=b>>3;
     g=g>>3;
     r=r>>3;
     picPal[i]=0x8000|(b<<10)|(g<<5)|r;
    }

/* if (swap)
    {picPal[255]=0x8000;
     picPal[0]=0x0000;
    } */
 if (swap)
    {picPal[255]=picPal[0];
     picPal[0]=0x0000;
    }

 memset(picdata,255,sizeof(picdata));

 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth+3)&(~3),1,ifile);
     for (j=picWidth;j<=(picWidth+3)&(~3);j++)
	picdata[i][j]=255;
    }
 fclose(ifile);
}


int getPicLen(void)
{return 2+2+2+picWidth*picHeight;
}

void writePic(int flags)
{int x,y;
 writeShort(picWidth);
 writeShort(picHeight);
 writeShort(flags);
 for (y=0;y<picHeight;y++)
    for (x=0;x<picWidth;x++)
       {if (picdata[y][x]==0)
	   {writeChar(255);
	    continue;
	   }
	if (picdata[y][x]==255)
	   {writeChar(0);
	    continue;
	   }
	writeChar(picdata[y][x]);
       }
}

int doStupidDecomp(TileInfo *tile)
{int w,h,n,x,y,notEmpty;
 char buff[80];
 strcpy(buff,"tiles\\");
 strcat(buff,tile->filename);
 readBmp(buff);
 n=0;
 for (h=0;h<picHeight;h+=64)
    for (w=0;w<picWidth;w+=64)
       {notEmpty=0;
	for (y=0;y<64;y++)
	   for (x=0;x<64;x++)
	      if (picdata[h+y][w+x]!=255)
		 notEmpty=1;
	if (!notEmpty)
	   continue;
	tile->subx[n]=w;
	tile->suby[n]=h;
	n++;
	assert(n<MAXNMSUBTILES);
       }
 if (n<=0)
    {n=1;
     tile->subx[0]=0;
     tile->suby[0]=0;
    }
 tile->nmSubTiles=n;
 return n;
}


void clearTileWithRim(int sx,int sy,int width,int height)
{int x,y;
 for (y=sy+1;y<sy+height-1;y++)
    for (x=sx+1;x<sx+width-1;x++)
       picdata[y][x]=255;

 for (x=sx;x<sx+width;x++)
    {if (sy-1<0 || picdata[sy-1][x]==255)
	picdata[sy][x]=255;
     if (sy+height>=picHeight || picdata[sy+height][x]==255)
	picdata[sy+height-1][x]=255;
    }

 for (y=sy;y<sy+height;y++)
    {if (sx-1<0 || picdata[y][sx-1]==255)
	picdata[y][sx]=255;
     if (sx+width>=picWidth || picdata[y][sx+width]==255)
	picdata[y][sx+width-1]=255;
    }

 /* do corners */
 {int ok,xc,yc;

  /* tl */
  ok=1;
  xc=sx; yc=sy;
  for (y=yc-1;y<yc+1;y++)
     for (x=xc-1;x<xc+1;x++)
	{if (y<0 || x<0 || y>=picHeight || x>=picWidth)
	    continue;
	 if (y==yc && x==xc)
	    continue;
	 if (picdata[y][x]!=255)
	    ok=0;
	}
  if (ok)
     picdata[yc][xc]=255;

  /* br */
  ok=1;
  xc=sx+picWidth-1; yc=sy+picHeight-1;
  for (y=yc-1;y<yc+1;y++)
     for (x=xc-1;x<xc+1;x++)
	{if (y<0 || x<0 || y>=picHeight || x>=picWidth)
	    continue;
	 if (y==yc && x==xc)
	    continue;
	 if (picdata[y][x]!=255)
	    ok=0;
	}
  if (ok)
     picdata[yc][xc]=255;

  /* tr */
  ok=1;
  xc=sx+picWidth-1; yc=sy;
  for (y=yc-1;y<yc+1;y++)
     for (x=xc-1;x<xc+1;x++)
	{if (y<0 || x<0 || y>=picHeight || x>=picWidth)
	    continue;
	 if (y==yc && x==xc)
	    continue;
	 if (picdata[y][x]!=255)
	    ok=0;
	}
  if (ok)
     picdata[yc][xc]=255;

  /* bl */
  ok=1;
  xc=sx; yc=sy+picHeight-1;
  for (y=yc-1;y<yc+1;y++)
     for (x=xc-1;x<xc+1;x++)
	{if (y<0 || x<0 || y>=picHeight || x>=picWidth)
	    continue;
	 if (y==yc && x==xc)
	    continue;
	 if (picdata[y][x]!=255)
	    ok=0;
	}
  if (ok)
     picdata[yc][xc]=255;
 }
}

int doSmartDecomp(TileInfo *tile,int tweek,int bestSoFar)
{int x,y,n,y1;
 int first;
 char buff[80];
 strcpy(buff,"tiles\\");
 strcat(buff,tile->filename);
 readBmp(buff);
 n=0;
 first=1;
 for (y=0;y<picHeight;y++)
    for (x=0;x<picWidth;x++)
       if (picdata[y][x]!=255)
	  {
	   /* drag back */
	   if (x>picWidth-64)
	      x=picWidth-64;

	   while (x>0)
	      {for (y1=0;y1<64;y1++)
		  if (picdata[y1+y][x+63]!=255)
		     break;
	       if (y1<64)
		  break;
	       else
		  x--;
	      }
	   if (tweek && first)
	      {x=0;
	       first=0;
	      }

	   tile->subx[n]=x;
	   tile->suby[n]=y;

	   n++;
	   if (n>=bestSoFar)
	      {return 1000;
	      }
	   assert(n<MAXNMSUBTILES);
	   clearTileWithRim(x,y,64,64);
	  }
 if (!strcmp("2790.bmp",tile->filename))
    {int i;
     printf("Tile: %s\n",tile->filename);
     for (i=0;i<n;i++)
	printf(" (%d,%d)\n",tile->subx[i],tile->suby[i]);
    }
 tile->nmSubTiles=n;
 return n;
}

#if 0
void mark32x32Tiles(TileInfo *tile)
{int i;
 int minx,miny,maxx,maxy,x,y,x1,y1;
 char buff[80];
 strcpy(buff,"tiles\\");
 strcat(buff,tile->filename);
 readBmp(buff);
 for (i=0;i<tile->nmSubTiles;i++)
    {minx=100000;
     miny=100000;
     maxx=-100000;
     maxy=-100000;
     for (y=tile->suby[i];y<tile->suby[i]+64;y++)
	for (x=tile->subx[i];x<tile->subx[i]+64;x++)
	   if (picdata[y][x]!=255)
	      {if (x<minx)
		  minx=x;
	       if (x>maxx)
		  maxx=x;
	       if (y<miny)
		  miny=y;
	       if (y>maxy)
		  maxy=y;
	      }
     assert(minx!=100000);
     if (maxx<minx+32 && maxy<miny+32)
	printf("Tile %s!!!\n",tile->filename);
    }
}
#endif

void computeSubTiles(TileInfo *tile)
{int best,bestMethod,i;
 bestMethod=0;
 best=doStupidDecomp(tile);
 if (tile->width<=64 && tile->height<=64)
    {/* mark32x32Tiles(tile); */
     return;
    }
 i=doSmartDecomp(tile,0,best);
 if (i<best)
    {printf("Tile:%s %d/%d\n",tile->filename,best,i);
     best=i;
     bestMethod=1;
    }
 i=doSmartDecomp(tile,1,best);
 if (i<best)
    {printf("TIle:%s %d/%d\n",tile->filename,best,i);
     best=i;
     bestMethod=2;
    }
 switch (bestMethod)
    {case 0: doStupidDecomp(tile); break;
     case 1: doSmartDecomp(tile,0,1000); break;
     case 2: doSmartDecomp(tile,1,1000); break;
    }
/* mark32x32Tiles(tile); */
}

int computeBmpValue(void)
{int x,y;
 int r,g,b,max;
 int accum=0;
 for (y=0;y<picHeight;y++)
    for (x=0;x<picWidth;x++)
       {r=(picPal[picdata[y][x]])&0x1f;
	g=(picPal[picdata[y][x]]>>5)&0x1f;
	b=(picPal[picdata[y][x]]>>10)&0x1f;
	max=r;
	if (g>max)
	   max=g;
	if (b>max)
	   max=b;
	accum+=max*3;
       }
 return (accum/3)/(picHeight*picWidth);
}

int tileForce8BPP=0;
int addTile(char *filename,char flags)
{int i;
 for (i=0;i<nmTiles;i++)
    if (!stricmp(filename,tiles[i].filename))
       break;
 if (i<nmTiles)
    {tiles[i].uses++;
     tiles[i].flags|=flags;
     if (tiles[i].flags & TILEFLAG_16BPP)
	tiles[i].flags&=~TILEFLAG_8BPP;
     if (tiles[i].flags & TILEFLAG_64x64)
	if (tiles[i].flags & TILEFLAG_32x32)
	   {printf("BARF on tile %s\n",filename);
	   }
     return i;
    }
 printf("adding tile '%s'\n",filename);
 strcpy(tiles[nmTiles].filename,filename);
 tiles[nmTiles].uses=1;
 tiles[nmTiles].flags=flags;
 {char buff[160];
  strcpy(buff,"tiles\\");
  strcat(buff,filename);
  readBmp(buff);
 }
 tiles[nmTiles].width=picWidth;
 tiles[nmTiles].height=picHeight;
 tiles[nmTiles].number=0;
 tiles[nmTiles].palNm=mapPal();
 if (tiles[nmTiles].palNm!=0)
    {tiles[nmTiles].flags&=~TILEFLAG_8BPP;
     tiles[nmTiles].flags|=TILEFLAG_16BPP;
    }
 if (tileForce8BPP)
    {tiles[nmTiles].flags&=~TILEFLAG_16BPP;
     tiles[nmTiles].flags|=TILEFLAG_8BPP;
    }
 if (flags & TILEFLAG_16BPP)
    {tiles[nmTiles].value=computeBmpValue();
    }
 else
    tiles[nmTiles].value=0;

 /* add sub tile info */
 computeSubTiles(tiles+nmTiles);
 if (nmTiles>0)
    tiles[nmTiles].number=tiles[nmTiles-1].number+tiles[nmTiles-1].nmSubTiles;
 else
    tiles[nmTiles].number=0;

 if (tiles[nmTiles].flags & TILEFLAG_64x64)
    if (tiles[nmTiles].flags & TILEFLAG_32x32)
       {printf("puke on tile %s\n",filename);
       }
 nmTiles++;
 return nmTiles-1;
}

int findTile(char *filename)
{int i;
 for (i=0;i<nmTiles;i++)
    if (!stricmp(filename,tiles[i].filename))
       break;
 if (i==nmTiles)
    {printf("Oops in findTile. Looking for %s\n",filename);
     exit(-1);
    }
 return i;
}

void sortTiles(void)
{int i,j;
 TileInfo swap;
 for (i=1;i<nmTiles;i++)
    {for (j=i;j>0;j--)
	{if (tiles[j].uses>tiles[j-1].uses)
	    {swap=tiles[j];
	     tiles[j]=tiles[j-1];
	     tiles[j-1]=swap;
	    }
	}
    }
}

void computeTileNms(void)
{int i,no;
 no=0;
 for (i=0;i<nmTiles;i++)
    {tiles[i].number=no;
     no+=tiles[i].nmSubTiles;
    }
}


int writeRLE(unsigned char *input,int inputSize,FILE *ofile)
{int i,pos,size;
 int nmWritten=0;
 pos=0;
 while (pos<inputSize)
    {/* output block of blank space */
     for (size=0;size<255 && pos+size<inputSize && input[pos+size]==0;size++) ;
     if (ofile)
	fputc(size,ofile);
     nmWritten++;
     pos+=size;
     /* output block of nonblank space */
     for (size=0;size<255 && pos+size<inputSize && input[pos+size]!=0;size++) ;
     if (ofile)
	fputc(size,ofile);
     nmWritten++;
     if (ofile)
	for (i=0;i<size;i++)
	   {fputc(input[i+pos],ofile);
	   }
     nmWritten+=size;
     pos+=size;
    }
 return nmWritten;
}

unsigned char RLEBuffer[100000];
int writeTileDataRLE(int x,int y,int xsize,int ysize)
{int x1,y1,pos,size;
 pos=0;
 for (y1=y;y1<y+ysize;y1++)
    for (x1=x;x1<x+xsize;x1++)
       {if (picdata[y1][x1]==0)
	   {RLEBuffer[pos++]=255;
	    continue;
	   }
	if (picdata[y1][x1]==255)
	   {RLEBuffer[pos++]=0;
	    continue;
	   }
	RLEBuffer[pos++]=picdata[y1][x1];
       }
 clearTileWithRim(x,y,xsize,ysize);

 size=writeRLE(RLEBuffer,pos,NULL);
 writeShort(size);
 writeRLE(RLEBuffer,pos,ofile);
 return size;
}

int writeTileData(int x,int y,int xsize,int ysize)
{int x1,y1;
 for (y1=y;y1<y+ysize;y1++)
    for (x1=x;x1<x+xsize;x1++)
       {if (picdata[y1][x1]==0)
	   {writeChar(255);
	    continue;
	   }
	if (picdata[y1][x1]==255)
	   {writeChar(0);
	    continue;
	   }
	writeChar(picdata[y1][x1]);
       }
 clearTileWithRim(x,y,xsize,ysize);
 return 64*64;
}


int writeTile(int tileNm)
{int s,x,y,size,dim;
 memset(picdata,255,sizeof(picdata));
 {char buff[80];
  strcpy(buff,"tiles\\");
  strcat(buff,tiles[tileNm].filename);
  readBmp(buff);
 }
 size=0;
 for (x=0;x<512;x++)
    for (y=0;y<512;y++)
       if (x>=picWidth || y>=picHeight)
	  picdata[y][x]=255;

 if (picWidth<=32 && picHeight<=32 /* &&
     (tiles[tileNm].flags & TILEFLAG_16BPP)*/) /* removed for new class */
    {tiles[tileNm].flags&=~TILEFLAG_64x64;
     tiles[tileNm].flags|=TILEFLAG_32x32;
     printf("Tile %s is 32x32\n",tiles[tileNm].filename);
    }

 if ((tiles[tileNm].flags & TILEFLAG_8BPP)/* &&
     (tiles[tileNm].flags & TILEFLAG_64x64)*/) /* removed for new class */
    tiles[tileNm].flags|=TILEFLAG_RLE;

 for (s=0;s<tiles[tileNm].nmSubTiles;s++)
    {tilesWrote++;
     writeShort(tiles[tileNm].flags);
     writeShort(tiles[tileNm].palNm);
     dim=0;
     if (tiles[tileNm].flags & TILEFLAG_64x64)
	dim=64;
     if (tiles[tileNm].flags & TILEFLAG_32x32)
	{assert(!(tiles[tileNm].flags & TILEFLAG_64x64));
	 dim=32;
	}
     assert(dim);

     if (tiles[tileNm].flags & TILEFLAG_RLE)
	size+=writeTileDataRLE(tiles[tileNm].subx[s],
			       tiles[tileNm].suby[s],dim,dim);
     else
	size+=writeTileData(tiles[tileNm].subx[s],
			    tiles[tileNm].suby[s],dim,dim);
    }
 return size;
}

int tileSize(int tileNm)
{return tiles[tileNm].nmSubTiles*(64*64+4);
}

static int maxMap=0;
int reverseMapVertex(int newIndex)
{int i;
 for (i=0;i<=maxMap;i++)
    {if (vertexMap[i]==newIndex)
	return i;
    }
 return -1;
}

int addVertex(int oldIndex)
{assert(nmVertex>=0);
 assert(nmVertex<MAXNMVERT);

 if (oldIndex>=MAXINVERT)
    {printf("s=%d Old index=%d\n",nmSectors,oldIndex);
    }
 assert(oldIndex<MAXINVERT);
 if (vertexMap[oldIndex]!=-1)
    return vertexMap[oldIndex];
 if (nmVertex>=MAXNMVERT)
    {printf("Yarg!\n");
     exit(-1);
    }
 outVert[nmVertex].x=vertex[oldIndex].x<<15;
 outVert[nmVertex].z=vertex[oldIndex].y<<15;
 outVert[nmVertex].y=vertex[oldIndex].z<<15;

/* outVert[nmVertex].light=vertexLight[oldIndex]; */

 vertexMap[oldIndex]=nmVertex;
 if (oldIndex>maxMap) maxMap=oldIndex;
 return nmVertex++;
}

void clearVertexMap(void)
{int i;
 for (i=0;i<MAXINVERT;i++)
    vertexMap[i]=-1;
}


void printVert(fVert f)
{printf("(%f,%f,%f)\n",f.x,f.y,f.z);
}

void getInVert(int v,fVert *out)
{assert(v>=0);
 assert(v<vertexcount);
 out->x=(vertex[v].x<<15)/65536.0;
 out->y=(vertex[v].z<<15)/65536.0;
 out->z=(vertex[v].y<<15)/65536.0;
}

void getVertex(cVertexType *v,fVert *out)
{out->x=v->x/65536.0;
 out->y=v->y/65536.0;
 out->z=v->z/65536.0;
}

int addFace(facetype *face,int sector,int wallFirstVertex)
{assert(nmFaces<MAXNMFACES);
 outFace[nmFaces].v[0]=addVertex(face->v0)-wallFirstVertex;
 outFace[nmFaces].v[1]=addVertex(face->v1)-wallFirstVertex;
 outFace[nmFaces].v[2]=addVertex(face->v2)-wallFirstVertex;
 outFace[nmFaces].v[3]=addVertex(face->v3)-wallFirstVertex;
 outFace[nmFaces].tile=face->tile;

 {fVert v[5];
  int i;
#if 0
  double d;
  for (i=0;i<4;i++)
     assert(outFace[nmFaces].v[i]>=0);
#endif
  for (i=0;i<4;i++)
     getVertex(&(outVert[outFace[nmFaces].v[i]+wallFirstVertex]),v+i);
  v[4]=v[0];
#if 0
  for (i=0;i<4;i++)
     {d=sqrt((v[i].x-v[i+1].x)*(v[i].x-v[i+1].x)+
	     (v[i].y-v[i+1].y)*(v[i].y-v[i+1].y)+
	     (v[i].z-v[i+1].z)*(v[i].z-v[i+1].z));
      if (d>128.0)
	 {printf("Error: Tile is really big in sector %d! %f\n",sector,d);
	  /* exit(0);*/
	  return nmFaces;
	 }

     }
#endif
 }

 addTile(translateWallTileNm(face->tile),
	 TILEFLAG_64x64|TILEFLAG_16BPP|TILEFLAG_PALLETE);
 return nmFaces++;
}


int wallIsRect(walltype *wall,
	       fVert *iv0,fVert *iv1,fVert *iv3,
	       int tileHeight,int tileLength)
{int f,c;
 fVert origin;
 fVert length,height;
 void grid(int x,int y,fVert *out)
    {out->x=origin.x+length.x*x+height.x*y;
     out->y=origin.y+length.y*x+height.y*y;
     out->z=origin.z+length.z*x+height.z*y;
    }
 double distance(fVert *one,fVert *two)
    {return
	(one->x-two->x)*(one->x-two->x)+
	(one->y-two->y)*(one->y-two->y)+
	(one->z-two->z)*(one->z-two->z);
    }

 if (wall->firstface<0)
    /* boundry wall */
    return 0;

 /* see if there are the right number of faces */
 if (wall->lastface-wall->firstface+1!=tileHeight*tileLength)
    {/*printf("wrong # of faces.\n");*/
     return 0;
    }
 /* see if faces are arranged normally */
 origin=*iv0;
 length=*iv1;
 height=*iv3;
 length.x-=origin.x; length.y-=origin.y; length.z-=origin.z;
 height.x-=origin.x; height.y-=origin.y; height.z-=origin.z;
 assert(tileLength>=1);
 assert(tileHeight>=1);
 length.x/=tileLength; length.y/=tileLength; length.z/=tileLength;
 height.x/=tileHeight; height.y/=tileHeight; height.z/=tileHeight;

 for (f=wall->firstface,c=0;
      f<=wall->lastface;
      f++,c++)
    {int x,y;
     fVert v1,v2;
     x=c % (tileLength);
     y=c / (tileLength);

     grid(x,y,&v1);
     getInVert(face[f].v0,&v2);
     if (distance(&v1,&v2)>2.0)
	{/*printf("Distance=%f\n",distance(&v1,&v2));*/
	 return 0;
	}

     grid(x+1,y,&v1);
     getInVert(face[f].v1,&v2);
     if (distance(&v1,&v2)>2.0)
	{/*printf("Distance=%f\n",distance(&v1,&v2));*/
	 return 0;
	}

     grid(x+1,y+1,&v1);
     getInVert(face[f].v2,&v2);
     if (distance(&v1,&v2)>2.0)
	{/*printf("Distance=%f\n",distance(&v1,&v2));*/
	 return 0;
	}

     grid(x,y+1,&v1);
     getInVert(face[f].v3,&v2);
     if (distance(&v1,&v2)>2.0)
	{/*printf("Distance=%f\n",distance(&v1,&v2));*/
	 return 0;
	}
    }
 return 1;
}

#if ELEVATOR
struct elevTag
{char wallIsElevatorWall;
 int approxElevHeight;
} elevTags[MAXNMWALLS];

void adjustElevatorGeometry(void)
{int s,s1,w,w1;
 int elevHeight;
 for (w=0;w<wallcount;w++)
    elevTags[w].wallIsElevatorWall=0;
 for (s=0;s<sectorcount;s++)
    {if (sectorprops[s].type<SR_LIFT_SHAFT_STYLE_1 ||
	 sectorprops[s].type>SR_LIFT_SHAFT_STYLE_20)
	continue;
     elevHeight=sector[s].cielz-sector[s].floorz;
     elevHeight=elevHeight>>1;
     if (sectorprops[s].type==SR_LIFT_SHAFT_STYLE_3)
	elevHeight=64;
     printf("Elevator height=%d\n",elevHeight);
     /* find other sector walls that are part of the elevator and
	stretch them appropriatly */
     for (s1=0;s1<sectorcount;s1++)
	for (w=sector[s1].firstwall;w<=sector[s1].lastwall;w++)
	   if (wall[w].nextsector==s)
	      {for (w1=sector[s1].firstwall;w1<=sector[s1].lastwall;w1++)
		  if (wall[w1].nextsector==-1 &&
		      wall[w1].normal==wall[w].normal &&
		      vertex[wall[w1].v0].x==vertex[wall[w].v0].x &&
		      vertex[wall[w1].v0].y==vertex[wall[w].v0].y)
		     {elevTags[w1].wallIsElevatorWall=1;
		      elevTags[w1].approxElevHeight=elevHeight;
		     }
	      }
    }
}
#endif

int ambientTerm;
int computeLightValue(int oldVertex,int oldWall)
{int psxtileval;
 int sat;
 int vertexValue;
 int light;
 int nmAdjacentFaces,f;
 nmAdjacentFaces=0;
 vertexValue=0;
 for (f=wall[oldWall].firstface;f<=wall[oldWall].lastface;f++)
    {if (face[f].v0==oldVertex ||
	 face[f].v1==oldVertex ||
	 face[f].v2==oldVertex ||
	 face[f].v3==oldVertex)
	{vertexValue+=tiles[brewTileToOurTile[face[f].tile]].value;
	 nmAdjacentFaces++;
	}
    }
 assert(nmAdjacentFaces>0);
 vertexValue=vertexValue/nmAdjacentFaces;
 assert(vertexValue>=0);
 assert(vertexValue<=31);

 light=vertexLight[oldVertex];
#define TWENTYONE 18
 psxtileval=(light*TWENTYONE)/128;
 sat=psxtileval-TWENTYONE+16;
 if (sat<0) sat=0;
 if (sat>31) sat=31;

 if (vertexValue+sat-16<0)
    sat=0+16-vertexValue;

 return sat /*<<3*/;
}

#define MAXWALLDIM 200
int wallFaceMap[MAXWALLDIM][MAXWALLDIM];
char wallTileFlip[MAXWALLDIM][MAXWALLDIM];
int wallVertMap[MAXWALLDIM][MAXWALLDIM];

int mapRectWall(walltype *wall,
		fVert *iv0,fVert *iv1,fVert *iv3,
		int tileHeight,int tileLength)
{static char pattern[][4]=
    {
     {0,1,2,3},
     {1,2,3,0},
     {2,3,0,1},
     {3,0,1,2},
     {0,3,2,1},
     {1,0,3,2},
     {2,1,0,3},
     {3,2,1,0}
    };
 int f,c;
 fVert origin;
 fVert length,height;
 int x,y;
 void grid(int x,int y,fVert *out)
    {out->x=origin.x+length.x*x+height.x*y;
     out->y=origin.y+length.y*x+height.y*y;
     out->z=origin.z+length.z*x+height.z*y;
    }
 double distance(fVert *one,fVert *two)
    {return
	(one->x-two->x)*(one->x-two->x)+
	(one->y-two->y)*(one->y-two->y)+
	(one->z-two->z)*(one->z-two->z);
    }

 double map(fVert *v,int *l,int *h)
    /* returns distance from closest grid point */
    {int x,y;
     int bestX,bestY;
     double bestDist;
     fVert t;
     bestDist=1000000.0;
     for (y=0;y<=tileHeight;y++)
	for (x=0;x<=tileLength;x++)
	   {grid(x,y,&t);
	    if (distance(&t,v)<bestDist)
	       {bestX=x;
		bestY=y;
		bestDist=distance(&t,v);
	       }
	   }
     *l=bestX;
     *h=bestY;
     return bestDist;
    }
 if (wall->firstface<0)
    /* boundry wall */
    return 0;

 /* see if there are the right number of faces */
 if (wall->lastface-wall->firstface+1!=tileHeight*tileLength)
    {/*printf("wrong # of faces.\n");*/
     return 0;
    }
 /* see if faces are arranged normally */
 origin=*iv0;
 length=*iv1;
 height=*iv3;
 length.x-=origin.x; length.y-=origin.y; length.z-=origin.z;
 height.x-=origin.x; height.y-=origin.y; height.z-=origin.z;
 assert(tileLength>=1);
 assert(tileHeight>=1);
 length.x/=tileLength; length.y/=tileLength; length.z/=tileLength;
 height.x/=tileHeight; height.y/=tileHeight; height.z/=tileHeight;
 /* map faces into grid */
 for (y=0;y<MAXWALLDIM;y++)
    for (x=0;x<MAXWALLDIM;x++)
       {wallFaceMap[y][x]=-1;
	wallTileFlip[y][x]=0;
	wallVertMap[y][x]=-1;
       }

 for (f=wall->firstface,c=0;
      f<=wall->lastface;
      f++,c++)
    {int h[4],l[4];
     fVert t;
     int bx,by,i,j,slot;
     int p[4];
     getInVert(face[f].v0,&t);
     if (map(&t,&l[0],&h[0])>1.0)
	return 0;
     getInVert(face[f].v1,&t);
     if (map(&t,&l[1],&h[1])>1.0)
	return 0;
     getInVert(face[f].v2,&t);
     if (map(&t,&l[2],&h[2])>1.0)
	return 0;
     getInVert(face[f].v3,&t);
     if (map(&t,&l[3],&h[3])>1.0)
	return 0;
     bx=1000;
     by=1000;
     for (i=0;i<4;i++)
	{if (h[i]<by)
	    by=h[i];
	 if (l[i]<bx)
	    bx=l[i];
	}
     for (i=0;i<4;i++)
	p[i]=-1;
     for (i=0;i<4;i++)
	{slot=-1;
	 if (h[i]-by==0 && l[i]-bx==0)
	    slot=0;
	 if (h[i]-by==0 && l[i]-bx==1)
	    slot=1;
	 if (h[i]-by==1 && l[i]-bx==1)
	    slot=2;
	 if (h[i]-by==1 && l[i]-bx==0)
	    slot=3;
	 if (slot==-1)
	    return 0;
	 if (p[slot]!=-1)
	    return 0;
	 p[slot]=i;
	}
     /* match pattern in p to pattern list */
     for (j=0;j<8;j++)
	{for (i=0;i<4;i++)
	    if (pattern[j][i]!=p[i])
	       break;
	 if (i==4)
	    break;
	}
     if (j==8)
	return 0;
     if (wallFaceMap[by][bx]!=-1)
	return 0;

     if (wallVertMap[h[0]][l[0]]==-1)
	wallVertMap[h[0]][l[0]]=face[f].v0;
     else
	assert(wallVertMap[h[0]][l[0]]==face[f].v0);

     if (wallVertMap[h[1]][l[1]]==-1)
	wallVertMap[h[1]][l[1]]=face[f].v1;
     else
	assert(wallVertMap[h[1]][l[1]]==face[f].v1);

     if (wallVertMap[h[2]][l[2]]==-1)
	wallVertMap[h[2]][l[2]]=face[f].v2;
     else
	assert(wallVertMap[h[2]][l[2]]==face[f].v2);

     if (wallVertMap[h[3]][l[3]]==-1)
	wallVertMap[h[3]][l[3]]=face[f].v3;
     else
	assert(wallVertMap[h[3]][l[3]]==face[f].v3);

     wallFaceMap[by][bx]=f;
     wallTileFlip[by][bx]=j;
    }

 for (y=0;y<tileHeight;y++)
    for (x=0;x<tileLength;x++)
       assert(wallVertMap[y][x]!=-1);

 return 1;
}

int addWall(walltype *wall,int sectorNm,int wallNm)
{int vrot;
 int i,rectangle;
 assert(nmWalls<MAXNMWALLS);
 outWalls[nmWalls].flags=0;
 outWalls[nmWalls].firstFace=-1;
 outWalls[nmWalls].lastFace=-1;

 outWalls[nmWalls].nextSector=wall->nextsector;
 outWalls[nmWalls].v[0]=addVertex(wall->v0);
 outWalls[nmWalls].v[1]=addVertex(wall->v0+1);
 outWalls[nmWalls].v[2]=addVertex(wall->v0+2);
 outWalls[nmWalls].v[3]=addVertex(wall->v0+3);
 outWalls[nmWalls].object=(void *)wallNm; /* naf! */

 outWalls[nmWalls].firstLight=0;

#if 0
 if (wall->nextsector!=-1 && pbList[wall->nextsector].isPB &&
     pbList[wall->nextsector].isInternal)
    outWalls[nmWalls].flags|=WALLFLAG_NOTONCONVEXHULL;
#endif

 assert(wall->firstface>=-1);
 if (wall->firstface==-1)
    outWalls[nmWalls].flags|=WALLFLAG_INVISIBLE;

 if (wall->nextsector==-1 || wall->isblocked)
    outWalls[nmWalls].flags|=WALLFLAG_BLOCKED;

 /* find tile length and tile height of wall and decide if its a rectangle*/
 {fVert v0,v1,v2,v3;
  double len;
  getVertex(outVert+outWalls[nmWalls].v[0],&v0);
  getVertex(outVert+outWalls[nmWalls].v[1],&v1);
  getVertex(outVert+outWalls[nmWalls].v[2],&v2);
  getVertex(outVert+outWalls[nmWalls].v[3],&v3);
  len=sqrt((v0.x-v1.x)*(v0.x-v1.x)+
	   (v0.y-v1.y)*(v0.y-v1.y)+
	   (v0.z-v1.z)*(v0.z-v1.z));
  outWalls[nmWalls].pixelLength=
     sqrt((v0.x-v1.x)*(v0.x-v1.x)+
	  (v0.z-v1.z)*(v0.z-v1.z));
  outWalls[nmWalls].tileLength=(len+TILESIZE/2)/TILESIZE;
  if (outWalls[nmWalls].tileLength<=0)
     outWalls[nmWalls].tileLength=1;
  len=sqrt((v2.x-v1.x)*(v2.x-v1.x)+
	   (v2.y-v1.y)*(v2.y-v1.y)+
	   (v2.z-v1.z)*(v2.z-v1.z));
  outWalls[nmWalls].tileHeight=(len+TILESIZE/2)/TILESIZE;
  if (outWalls[nmWalls].tileHeight<=0)
     outWalls[nmWalls].tileHeight=1;
  rectangle=mapRectWall(wall,
			&v0,&v1,&v3,
			outWalls[nmWalls].tileHeight,
			outWalls[nmWalls].tileLength);
 }

#if FORCERECT
 if (wall->firstface>=0)
    rectangle=1;
#endif

#if ELEVATOR
 /* wall & elevator stuff */
 if (wall->firstface>wall->lastface)
    {printf("Broken wall in sector %d\n",sectorNm);
     assert(0);
     rectangle=1; /* fix broken doors */
    }

 if (elevTags[wallNm].wallIsElevatorWall)
    {rectangle=1;
     outWalls[nmWalls].tileHeight=
	(elevTags[wallNm].approxElevHeight+TILESIZE/2)/TILESIZE;
     if (outWalls[nmWalls].tileHeight<=0)
	outWalls[nmWalls].tileHeight=1;
     {int x,y;
      for (y=0;y<outWalls[nmWalls].tileHeight;y++)
	 for (x=0;x<outWalls[nmWalls].tileLength;x++)
	    {wallFaceMap[y][x]=wall->firstface;
	     wallTileFlip[y][x]=0;
	    }
      for (y=0;y<=outWalls[nmWalls].tileHeight;y++)
	 for (x=0;x<=outWalls[nmWalls].tileLength;x++)
	    {wallVertMap[y][x]=face[wall->firstface].v0;
	    }
     }
    }
#endif

 if (wall->nextsector!=-1 && abs(wall->nz)==16384 &&
     wall->firstface>=0 &&
     !stricmp(translateWallTileNm(face[wall->firstface].tile),
	      "1default\\def5_s.bmp"))
    {rectangle=0; /* water surface */
     outWalls[nmWalls].flags|=WALLFLAG_WATERSURFACE;
    }

 /* find parallax walls */
#if 1
 if (wall->firstface>=0 &&
     !stricmp(translateWallTileNm(face[wall->firstface].tile),
	      "1default\\def2_c.bmp"))
    {/* parallax face */
     rectangle=0;
     wall->firstface=-1;
     wall->lastface=-1;
     outWalls[nmWalls].flags|=WALLFLAG_PARALLAX;
    }
#endif

 if (!rectangle)
    {if (wall->firstface!=-1)
	{nmNonRect++;
	 clearVertexMap();
	 outWalls[nmWalls].firstFace=nmFaces;
	 outWalls[nmWalls].firstVertex=nmVertex;
	 for (i=wall->firstface;i<=wall->lastface;i++)
	    addFace(face+i,sectorNm,outWalls[nmWalls].firstVertex);
	 outWalls[nmWalls].lastFace=nmFaces-1;
	 outWalls[nmWalls].lastVertex=nmVertex-1;
	 for (i=wall->firstface;i<=wall->lastface;i++)
	    {outVert[vertexMap[face[i].v0]].light=
		computeLightValue(face[i].v0,wallNm);
	     outVert[vertexMap[face[i].v1]].light=
		computeLightValue(face[i].v1,wallNm);
	     outVert[vertexMap[face[i].v2]].light=
		computeLightValue(face[i].v2,wallNm);
	     outVert[vertexMap[face[i].v3]].light=
		computeLightValue(face[i].v3,wallNm);
	    }
	}
     else
	{nmInvisible++;
	 outWalls[nmWalls].firstFace=-1;
	 assert(outWalls[nmWalls].flags&
		(WALLFLAG_INVISIBLE|WALLFLAG_PARALLAX));
	}
    }
 else
    {outWalls[nmWalls].textures=nmTextures;
     outWalls[nmWalls].flags|=WALLFLAG_PARALLELOGRAM;
     outWalls[nmWalls].firstLight=nmLight;
     if (wall->firstface<0)
	{printf("Something wrong in sector %d\n",sectorNm);
	}
     assert(wall->firstface!=-1);
     /* if (wall->lastface-wall->firstface+1==
	outWalls[nmWalls].tileHeight*outWalls[nmWalls].tileLength) */
     {int h,w,height,width,f,v;
      height=outWalls[nmWalls].tileHeight;
      width=outWalls[nmWalls].tileLength;
#if 0
      for (h=0;h<=height;h++)
	 for (w=0;w<=width;w++)
	    {if (h<height)
		if (w<width)
		   v=face[wallFaceMap[h][w]].v0;
		else
		   v=face[wallFaceMap[h][w-1]].v1;
	     else
		if (w<width)
		   v=face[wallFaceMap[h-1][w]].v3;
		else
		   v=face[wallFaceMap[h-1][w-1]].v2;
	     outLight[nmLight++]=computeLightValue(v,wallNm);
	    }
#else
      for (h=0;h<=height;h++)
	 for (w=0;w<=width;w++)
	    {assert(wallVertMap[h][w]>=0);
	     outLight[nmLight++]=computeLightValue(wallVertMap[h][w],wallNm);
	    }
#endif


      for (h=0;h<height;h++)
	 for (w=0;w<width;w++)
	    {outTexture[nmTextures++]=wallTileFlip[h][w];
	     assert(nmTextures<MAXNMTEXTURES);
	     outTexture[nmTextures++]=face[wallFaceMap[h][w]].tile;
	     assert(nmTextures<MAXNMTEXTURES);
	     addTile(translateWallTileNm(outTexture[nmTextures-1]),
		     TILEFLAG_64x64|TILEFLAG_16BPP|TILEFLAG_PALLETE);
	    }
     }
     outWalls[nmWalls].firstFace=-1;
     outWalls[nmWalls].lastFace=-1;
     outWalls[nmWalls].firstVertex=-1;
     outWalls[nmWalls].lastVertex=-1;
    }

 /* compute plane equations */
 {fVert n;
  float d;
  fVert v[4];
  fVert P;
  float f;
  int i,j;
  getVertex(outVert+outWalls[nmWalls].v[0],&(v[0]));
  getVertex(outVert+outWalls[nmWalls].v[1],&(v[1]));
  getVertex(outVert+outWalls[nmWalls].v[2],&(v[2]));
  getVertex(outVert+outWalls[nmWalls].v[3],&(v[3]));
  n.x=0; n.y=0; n.z=0;
  for (i=0;i<4;i++)
     {j=i+1; if (j>=4) j=0;
      n.x+=(v[i].y-v[j].y)*(v[i].z+v[j].z);
      n.y+=(v[i].z-v[j].z)*(v[i].x+v[j].x);
      n.z+=(v[i].x-v[j].x)*(v[i].y+v[j].y);
     }
  f=sqrt(n.x*n.x+n.y*n.y+n.z*n.z);
  if (f<=  0.000001)
     {printf("Degenerate wall skipped in sector %d!\n",sectorNm);
      printf("svert:%d %d %d %d\n",
	     outWalls[nmWalls].v[0],
	     outWalls[nmWalls].v[1],
	     outWalls[nmWalls].v[2],
	     outWalls[nmWalls].v[3]);

      printf("Vertexes:%d %d %d %d\n",
	     reverseMapVertex(outWalls[nmWalls].v[0]),
	     reverseMapVertex(outWalls[nmWalls].v[1]),
	     reverseMapVertex(outWalls[nmWalls].v[2]),
	     reverseMapVertex(outWalls[nmWalls].v[3]));

      printVert(v[0]);
      printVert(v[1]);
      printVert(v[2]);
      printVert(v[3]);
      return nmWalls;
     }

  assert(f>0.000001);
  n.x/=f; n.y/=f; n.z/=f;
  P.x=(v[0].x+v[1].x+v[2].x+v[3].x)/4;
  P.y=(v[0].y+v[1].y+v[2].y+v[3].y)/4;
  P.z=(v[0].z+v[1].z+v[2].z+v[3].z)/4;
  d=-(P.x*n.x+P.y*n.y+P.z*n.z);
  outWalls[nmWalls].normal[0]=n.x*65536.0;
  outWalls[nmWalls].normal[1]=n.y*65536.0;
  outWalls[nmWalls].normal[2]=n.z*65536.0;
  outWalls[nmWalls].d=d*65536.0;
 }

 if (outWalls[nmWalls].nextSector!=-1 &&
     outWalls[nmWalls].normal[1]!=0 &&
     outWalls[nmWalls].firstFace!=-1)
    outWalls[nmWalls].flags&=~WALLFLAG_INVISIBLE;

 return nmWalls++;
}

void addSector(sectortype *s,int sNm)
{int w;
 assert(nmSectors<MAXNMSECTORS);
 outSector[nmSectors].firstWall=nmWalls;
 for (w=s->firstwall;w<=s->lastwall+2;w++)
    addWall(wall+w,s-sector,w);
 outSector[nmSectors].lastWall=nmWalls-1;
 outSector[nmSectors].light=s->light;

 /* find sector's center */
 {int center[3];
  int w,v,n;
  center[0]=0;  center[1]=0;  center[2]=0;
  n=0;
  for (w=outSector[nmSectors].firstWall;w<=outSector[nmSectors].lastWall;w++)
     {for (v=0;v<4;v++)
	 {center[0]+=outVert[outWalls[w].v[v]].x>>16;
	  center[1]+=outVert[outWalls[w].v[v]].y>>16;
	  center[2]+=outVert[outWalls[w].v[v]].z>>16;
	  n++;
	 }
     }
  assert(n);
  outSector[nmSectors].center[0]=(center[0]/n);
  outSector[nmSectors].center[1]=(center[1]/n);
  outSector[nmSectors].center[2]=(center[2]/n);
 }
 /* find sector's floor level */
 {int level;
  int w,v,n;
  level=0;
  n=0;
  for (w=outSector[nmSectors].firstWall;w<=outSector[nmSectors].lastWall;w++)
     {if (outWalls[w].normal[1]<=0)
	 continue;
      for (v=0;v<4;v++)
	 {level+=outVert[outWalls[w].v[v]].y>>16;
	  n++;
	 }
     }
  if (!n)
     printf("Sector %d has no floor!\n",sNm);
  assert(n);
  outSector[nmSectors].floorLevel=(level/n);
 }

#if 0
 /* maybe add extra walls and sectors for push block */
 if (pbList[sNm].isPB)
    {int p[8][3];
     int w,i,j;
     /* find floor */
     for (w=outSector[nmSectors].firstWall;w<=outSector[nmSectors].lastWall;
	  w++)
	if (outWalls[w].normal[1]==1<<16)
	   break;
     assert(w<=outSector[nmSectors].lastWall);
     for (i=0;i<4;i++)
	{p[i][0]=outVert[outWalls[w].v[i]].x;
	 p[i][1]=outVert[outWalls[w].v[i]].y;
	 p[i][2]=outVert[outWalls[w].v[i]].z;
	 p[i+4][0]=outVert[outWalls[w].v[i]].x;
	 p[i+4][1]=outVert[outWalls[w].v[i]].y;
	 p[i+4][2]=outVert[outWalls[w].v[i]].z;
	}
     /* find ceiling */
     for (w=outSector[nmSectors].firstWall;w<=outSector[nmSectors].lastWall;
	  w++)
	if (outWalls[w].normal[1]==-1<<16)
	   break;
     assert(w<=outSector[nmSectors].lastWall);
     for (i=0;i<4;i++)
	p[i+4][1]=outVert[outWalls[w].v[0]].y;


     /* add walls */



    }

#endif

 nmSectors++;
}

void sortWalls(void)
{int s,w,i,wallCount,firstWall;
 int doSwap;
 sWallType swap;
 /* sort sector boundries first, wall and floors next */
 for (s=0;s<nmSectors;s++)
    {firstWall=outSector[s].firstWall;
     wallCount=outSector[s].lastWall-outSector[s].firstWall+1;
     for (w=1;w<wallCount;w++)
	{for (i=w;i>0;i--)
	    {doSwap=0;
	     if (outWalls[firstWall+i-1].nextSector==-1 &&
		 outWalls[firstWall+i].nextSector!=-1)
		doSwap=1;
	     if (outWalls[firstWall+i-1].nextSector==-1 &&
		 outWalls[firstWall+i].nextSector==-1 &&
		 outWalls[firstWall+i-1].normal[1]!=0 &&
		 outWalls[firstWall+i].normal[1]==0)
		doSwap=1;
	     if (doSwap)
		{swap=outWalls[firstWall+i-1];
		 outWalls[firstWall+i-1]=outWalls[firstWall+i];
		 outWalls[firstWall+i]=swap;
		}
	    }
	}
    }
}

#define f(x) (((double)(x))/65536.0)

void printWall(sWallType *w)
{int v;
 printf("normal (%f,%f,%f) ns:%d\n",
	w->normal[0]/65536.0,
	w->normal[1]/65536.0,
	w->normal[2]/65536.0,w->nextSector);
 for (v=0;v<4;v++)
    {fVert fv;
     getVertex(outVert+w->v[v],&fv);
     printf("   (%f,%f,%f)\n",
	    fv.x,fv.y,fv.z);
    }
 printf("\n");
}

void checkForIntersectingSectors(void)
{int sec1,sec2,wall,w,v;
 for (sec1=0;sec1<nmSectors;sec1++)
    for (sec2=0;sec2<nmSectors;sec2++)
       {sSectorType *s1=outSector+sec1;
	sSectorType *s2=outSector+sec2;
	if (sec1==sec2)
	   continue;
	for (w=s2->firstWall;w<=s2->lastWall;w++)
	   {/* exclude the floor and ceiling */
	    if (outWalls[w].normal[1]!=0)
	       continue;
	    for (v=0;v<4;v++)
	       {for (wall=s1->firstWall;wall<=s1->lastWall;wall++)
		   {/* if any verticies in sector s2 are on the front face of
		       all the walls in s1 then the sectors intersect */
		    fVert normal,testVert,wallVert;
		    sWallType *testWall=outWalls+wall;
#if 0
		    if (testWall->flags & WALLFLAG_NOTONCONVEXHULL)
		       continue;
#endif
		    getVertex(outVert+testWall->v[0],&testVert);
		    normal.x=testWall->normal[0]/65536.0;
		    normal.y=testWall->normal[1]/65536.0;
		    normal.z=testWall->normal[2]/65536.0;
		    getVertex(outVert+outWalls[w].v[v],&wallVert);
		    if ((wallVert.x-testVert.x)*normal.x+
			(wallVert.y-testVert.y)*normal.y+
			(wallVert.z-testVert.z)*normal.z
			<1.0)
		       /* this vertex is ok */
		       break;
		   }
		if (wall>s1->lastWall)
		   {/* this vertex is not ok */
		    printf("Warning: sector %d and %d intersect.\n",sec1,sec2);
		    goto nextSector;
		   }
	       }
	   }
     nextSector:
	continue;
       }
}

void checkForDuplicateWalls(void)
{int sec,wall,w,i;
 for (sec=0;sec<nmSectors;sec++)
    {sSectorType *s=outSector+sec;
     for (wall=s->firstWall;wall<=s->lastWall;wall++)
	{/* if any verticies in sector s are on the back face of wall
	    w then the sector is not convex */
	 fVert testVert,wallVert;
	 sWallType *testWall=outWalls+wall;

	 for (w=s->firstWall;w<=s->lastWall;w++)
	    {sWallType *wally=outWalls+w;
	     if (w==wall)
		continue;
	     if (wally->normal[0]!=testWall->normal[0] ||
		 wally->normal[1]!=testWall->normal[1] ||
		 wally->normal[2]!=testWall->normal[2])
		continue;
	     for (i=0;i<4;i++)
		{getVertex(outVert+testWall->v[i],&testVert);
		 getVertex(outVert+wally->v[i],&wallVert);
		 if (testVert.x!=wallVert.x ||
		     testVert.y!=wallVert.y ||
		     testVert.z!=wallVert.z)
		    break;
		}
	     if (i<4)
		continue;
	     printf("Duplicate wall in sector %d\n",sec);
	     {FILE *ffile;
	      ffile=fopen("convlog.txt","a");
	      fprintf(ffile,"Level %s: duplicate wall in sector %d\n",
		      levelName,sec);
	      fclose(ffile);
	     }

	    }
	}
    }
}

void checkForConcaveSectors(void)
{int sec,wall,w,v;
 for (sec=0;sec<nmSectors;sec++)
    {sSectorType *s=outSector+sec;
     if (sectorprops[sec].type>=SR_DOORWAY_STYLE_1 &&
	 sectorprops[sec].type<=SR_LIFT_SHAFT_STYLE_20)
	continue;

     for (wall=s->firstWall;wall<=s->lastWall;wall++)
	{/* if any verticies in sector s are on the back face of wall
	    w then the sector is not convex */
	 fVert normal,testVert,wallVert;
	 sWallType *testWall=outWalls+wall;
#if 0
	 if (testWall->flags & WALLFLAG_NOTONCONVEXHULL)
	    continue;
#endif
	 getVertex(outVert+testWall->v[0],&testVert);
	 normal.x=testWall->normal[0]/65536.0;
	 normal.y=testWall->normal[1]/65536.0;
	 normal.z=testWall->normal[2]/65536.0;
	 for (w=s->firstWall;w<=s->lastWall;w++)
	    {/* exclude the floor and ceiling */
	     if (outWalls[w].normal[1]!=0)
		continue;
	     for (v=0;v<4;v++)
		{getVertex(outVert+outWalls[w].v[v],&wallVert);
		 if ((wallVert.x-testVert.x)*normal.x+
		    (wallVert.y-testVert.y)*normal.y+
		    (wallVert.z-testVert.z)*normal.z
		    <-1.0)
		   {printf("Warning: sector %d is not convex.\n",sec);
		    {FILE *ffile;
		     ffile=fopen("convlog.txt","a");
		     fprintf(ffile,"Level %s: sector %d not convex\n",
			     levelName,sec);
		     fclose(ffile);
		    }
                    goto nextSector;
		   }
	       }
	    }
	}
  nextSector:
     continue;
    }

}

/* sees if sector s1 is wholely contained in sector s2 */
int checkInternal(int s1,int s2)
{/* check all verticies of s1 against all walls of s2 */
 fVert normal,testVert,wallVert;
 int w2,w1,v1;
 for (w2=outSector[s2].firstWall;w2<=outSector[s2].lastWall;w2++)
    {sWallType *testWall=outWalls+w2;
#if 0
     if (testWall->flags & WALLFLAG_NOTONCONVEXHULL)
	continue;
#endif
     getVertex(outVert+testWall->v[0],&testVert);
     normal.x=testWall->normal[0]/65536.0;
     normal.y=testWall->normal[1]/65536.0;
     normal.z=testWall->normal[2]/65536.0;
     for (w1=outSector[s1].firstWall;w1<=outSector[s1].lastWall;w1++)
	for (v1=0;v1<4;v1++)
	   {getVertex(outVert+outWalls[w1].v[v1],&wallVert);
	    if ((wallVert.x-testVert.x)*normal.x+
		(wallVert.y-testVert.y)*normal.y+
		(wallVert.z-testVert.z)*normal.z
		<-1.0)
	       return 0;
	   }

    }
 return 1;
}

/* check to see if all the points in 'sector' are on the back side of
   'wall' */
int testWall(sWallType *testWall,int sector)
{int w,v;
 fVert normal,testVert,wallVert;
 sSectorType *s=outSector+sector;

 getVertex(outVert+testWall->v[0],&testVert);
 normal.x=testWall->normal[0]/65536.0;
 normal.y=testWall->normal[1]/65536.0;
 normal.z=testWall->normal[2]/65536.0;
 for (w=s->firstWall;w<=s->lastWall;w++)
    {/* exclude the floor and ceiling */
     if (outWalls[w].normal[1]!=0)
	continue;
     for (v=0;v<4;v++)
	{getVertex(outVert+outWalls[w].v[v],&wallVert);
	 if ((wallVert.x-testVert.x)*normal.x+
	     (wallVert.y-testVert.y)*normal.y+
	     (wallVert.z-testVert.z)*normal.z
	     >1.0 /*0.001*/)
	    return 0;
	}
    }
 return 1;
}

int wallUsedForCut[MAXNMWALLS];

int findCutWall(int s1,int s2,int i1,int i2)
{int w;
 /* check walls that are already used for cut planes */
 if (s1==s2)
    return 0;
 if (outCutPlane[i1][i2]!=99)
    return outCutPlane[i1][i2];
 if (outCutPlane[i2][i1]!=99)
    return 0x80 ^ outCutPlane[i2][i1];

 for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
    {sWallType *wall=outWalls+w;
     if (wallUsedForCut[w])
	if (testWall(wall,s2))
	   {wallUsedForCut[w]=1;
	    return w-outSector[s1].firstWall;
	   }
    }

 for (w=outSector[s2].firstWall;w<=outSector[s2].lastWall;w++)
    {sWallType *wall=outWalls+w;
     if (wallUsedForCut[w])
	if (testWall(wall,s1))
	   {wallUsedForCut[w]=1;
	    return 0x80|(w-outSector[s2].firstWall);
	   }
    }

 /* otherwise check wall walls in sector */
 for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
    {
#if 0
     if (outWalls[w].flags & WALLFLAG_NOTONCONVEXHULL)
	continue;
#endif
     if (testWall(outWalls+w,s2))
	{wallUsedForCut[w]=1;
	 return w-outSector[s1].firstWall;
	}
    }
 for (w=outSector[s2].firstWall;w<=outSector[s2].lastWall;w++)
    {
#if 0
     if (outWalls[w].flags & WALLFLAG_NOTONCONVEXHULL)
	continue;
#endif
     if (testWall(outWalls+w,s1))
	{wallUsedForCut[w]=1;
	 return 0x80|(w-outSector[s2].firstWall);
	}
    }
 printf("Warning: no cut plane for sectors %d and %d.\n",s1,s2);
 return 0;
}

void findCutPlanes(void)
{int s1,s2,w;
 int i,i1,i2;
 nmCutSectors=0;
 for (i=0;i<MAXNMWALLS;i++)
    wallUsedForCut[i]=0;

 for (s1=0;s1<MAXCUTSECTORS;s1++)
    for (s2=0;s2<nmSectors;s2++)
       outCutPlane[s2][s1]=99;

 for (s1=0;s1<nmSectors;s1++)
    {if (sectorprops[s1].type==SR_SPECIAL_ROLE_18)
	{outSector[s1].flags|=SECFLAG_CUTSORT;
	 outSector[s1].cutIndex=nmCutSectors++;
	 outSector[s1].cutChannel=sectorprops[s1].index;
	 assert(nmCutSectors<MAXCUTSECTORS);
	}
    }

 for (s1=0;s1<nmSectors;s1++)
    for (s2=0;s2<nmSectors;s2++)
       {if (!((outSector[s1].flags & SECFLAG_CUTSORT) &&
	      (outSector[s2].flags & SECFLAG_CUTSORT)))
	   continue;
	i1=outSector[s1].cutIndex;
	i2=outSector[s2].cutIndex;
	outCutPlane[i1][i2]=findCutWall(s1,s2,i1,i2);
       }
}

#if 0
void checkLevel(void)
{int w,f,v;
 sWallType *wall;
 for (w=0;w<nmWalls;w++)
    if (!(outWalls[w].flags & WALLFLAG_PARALLELOGRAM) &&
	outWalls[w].firstFace!=-1)
       {wall=outWalls+w;
	for (f=wall->firstFace;f<=wall->lastFace;f++)
	   {for (v=0;v<4;v++)
	       {assert(outFace[f].v[v]>=wall->firstVertex);
		assert(outFace[f].v[v]<=wall->lastVertex);
	       }
	   }
       }
}
#else
void checkLevel()
{
}
#endif


#define F(x) ((x)<<16)

void printSectorsWaterNodes(int s)
{int w;
 printf("Sector %d:\n",s);
 for (w=outSector[s].firstWall;
      w<=outSector[s].lastWall;w++)
    if (outWalls[w].nextSector!=-1 && abs(outWalls[w].normal[1])==F(1))
       {int wface=(int)outWalls[w].object;
	int v;
	for (v=0;v<4;v++)
	   {int wvert=outWaveFace[wface].connect[v];
	    int j;
	    printf("  v%d: ",wvert);
	    for (j=0;j<4;j++)
	       {if (outWaveVert[wvert].connect[j]==-1)
		   break;
		printf("%d ",outWaveVert[wvert].connect[j]);
	       }
	    printf("\n");
	   }
       }
}

double vertDistance(int v1,int v2)
{fVert fv1,fv2;
 getVertex(outVert+v1,&fv1);
 getVertex(outVert+v2,&fv2);
 return sqrt((fv1.x-fv2.x)*(fv1.x-fv2.x)+
	     (fv1.y-fv2.y)*(fv1.y-fv2.y)+
	     (fv1.z-fv2.z)*(fv1.z-fv2.z));
}


void setBlockingBits(void)
{int s,s1;
 int w,w1;

#if 0
 /* mark water surfaces */
 for (s=0;s<nmSectors;s++)
    for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
       {if (outWalls[w].nextSector==-1)
	   continue;
	if (outWalls[w].firstface>=0 &&
	    !stricmp(translateWallTileNm(face[outWalls[w].firstface].tile),
		     "1default\\def5_s.bmp") &&
	    outWalls[w].normal[1]!=0 &&
	    ((outSector[outWalls[w].nextSector].flags & SECFLAG_WATER) ||
	     (outSector[s].flags & SECFLAG_WATER)))
	   {outWalls[w].flags|=WALLFLAG_WATERSURFACE;
	    assert(!(outWalls[w].flags & WALLFLAG_PARALLELOGRAM));
	   }
       }
#endif

 /* mark water boundries */
 for (s=0;s<nmSectors;s++)
    for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
       {if (outWalls[w].nextSector==-1)
	   continue;
	if (outWalls[w].normal[1]!=0)
	   continue;
	s1=outWalls[w].nextSector;
	for (w1=outSector[s1].firstWall;w1<=outSector[s1].lastWall;w1++)
	   {if (outWalls[w1].normal[1]>0 &&
		(outWalls[w1].flags & WALLFLAG_WATERSURFACE))
	       outWalls[w].flags|=WALLFLAG_WATERBNDRY;
	   }
       }

 /* mark lava boundries */
 {int sectorHasLavaFloor(int s)
     {int w;
      for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
	 {if (outWalls[w].normal[1]>0 && (outWalls[w].flags & WALLFLAG_LAVA))
	     return 1;
	 }
      return 0;
     }
  for (s=0;s<nmSectors;s++)
     {for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
	 {if (outWalls[w].nextSector==-1)
	     continue;
	  if (outWalls[w].normal[1]!=0)
	     continue;
	  s1=outWalls[w].nextSector;
	  if (sectorHasLavaFloor(s) ^ sectorHasLavaFloor(s1))
	     {outWalls[w].flags|=WALLFLAG_WATERBNDRY;
	     }
	 }
     }
 }

 for (s=0;s<nmSectors;s++)
    for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
       {int s1;
	if (outWalls[w].nextSector==-1)
	   continue;
	if (outWalls[w].normal[1]!=0)
	   continue;
	if (sectorprops[s].type>=SR_LIFT_SHAFT_STYLE_1 &&
	    sectorprops[s].type<=SR_LIFT_SHAFT_STYLE_20)
	   continue;
	s1=outWalls[w].nextSector;
	if (sectorprops[s1].type>=SR_LIFT_SHAFT_STYLE_1 &&
	    sectorprops[s1].type<=SR_LIFT_SHAFT_STYLE_20)
	   continue;
	if (outWalls[w].flags & (WALLFLAG_WATERBNDRY|WALLFLAG_WATERSURFACE))
	   continue;
	{/* find height of opening */
	 fVert v;
	 int i;
	 double maxy,miny;
	 maxy=-1000000;
	 miny=1000000;
	 for (i=0;i<4;i++)
	    {getVertex(outVert+outWalls[w].v[i],&v);
	     if (v.y>maxy)
		maxy=v.y;
	     if (v.y<miny)
		miny=v.y;
	    }
	 if (maxy-miny<90.0 && maxy-miny>1.0)
	    outWalls[w].flags|=WALLFLAG_SHORTOPENING;
	}
       }

 /* mark cliff boundries */
 for (s=0;s<nmSectors;s++)
    for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
       {if (outWalls[w].nextSector==-1)
	   continue;
	if (outWalls[w].normal[1]!=0)
	   continue;
	s1=outWalls[w].nextSector;
	if (outSector[s].floorLevel-outSector[s1].floorLevel>320)
	   outWalls[w].flags|=WALLFLAG_CLIFFBNDRY;
       }
}


void initWater(void)
{int w,s,c,i,w1,s1,w2,j;
 /* we will only track the top surface of the water, the
    bottom surface will be the top's mirror image */
 nmWaveFace=0;
 /* assign waveFaces to water faces */
 for (w=0;w<nmWalls;w++)
    if (outWalls[w].nextSector!=-1 &&
	outWalls[w].normal[1]==F(1))
       {for (i=0;i<4;i++)
	   outWaveFace[nmWaveFace].connect[i]=-1;
	outWalls[w].object=(void *)(nmWaveFace++);
       }
 nmWaveVert=0;
 /* for each face, look for neighboring water faces to get vertices from,
    add new vertices for each unshared vert */
 for (s=0;s<nmSectors;s++)
    /* for each sector */
    for (w=outSector[s].firstWall;
	 w<=outSector[s].lastWall;w++)
       if (outWalls[w].nextSector!=-1 && outWalls[w].normal[1]==F(1))
	  /* if there is a water surface in that sector */
	  {for (w1=outSector[s].firstWall;
		w1<=outSector[s].lastWall;w1++)
	      if (outWalls[w1].nextSector!=-1 && outWalls[w1].normal[1]==0)
		 {s1=outWalls[w1].nextSector;
		  /* ... then for each ajoining sector */
		  for (w2=outSector[s1].firstWall;
		       w2<=outSector[s1].lastWall;w2++)
		     if (outWalls[w2].nextSector!=-1 &&
			 outWalls[w2].normal[1]==F(1))
			/* any water surface in that sector */
			{/* w=the surface under consideration &
			    w2=another surface in an ajoining sector */
			 /* see if w's verticies match any of w2's verticies */
			 int i1,i2;
			 for (i2=0;i2<4;i2++)
			    if (outWaveFace[(int)outWalls[w2].object].
				connect[i2]!=-1)
			       for (i1=0;i1<4;i1++)
				  {if (vertDistance(outWalls[w].v[i1],
						    outWalls[w2].v[i2])<1.0)
				      /* we found a matching vertex */
				      {outWaveFace[(int)outWalls[w].object]
					  .connect[i1]=
					     outWaveFace[(int)outWalls[w2].
							 object].connect[i2];
				      }
				  }
			}
		  /* fill in the rest of the verticies */
		  for (i=0;i<4;i++)
		     {if (outWaveFace[(int)outWalls[w].object].
			  connect[i]==-1)
			 outWaveFace[(int)outWalls[w].object].connect[i]=
			    nmWaveVert++;
		     }
		 }
	  }

 /* fill in vertex info */
 for (i=0;i<nmWaveVert;i++)
    {outWaveVert[i].pos=((rand()%16)-8)<<16;
     outWaveVert[i].vel=0;
     for (j=0;j<4;j++)
	outWaveVert[i].connect[j]=-1;
     c=0;
     for (j=0;j<nmWaveFace;j++)
	{int p;
	 for (p=0;p<4;p++)
	    {if (outWaveFace[j].connect[p]==i)
		{/* connect i to the verticies next to it */
		 int w=p-1;
		 int z,g;
		 if (w<0)
		    w+=4;
		 z=outWaveFace[j].connect[w];
		 for (g=0;g<4;g++)
		    if (outWaveVert[i].connect[g]==z ||
			outWaveVert[i].connect[g]==-1)
		       break;
		 if (g<4 && outWaveVert[i].connect[g]==-1)
		    outWaveVert[i].connect[g]=z;

		 w=p+1;
		 if (w>3)
		    w=0;
		 z=outWaveFace[j].connect[w];
		 for (g=0;g<4;g++)
		    if (outWaveVert[i].connect[g]==z ||
			outWaveVert[i].connect[g]==-1)
		       break;
		 if (g<4 && outWaveVert[i].connect[g]==-1)
		    outWaveVert[i].connect[g]=z;
		}
	    }
	}
    }

 /* copy topside of water to underside */
 for (s=0;s<nmSectors;s++)
    /* for each sector */
    for (w=outSector[s].firstWall;
	 w<=outSector[s].lastWall;w++)
       if (outWalls[w].nextSector!=-1 && outWalls[w].normal[1]==F(1))
	  /* if there is a water surface in that sector */
	  {s1=outWalls[w].nextSector;
	   /***************** temp auto tagging for water *****************/
	   /* outSector[s1].flags|=SECFLAG_WATER; */
	   for (w2=outSector[s1].firstWall;
		w2<=outSector[s1].lastWall;w2++)
	      {/* find the underside of the surface */
	       if (outWalls[w2].nextSector==s)
		  {int i1,i2;
		   outWalls[w2].object=(void *)nmWaveFace;
		   for (i2=0;i2<4;i2++)
		      for (i1=0;i1<4;i1++)
			 {if (outVert[outWalls[w].v[i1]].x==
			      outVert[outWalls[w2].v[i2]].x &&
			      outVert[outWalls[w].v[i1]].y==
			      outVert[outWalls[w2].v[i2]].y &&
			      outVert[outWalls[w].v[i1]].z==
			      outVert[outWalls[w2].v[i2]].z)
			     /* we found a matching vertex */
			     {outWaveFace[(int)outWalls[w2].object]
				 .connect[i2]=
				    outWaveFace[(int)outWalls[w].
						object].connect[i1];
			     }
			 }
		   /* for (i=0;i<4;i++)
		      assert(outWaveFace[(int)outWalls[w2].object].connect[i]!=
			     -1); */
		   nmWaveFace++;
		  }
	      }
	  }


 /************************ propogate water auto tags ***********************/
 {int done;
  do
     {done=1;
      for (s=0;s<nmSectors;s++)
	 {/* for each sector */
	  if (!(outSector[s].flags & SECFLAG_WATER))
	     continue;
	  for (w=outSector[s].firstWall;
	       w<=outSector[s].lastWall;w++)
	     if (outWalls[w].normal[1]>=0 &&
		 outWalls[w].nextSector!=-1 /* &&
		 (outVert[outWalls[w].v[0]].y-outVert[outWalls[w].v[3]].y)
		 >F(70) */ &&
		 !(outSector[outWalls[w].nextSector].flags & SECFLAG_WATER))
		{outSector[outWalls[w].nextSector].flags|=SECFLAG_WATER;
		 done=0;
		}
	 }
     }
  while (!done);
 }
}

void object_addInt(int s)
{outObjectParams[nmObjectParams++]=(s>>24)&0xff;
 outObjectParams[nmObjectParams++]=(s>>16)&0xff;
 outObjectParams[nmObjectParams++]=(s>>8)&0xff;
 outObjectParams[nmObjectParams++]=(s)&0xff;
}

void object_addShort(int s)
{outObjectParams[nmObjectParams++]=(s>>8)&0xff;
 outObjectParams[nmObjectParams++]=(s)&0xff;
}

void object_addChar(int s)
{outObjectParams[nmObjectParams++]=(s)&0xff;
}

void object_addObject(int type)
{assert(type!=41);

 outObjects[nmObjects].type=type;
 outObjects[nmObjects].firstParam=nmObjectParams;
 nmObjects++;
}

void object_addSpriteObject(int type,int sector)
{object_addObject(type);
 object_addShort(sector);
}

FILE *extraFile;

void addExtraObjects(void)
{char buff[160];
 char name[80];
 static struct {int ot; char *name;}
 nameMap[]=
    {
     {OT_PLAYER,"player"},
     {OT_SPIDER,"spider"},
     {OT_ANUBIS,"anubis"},
     {OT_MUMMY,"mummy"},
     {OT_BASTET,"bastet"},
     {OT_WASP,"wasp"},
     {OT_FISH,"fish"},
     {OT_BUGKEY,"bugkey"},
     {OT_TIMEKEY,"timekey"},
     {OT_XKEY,"xkey"},
     {OT_PLANTKEY,"plantkey"},
     {OT_HAWK,"hawk"},
     {OT_M60,"m60"},
     {OT_COBRASTAFF,"cobrastaff"},
     {OT_FLAMER,"flamer"},
     {OT_GRENADEAMMO,"grenade"},
     {OT_MANACLE,"manacle"},
     {OT_PISTOL,"pistol"},
     {OT_CAPE,"shawl"},
     {OT_FEATHER,"feather"},
     {OT_MASK,"mask"},
     {OT_SANDALS,"sandals"},
     {OT_SCEPTER,"scepter"},
     {OT_RAMSESTRIGGER,"ramses"},
     {OT_CAMEL,"camel"},
     {OT_RING,"ring"},
     {OT_ANKLETS,"anklets"},
     {OT_BLOODBOWL,"bloodbowl"},
     {OT_AMMOSPHERE,"fullammo"},
     {OT_HEALTHSPHERE,"fullhealth"},
     {OT_AMMOORB,"ammo"},
     {OT_PYRAMID,"pyramid"},
     {OT_SENTRY,"sentry"},
     {OT_MAGMANTIS,"magmantis"},
     {OT_SET,"set"},
     {OT_SELKIS,"selkis"},
     {OT_BLOB,"blob"},
     {OT_QUEEN,"queen"},
     {OT_CONTAIN1,"blow"},
     {OT_TRANSMITTER,"transmitter"},
     {OT_BOOMPOT1,"boompot1"},
     {OT_BOOMPOT2,"boompot2"},
     {OT_COMM_BATTERY,"battery"},
     {OT_COMM_BOTTOM,"bottom"},
     {OT_COMM_DISH,"dish"},
     {OT_COMM_HEAD,"head"},
     {OT_COMM_KEYBOARD,"keyboard"},
     {OT_COMM_MOUSE,"mouse"},
     {OT_COMM_SCREEN,"screen"},
     {OT_COMM_TOP,"top"},
     {OT_TELEPSECTOR,"teleport"},
     {OT_TELEPRETURN,"teleportReturn"},
     {OT_RAMSIDEMUMMY,"tombmummy"},
     {OT_DOLL1,"doll"},
     {OT_DEAD,""}};

 int args[5];
 int i,nmArgs;
 if (!extraFile)
    return;

 while (1)
    {if (feof(extraFile))
	return;
     buff[0]=0;
     fgets(buff,160,extraFile);
     name[0]=0;
     nmArgs=sscanf(buff,"%s %d %d %d %d %d",name,args,args+1,args+2,args+3,
		   args+4);
     if (nmArgs==EOF)
	return;
     if (name[0]==0)
	return;
     if (!stricmp(name,"water"))
	{outSector[args[0]].flags|=SECFLAG_WATER;
	 levelHasWater=1;
	 continue;
	}
     for (i=0;nameMap[i].ot!=OT_DEAD;i++)
	if (!stricmp(name,nameMap[i].name))
	   break;
     if (nameMap[i].ot==OT_DEAD)
	{printf("Unknown object type '%s'.\n",name);
	 exit(-1);
	}
     if (nameMap[i].ot==OT_TELEPRETURN ||
	 nameMap[i].ot==OT_TELEPSECTOR)
	{printf("Adding special object: %d: ",nameMap[i].ot);
	 object_addObject(nameMap[i].ot);
	 for (i=0;i<nmArgs-1;i++)
	    {object_addShort(args[i]);
	     printf("%d ",args[i]);
	    }
	 printf("\n");
	 continue;
	}
     if (nameMap[i].ot==OT_CAMEL)
	printf("Camel(%d,%d)\n",args[0],args[1]);

     object_addObject(nameMap[i].ot);
     object_addShort(args[0]);
     object_addShort(outSector[args[0]].center[0]);
     object_addShort(outSector[args[0]].center[1]);
     object_addShort(outSector[args[0]].center[2]);
     object_addShort(0);
    }
}

double pointIsInSector(fVert *p,int s)
{int w;
 double distance,d;
 fVert testVert,normal;
 distance=1000000.0;
 for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
    {sWallType *testWall=outWalls+w;
     getVertex(outVert+testWall->v[0],&testVert);
     normal.x=testWall->normal[0]/65536.0;
     normal.y=testWall->normal[1]/65536.0;
     normal.z=testWall->normal[2]/65536.0;
     d=((p->x-testVert.x)*normal.x+
	(p->y-testVert.y)*normal.y+
	(p->z-testVert.z)*normal.z);
     if (d<distance)
	distance=d;
    }
 return distance;
}

void addJeffObjects(void)
{int i,type,bestS;
 for (i=0;i<nmJeffMonsters;i++)
    {if (jeffMonster[i].type==-2)
	continue;
     for (type=0;
	  jeffMonsterMap[type].jeff!=-1 &&
	  jeffMonsterMap[type].jeff!=jeffMonster[i].type;
	  type++) ;
     if (jeffMonsterMap[type].me==OT_MAGMANTIS)
	{printf("Found magmantis in sector %d\n",jeffMonster[i].s);
	}
     if (jeffMonsterMap[type].jeff==-1)
	{printf("Unknown object type in jeff file (%d)!\007\n",
		jeffMonster[i].type);
	 continue;
	}
     /* see if object is in the sector it's supposed to be */
     {fVert p;
      int s;
      double bestDistance,d;
      p.x=jeffMonster[i].x>>1;
      p.y=jeffMonster[i].z>>1;
      p.z=jeffMonster[i].y>>1;
      bestS=-1;
      bestDistance=-1000000.0;
      for (s=0;s<nmSectors;s++)
	 {d=pointIsInSector(&p,s);
	  if (d>bestDistance)
	     {bestDistance=d;
	      bestS=s;
	     }
	 }
      if (bestDistance<-3.0)
	 {printf("Object from sector %d not in world!\007\n",jeffMonster[i].s);
	  continue;
	 }
      if (bestS!=jeffMonster[i].s)
	 {printf("Jeff monster from sector %d is really in sector %d\007\n",
		 jeffMonster[i].s,bestS);
	 }
     }
     object_addObject(jeffMonsterMap[type].me);
     object_addShort(bestS);
     object_addShort(jeffMonster[i].x>>1);
     object_addShort(jeffMonster[i].z>>1);
     object_addShort(jeffMonster[i].y>>1);
     object_addShort(jeffMonster[i].a);
    }
}

void doTileTags(void)
{int w,s,i,f;
 walltype *theWall;
 for (s=0;s<nmSectors;s++)
 for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
    {theWall=&(wall[(int)outWalls[w].object]);
     if (theWall->firstface<0)
	continue;
     for (f=theWall->firstface;f<=theWall->lastface;f++)
	{if (wallTileType[face[f].tile]==WT_LAVA)
	    outWalls[w].flags|=WALLFLAG_LAVA;
	 if (wallTileType[face[f].tile]==WT_SWAMP)
	    outWalls[w].flags|=WALLFLAG_SWAMP;
	}
     if (wallTileType[face[theWall->firstface].tile]==WT_FORCE)
	{printf("Found force field\n");
	 outWalls[w].flags|=WALLFLAG_BLOCKED;
	 object_addObject(OT_FORCEFIELD);
	 object_addShort(w);
	 continue;
	}
     if (theWall->nextsector!=-1 &&
	 wallTileType[face[theWall->firstface].tile]!=WT_WATER &&
	 wallTileType[face[theWall->firstface].tile]!=WT_NOTEXPLODABLE)
	{outWalls[w].flags|=WALLFLAG_EXPLODABLE|WALLFLAG_BLOCKED|
	    WALLFLAG_BLOCKSSIGHT;
	 levelHasExplodeWalls=1;
	}
     /* find single tile tags */
     for (i=theWall->firstface;i<=theWall->lastface;i++)
	{int type=-1;
	 if (wallTileType[face[i].tile]==WT_SHOOTER1)
	    type=OT_SHOOTER1;
	 if (wallTileType[face[i].tile]==WT_SHOOTER2)
	    {printf("Found shooter type 2 on wall %d\n",w);
	     type=OT_SHOOTER2;
	    }
	 if (wallTileType[face[i].tile]==WT_SHOOTER3)
	    type=OT_SHOOTER3;
	 if (type!=-1)
	    {object_addObject(type);
	     object_addShort(s);
	     object_addShort(w);
	     if (sectorprops[s].type==SR_SPECIAL_ROLE_17)
		{object_addShort(1); /* switched on */
		 object_addShort(sectorprops[s].index);
		 /*with channel*/
		}
	     else
		if (sectorprops[s].type==SR_SPECIAL_ROLE_21)
		   {object_addShort(0); /* switched off */
		    object_addShort(sectorprops[s].index);
		    /*with channel*/
		   }
		else
		   if (sectorprops[s].type==SR_SPECIAL_ROLE_22)
		      {object_addShort(2); /* toggled (start off)*/
		       object_addShort(sectorprops[s].index);
		      }
		   else
		      if (sectorprops[s].type==SR_SPECIAL_ROLE_24)
			 {object_addShort(3); /* toggled (start on)*/
			  object_addShort(sectorprops[s].index);
			 }
		      else
			 {object_addShort(-1);
			  object_addShort(-1);
			 }
	     /* tile's center */
	     {int x,y,z;
	      findFaceCenter(i,&x,&y,&z);
	      object_addShort(x);
	      object_addShort(y);
	      object_addShort(z);
	     }
	     /* add normal */
	     object_addInt(outWalls[w].normal[0]);
	     object_addInt(outWalls[w].normal[1]);
	     object_addInt(outWalls[w].normal[2]);
	     continue;
	    }
	 if (wallTileType[face[i].tile]==WT_SW1)
	    type=OT_SW1;
	 if (wallTileType[face[i].tile]==WT_SW2)
	    type=OT_SW2;
	 if (wallTileType[face[i].tile]==WT_SW3)
	    type=OT_SW3;
	 if (wallTileType[face[i].tile]==WT_SW4)
	    type=OT_SW4;
	 if (type!=-1)
	    {printf("Found switch on wall %d\n",w);
	     object_addObject(type);
	     object_addShort(s);
	     if (sectorprops[s].type==SR_SPECIAL_ROLE_11)
		object_addShort(sectorprops[s].index); /* channel */
	     else
		object_addShort(1);
	     {int x,y,z;
	      findFaceCenter(i,&x,&y,&z);
	      object_addShort(x);
	      object_addShort(y);
	      object_addShort(z);
	     }
	     continue;
	    }
	}
    }
}

static int puzzleReturn[]={-1,-1,-1,-1,-1,-1,-1};
static int puzzleObject[]={-1,-1,-1,-1,-1,-1,-1};
static int puzzleToTable[]={13,23,24,25,26,27,28,29};
static int puzzleArtifactTable[]={-1,2,5,3,4,1,0};

/* returns level size */
int convertLevel(void)
{int s,w,i;
 nmNonRect=0;
 nmInvisible=0;

 nmVertex=0;
 nmFaces=0;
 nmTextures=0;
 nmLight=0;
 nmWalls=0;
 nmSectors=0;
 nmObjectParams=0;
 nmObjects=0;
 nmPBs=0;
 nmPBVert=0;
 nmPBWalls=0;
 nmWaveVert=0;
 nmWaveFace=0;

 clearVertexMap();

#if ELEVATOR
 adjustElevatorGeometry();
#endif
 printf("Adding sectors...\n");
 for (s=0;s<sectorcount;s++)
    addSector(sector+s,s);
 printf("Done...\n");
 sortWalls();

 doTileTags();
 checkLevel();
 findCutPlanes();

 checkForConcaveSectors();

 for (i=0;i<nmSectors;i++)
    {switch (sectorprops[i].type)
	{case SR_LIFT_SHAFT_STYLE_1 ... SR_LIFT_SHAFT_STYLE_20:
	    makeElevator(i);
	    break;
	 case SR_DOORWAY_STYLE_1 ... SR_DOORWAY_STYLE_20:
	    makeDoorWay(i,sectorprops[i].type);
	    break;
	 case SR_SPECIAL_ROLE_2:
	    makeBobbingBlock(i);
	    break;
	 case SR_SPECIAL_ROLE_10:
	    /* add sector switch */
	    object_addSpriteObject(OT_SECTORSWITCH,i);
	    object_addShort(sectorprops[i].index); /* channel */
	    break;
	 case SR_SPECIAL_ROLE_16:
	    /* water sector */
	    outSector[i].flags|=SECFLAG_WATER;
	    levelHasWater=1;
	    break;
	 case SR_SPECIAL_ROLE_9:
	 case SR_SPECIAL_ROLE_23:
	    if (sectorprops[i].type==SR_SPECIAL_ROLE_9)
	       ram_homeSector=i;
	    if (sectorprops[i].type==SR_SPECIAL_ROLE_23)
	       ram_triggerSector=i;
	    if (ram_homeSector>=0 && ram_triggerSector>=0)
	       {object_addObject(OT_RAMSESTRIGGER);
		object_addShort(ram_triggerSector);
		object_addShort(ram_homeSector);
	       }
	 case SR_SPECIAL_ROLE_26:
	    outSector[i].flags|=SECFLAG_EXPLODINGWALLS;
	    outSector[i].cutChannel=sectorprops[i].index;
	    break;
	 case SR_SPECIAL_ROLE_12:
	    object_addObject(OT_TELEPSECTOR);
	    object_addShort(i); /* telep sector */
	    object_addShort(0); /* not end of puzzle */
	    assert(sectorprops[i].index<8);
	    assert(sectorprops[i].index>=0);
	    object_addShort(puzzleToTable[sectorprops[i].index]);
	    break;
	 case SR_SPECIAL_ROLE_15:
	    object_addObject(OT_TELEPSECTOR);
	    object_addShort(i); /* telep sector */
	    object_addShort(1); /* yes end of puzzle */
	    object_addShort(13);
	    break;
	 case SR_SPECIAL_ROLE_13:
	 case SR_SPECIAL_ROLE_14:
	    {int index=sectorprops[i].index;
	     assert(index>=1);
	     assert(index<=6);
	     if (sectorprops[i].type==SR_SPECIAL_ROLE_13)
		puzzleReturn[index]=i;
	     if (sectorprops[i].type==SR_SPECIAL_ROLE_14)
		puzzleObject[index]=i;
	     if (puzzleReturn[index]>=0 && puzzleObject[index]>=0)
		{object_addObject(OT_TELEPRETURN);
		 object_addShort(puzzleReturn[index]);
		 object_addShort(puzzleToTable[index]);
		 object_addShort(puzzleObject[index]);
		 object_addShort(puzzleArtifactTable[index]);
		}
	     break;
	    }
	 case SR_SPECIAL_ROLE_27:
	    outSector[i].flags|=SECFLAG_NOMAP;
	    break;
	   }
    }

 checkForDuplicateWalls();

 addJeffObjects();
 addExtraObjects();


 initWater();
 setBlockingBits();

 /* spread no map tags */
 {int done;
  int s,w,s2;
  do
     {done=1;
      for (s=0;s<nmSectors;s++)
	 {if (sectorprops[s].type>=SR_DOORWAY_STYLE_1 &&
	      sectorprops[s].type<=SR_DOORWAY_STYLE_20)
	     continue;
	  if (outSector[s].flags & SECFLAG_NOMAP)
	     for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
		{s2=outWalls[w].nextSector;
		 if (s2==-1)
		    continue;
		 if (outWalls[w].flags & WALLFLAG_BLOCKED)
		    continue;
		 if (outSector[s2].flags & SECFLAG_NOMAP)
		    continue;
		 outSector[s2].flags|=SECFLAG_NOMAP;
		 done=0;
		}
	 }
     }
  while (!done);
 }

 nmVertex=(nmVertex+1)&(~1);

 i=sizeof(struct sLevelHeader)+
   nmSectors*sizeof(sSectorType)+
   nmWalls*sizeof(sWallType)+
   nmVertex*sizeof(sVertexType)+
   nmFaces*sizeof(sFaceType)+
   nmTextures*sizeof(unsigned char)+
   nmLight*sizeof(unsigned char)+
   MAXCUTSECTORS*nmCutSectors*sizeof(char)+
   nmObjects*sizeof(sObjectType)+
   nmObjectParams*sizeof(unsigned char)+
   nmPBs*sizeof(sPBType)+
   nmPBVert*sizeof(sPBVertex)+
   nmPBWalls*sizeof(short)+
   nmWaveVert*sizeof(WaveVert)+
   nmWaveFace*sizeof(WaveFace);
 return i;
}


void writeLevel(void)
{int s,w;

 printf("Checking level...\n");
 checkLevel();
 printf("done\n");

 writeInt(nmSectors);
 writeInt(nmWalls);
 writeInt(nmVertex);
 writeInt(nmFaces);
 writeInt(nmTextures);
 writeInt(nmLight);
 writeInt(nmObjects);
 writeInt(nmObjectParams);
 writeInt(nmPBs);
 writeInt(nmPBWalls);
 writeInt(nmPBVert);
 writeInt(nmWaveVert);
 writeInt(nmWaveFace);
 writeInt(nmCutSectors);
 printf("---------------------------------------------\n");
 printf("NmSectors=%d\n",nmSectors);
 for (s=0;s<nmSectors;s++)
    {writeInt(0);
     writeShort(outSector[s].center[0]);
     writeShort(outSector[s].center[1]);
     writeShort(outSector[s].center[2]);
     writeShort(outSector[s].floorLevel);
     writeShort(outSector[s].firstWall);
     writeShort(outSector[s].lastWall);
     writeShort(outSector[s].light);
     writeShort(outSector[s].flags);
     writeChar(outSector[s].cutIndex);
     writeChar(outSector[s].cutChannel);
     writeShort(0);
    }
 printf("NmWalls=%d (%d rect, %d non-rect, %d boundry)\n",
	nmWalls,
	nmWalls-nmNonRect-nmInvisible,
	nmNonRect,
	nmInvisible);
 for (s=0;s<nmWalls;s++)
    {writeInt(outWalls[s].normal[0]);
     writeInt(outWalls[s].normal[1]);
     writeInt(outWalls[s].normal[2]);
     writeInt(outWalls[s].d);
     writeInt(0);
     writeShort(outWalls[s].flags);
     writeShort(outWalls[s].textures);
     writeShort(outWalls[s].firstFace);
     writeShort(outWalls[s].lastFace);
     writeShort(outWalls[s].firstVertex);
     writeShort(outWalls[s].lastVertex);
     writeShort(outWalls[s].v[0]);
     writeShort(outWalls[s].v[1]);
     writeShort(outWalls[s].v[2]);
     writeShort(outWalls[s].v[3]);
     writeShort(outWalls[s].nextSector);
     writeShort(outWalls[s].firstLight);
     writeShort(outWalls[s].pixelLength);
     writeChar(outWalls[s].tileLength);
     writeChar(outWalls[s].tileHeight);
    }
 printf("NmVert=%d\n",nmVertex);
 for (s=0;s<nmVertex;s++)
    {writeShort(f(outVert[s].x));
     writeShort(f(outVert[s].y));
     writeShort(f(outVert[s].z));
     writeChar(outVert[s].light);
     writeChar(0);
    }
 printf("NmFaces=%d\n",nmFaces);
 for (s=0;s<nmFaces;s++)
    {writeShort(outFace[s].v[0]);
     writeShort(outFace[s].v[1]);
     writeShort(outFace[s].v[2]);
     writeShort(outFace[s].v[3]);
     writeChar(outFace[s].tile);
     writeChar(0);
/*     writeShort(0);*/
    }
 printf("NmObjects=%d\n",nmObjects);
 for (s=0;s<nmObjects;s++)
    {writeShort(outObjects[s].type);
     writeShort(outObjects[s].firstParam);
    }
 printf("NmPbs=%d\n",nmPBs);
 for (s=0;s<nmPBs;s++)
    {writeShort(outPBs[s].enclosingSector);
     writeShort(outPBs[s].startWall);
     writeShort(outPBs[s].endWall);
     writeShort(outPBs[s].startVertex);
     writeShort(outPBs[s].endVertex);
     writeShort(outPBs[s].floorSector);
     writeInt(0);
     writeShort(0);
    }
 printf("NmPBVert=%d\n",nmPBVert);
 for (s=0;s<nmPBVert;s++)
    {writeShort(outPBVertex[s].vStart);
     writeChar(outPBVertex[s].vNm);
     writeChar(outPBVertex[s].flags);
    }
 printf("NmWaveVert=%d\n",nmWaveVert);
 for (s=0;s<nmWaveVert;s++)
    {writeInt(outWaveVert[s].pos);
     writeInt(outWaveVert[s].vel);
     writeShort(outWaveVert[s].connect[0]);
     writeShort(outWaveVert[s].connect[1]);
     writeShort(outWaveVert[s].connect[2]);
     writeShort(outWaveVert[s].connect[3]);
    }
 printf("NmWaveFace=%d\n",nmWaveFace);
 for (s=0;s<nmWaveFace;s++)
    {writeShort(outWaveFace[s].connect[0]);
     writeShort(outWaveFace[s].connect[1]);
     writeShort(outWaveFace[s].connect[2]);
     writeShort(outWaveFace[s].connect[3]);
    }
 printf("NmPBWalls=%d\n",nmPBWalls);
 for (s=0;s<nmPBWalls;s++)
    writeShort(outPBWalls[s]);
 printf("NmObjectParams=%d\n",nmObjectParams);
 for (s=0;s<nmObjectParams;s++)
    writeChar(outObjectParams[s]);
 printf("NmTextures=%d\n",nmTextures);
 for (s=0;s<nmTextures;s++)
    {writeChar(outTexture[s]);
    }
 printf("NmLight=%d\n",nmLight);
 for (s=0;s<nmLight;s++)
    writeChar(outLight[s]);
#if 1
 printf("NmCut=%d\n",MAXCUTSECTORS*nmCutSectors);
 for (s=0;s<nmCutSectors;s++)
    for (w=0;w<MAXCUTSECTORS;w++)
       writeChar(outCutPlane[s][w]);
#endif
}

void addPBWall(int w)
{if (outWalls[w].flags & WALLFLAG_PARALLELOGRAM)
    {outPBVertex[nmPBVert].vStart=outWalls[w].v[0];
     outPBVertex[nmPBVert].vNm=4;
     outPBVertex[nmPBVert].flags=0;
     nmPBVert++;
    }
 else
    {outPBVertex[nmPBVert].vStart=outWalls[w].v[0];
     outPBVertex[nmPBVert].vNm=4;
     outPBVertex[nmPBVert].flags=0;
     nmPBVert++;

     outPBVertex[nmPBVert].vStart=outWalls[w].firstVertex;
     outPBVertex[nmPBVert].vNm=
	outWalls[w].lastVertex-outWalls[w].firstVertex+1;
     outPBVertex[nmPBVert].flags=0;
     nmPBVert++;
    }
 outPBWalls[nmPBWalls++]=w;
}

int findFloorHeight(int sector)
{int w;
 /* find floor */
 for (w=outSector[sector].firstWall;w<=outSector[sector].lastWall;
      w++)
    if (outWalls[w].normal[1]==1<<16)
       break;
 assert(w<=outSector[sector].lastWall);
 return outVert[outWalls[w].v[0]].y;
}

void makeBobbingBlock(int s)
{int mate,i,bestDistance,s1,w,w1;

 /* find bobbing sector under this one */
 bestDistance=INT_MAX;
 mate=-1;
 for (i=0;i<nmSectors;i++)
    {int d,x,z;
     if (i==s)
	continue;
     if (sectorprops[i].type!=SR_SPECIAL_ROLE_1 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_3 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_4 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_5 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_6 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_7 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_8 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_9 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_20 &&
	 sectorprops[i].type!=SR_SPECIAL_ROLE_25)
	continue;

     x=(outSector[s].center[0]-outSector[i].center[0]);
     x=x*x;
     z=(outSector[s].center[2]-outSector[i].center[2]);
     z=z*z;
     d=x+z;
     if (d<bestDistance)
	{bestDistance=d;
	 mate=i;
	}
    }
 assert(mate!=-1);
 switch (sectorprops[mate].type)
    {case SR_SPECIAL_ROLE_1 ... SR_SPECIAL_ROLE_7:
	object_addSpriteObject(OT_BOBBINGBLOCK,nmPBs);
	object_addShort(0);
	switch (sectorprops[mate].type)
	   {case SR_SPECIAL_ROLE_1: object_addShort(0); break;
	    case SR_SPECIAL_ROLE_3: object_addShort(60); break;
	    case SR_SPECIAL_ROLE_4: object_addShort(120); break;
	    case SR_SPECIAL_ROLE_5: object_addShort(180); break;
	    case SR_SPECIAL_ROLE_6: object_addShort(240); break;
	    case SR_SPECIAL_ROLE_7: object_addShort(300); break;
	   }
	break;
     case SR_SPECIAL_ROLE_8:
	object_addSpriteObject(OT_SINKINGBLOCK,nmPBs);
	break;
     case SR_SPECIAL_ROLE_9:
	object_addSpriteObject(OT_RAMSESLID,nmPBs);
	break;
     case SR_SPECIAL_ROLE_20:
	object_addSpriteObject(OT_BOBBINGBLOCK,nmPBs);
	object_addShort(1);
	object_addShort(sectorprops[mate].index);
	break;
     case SR_SPECIAL_ROLE_25:
	object_addSpriteObject(OT_EARTHQUAKEBLOCK,nmPBs);
	break;
       }
 outPBs[nmPBs].enclosingSector=s;
 outPBs[nmPBs].startVertex=nmPBVert;
 outPBs[nmPBs].startWall=nmPBWalls;
 outPBs[nmPBs].floorSector=s;

 /* add ceiling */
 {int fl;
  for (w=outSector[mate].firstWall;w<=outSector[mate].lastWall;w++)
     {if (outWalls[w].normal[1]<0)
	 {addPBWall(w);
	  fl=f(outVert[outWalls[w].v[0]].y)-outSector[mate].floorLevel;
	 }
      if (outWalls[w].nextSector!=-1)
	 {outPBVertex[nmPBVert].vStart=outWalls[w].v[0];
	  outPBVertex[nmPBVert].vNm=2;
	  outPBVertex[nmPBVert++].flags=0;
	 }
     }
 if (sectorprops[mate].type==SR_SPECIAL_ROLE_8)
    {object_addShort(fl);
     object_addShort(sectorprops[mate].index); /* reset channel */
     object_addShort(sectorprops[s].index);
    }
 }
 /* add floor */
 {for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
     {if (outWalls[w].normal[1]>0)
	 {addPBWall(w);
	 }
      if (outWalls[w].nextSector!=-1)
	 {outPBVertex[nmPBVert].vStart=outWalls[w].v[2];
	  outPBVertex[nmPBVert].vNm=2;
	  outPBVertex[nmPBVert++].flags=0;
	 }
     }
 }
 /* add walls into top sector */
 for (s1=0;s1<nmSectors;s1++)
    {int yes=-1;
     for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
	{if (outWalls[w].nextSector==s)
	    {yes=w;
	     outPBVertex[nmPBVert].vStart=outWalls[w].v[2];
	     outPBVertex[nmPBVert].vNm=2;
	     outPBVertex[nmPBVert++].flags=0;
	    }
	 if (outWalls[w].nextSector==mate)
	    {yes=w;
	     outPBVertex[nmPBVert].vStart=outWalls[w].v[0];
	     outPBVertex[nmPBVert].vNm=2;
	     outPBVertex[nmPBVert++].flags=0;
	    }
	}
     if (yes!=-1)
	{w=yes;
	 for (w1=outSector[s1].firstWall;w1<=outSector[s1].lastWall;w1++)
	    if (outWalls[w1].nextSector==-1 &&
		outWalls[w1].normal[0]==outWalls[w].normal[0] &&
		outWalls[w1].normal[1]==outWalls[w].normal[1] &&
		outWalls[w1].normal[2]==outWalls[w].normal[2] &&
		outVert[outWalls[w1].v[0]].x==outVert[outWalls[w].v[0]].x &&
		outVert[outWalls[w1].v[0]].z==outVert[outWalls[w].v[0]].z)
	       addPBWall(w1);
	}
    }
 outPBs[nmPBs].endVertex=nmPBVert-1;
 outPBs[nmPBs].endWall=nmPBWalls-1;
 nmPBs++;
}

void makeDoorWay(int s,int type)
{int w,w1;
 int s1,doorHeight;
 /* add a push block to the pb list with the sector's ceiling and
    all the other walls in the world that are in sectors that have a
    doorway into this sector and have a normal that is the same as that doorway
    */
 switch (type)
    {case SR_DOORWAY_STYLE_3:
	object_addSpriteObject(OT_BUGDOOR,nmPBs); break;
     case SR_DOORWAY_STYLE_4:
	object_addSpriteObject(OT_TIMEDOOR,nmPBs); break;
     case SR_DOORWAY_STYLE_5:
	object_addSpriteObject(OT_XDOOR,nmPBs); break;
     case SR_DOORWAY_STYLE_6:
	object_addSpriteObject(OT_PLANTDOOR,nmPBs); break;
     case SR_DOORWAY_STYLE_7:
	object_addSpriteObject(OT_STUCKUPDOOR,nmPBs); break;
     case SR_DOORWAY_STYLE_8:
	object_addSpriteObject(OT_STUCKDOWNDOOR,nmPBs); break;
     default:
	object_addSpriteObject(OT_NORMALDOOR,nmPBs);
	break;
     }
 if (sectorprops[s].type==SR_DOORWAY_STYLE_2 ||
     sectorprops[s].type==SR_DOORWAY_STYLE_3 ||
     sectorprops[s].type==SR_DOORWAY_STYLE_4 ||
     sectorprops[s].type==SR_DOORWAY_STYLE_5 ||
     sectorprops[s].type==SR_DOORWAY_STYLE_6 ||
     sectorprops[s].type==SR_DOORWAY_STYLE_7 ||
     sectorprops[s].type==SR_DOORWAY_STYLE_8
     )
    object_addShort(sectorprops[s].index); /* channel */
 else
    object_addShort(-1);
 outPBs[nmPBs].enclosingSector=s;
 outPBs[nmPBs].startVertex=nmPBVert;
 outPBs[nmPBs].startWall=nmPBWalls;
 outPBs[nmPBs].floorSector=-1;
 /* add floor */
 for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
    if (outWalls[w].normal[1]<0)
       addPBWall(w);
 doorHeight=0;
 for (s1=0;s1<nmSectors;s1++)
    for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
       if (outWalls[w].nextSector==s)
	  {outPBVertex[nmPBVert].vStart=outWalls[w].v[0];
	   outPBVertex[nmPBVert].vNm=2;
	   outPBVertex[nmPBVert++].flags=0;

	   outWalls[w].flags|=WALLFLAG_DOORWALL;
	   outPBWalls[nmPBWalls++]=w;

	   doorHeight=outVert[outWalls[w].v[0]].y-outVert[outWalls[w].v[3]].y;
	   /* fix for brew output doorways wrong */
	   outVert[outWalls[w].v[0]].y=outVert[outWalls[w].v[3]].y;
	   outVert[outWalls[w].v[1]].y=outVert[outWalls[w].v[2]].y;
	   for (w1=outSector[s1].firstWall;w1<=outSector[s1].lastWall;w1++)
	      if (outWalls[w1].nextSector==-1 &&
		  outWalls[w1].normal[0]==outWalls[w].normal[0] &&
		  outWalls[w1].normal[1]==outWalls[w].normal[1] &&
		  outWalls[w1].normal[2]==outWalls[w].normal[2] &&
		  outVert[outWalls[w1].v[0]].x==outVert[outWalls[w].v[0]].x &&
		  outVert[outWalls[w1].v[0]].y>outVert[outWalls[w].v[3]].y &&
		  outVert[outWalls[w1].v[0]].z==outVert[outWalls[w].v[0]].z)
		 addPBWall(w1);
	  }
 object_addShort(f(doorHeight));
 outPBs[nmPBs].endVertex=nmPBVert-1;
 outPBs[nmPBs].endWall=nmPBWalls-1;
 nmPBs++;
}

void makeElevator(int s)
{int w,w1;
 int s1,i;
 int upperFloorLevel;
 int lowerFloorLevel;
 /* add a push block to the pb list with the sector's ceiling and
    all the other walls in the world that are in sectors that have a
    doorway into this sector and have a normal that is the same as that doorway
    */
 printf("elevator in sector %d\n",s);
 switch (sectorprops[s].type)
    {case SR_LIFT_SHAFT_STYLE_4:
	object_addSpriteObject(OT_STARTSDOWNELEVATOR,nmPBs);
	break;
     case SR_LIFT_SHAFT_STYLE_3:
	object_addSpriteObject(OT_FLOORSWITCH,nmPBs);
	break;
     case SR_LIFT_SHAFT_STYLE_2:
	object_addSpriteObject(OT_STUCKDOWNELEVATOR,nmPBs);
	break;
     case SR_LIFT_SHAFT_STYLE_5:
	object_addSpriteObject(OT_DOWNTHENUPELEVATOR,nmPBs);
	break;
     default:
	object_addSpriteObject(OT_NORMALELEVATOR,nmPBs);
	break;
       }
 outPBs[nmPBs].enclosingSector=s;
 outPBs[nmPBs].startVertex=nmPBVert;
 outPBs[nmPBs].startWall=nmPBWalls;

 outPBs[nmPBs].floorSector=s;

 /* find coordinates of elevator stops */
 upperFloorLevel=-2000000000;
 lowerFloorLevel= 2000000000;
 for (s1=0;s1<nmSectors;s1++)
    {for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
	if (outWalls[w].nextSector==s)
	   break;
     if (w>outSector[s1].lastWall)
	continue;
     /* sector s1 ajoins the elevator sector */
     for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
	if (outWalls[w].normal[1]==F(1))
	   {if (outVert[outWalls[w].v[0]].y<lowerFloorLevel)
	       lowerFloorLevel=outVert[outWalls[w].v[0]].y;
	    if (outVert[outWalls[w].v[0]].y>upperFloorLevel)
	       upperFloorLevel=outVert[outWalls[w].v[0]].y;
	   }
    }
 assert(upperFloorLevel!=-2000000000);
 assert(lowerFloorLevel!=2000000000);

 {/* if lowerFloorLevel is below elevator's sector's floor then move it up */
  int sfl;
  sfl= 2000000000;
  for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
     if (outWalls[w].normal[1]==0)
	if (outVert[outWalls[w].v[2]].y<sfl)
	   sfl=outVert[outWalls[w].v[2]].y;
  if (lowerFloorLevel<sfl)
     lowerFloorLevel=sfl;
 }

 if (upperFloorLevel==lowerFloorLevel)
    for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
       if (outWalls[w].normal[1]<0)
	  upperFloorLevel=outVert[outWalls[w].v[0]].y;
 if (sectorprops[s].type==SR_LIFT_SHAFT_STYLE_3)
    upperFloorLevel=lowerFloorLevel+F(4);

 object_addShort(lowerFloorLevel>>16);
 object_addShort(upperFloorLevel>>16);

 switch (sectorprops[s].type)
    {case SR_LIFT_SHAFT_STYLE_3:
	lowerFloorLevel=upperFloorLevel-F(64);
	object_addShort(sectorprops[s].index); /* channel */
	break;
     case SR_LIFT_SHAFT_STYLE_5:
     case SR_LIFT_SHAFT_STYLE_2:
	object_addShort(sectorprops[s].index); /* channel */
	break;
     default:
	object_addShort(-1);
	break;
       }

 /* add floor */
 for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
    if (outWalls[w].normal[1]>0)
       {addPBWall(w);
	for (i=0;i<4;i++)
	   outVert[outWalls[w].v[i]].y=upperFloorLevel;
	if (!(outWalls[w].flags & WALLFLAG_PARALLELOGRAM))
	   {for (i=outWalls[w].firstVertex;i<=outWalls[w].lastVertex;i++)
	       outVert[i].y=upperFloorLevel;
	   }
       }
 /* add out red walls */
 for (w=outSector[s].firstWall;w<=outSector[s].lastWall;w++)
    if (outWalls[w].nextSector!=-1 &&
	outVert[outWalls[w].v[2]].y==lowerFloorLevel)
       {outPBVertex[nmPBVert].vStart=outWalls[w].v[2];
	outPBVertex[nmPBVert].vNm=2;
	outPBVertex[nmPBVert++].flags=0;

	outVert[outWalls[w].v[2]].y=upperFloorLevel;
	outVert[outWalls[w].v[3]].y=upperFloorLevel;
       }
 /* add in red walls & walls that belong to other sectors */
 for (s1=0;s1<nmSectors;s1++)
    for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
       if (outWalls[w].nextSector==s)
	  {if (outVert[outWalls[w].v[2]].y==lowerFloorLevel)
	      {outPBVertex[nmPBVert].vStart=outWalls[w].v[2];
	       outPBVertex[nmPBVert].vNm=2;
	       outPBVertex[nmPBVert++].flags=0;

	       outVert[outWalls[w].v[2]].y=upperFloorLevel;
	       outVert[outWalls[w].v[3]].y=upperFloorLevel;
	      }

	   for (w1=outSector[s1].firstWall;w1<=outSector[s1].lastWall;w1++)
	      if (outWalls[w1].nextSector==-1 &&
		  outWalls[w1].normal[0]==outWalls[w].normal[0] &&
		  outWalls[w1].normal[1]==outWalls[w].normal[1] &&
		  outWalls[w1].normal[2]==outWalls[w].normal[2] &&
		  outVert[outWalls[w1].v[0]].x==outVert[outWalls[w].v[0]].x &&
		  outVert[outWalls[w1].v[0]].z==outVert[outWalls[w].v[0]].z)
		 {addPBWall(w1);
		  if (!(outWalls[w1].flags & WALLFLAG_PARALLELOGRAM))
		     printf("Elevator adjoining wall not rect in s %d\n",s1);
		  outVert[outWalls[w1].v[0]].y=upperFloorLevel;
		  outVert[outWalls[w1].v[1]].y=upperFloorLevel;
		  outVert[outWalls[w1].v[2]].y=lowerFloorLevel;
		  outVert[outWalls[w1].v[3]].y=lowerFloorLevel;
		  outWalls[w1].tileHeight=
		     (((upperFloorLevel-lowerFloorLevel)>>16)+TILESIZE/2)
			/TILESIZE;
		 }
	  }
 outPBs[nmPBs].endVertex=nmPBVert-1;
 outPBs[nmPBs].endWall=nmPBWalls-1;
 nmPBs++;
}

char waveBuff[500000];
int waveSize,waveSampleRate,waveBPS;
int waveLoopStart; /* -1 if no loop */

int writeSound(void)
{int i,octave,fns;
 double x;
 int wrote;
 x=log(waveSampleRate)/log(2.0);
 x=x-log(44100.0)/log(2.0);
 octave=floor(x);
 fns=fabs((x-floor(x))*1024.0);
/* printf("Octave=%d fns=%d\n",octave,fns);*/
 assert((fns & 0x03ff)==fns);

 writeInt(waveSize);
 writeInt(((octave<<11)&0x7800)|fns);
 writeInt(waveBPS);
 writeInt(waveLoopStart);

 if (waveBPS==16)
    {/* size better be even */
     assert(!(waveSize & 1));
     wrote=0;
     for (i=0;i<waveSize;i+=2)
	{writeChar(waveBuff[i+1]);
	 wrote++;
	 writeChar(waveBuff[i]);
	 wrote++;
	}
     assert(wrote==waveSize);
    }
 else
    for (i=0;i<waveSize;i++)
       writeChar(waveBuff[i] ^ 0x80);
 return waveSize+12;
}

void loadVoc(char *name)
{int i;
 FILE *ifile;
 int size;
 char type;
 ifile=fopen(name,"rb");
 if (!ifile)
    {printf("Can't load voc file %s\n",name);
     exit(0);
    }
 fread(waveBuff,19,1,ifile);
 waveBuff[19]=0;
 if (strcmp(waveBuff,"Creative Voice File"))
    {printf("Voc file '%s' bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(waveBuff,7,1,ifile);
 waveSize=0;
 waveLoopStart=-1;
 /* read blocks */
 while (1)
    {fread(&type,1,1,ifile);
     switch (type)
	{case 0:
	    printf("Terminate\n");
	    return;
	 case 1:
	    size=0;
	    fread(&size,3,1,ifile);
	    printf("Sound Data size=%d\n",size);
	    assert(0);
	    break;
	 case 2:
	    printf("Sound continue\n");
	    assert(0);
	    break;
	 case 3:
	    printf("Silence\n");
	    assert(0);
	    break;
	 case 4:
	    printf("Marker\n");
	    assert(0);
	    break;
	 case 5:
	    printf("Ascii string\n");
	    assert(0);
	    break;
	 case 6:
	    i=0;
	    fread(&i,3,1,ifile);
	    assert(i==2);
	    i=0;
	    fread(&i,2,1,ifile);
	    printf("Repeat %d\n",i);
	    assert(i==0x0ffff);
	    waveLoopStart=waveSize;
	    break;
	 case 7:
	    printf("End Repeat\n");
	    assert(0);
	    break;
	 case 9:
	    {char c;
	     int i;
	     printf("Sound Data\n");
	     size=0;
	     fread(&size,3,1,ifile);
	     printf("Size=%d\n",size);
	     fread(&waveSampleRate,4,1,ifile);
	     printf("SampleRate=%d\n",waveSampleRate);
	     waveBPS=0;
	     fread(&waveBPS,1,1,ifile);
	     printf("Wave BPS=%d\n",waveBPS);
	     assert(waveBPS==8);
	     fread(&c,1,1,ifile);
	     printf("Channels=%d\n",c);
	     assert(c==1);
	     i=0;
	     fread(&i,2,1,ifile);
	     printf("Format=%d\n",i);
	     assert(!i);
	     fread(&i,4,1,ifile);
	     /* read wave data */
	     fread(waveBuff+waveSize,size-12,1,ifile);
	     waveSize+=size-12;
	     break;
	    }
	 default:
	    printf("Unknown block type %d in voc\n",type);
	    assert(0);
	    break;
	   }
    }
 fclose(ifile);
}


void loadWave(char *name)
{FILE *ifile;
 int size;
 ifile=fopen(name,"rb");
 if (!ifile)
    {printf("Can't load wave file %s\n",name);
     exit(0);
    }
 waveLoopStart=-1;
 fread(waveBuff,8,1,ifile);
 fread(waveBuff,4,1,ifile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"WAVE"))
    {printf("Wave file '%s' bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(waveBuff,4,1,ifile);
 fread(&size,4,1,ifile);

 fread(waveBuff,4,1,ifile);
 fread(&waveSampleRate,4,1,ifile);
 fread(waveBuff,6,1,ifile);
 waveBPS=0;
 fread(&waveBPS,2,1,ifile);
 fread(waveBuff,size-16,1,ifile);

 do /* read blocks until we find the data one */
    {fread(waveBuff,4,1,ifile);
     waveBuff[4]=0;
     if (strcmp(waveBuff,"data"))
	{fread(&size,4,1,ifile);
	 printf("Got unknown block %s, size %d\n",waveBuff,size);
	 fread(waveBuff,size,1,ifile);
	 if (feof(ifile))
	     {printf("Wave file '%s' half bad! %s\n",name,waveBuff);
	      exit(-1);
	     }
	}
    }
 while (strcmp(waveBuff,"data")) ;

 fread(&waveSize,4,1,ifile);
 if (!waveSize)
    {printf("Wave size==0!\n");
     exit(-1);
    }
 printf("Wave: %s \trate: %d \tsize: %d\n",name,waveSampleRate,waveSize);

 fread(waveBuff,1,waveSize,ifile);
 fclose(ifile);
}

#define MAXNMSOUNDS 100

int nmSounds=0;
char soundList[MAXNMSOUNDS][80];

int addSound(char *filename)
{assert(nmSounds<MAXNMSOUNDS);
 printf("adding sound %s\n",filename);
 strcpy(soundList[nmSounds],filename);
 return nmSounds++;
}

int maybeAddSound(char *filename)
{int i;
 for (i=0;i<nmSounds;i++)
    if (!stricmp(filename,soundList[i]))
       return i;
 return addSound(filename);
}

int writeSoundSet(void)
{int i;
 int size;
 char buff[160];
 size=0;
 writeInt(OT_NMTYPES);
 for (i=0;i<OT_NMTYPES;i++)
    writeShort(outSoundMap[i]);
 writeInt(nmSounds);
 for (i=0;i<nmSounds;i++)
    {strcpy(buff,"sounds\\");
     strcat(buff,soundList[i]);
     if (strstr(buff,".wav"))
	loadWave(buff);
     else
	loadVoc(buff);
     size+=writeSound();
    }
 return size;
}

#define MAXNMTHINGS 2000
short sequenceIndex[MAXNMTHINGS],frameList[MAXNMTHINGS],
   frameFlags[MAXNMTHINGS];
short chunkx[MAXNMTHINGS],chunky[MAXNMTHINGS],chunkPic[MAXNMTHINGS],
   chunkFlags[MAXNMTHINGS];
int tileMap[MAXNMTHINGS];
short frameSounds[MAXNMTHINGS];
char soundMap[MAXNMTHINGS][80];
char tileNames[MAXNMTHINGS][80];
int loadSequenceSet(char *filename,int extraFlags)
{int i,c,s;
 FILE *ifile;
 char buff[160];
 char path[160];
 char *p;
 short nmSounds,nmTiles,nmSeq,nmFrm,nmChnk;
 int retVal;
 printf("Loading sequence set %s\n",filename);
 retVal=nmSequences;
 strcpy(path,filename);
 for (p=path+strlen(path)-1;p>path && *p!='\\';p--) ;
 *p=0;
 if (p!=path)
    strcat(path,"\\");

 strcpy(buff,"tiles\\");
 strcat(buff,filename);
 ifile=fopen(buff,"rb");
 if (!ifile)
    {printf("Can't open sequence file `%s'.\n",buff);
     exit(-1);
    }
 fread(buff,2,1,ifile);
 if (buff[0]!='P' || buff[1]!='S')
    {printf("Sequence file %s is bad.\n",filename);
     exit(-1);
    }

 /* load sounds */
 fread(&nmSounds,2,1,ifile);
 for (i=0;i<nmSounds;i++)
    {p=buff;
     while (1)
	{fread(p,1,1,ifile);
	 if (*p=='\n')
	    break;
	 p++;
	}
     *p=0;
     strcpy(soundMap[i],buff);
    }

 fread(&nmTiles,2,1,ifile);
 assert(nmTiles<MAXNMTHINGS);
 for (i=0;i<nmTiles;i++)
    {p=tileNames[i];
     while (1)
	{fread(p,1,1,ifile);
	 if (*p=='\n')
	    break;
	 p++;
	}
     *p=0;
    }

 fread(&nmSeq,2,1,ifile);
 assert(nmSeq<MAXNMTHINGS);
 fread(sequenceIndex,nmSeq,2,ifile);
 for (i=0;i<nmSeq;i++)
    {outSequenceList[nmSequences+i]=nmFrames+sequenceIndex[i];
    }
 nmSequences+=nmSeq;
 assert(nmSequences<32000);

 fread(&nmFrm,2,1,ifile);
 assert(nmFrm<MAXNMTHINGS);
 fread(frameList,nmFrm,2,ifile);
 fread(frameFlags,nmFrm,2,ifile);

 fread(&nmChnk,2,1,ifile);
 assert(nmChnk<MAXNMTHINGS);
 fread(chunkx,nmChnk,2,ifile);
 fread(chunky,nmChnk,2,ifile);
 fread(chunkPic,nmChnk,2,ifile);
 fread(chunkFlags,nmChnk,2,ifile);

 /* load tiles */
 for (i=0;i<nmTiles;i++)
    {char buff2[80];
     for (c=0;c<nmChnk;c++)
	if (chunkPic[c]==i)
	   break;
     if (c==nmChnk)
	{printf("Tile %s not used\n",tileNames[i]);
	 continue;
	}
     strcpy(buff2,path);
     strcat(buff2,tileNames[i]);
     {int flags=TILEFLAG_64x64|TILEFLAG_8BPP|TILEFLAG_PALLETE|extraFlags;
      if (flags & TILEFLAG_16BPP)
	 flags=flags & ~TILEFLAG_8BPP;
      tileMap[i]=addTile(buff2,flags);
     }
    }

 fread(frameSounds,nmFrm,2,ifile);

 for (i=0;i<nmFrm;i++)
    {outFrames[nmFrames].chunkIndex=nmChunks;
     outFrames[nmFrames].flags=frameFlags[i];
      if (!frameSounds[i])
	outFrames[nmFrames].sound=-1;
     else
	{assert((frameSounds[i]&0xff)>0);
	 assert((frameSounds[i]&0xff)<=nmSounds);
	 outFrames[nmFrames].sound=
	    maybeAddSound(soundMap[(frameSounds[i]&0xff)-1]);
	}
     nmFrames++;
     {int frameSize;
      TileInfo *ti;
      if (i<nmFrm-1)
	 frameSize=frameList[i+1]-frameList[i];
      else
	 frameSize=nmChnk-frameList[i];

      for (c=frameList[i];c<frameList[i]+frameSize;c++)
	 {ti=tiles+tileMap[chunkPic[c]];
	  for (s=0;s<ti->nmSubTiles;s++)
	     {outChunk[nmChunks].chunkx=chunkx[c]+ti->subx[s];
	      outChunk[nmChunks].chunky=chunky[c]+ti->suby[s];
	      outChunk[nmChunks].tile=ti->number+s;
	      outChunk[nmChunks].flags=chunkFlags[c];
	      if (outChunk[nmChunks].flags & 1)
		 {/* flip across x axis */
		  if (ti->width<=32 && ti->height<=32) /* new class stuff */
		     outChunk[nmChunks].chunkx=
			chunkx[c]+
			   ti->width-(ti->subx[s]+32);
		  else
		     outChunk[nmChunks].chunkx=
			chunkx[c]+
			   ti->width-(ti->subx[s]+64);
		 }
	      /*printf("#%d: x%d y%d tile%d %s\n",nmChunks,
		     outChunk[nmChunks].chunkx,
		     outChunk[nmChunks].chunky,
		     outChunk[nmChunks].tile,
		     tiles[outChunk[nmChunks].tile].filename);
	      getch();*/
	      nmChunks++;
	      assert(nmChunks<32000);
	     }
	 }
     }
    }
 fclose(ifile);
 return retVal;
}


int loadSequences(void)
{int o,size,i;
 nmSequences=0;
 nmFrames=0;
 nmChunks=0;
 for (i=0;i<OT_NMTYPES;i++)
    {outSequenceMap[i]=-2;
     outSoundMap[i]=-2;
    }

 /* find anim tiles */
 {int i,j;
  static struct {char *tilename,*seqName; int type;} animMap[]=
  {
   {"animated\\lava\\k17.bmp","animated\\lava\\chaos1.seq",OT_ANIM_CHAOS1},
   {"animated\\lava\\k20.bmp","animated\\lava\\choas2.seq",OT_ANIM_CHAOS2},
   {"animated\\lava\\k23.bmp","animated\\lava\\choas3",OT_ANIM_CHAOS3},
   {"animated\\lava\\m1.bmp","animated\\lava\\lava1.seq",OT_ANIM_LAVA1},
   {"animated\\lava\\m5.bmp","animated\\lava\\lava2.seq",OT_ANIM_LAVA2},
   {"animated\\lava\\m10.bmp","animated\\lava\\lava3.seq",OT_ANIM_LAVA3},
   {"animated\\lava\\m22.bmp","animated\\lava\\lavafall.seq",OT_ANIM_LAVAFALL},
   {"animated\\lava\\m16.bmp","animated\\lava\\lavapo~1.seq",OT_ANIM_LAVAPO1},
   {"animated\\lava\\m19.bmp","animated\\lava\\lavapo~2.seq",OT_ANIM_LAVAPO2},
   {"animated\\lava\\m13.bmp","animated\\lava\\lavahead.seq",OT_ANIM_LAVAHEAD},
   {"animated\\fcefield\\force1a.bmp","animated\\fcefield\\force1",
       OT_ANIM_FORCEFIELD},
   {"animated\\lava\\lavfal1.bmp","animated\\lava\\lavafal1.seq",
       OT_ANIM_LAVAFAL1},
   {"animated\\teleport\\port1a.bmp","animated\\teleport\\telport1.seq",
       OT_ANIM_TELEP1},
   {"animated\\teleport\\port2a.bmp","animated\\teleport\\telport2.seq",
       OT_ANIM_TELEP2},
   {"animated\\teleport\\port3a.bmp","animated\\teleport\\telport3.seq",
       OT_ANIM_TELEP3},
   {"animated\\teleport\\port4a.bmp","animated\\teleport\\telport4.seq",
       OT_ANIM_TELEP4},
   {"animated\\teleport\\port5a.bmp","animated\\teleport\\telport5.seq",
       OT_ANIM_TELEP5},
#if 0
   {"animated\\water\\wet34.bmp","animated\\water\\sand.seq",OT_ANIM_WSAND},
   {"animated\\water\\wet42.bmp","animated\\water\\brick.seq",OT_ANIM_WBRICK},
#endif
   {"switches\\0280.bmp","switches\\swc1.seq",OT_SW1},
   {"switches\\0282.bmp","switches\\swc2.seq",OT_SW2},
   {"switches\\0285.bmp","switches\\swc3.seq",OT_SW3},
   {"switches\\0295.bmp","switches\\swc4.seq",OT_SW4},
   {"animated\\swamp\\goop1.bmp","animated\\swamp\\goop.seq",OT_ANIM_SWAMP},
#if 0
   {"stages\\sunken\\metal1.bmp","stages\\sunken\\metaldrt.seq",OT_ANM1},
   {"stages\\sunken\\metal2.bmp","stages\\sunken\\metaldrb.seq",OT_ANM2},
   {"stages\\sunken\\x1.bmp","stages\\sunken\\xdoort.seq",OT_ANM3},
   {"stages\\sunken\\x2.bmp","stages\\sunken\\xdoorb.seq",OT_ANM4},
   {"stages\\sunken\\ydoor1.bmp","stages\\sunken\\ydort.seq",OT_ANM5},
   {"stages\\sunken\\ydoor2.bmp","stages\\sunken\\ydorb.seq",OT_ANM6},
   {"stages\\sunken\\z1.bmp","stages\\sunken\\zdort.seq",OT_ANM7},
   {"stages\\sunken\\z2.bmp","stages\\sunken\\zdorb.seq",OT_ANM8},
   {"stages\\sunken\\undor1.bmp","stages\\sunken\\undor.seq",OT_ANM9},
   {"stages\\sunken\\blok1.bmp","stages\\sunken\\block.seq",OT_ANM10},
   {"animated\\water\\capt0038.bmp","animated\\water\\water.seq",OT_ANM11},
#endif
   {"animated\\water\\fall1.bmp","animated\\water\\fall.seq",OT_ANM12},
   {NULL,NULL,0}
  };
  for (i=0;i<nmWallTileMaps;i++)
     {for (j=0;animMap[j].tilename;j++)
	 {if (!stricmp(translateWallTileNm(i),animMap[j].tilename))
	     outSequenceMap[animMap[j].type]=
		loadSequenceSet(animMap[j].seqName,TILEFLAG_16BPP);
	 }
     }

 }
 if (levelHasWater)
    {outSequenceMap[OT_1BUBBLE]=loadSequenceSet("1bubble.seq",0);

     /* outSequenceMap[OT_BUBBLES]=loadSequenceSet("bubls.seq",0); */
     outSequenceMap[OT_BUBBLES]=outSequenceMap[OT_1BUBBLE];

     outSequenceMap[OT_SPLASH]=loadSequenceSet("splash.seq",0);
     outSequenceMap[OT_GRENBUBB]=loadSequenceSet("grenbubb.seq",0);
     outSoundMap[OT_GRENBUBB]=nmSounds;
     addSound("grn_boom.wav");
    }

 if (levelHasExplodeWalls)
    outSequenceMap[OT_BRICK]=loadSequenceSet("bricks.seq",0);

 outSequenceMap[OT_KAPOW]=loadSequenceSet("kapow.seq",0);
 outSequenceMap[OT_FLAMEBALL]=loadSequenceSet("ball1.seq",0);
 outSequenceMap[OT_FLAMEWALL]=loadSequenceSet("flameth.seq",0);
 outSequenceMap[OT_GRENADE]=loadSequenceSet("grenroll.seq",0);
 outSequenceMap[OT_LANDGUTS]=loadSequenceSet("gutss1.seq",0);
 outSequenceMap[OT_AIRGUTS]=loadSequenceSet("gutss2.seq",0);
 outSequenceMap[OT_BLUEZORCH]=loadSequenceSet("blue.seq",0);
 outSequenceMap[OT_REDZORCH]=loadSequenceSet("red.seq",0);
 outSequenceMap[OT_GRENPOW]=loadSequenceSet("grenpoww.seq",0);

 outSequenceMap[OT_AMMOBALL]=loadSequenceSet("ammo.seq",0);
 outSequenceMap[OT_HEALTHBALL]=loadSequenceSet("health.seq",0);
 outSequenceMap[OT_AMMOORB]=outSequenceMap[OT_AMMOBALL];
 outSequenceMap[OT_HEALTHORB]=outSequenceMap[OT_HEALTHBALL];
 outSequenceMap[OT_INVISIBLEBALL]=loadSequenceSet("greenorb.seq",0);
 outSequenceMap[OT_WEAPONPOWERBALL]=loadSequenceSet("purporb.seq",0);
 outSequenceMap[OT_EYEBALL]=loadSequenceSet("eyeorb.seq",0);

 outSequenceMap[OT_COBRA]=loadSequenceSet("cobball.seq",0);
 outSequenceMap[OT_RINGO]=loadSequenceSet("ringo.seq",0);
/* outSequenceMap[OT_RACLOUD]=loadSequenceSet("eyehit.seq",0); */

 for (o=0;o<nmObjects;o++)
    {if (outSequenceMap[outObjects[o].type]!=-2)
	continue;
     switch (outObjects[o].type)
	{case OT_RAMMUMMY:
	    outSequenceMap[OT_RAMMUMMY]=
	       loadSequenceSet("stndcorp.seq",TILEFLAG_RLE);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_FORCEFIELD:
	    outSequenceMap[outObjects[o].type]=0;
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("ff_off.wav");
	    addSound("ff_on2.wav");
	    break;
	 case OT_RAMSIDEMUMMY:
	    outSequenceMap[OT_RAMSIDEMUMMY]=
	       loadSequenceSet("laidcorp.seq",TILEFLAG_RLE);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
         case OT_CHOPPER:
	    outSequenceMap[OT_CHOPPER]=loadSequenceSet("chopper.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
         case OT_SHOOTER1:
	    if (outSequenceMap[OT_MAGBALL]==-2)
	       outSequenceMap[OT_MAGBALL]=loadSequenceSet("projlava.seq",0);
	    outSequenceMap[outObjects[o].type]=0;
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("wallshot.wav");
	    break;
	 case OT_SHOOTER2:
	    outSequenceMap[outObjects[o].type]=0;
	    outSequenceMap[OT_BOINGROCK]=loadSequenceSet("rollrock.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("set_land.wav");
	    /* addSound("soft_pow.wav"); */
	    break;
         case OT_BLOB:
	    outSequenceMap[OT_BLOB]=loadSequenceSet("blob.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("ballopen.wav");
	    break;
	 case OT_FLOORSWITCH:
	    outSequenceMap[outObjects[o].type]=0;
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("switch1.wav");
	    break;
	 case OT_RAMSESLID:
	    outSequenceMap[OT_RAMSESLID]=0;
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("bak\\set_land.wav");
	    break;
	 case OT_SINKINGBLOCK:
	    outSequenceMap[OT_SINKINGBLOCK]=0;
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("bak\\set_land.wav");
	    break;
         case OT_SPIDER:
	    outSequenceMap[OT_SPIDER]=loadSequenceSet("spiderr.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("spi_hurt.wav");
	    addSound("spi_newd.wav");
	    addSound("spi_jump.wav");
	    addSound("spi_bite.wav");
	    break;
	 case OT_MUMMY:
	    outSequenceMap[OT_MUMMY]=loadSequenceSet("mummyy.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("m-deth3.wav");
	    outSequenceMap[OT_MUMBALL]=loadSequenceSet("projmumm.seq",0);
	    break;
	 case OT_BASTET:
	    outSequenceMap[OT_BASTET]=loadSequenceSet("lionn.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("cat_icu.wav");
	    addSound("cat_xpld.wav");
	    break;
	 case OT_ANUBIS:
	    outSequenceMap[OT_ANUBIS]=loadSequenceSet("anubiss.seq",0);
	    outSequenceMap[OT_ANUBALL]=loadSequenceSet("anuballl.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("anu-icu.wav");
	    addSound("anu_xpld.wav");
	    break;
	 case OT_HAWK:
	    outSequenceMap[OT_HAWK]=loadSequenceSet("hawkk.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("hwk_cry.wav");
	    break;
	 case OT_WASP:
	    outSequenceMap[OT_WASP]=loadSequenceSet("wasp.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("wasp_dy1.wav");
	    break;
	 case OT_FISH:
	    outSequenceMap[OT_FISH]=loadSequenceSet("fishh.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("fish_see.wav");
	    addSound("fish_die.wav");
	    break;
	 case OT_BUGKEY:
	    outSequenceMap[OT_BUGKEY]=loadSequenceSet("bugkey.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_TIMEKEY:
	    outSequenceMap[OT_TIMEKEY]=loadSequenceSet("timekey.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_XKEY:
	    outSequenceMap[OT_XKEY]=loadSequenceSet("xkey.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_PLANTKEY:
	    outSequenceMap[OT_PLANTKEY]=loadSequenceSet("plantkey.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_M60:
	    outSequenceMap[OT_M60]=loadSequenceSet("m-60.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_COBRASTAFF:
	    outSequenceMap[OT_COBRASTAFF]=loadSequenceSet("cob.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_FLAMER:
	    outSequenceMap[OT_FLAMER]=loadSequenceSet("flam.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_GRENADEAMMO:
	    outSequenceMap[OT_GRENADEAMMO]=loadSequenceSet("bomb.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_MANACLE:
	    outSequenceMap[OT_MANACLE]=loadSequenceSet("man.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_PISTOL:
	    outSequenceMap[OT_PISTOL]=loadSequenceSet("pist.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_TELEPRETURN:
	    outSequenceMap[OT_TELEPRETURN]=0;
	    outSequenceMap[OT_SANDALS]=loadSequenceSet("sandals.seq",0);
	    outSequenceMap[OT_CAPE]=loadSequenceSet("cape.seq",0);
	    outSequenceMap[OT_FEATHER]=loadSequenceSet("feather.seq",0);
	    outSequenceMap[OT_MASK]=loadSequenceSet("mask.seq",0);
	    outSequenceMap[OT_SCEPTER]=loadSequenceSet("scepter.seq",0);
	    outSequenceMap[OT_ANKLETS]=loadSequenceSet("anklets.seq",0);
	    break;
	 case OT_CAPE:
	    outSequenceMap[OT_CAPE]=loadSequenceSet("cape.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_FEATHER:
	    outSequenceMap[OT_FEATHER]=loadSequenceSet("feather.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_MASK:
	    outSequenceMap[OT_MASK]=loadSequenceSet("mask.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_SANDALS:
	    outSequenceMap[OT_SANDALS]=loadSequenceSet("sandals.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_SCEPTER:
	    outSequenceMap[OT_SCEPTER]=loadSequenceSet("scepter.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_PYRAMID:
	    outSequenceMap[OT_PYRAMID]=loadSequenceSet("pyramid.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_TRANSMITTER:
	    outSequenceMap[OT_TRANSMITTER]=loadSequenceSet("comm2.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_RAMSESTRIGGER:
	    tileForce8BPP=1;
	    outSequenceMap[OT_RAMSESTRIGGER]=loadSequenceSet("riseup1.seq",0);
	    loadSequenceSet("talk1.seq",0);
	    tileForce8BPP=0;
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("quake.wav");
	    break;
	 case OT_CAMEL:
	    outSequenceMap[OT_CAMEL]=loadSequenceSet("camel.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    switch (nmSectors % 3)
	       {case 0: addSound("camel_1b.wav"); break;
	        case 1: addSound("camel_2b.wav"); break;
	        case 2: addSound("camel_3b.wav"); break;
	       }
	    break;
	 case OT_RING:
	    outSequenceMap[OT_RING]=loadSequenceSet("ring.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_ANKLETS:
	    outSequenceMap[OT_ANKLETS]=loadSequenceSet("anklets.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_BLOODBOWL:
	    outSequenceMap[OT_BLOODBOWL]=loadSequenceSet("life.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_AMMOSPHERE:
	    outSequenceMap[OT_AMMOSPHERE]=loadSequenceSet("fullammo.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_HEALTHSPHERE:
	    outSequenceMap[OT_HEALTHSPHERE]=loadSequenceSet("fulllife.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    break;
	 case OT_SENTRY:
	    outSequenceMap[OT_SENTRY]=loadSequenceSet("roach.seq",0);
	    outSequenceMap[OT_SBALL]=outSequenceMap[OT_SENTRY];
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("ro_icu.wav");
	    addSound("ro_dead1.wav");
	    break;
	 case OT_MAGMANTIS:
	    outSequenceMap[OT_MAGMANTIS]=loadSequenceSet("lavag.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("lav_dead.wav");
	    outSequenceMap[OT_MAGBALL]=loadSequenceSet("projlava.seq",0);
	    break;
	 case OT_SET:
	    outSequenceMap[OT_SET]=loadSequenceSet("set.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;

	    outSequenceMap[OT_SETBALL]=loadSequenceSet("projset.seq",0);
	    break;
	 case OT_SELKIS:
	    outSequenceMap[OT_SELKIS]=loadSequenceSet("scorp.seq",0);
	    if (outSequenceMap[OT_ANUBALL]<0)
	       outSequenceMap[OT_ANUBALL]=loadSequenceSet("anuballl.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("scrp_die.wav");
	    addSound("scrp_cry.wav");
	    if (outSoundMap[OT_SPIDER]==-2)
	       {outSequenceMap[OT_SPIDER]=loadSequenceSet("spiderr.seq",0);
		outSoundMap[OT_SPIDER]=nmSounds;
		addSound("spi_hurt.wav");
		addSound("spi_newd.wav");
		addSound("spi_jump.wav");
		addSound("spi_bite.wav");
	       }
	    outSequenceMap[OT_MUMBALL]=loadSequenceSet("projmumm.seq",0);
	    break;
	 case OT_QUEEN:
	    outSequenceMap[OT_QUEEN]=loadSequenceSet("queen.seq",0);
	    outSequenceMap[OT_SBALL]=outSequenceMap[OT_QUEEN];
	    outSequenceMap[OT_QHEAD]=outSequenceMap[OT_QUEEN];
	    outSequenceMap[OT_QUEENEGG]=loadSequenceSet("queenegg.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("q_icu.wav");
	    addSound("q_bite.wav");
	    addSound("jon-hlnd.wav");
	    addSound("null.wav");
	    addSound("null.wav");
	    /* addSound("jon-eat.wav");
	       addSound("jon-smell.wav"); */

	    outSoundMap[OT_QUEENEGG]=nmSounds;
	    addSound("egg_bite.wav");
	    break;
	 case OT_BOOMPOT1:
	    outSequenceMap[OT_BOOMPOT1]=
	       loadSequenceSet("torches\\flampot.seq",0);
	    break;
	 case OT_BOOMPOT2:
	    outSequenceMap[OT_BOOMPOT2]=
	       loadSequenceSet("torches\\snkpot1.seq",0);
	    break;
	 case OT_TORCH1:
	    outSequenceMap[OT_TORCH1]=
	       loadSequenceSet("torches\\bowl.seq",0);
	    break;
	 case OT_TORCH2:
	    outSequenceMap[OT_TORCH2]=
	       loadSequenceSet("torches\\camp1.seq",0);
	    break;
	 case OT_TORCH3:
	    outSequenceMap[OT_TORCH3]=
	       loadSequenceSet("torches\\camp2.seq",0);
	    break;
	 case OT_TORCH4:
	    outSequenceMap[OT_TORCH4]=
	       loadSequenceSet("torches\\camp3.seq",0);
	    break;
	 case OT_TORCH5:
	    outSequenceMap[OT_TORCH5]=
	       loadSequenceSet("torches\\chaos1.seq",0);
	    break;
	 case OT_TORCH6:
	    outSequenceMap[OT_TORCH6]=
	       loadSequenceSet("torches\\chaos2.seq",0);
	    break;
	 case OT_TORCH7:
	    outSequenceMap[OT_TORCH7]=
	       loadSequenceSet("torches\\col1.seq",0);
	    break;
	 case OT_TORCH8:
	    outSequenceMap[OT_TORCH8]=
	       loadSequenceSet("torches\\col2.seq",0);
	    break;
	 case OT_TORCH9:
	    outSequenceMap[OT_TORCH9]=
	       loadSequenceSet("torches\\col3.seq",0);
	    break;
	 case OT_TORCH10:
	    outSequenceMap[OT_TORCH10]=
	       loadSequenceSet("torches\\flame.seq",0);
	    break;
	 case OT_TORCH11:
	    outSequenceMap[OT_TORCH11]=
	       loadSequenceSet("torches\\glow.seq",0);
	    break;
	 case OT_TORCH12:
	    outSequenceMap[OT_TORCH12]=
	       loadSequenceSet("torches\\hant1.seq",0);
	    break;
	 case OT_TORCH13:
	    outSequenceMap[OT_TORCH13]=
	       loadSequenceSet("torches\\hant2.seq",0);
	    break;
	 case OT_TORCH14:
	    outSequenceMap[OT_TORCH14]=
	       loadSequenceSet("torches\\hant3.seq",0);
	    break;
	 case OT_TORCH15:
	    outSequenceMap[OT_TORCH15]=
	       loadSequenceSet("torches\\hant4.seq",0);
	    break;
	 case OT_TORCH16:
	    outSequenceMap[OT_TORCH16]=
	       loadSequenceSet("torches\\magma1.seq",0);
	    break;
	 case OT_TORCH17:
	    outSequenceMap[OT_TORCH17]=
	       loadSequenceSet("torches\\magma2.seq",0);
	    break;
	 case OT_TORCH18:
	    outSequenceMap[OT_TORCH18]=
	       loadSequenceSet("torches\\marsh1.seq",0);
	    break;
	 case OT_TORCH19:
	    outSequenceMap[OT_TORCH19]=
	       loadSequenceSet("torches\\mummy.seq",0);
	    break;
	 case OT_TORCH20:
	    outSequenceMap[OT_TORCH20]=
	       loadSequenceSet("torches\\peril1.seq",0);
	    break;
	 case OT_TORCH21:
	    outSequenceMap[OT_TORCH21]=
	       loadSequenceSet("torches\\quar1.seq",0);
	    break;
	 case OT_TORCH22:
	    outSequenceMap[OT_TORCH22]=
	       loadSequenceSet("torches\\selkis1.seq",0);
	    break;
	 case OT_TORCH23:
	    outSequenceMap[OT_TORCH23]=
	       loadSequenceSet("torches\\selkis2.seq",0);
	    break;
	 case OT_TORCH24:
	    outSequenceMap[OT_TORCH24]=
	       loadSequenceSet("torches\\set1.seq",0);
	    break;
	 case OT_TORCH25:
	    outSequenceMap[OT_TORCH25]=
	       loadSequenceSet("torches\\set2.seq",0);
	    break;
	 case OT_TORCH26:
	    outSequenceMap[OT_TORCH26]=
	       loadSequenceSet("torches\\set3.seq",0);
	    break;
	 case OT_TORCH27:
	    outSequenceMap[OT_TORCH27]=
	       loadSequenceSet("torches\\set4.seq",0);
	    break;
	 case OT_TORCH28:
	    outSequenceMap[OT_TORCH28]=
	       loadSequenceSet("torches\\thoth1.seq",0);
	    break;
	 case OT_TORCH29:
	    outSequenceMap[OT_TORCH29]=
	       loadSequenceSet("torches\\thoth2.seq",0);
	    break;
	 case OT_TORCH30:
	    outSequenceMap[OT_TORCH30]=
	       loadSequenceSet("torches\\thoth3.seq",0);
	    break;
	 case OT_TORCH31:
	    outSequenceMap[OT_TORCH31]=
	       loadSequenceSet("torches\\tom1.seq",0);
	    break;
	 case OT_TORCH32:
	    outSequenceMap[OT_TORCH32]=
	       loadSequenceSet("torches\\tom2.seq",0);
	    break;
	 case OT_TORCH33:
	    outSequenceMap[OT_TORCH33]=
	       loadSequenceSet("torches\\torch2.seq",0);
	    break;
	 case OT_TORCH34:
	    outSequenceMap[OT_TORCH34]=
	       loadSequenceSet("torches\\town1.seq",0);
	    break;
	 case OT_TORCH35:
	    outSequenceMap[OT_TORCH35]=
	       loadSequenceSet("torches\\waltrch.seq",0);
	    break;
	 case OT_TORCH36:
	    outSequenceMap[OT_TORCH36]=
	       loadSequenceSet("torches\\waltrch1.seq",0);
	    break;
	 case OT_TORCH37:
	    outSequenceMap[OT_TORCH37]=
	       loadSequenceSet("torches\\waltrch2.seq",0);
	    break;
	 case OT_TORCH38:
	    outSequenceMap[OT_TORCH38]=
	       loadSequenceSet("torches\\wtrlite.seq",0);
	    break;

	 case OT_CONTAIN1:
	    outSequenceMap[OT_CONTAIN1]=
	       loadSequenceSet("explode\\tomb1.seq",0);
	    break;
	 case OT_CONTAIN2:
	    outSequenceMap[OT_CONTAIN2]=
	       loadSequenceSet("explode\\tbang.seq",0);
	    break;
	 case OT_CONTAIN3:
	    outSequenceMap[OT_CONTAIN3]=
	       loadSequenceSet("explode\\blow3.seq",0);
	    break;
	 case OT_CONTAIN4:
	    outSequenceMap[OT_CONTAIN4]=
	       loadSequenceSet("explode\\blow8.seq",0);
	    break;
	 case OT_CONTAIN5:
	    outSequenceMap[OT_CONTAIN5]=
	       loadSequenceSet("explode\\sbang.seq",0);
	    break;
	 case OT_CONTAIN6:
	    outSequenceMap[OT_CONTAIN6]=
	       loadSequenceSet("explode\\blow1.seq",0);
	    break;
	 case OT_CONTAIN7:
	    outSequenceMap[OT_CONTAIN7]=
	       loadSequenceSet("explode\\blow7.seq",0);
	    break;
	 case OT_CONTAIN8:
	    outSequenceMap[OT_CONTAIN8]=
	       loadSequenceSet("explode\\vas.seq",0);
	    break;
	 case OT_CONTAIN9:
	    outSequenceMap[OT_CONTAIN9]=
	       loadSequenceSet("explode\\pbang.seq",0);
	    break;
	 case OT_CONTAIN10:
	    outSequenceMap[OT_CONTAIN10]=
	       loadSequenceSet("explode\\blow4.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("pod_pc1.wav");
	    addSound("pod_pc2.wav");
	    addSound("pod_pc3.wav");
	    break;
	 case OT_CONTAIN11:
	    outSequenceMap[OT_CONTAIN11]=
	       loadSequenceSet("explode\\mbang.seq",0);
	    break;
	 case OT_CONTAIN12:
	    outSequenceMap[OT_CONTAIN12]=
	       loadSequenceSet("explode\\blow5.seq",0);
	    break;
	 case OT_CONTAIN13:
	    outSequenceMap[OT_CONTAIN13]=
	       loadSequenceSet("explode\\dog.seq",0);
	    break;
	 case OT_CONTAIN14:
	    outSequenceMap[OT_CONTAIN14]=
	       loadSequenceSet("explode\\blow6.seq",0);
	    break;
	 case OT_CONTAIN15:
	    outSequenceMap[OT_CONTAIN15]=
	       loadSequenceSet("explode\\blow2.seq",0);
	    break;
	 case OT_CONTAIN16:
	    outSequenceMap[OT_CONTAIN16]=
	       loadSequenceSet("explode\\kbang.seq",0);
	    break;
	 case OT_CONTAIN17:
	    outSequenceMap[OT_CONTAIN17]=
	       loadSequenceSet("explode\\cbang.seq",0);
	    break;
	 case OT_COMM_BATTERY:
	    outSequenceMap[OT_COMM_BATTERY]=
	       loadSequenceSet("comm\\battery.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_BOTTOM:
	    outSequenceMap[OT_COMM_BOTTOM]=
	       loadSequenceSet("comm\\bottom.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_DISH:
	    outSequenceMap[OT_COMM_DISH]=
	       loadSequenceSet("comm\\dish.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_HEAD:
	    outSequenceMap[OT_COMM_HEAD]=
	       loadSequenceSet("comm\\head.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_KEYBOARD:
	    outSequenceMap[OT_COMM_KEYBOARD]=
	       loadSequenceSet("comm\\keyboard.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_MOUSE:
	    outSequenceMap[OT_COMM_MOUSE]=
	       loadSequenceSet("comm\\mouse.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_SCREEN:
	    outSequenceMap[OT_COMM_SCREEN]=
	       loadSequenceSet("comm\\screen.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_COMM_TOP:
	    outSequenceMap[OT_COMM_TOP]=
	       loadSequenceSet("comm\\top.seq",0);
	    outSoundMap[outObjects[o].type]=nmSounds;
	    addSound("transmit.wav");
	    break;
	 case OT_DOLL1:
	    outSequenceMap[OT_DOLL1]=
	       loadSequenceSet("doll\\doll1.seq",0);
	    break;
	 case OT_DOLL2:
	    outSequenceMap[OT_DOLL2]=
	       loadSequenceSet("doll\\doll2.seq",0);
	    break;
	 case OT_DOLL3:
	    outSequenceMap[OT_DOLL3]=
	       loadSequenceSet("doll\\doll3.seq",0);
	    break;
	 case OT_DOLL4:
	    outSequenceMap[OT_DOLL4]=
	       loadSequenceSet("doll\\doll4.seq",0);
	    break;
	 case OT_DOLL5:
	    outSequenceMap[OT_DOLL5]=
	       loadSequenceSet("doll\\doll5.seq",0);
	    break;
	 case OT_DOLL6:
	    outSequenceMap[OT_DOLL6]=
	       loadSequenceSet("doll\\doll6.seq",0);
	    break;
	 case OT_DOLL7:
	    outSequenceMap[OT_DOLL7]=
	       loadSequenceSet("doll\\doll7.seq",0);
	    break;
	 case OT_DOLL8:
	    outSequenceMap[OT_DOLL8]=
	       loadSequenceSet("doll\\doll8.seq",0);
	    break;
	 case OT_DOLL9:
	    outSequenceMap[OT_DOLL9]=
	       loadSequenceSet("doll\\doll9.seq",0);
	    break;
	 case OT_DOLL10:
	    outSequenceMap[OT_DOLL10]=
	       loadSequenceSet("doll\\doll10.seq",0);
	    break;
	 case OT_DOLL11:
	    outSequenceMap[OT_DOLL11]=
	       loadSequenceSet("doll\\doll11.seq",0);
	    break;
	 case OT_DOLL12:
	    outSequenceMap[OT_DOLL12]=
	       loadSequenceSet("doll\\doll12.seq",0);
	    break;
	 case OT_DOLL13:
	    outSequenceMap[OT_DOLL13]=
	       loadSequenceSet("doll\\doll13.seq",0);
	    break;
	 case OT_DOLL14:
	    outSequenceMap[OT_DOLL14]=
	       loadSequenceSet("doll\\doll14.seq",0);
	    break;
	 case OT_DOLL15:
	    outSequenceMap[OT_DOLL15]=
	       loadSequenceSet("doll\\doll15.seq",0);
	    break;
	 case OT_DOLL16:
	    outSequenceMap[OT_DOLL16]=
	       loadSequenceSet("doll\\doll16.seq",0);
	    break;
	 case OT_DOLL17:
	    outSequenceMap[OT_DOLL17]=
	       loadSequenceSet("doll\\doll17.seq",0);
	    break;
	 case OT_DOLL18:
	    outSequenceMap[OT_DOLL18]=
	       loadSequenceSet("doll\\doll18.seq",0);
	    break;
	 case OT_DOLL19:
	    outSequenceMap[OT_DOLL19]=
	       loadSequenceSet("doll\\doll19.seq",0);
	    break;
	 case OT_DOLL20:
	    outSequenceMap[OT_DOLL20]=
	       loadSequenceSet("doll\\doll20.seq",0);
	    break;
	 case OT_DOLL21:
	    outSequenceMap[OT_DOLL21]=
	       loadSequenceSet("doll\\doll21.seq",0);
	    break;
	 case OT_DOLL22:
	    outSequenceMap[OT_DOLL22]=
	       loadSequenceSet("doll\\doll22.seq",0);
	    break;
	 case OT_DOLL23:
	    outSequenceMap[OT_DOLL23]=
	       loadSequenceSet("doll\\doll23.seq",0);
	    break;
	   }
    }
 outSequenceMap[OT_POOF]=loadSequenceSet("boom1.seq",TILEFLAG_RLE);

 outSequenceList[nmSequences++]=nmFrames;
 outFrames[nmFrames].sound=-1;
 outFrames[nmFrames++].chunkIndex=nmChunks;

 printf("---------------------------------------------\n");
 printf("nmSequences=%d\n",nmSequences);
 printf("nmFrames=%d\n",nmFrames);
 printf("nmChunks=%d\n",nmChunks);

 size=sizeof(struct seqHeader)+
    nmSequences*sizeof(short)+
       nmFrames*sizeof(sFrameType)+
	  nmChunks*sizeof(sChunkType)+
	     OT_NMTYPES*sizeof(short);
 return size;
}

void writeSequences(int size)
{int i;
 assert(size==sizeof(struct seqHeader)+
	OT_NMTYPES*sizeof(short)+
	nmSequences*sizeof(short)+
	nmFrames*sizeof(sFrameType)+
	nmChunks*sizeof(sChunkType));
 /* write sequence header */
 writeInt(nmSequences);
 writeInt(nmFrames);
 writeInt(nmChunks);
 /* write frames */
 for (i=0;i<nmFrames;i++)
    {writeShort(outFrames[i].chunkIndex);
     writeShort(outFrames[i].flags);
     writeShort(outFrames[i].sound);
     writeShort(0);
    }
 /* write chunks */
 for (i=0;i<nmChunks;i++)
    {writeShort(outChunk[i].chunkx);
     writeShort(outChunk[i].chunky);
     writeShort(outChunk[i].tile);
     writeChar(outChunk[i].flags);
     writeChar(0);
    }
 /* write sequence index list */
 assert(outSequenceList[0]==0);
 for (i=0;i<nmSequences;i++)
    writeShort(outSequenceList[i]);
 for (i=0;i<OT_NMTYPES;i++)
    writeShort(outSequenceMap[i]);
}

void mapWallTiles(void)
{int i;
 printf("In map\n");
 for (i=1;i<nmTextures;i+=2)
    outTexture[i]=tiles[findTile(translateWallTileNm(outTexture[i]))].number;
 for (i=0;i<nmFaces;i++)
    {outFace[i].tile=tiles[findTile(translateWallTileNm(outFace[i].tile))].
	number;
    }
 printf("Out map\n");
}

void makePB(int sector,int internal,int floorClearance,int thickness)
{pbList[sector].isPB=1;
 pbList[sector].isInternal=internal;
 pbList[sector].floorClearance=floorClearance;
 pbList[sector].thickness=thickness;
}

/* args 1: level file to convert
        2: sky to use
        3: output file
	(4: extra file)
 */
int main(int argc,char **argv)
{int totalSize=0,footPrint=0,soundSize=0;
 int i,j,objectPallete;
 int levelSize,size,seqSize;

 if (argc<4)
    {printf("Args bad.\n");
     exit(-1);
    }
 ofile=fopen(argv[3],"wb");
 if (!ofile)
    printf("Can't open output file.\n");
 if (argc==5)
    extraFile=fopen(argv[4],"r");
 else
    extraFile=NULL;

 for (i=0;i<MAXNMSECTORS;i++)
    pbList[i].isPB=0;

 {char *c;
  for (c=argv[1];*c;c++)
     *c=toupper(*c);
 }

/* makePB(1,1,0,64<<16);*/
 /* lookup ambient term */
 {struct {char *lev; int term;} amMap[]=
     {
      {"CAVERN",3},
      {"SETARENA",3},
      {"SETPALAC",3},
      {"MINES",3},
      {"MAGMA",6},
      {NULL,0}
     };
  for (i=0;amMap[i].lev;i++)
     if (strstr(argv[1],amMap[i].lev))
	break;
  ambientTerm=amMap[i].term;
 }

 /* write sky bitmap */
 {int y,x;
  struct {char *lev,*plax;} plaxMap[]=
     {
      {"TOMB","DAYSKY1.BMP"},
      {"KARNAK","DAYSKY1.BMP"},
      {"SANCTUAR","DAYSKY1.BMP"},
      {"PASS","DAYSKY1.BMP"},
      {"SHRINE","DAYSKY1.BMP"},
      {"COLONY","DAYSKY9.BMP"},
      {"KILENTRY","DAYSKY2.BMP"},
      {"SELBUROW","DAYSKY10.BMP"},
      {"MAGMA","DAYSKY4.BMP"},
      {"CHAOS","DAYSKY4.BMP"},
      {"MARSH","DAYSKY5.BMP"},
      {"SUNKEN","DAYSKY5.BMP"},
      {"SLAVCAMP","DAYSKY5.BMP"},
      {"MINES","DAYSKY6.BMP"},
      {"QUARRY","DAYSKY6.BMP"},
      {"CAVERN","DAYSKY10.BMP"},
      {"SETPALAC","DAYSKY6.BMP"},
      {"SETARENA","DAYSKY6.BMP"},
      {"GORGE","DAYSKY8.BMP"},
      {"PEAK","DAYSKY8.BMP"},
      {"THOTH","DAYSKY8.BMP"},
      {"KILARENA","STARS.BMP"},
      {"KIL","DAYSKY9.BMP"},
      {"ARENA","DAYSKY9.BMP"},
      {NULL,"DAYSKY1.BMP"}
     };
  for (i=0;plaxMap[i].lev;i++)
     if (strstr(argv[1],plaxMap[i].lev))
	break;
  printf("level #%d\n",i);
  swap=0;
  readBmp(plaxMap[i].plax);
  swap=1;
  for (x=0;x<128;x++)
     for (y=0;y<256;y++)
	picdata[y][x]=picdata[y][511];
  for (i=0;i<256;i++)
     writeShort(picPal[i]);
  writeInt(picWidth);
  writeInt(picHeight);
  for (y=0;y<picHeight;y++)
     for (x=0;x<picWidth;x++)
	writeChar(picdata[y][x]);
  /* write sky coefficient table */
#define PLAXPERSCREEN 128
  {int x,d;
   double f;
   for (x=0;x<320;x++)
      {f=atan((x-160.0)/160.0)*((double)PLAXPERSCREEN)/0.785398;
       if (abs(x-160)>1)
	  {f=f/(x-160);
	   d=f*65536.0;
	   d=d&0x007fffff;
	  }
       else
	  d=66754/*83200*/ /*0x400*/;
       writeInt(d);
      }
  }

 }
 /* load the ruins pallete into pallete space 0 */
 {readBmp("ruinspal.bmp");
  mapPal();
  assert(nmPals==1);
 }
 strcpy(levelName,argv[1]);

 /* load wall tile map */
 {char buff[80];
  strcpy(buff,argv[1]);
  strcat(buff,".til");
  loadWallTileMap(buff);
 }
 /* load light map */
 {char buff[80];
  strcpy(buff,argv[1]);
  strcat(buff,".lit");
  loadLightMap(buff);
 }
 /* load level */
 levelSize=convertLevel(/*argv[1]*/);
 checkLevel();

#if 0
 {int w;
  for (w=outSector[116].firstWall;w<=outSector[116].lastWall;w++)
     {printf("Wall %d: (%d,%d,%d)\n",w,outWalls[w].normal[0],
	     outWalls[w].normal[1],
	     outWalls[w].normal[2]);
     }
 }
 exit(-1);
#endif

 /* map tiles */
 sortTiles();
 computeTileNms();
 mapWallTiles();
 /* load sequences */
 seqSize=loadSequences();
 totalSize+=seqSize;
 footPrint+=seqSize;
 /* write level */
 totalSize+=levelSize;
 footPrint+=levelSize;
 writeInt(levelSize);

 writeLevel();

 /* write sounds */
 soundSize=writeSoundSet();

 /* write palletes */
 objectPallete=0;
 for (i=0;i<nmTiles;i++)
    {if (tiles[i].flags & TILEFLAG_8BPP)
	{if (objectPallete<0)
	    objectPallete=tiles[i].palNm;
	 if (tiles[i].palNm!=objectPallete)
	    {printf("Warning: object tile %s's pallete does "
		    "not match the others.\n",tiles[i].filename);
	    }
	}
    }
 if (objectPallete==-1)
    {printf("No objects!\n");
     exit(-1);
    }
 size=nmPals*512+2;
 totalSize+=size;
 footPrint+=size;
 writeInt(size);
 writeShort(objectPallete);
 printf("Nm Palletes=%d\n",nmPals);
 for (i=0;i<nmPals;i++)
    for (j=0;j<256;j++)
       writeShort(palletes[i][j]);

 /* write swapable tiles */
 {int nm=0;
  for (i=0;i<nmTiles;i++)
     nm+=tiles[i].nmSubTiles;
  writeInt(nm);
 }
 for (i=0;i<nmTiles;i++)
    {int q;
     q=writeTile(i);
     totalSize+=q;
     footPrint+=q;
    }
 printf("Nm Tiles=%d\n",tilesWrote);

 /* write sequences */
 writeInt(seqSize);
 printf("Seqsize=%d\n",seqSize);
 writeSequences(seqSize);
 fclose(ofile);
 printf("---------------------------------------------\n");
 printf("Total size:%d\nMemory footprint:%d\n",totalSize,footPrint);
 printf("Sound Memory used:%d\n",soundSize);
 return 0;
}
