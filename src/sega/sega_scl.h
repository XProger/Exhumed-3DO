#ifndef SEGA_SCL_H
#define SEGA_SCL_H

#include <sega_xpt.h>

#ifdef TODO
#endif

#define VRAM_SIZE   (1024 * 1024 * 1)

extern Uint8 VRAM[VRAM_SIZE];
extern Uint32 VRAM_ADDR;

#define FBUF_ADDR 0

#define SCL_SetFrameInterval(unk)
#define SCL_SetColOffset(offset,unk,a,b,c)
#define SCL_DisplayFrame()
#define pollhost()
#define	SCL_VblankStart()
#define SCL_VblankEnd()

typedef	struct	SclConfig {
    Uint8       dispenbl;
    Uint8       charsize;
    Uint8       pnamesize;
    Uint8       platesize;
    Uint8       bmpsize;
    Uint8       coltype;
    Uint8       datatype;
    Uint8       mapover;
    Uint8       flip;
    Uint16      patnamecontrl;
    Uint32      plate_addr[16];
} SclConfig;

typedef struct SclVramConfig{
    Uint32  ktboffsetA;
    Uint32  ktboffsetB;
    Uint8   vramModeA;
    Uint8   vramModeB;
    Uint8   vramA0;
    Uint8   vramA1;
    Uint8   vramB0;
    Uint8   vramB1;
    Uint8   colram;
} SclVramConfig;

 typedef struct	SclSysreg{
    Uint16  tvmode;
    Uint16  extenbl;
    Uint16  tvstatus;
    Uint16  vramsize;
    Uint16  H_val;
    Uint16  V_val;
    Uint16  vramchg;
    Uint16  ramcontrl;
    Uint16  vramcyc[8];
    Uint16  dispenbl;
    Uint16  mosaic;
    Uint16  specialcode_sel;
    Uint16  specialcode;
 } SclSysreg;

 typedef struct SclDataset{
	Uint16	charcontrl0;
	Uint16	charcontrl1;
	Uint16	bmpalnum0;
	Uint16	bmpalnum1;
	Uint16	patnamecontrl[5];
	Uint16	platesize;
	Uint16	mapoffset0;
	Uint16	mapoffset1;
	Uint16	normap[8];
	Uint16	rotmap[16];
} SclDataset;

 typedef struct SclNorscl{
	Fixed32	n0_move_x;
	Fixed32	n0_move_y;
	Fixed32	n0_delta_x;
	Fixed32	n0_delta_y;
	Fixed32	n1_move_x;
	Fixed32	n1_move_y;
	Fixed32	n1_delta_x;
	Fixed32	n1_delta_y;
	Uint16	n2_move_x;
	Uint16	n2_move_y;
	Uint16	n3_move_x;
	Uint16	n3_move_y;
	Uint16	zoomenbl;
	Uint16	linecontrl;
	Uint32	celladdr;
	Uint32	lineaddr[2];
	Uint32	linecolmode;
	Uint32	backcolmode;
} SclNorscl;

 typedef	struct	SclSblSgl{
	Uint16		sgl_flag;
}SclSblSgl;

 typedef struct
{
    Uint32 SclOtherPri:1;
    Uint32 SclSpPriNum:1;
    Uint32 SclBgPriNum:1;
    Uint32 SclSpColMix:1;
    Uint32 SclBgColMix:1;
    Uint32 SclColOffset:1;
} SclPriBuffDirtyFlags;

 typedef struct  SclXy {
	 Fixed32         x;
	 Fixed32         y;
} SclXy;

 typedef struct  SclXyz {
	 Fixed32         x;
	 Fixed32         y;
	 Fixed32         z;
} SclXyz;

typedef struct  SclXy16 {
        Uint16         x;
        Uint16         y;
} SclXy16;

