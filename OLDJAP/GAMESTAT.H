#ifndef __INCLUDEDgamestath
#define __INCLUDEDgamestath

#define INV_SANDALS  0x00000001
#define INV_MASK     0x00000002
#define INV_SHAWL    0x00000004
#define INV_ANKLET   0x00000008
#define INV_SCEPTER  0x00000010
#define INV_FEATHER  0x00000020

#define INV_SWORD    0x00000100
#define INV_PISTOL   0x00000200
#define INV_M60      0x00000400
#define INV_GRENADE  0x00000800
#define INV_FLAMER   0x00001000
#define INV_COBRA    0x00002000
#define INV_RING     0x00004000
#define INV_MANACLE  0x00008000

#define INV_TRANSMITTER 0x00ff0000
#define INV_MUMMY       0x01000000

#define WEAPONINV(x) ((x>>8)&0xff)
#define UNDERWATERWEAPONS 0x29


enum Weapon {WP_SWORD,WP_PISTOL,WP_M60,WP_GRENADE,WP_FLAMER,
		WP_COBRA,WP_RING,WP_RAVOLT,WP_NMWEAPONS};

#define NMLEVELS 31

#define LEVFLAG_GOTPYRAMID  1
#define LEVFLAG_GOTVESSEL   2
#define LEVFLAG_CANENTER    4

#define GAMEFLAG_GOTSANDALS    0x0001
#define GAMEFLAG_GOTMASK       0x0002
#define GAMEFLAG_GOTSHAWL      0x0004
#define GAMEFLAG_GOTANKLET     0x0008
#define GAMEFLAG_GOTSCEPTER    0x0010
#define GAMEFLAG_GOTFEATHER    0x0020
#define GAMEFLAG_DOLLPOWERMODE 0x0040
#define GAMEFLAG_KILLEDSET     0x0080
#define GAMEFLAG_KILLEDSELKIS  0x0100

#define GAMEFLAG_JUSTTELEPORTED 0x0200
    /* set by pyramid, cleared by ramses trigger */
#define GAMEFLAG_TALKEDTORAMSES 0x0400
    /* cleared by pyramid, set by camel */
#define GAMEFLAG_FIRSTLEVEL     0x0800
    /* starts set, cleared by camel */
#define GAMEFLAG_KILENTRYCHEATENABLED 0x1000

typedef struct
{int inventory;
#define ALLDOLLS 0x7fffff
 int dolls;
 int nmBowls;
 int health;
 int gameFlags;
 int weaponAmmo[WP_NMWEAPONS];
 char levFlags[NMLEVELS];
 char currentLevel,desiredWeapon;
 short year,month,day,hour,min;
} SaveState;

extern SaveState currentState;

#endif
