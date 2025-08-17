#ifndef __INCLUDEDpich
#define __INCLUDEDpich

#include "picset.h"

enum Class {TILE16BPP,TILE8BPP,TILEVDP,TILESMALL16BPP,TILESMALL8BPP,
#ifdef JAPAN
	       TILEJCHAR,
#endif
	       NMCLASSES};

int initPicSystem(int _picNmBase,int *classSizes);

int addPic(enum Class bpp,void *data,void *pal,int lock);
   /* if a pic is locked, its memory may be free'd after this call */

int mapPic(int picNm);

void pic_nextFrame(int *swaps,int *used);

void loadTiles(int fd);
int loadWeaponTiles(int fd);

int getPicClass(int p);
void displayVDP2Pic(int picNm,int xo,int yo);
void updateVDP2Pic(void);
void dontDisplayVDP2Pic(void);
void delay_dontDisplayVDP2Pic(void);
void resetPics(void);

void setDrawModeBit(int class,int bit,int onOff);

void markAnimTiles(void);
void advanceWallAnimations(void);

int loadPicSetAsPics(int fd,int class);

#endif