typedef struct  SclXyz16 {
        Sint16         x;
        Sint16         y;
        Sint16         z;
} SclXyz16;

 typedef struct SclRotreg{
	SclXyz		screenst;
	SclXy		screendlt;
	SclXy		delta;
	Fixed32		matrix_a;
	Fixed32		matrix_b;
	Fixed32		matrix_c;
	Fixed32		matrix_d;
	Fixed32		matrix_e;
	Fixed32		matrix_f;
	SclXyz16	viewp;
	Uint16		dummy1;
	SclXyz16	rotatecenter;
	Uint16		dummy2;
	SclXy		move;
	SclXy		zoom;
	Fixed32		k_tab;
	SclXy		k_delta;
/*	Fixed32		dummy3[8];	*/
	Fixed32		dummy3[2];
} SclRotreg;

 typedef struct SclWinscl{
	Uint16	win0_start[2];
	Uint16	win0_end[2];
	Uint16	win1_start[2];
	Uint16	win1_end[2];
	Uint16	wincontrl[4];
	Uint32 	linewin0_addr;
	Uint32 	linewin1_addr;
} SclWinscl;

 typedef struct SclRotscl{
	Uint16	paramode;		/* Rotate Parameter Mode */
	Uint16	paramcontrl;		/* Rotate Parameter Read Control */
	Uint16	k_contrl;		/* Keisu Table Control */
	Uint16	k_offset;		/* Keisu Addres Offset */
	Uint16	mapover[2];		/* Rotate Scroll Map Over */
	Uint32	paramaddr;		/* Rotate Parameter Tabel Address */
} SclRotscl;

extern	SclSysreg	Scl_s_reg;
extern	SclRotscl	Scl_r_reg;
extern	Uint16		SclProcess;

#define	SCL_W0  0
#define	SCL_W1  1

#define SCL_RBG0	0x00000001
#define SCL_RBG1	0x00000002
#define SCL_NBG0	0x00000004
#define SCL_NBG1	0x00000008
#define SCL_NBG2	0x00000010
#define SCL_NBG3	0x00000020

#define SCL_EXBG	0x00000080
#define SCL_SPR 	0x00000100
#define SCL_SP0 	0x00000100
#define SCL_SP1 	0x00000200
#define SCL_SP2 	0x00000400
#define SCL_SP3 	0x00000800
#define SCL_SP4 	0x00001000
#define SCL_SP5 	0x00002000
#define SCL_SP6 	0x00004000
#define SCL_SP7 	0x00008000
#define SCL_RP  	0x00010000
#define SCL_RP_R	0xfffeffff
#define SCL_CC  	0x00020000
#define SCL_LNCL	0x00040000
#define SCL_BACK	0x00080000

#define SCL_VDP2_VRAM		(VRAM + (0x25e00000 - 0x25e00000))
#define SCL_VDP2_VRAM_A		(VRAM + (0x25e00000 - 0x25e00000))
#define SCL_VDP2_VRAM_A0	(VRAM + (0x25e00000 - 0x25e00000))
#define SCL_VDP2_VRAM_A1	(VRAM + (0x25e20000 - 0x25e00000))
#define SCL_VDP2_VRAM_B		(VRAM + (0x25e40000 - 0x25e00000))
#define SCL_VDP2_VRAM_B0	(VRAM + (0x25e40000 - 0x25e00000))
#define SCL_VDP2_VRAM_B1	(VRAM + (0x25e60000 - 0x25e00000))
#define	SCL_COLRAM_ADDR		(VRAM + (0x25F00000 - 0x25e00000))

#define SCL_TYPE0       0
#define SCL_TYPE1       1
#define SCL_TYPE2       2
#define SCL_TYPE3       3
#define SCL_TYPE4       4
#define SCL_TYPE5       5
#define SCL_TYPE6       6
#define SCL_TYPE7       7
#define SCL_TYPE8       8
#define SCL_TYPE9       9
#define SCL_TYPEA       10
#define SCL_TYPEB       11
#define SCL_TYPEC       12
#define SCL_TYPED       13
#define SCL_TYPEE       14
#define SCL_TYPEF       15

#define SCL_PALETTE     0
#define SCL_MIX         1

#define SCL_MSB_SHADOW  0
#define SCL_SP_WINDOW   1

#define SCL_IF_BEHIND   0
#define SCL_IF_EQUAL    1
#define SCL_IF_FRONT    2
#define SCL_MSB_ON      3

#define SCL_OFFSET_A    0
#define SCL_OFFSET_B    1

