/* stuff thats not used */
typedef enum {
  SR_STANDARD_VOLUME     =  0,
  SR_DOORWAY_STYLE_1     =  1,
  SR_DOORWAY_STYLE_2     =  2,
  SR_DOORWAY_STYLE_3     =  3,
  SR_DOORWAY_STYLE_4     =  4,
  SR_DOORWAY_STYLE_5     =  5,
  SR_DOORWAY_STYLE_6     =  6,
  SR_DOORWAY_STYLE_7     =  7,
  SR_DOORWAY_STYLE_8     =  8,
  SR_DOORWAY_STYLE_9     =  9,
  SR_DOORWAY_STYLE_10    = 10,
  SR_DOORWAY_STYLE_11    = 11,
  SR_DOORWAY_STYLE_12    = 12,
  SR_DOORWAY_STYLE_13    = 13,
  SR_DOORWAY_STYLE_14    = 14,
  SR_DOORWAY_STYLE_15    = 15,
  SR_DOORWAY_STYLE_16    = 16,
  SR_DOORWAY_STYLE_17    = 17,
  SR_DOORWAY_STYLE_18    = 18,
  SR_DOORWAY_STYLE_19    = 19,
  SR_DOORWAY_STYLE_20    = 20,
  SR_LIFT_SHAFT_STYLE_1  = 21,
  SR_LIFT_SHAFT_STYLE_2  = 22,
  SR_LIFT_SHAFT_STYLE_3  = 23,
  SR_LIFT_SHAFT_STYLE_4  = 24,
  SR_LIFT_SHAFT_STYLE_5  = 25,
  SR_LIFT_SHAFT_STYLE_6  = 26,
  SR_LIFT_SHAFT_STYLE_7  = 27,
  SR_LIFT_SHAFT_STYLE_8  = 28,
  SR_LIFT_SHAFT_STYLE_9  = 29,
  SR_LIFT_SHAFT_STYLE_10 = 30,
  SR_LIFT_SHAFT_STYLE_11 = 31,
  SR_LIFT_SHAFT_STYLE_12 = 32,
  SR_LIFT_SHAFT_STYLE_13 = 33,
  SR_LIFT_SHAFT_STYLE_14 = 34,
  SR_LIFT_SHAFT_STYLE_15 = 35,
  SR_LIFT_SHAFT_STYLE_16 = 36,
  SR_LIFT_SHAFT_STYLE_17 = 37,
  SR_LIFT_SHAFT_STYLE_18 = 38,
  SR_LIFT_SHAFT_STYLE_19 = 39,
  SR_LIFT_SHAFT_STYLE_20 = 40,
  SR_SPECIAL_ROLE_1      = 41,
  SR_SPECIAL_ROLE_2      = 42,
  SR_SPECIAL_ROLE_3      = 43,
  SR_SPECIAL_ROLE_4      = 44,
  SR_SPECIAL_ROLE_5      = 45,
  SR_SPECIAL_ROLE_6      = 46,
  SR_SPECIAL_ROLE_7      = 47,
  SR_SPECIAL_ROLE_8      = 48,
  SR_SPECIAL_ROLE_9      = 49,
  SR_SPECIAL_ROLE_10     = 50,
  SR_SPECIAL_ROLE_11     = 51,
  SR_SPECIAL_ROLE_12     = 52,
  SR_SPECIAL_ROLE_13     = 53,
  SR_SPECIAL_ROLE_14     = 54,
  SR_SPECIAL_ROLE_15     = 55,
  SR_SPECIAL_ROLE_16     = 56,
  SR_SPECIAL_ROLE_17     = 57,
  SR_SPECIAL_ROLE_18     = 58,
  SR_SPECIAL_ROLE_19     = 59,
  SR_SPECIAL_ROLE_20     = 60,
  SR_SPECIAL_ROLE_21     = 61,
  SR_SPECIAL_ROLE_22     = 62,
  SR_SPECIAL_ROLE_23     = 63,
  SR_SPECIAL_ROLE_24     = 64,
  SR_SPECIAL_ROLE_25     = 65,
  SR_SPECIAL_ROLE_26     = 66,
  SR_SPECIAL_ROLE_27     = 67,
  SR_SPECIAL_ROLE_28     = 68,
  SR_SPECIAL_ROLE_29     = 69,
  SR_SPECIAL_ROLE_30     = 70,
  SR_SPECIAL_ROLE_31     = 71,
  SR_SPECIAL_ROLE_32     = 72,
} SECTOR_ROLES;

typedef struct
{
	SECTOR_ROLES type;
	short index;
} sectorpropstype;

typedef struct
{
	short firstwall;
	short lastwall;
	short sector;
	short speed;
} doorpropstype;

typedef struct
{
	short firstwall;
	short lastwall;
	short sector;
	short speed;
} liftpropstype;

typedef struct
{
	int	x1,y1,y2,n,w;
}	clipwalltype;

/* #define PLAYER_TYPE OT_PLAYER */

typedef struct
{
	long x,y,z;
	short s;
	short type;
	short wscale;
	short hscale;
	short ang;
	short frame;
	short clock;
	short radius;
	short height;
	long dx,dy,dz;
	int isfloor:1;
}	monstertype;

/* stuff that is used */
typedef struct
{short x,y,z;
} vertextype;

typedef struct
{int v0,v1,v2,v3; /*index into vertex array for each vertex of face*/
 signed char tile; /*index of tile (texture)*/
 int light;
 signed char flipBit;
} facetype;

typedef struct
{short firstface; /*index into face array of first face of wall
		    (-1, no visible face) */
 short lastface; /*index of last face */
 int v0; /*end points of wall in vertex array*/
 short nextsector; /*index of sector on other side of wall, no nextsector=-1*/
 short normal; /*normal angle of wall (facing into sector, 0-4095)*/
 short nx,ny,nz;
 int isblocked:1; /*if wall has a nextsector, set this bit if wall should
		    block motion anyway */
} walltype;


typedef struct
{short firstwall; /*index into wall array of first wall of sector*/
 short lastwall; /*index of last wall*/
 short light; /*lighting level (0-255)*/
 short cielz,floorz,cielh,floorh,ceils,floors;
} sectortype;

vertextype vertex[];
sectorpropstype sectorprops[];
short sectorcount;
sectortype sector[];
short wallcount;
walltype wall[];
short facecount;
facetype face[];
unsigned short vertexcount;