#define SCL_NON			0
#define SCL_RBG0_K 		1
#define SCL_RBG0_PN 	2
#define SCL_RBG0_CHAR 	3
#define SCL_RBG1_K 		4

#define	SCL_CELL		0
#define	SCL_BITMAP		1

#define	SCL_OVER_0		0
#define	SCL_OVER_1		1
#define	SCL_OVER_2		2
#define	SCL_OVER_3		3

#define	SCL_BMP_SIZE_512X256	0
#define	SCL_BMP_SIZE_512X512	1
#define	SCL_BMP_SIZE_1024X256	2
#define	SCL_BMP_SIZE_1024X512	3

#define	SCL_COL_TYPE_16		0
#define	SCL_COL_TYPE_256	1
#define	SCL_COL_TYPE_2048	2
#define	SCL_COL_TYPE_32K	3
#define	SCL_COL_TYPE_1M		4

#define	SCL_NON_INTER		0
#define	SCL_SINGLE_INTER	2
#define	SCL_DOUBLE_INTER	3

#define	SCL_224LINE		0
#define	SCL_240LINE		1
#define	SCL_256LINE		2

#define	SCL_NORMAL_A		0
#define	SCL_NORMAL_B		1
#define	SCL_HIRESO_A		2
#define	SCL_HIRESO_B		3
#define	SCL_NORMAL_AE		4
#define	SCL_NORMAL_BE		5
#define	SCL_HIRESO_AE		6
#define	SCL_HIRESO_BE		7

#define SCL_CRM15_1024 0
#define SCL_CRM15_2048 1
#define SCL_CRM24_1024 2

#define SCL_RBG_TB_A	SCL_RBG0
#define SCL_RBG_TB_B	SCL_RBG1

void    SCL_Vdp2Init(void);
void    SCL_Open(Uint32 sclnum);
void    SCL_Close(void);
void    SCL_MoveTo(Fixed32 x,Fixed32 y,Fixed32 z);
void    SCL_SetColRamMode(Uint32 ComRamMode);

// dummy functions
#define SCL_InitVramConfigTb(tp)
#define SCL_SetVramConfig(tp)
#define SCL_SetCycleTable(tp)
#define SCL_InitConfigTb(scfg)
#define SCL_SetConfig(sclnum, scfg)
#define SCL_SetPriority(Object, Priority)
#define SCL_SetWindow(win,logic,enable,area,sx,sy,ex,ey)
#define SCL_SetDisplayMode(interlace,vertical,horizontal)
#define SCL_SetColMixRate(Surfaces,Rate)
#define SCL_SetSpriteMode(Type,ColMode,WinMode)
#define SCL_SetMosaic(surface,x,y)
#define SCL_AutoExec()
#define SCL_VblInit()
#define SCL_PriorityInit()
#define SCL_Rotate(xy,z,disp)
#define SCL_InitRotateTable(Address,Mode,rA,rB) 0

#define SCL_SET_N0CAOS(n0caos)
#define SCL_SET_N1CAOS(n1caos)
#define SCL_SET_N0CCEN(n0ccen)
#define SCL_SET_R0CAOS(r0caos)
#define SCL_SET_S0CCRT(s0ccrt)
#define SCL_SET_S1CCRT(s0ccrt)
#define SCL_SET_S2CCRT(s0ccrt)
#define SCL_SET_S3CCRT(s0ccrt)
#define SCL_SET_S4CCRT(s0ccrt)
#define SCL_SET_S5CCRT(s0ccrt)
#define SCL_SET_S6CCRT(s0ccrt)
#define SCL_SET_S0PRIN(s0prin)
#define SCL_SET_S1PRIN(s0prin)
#define SCL_SET_S2PRIN(s0prin)
#define SCL_SET_S3PRIN(s0prin)
#define SCL_SET_S4PRIN(s0prin)
#define SCL_SET_S5PRIN(s0prin)
#define SCL_SET_S6PRIN(s0prin)
#define SCL_SET_S7PRIN(s0prin)
#define SCL_SET_SPTYPE(sptype)
#define SCL_SET_SPCLMD(sptype)
#define SCL_SET_SPWINEN(sptype)
#define SCL_SET_SPCCCS(sptype)
#define SCL_SET_SPCCN(sptype)

#endif
