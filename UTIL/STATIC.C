#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>

void addPB(void);

#include "..\slevel.h"
#include "..\sound.h"

#define MAXNMSEQ 800
#define MAXNMFRAMES 3000
#define MAXNMCHUNKS 9000

FILE *ofile;

short outSequenceList[MAXNMSEQ];
int nmSequences;
sFrameType outFrames[MAXNMFRAMES];
int nmFrames;
sChunkType outChunk[MAXNMCHUNKS];
int nmChunks;

int nmNonRect;
int tilesWrote=0;

typedef struct
{int x,y,z;} MthXyz;

#define MAXNMSUBTILES 10

typedef struct
{char filename[80];
 int subx[MAXNMSUBTILES],suby[MAXNMSUBTILES];
 int nmSubTiles;
 int uses;
 int width,height;
 int number,palNm;
 char flags;
} TileInfo;

#define MAXNMTILES 100
TileInfo tiles[MAXNMTILES];
int nmTiles=0;

#define MAXNMPAL 50
unsigned short palletes[MAXNMPAL][256];
int nmPals=0;

int bytesWritten=0;

void writeShort(short i)
{fputc(i >> 8,ofile);
 fputc(i & 0xff,ofile);
 bytesWritten+=2;
}

void writeInt(int i)
{fputc(i >> 24,ofile);
 fputc((i >> 16)&0xff,ofile);
 fputc((i >> 8)&0xff,ofile);
 fputc(i & 0xff,ofile);
 bytesWritten+=4;
}

void writeChar(char i)
{fputc(i,ofile);
 bytesWritten++;
}

unsigned char vdp2PicData[512][512];
unsigned char vdp2PicMap[512][512];

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

void readBmp(char *filename)
{FILE *ifile;
 char buff[160];
 int width,height,i;
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
 for (i=picHeight-1;i>=0;i--)
    {fread(picdata[i],(picWidth+3)&0xfffffffc,1,ifile);
    }
/* picPal[255]=picPal[0];
 picPal[0]=0x0000;*/
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

int checkIfVDP2Tile(char *name)
{static char *list[]=
    {"2422.bmp","2424.bmp","0232.bmp",
     "0233.bmp","0234.bmp",
     "3191.bmp","3192.bmp","3190.bmp",
     "3267.bmp","3268.bmp","2177.bmp",
     "2178.bmp","1214.bmp","1220.bmp",
     "2400.bmp","2419.bmp","3296.bmp",
     "3299.bmp","3297.bmp","3197.bmp","x-3197.bmp",NULL};
 int i;
 for (i=0;list[i];i++)
    if (!stricmp(name,list[i]))
       return 1;
 return 0;
}

int addTile(char *filename,char flags)
{int i;
 for (i=0;i<nmTiles;i++)
    if (!stricmp(filename,tiles[i].filename))
       break;
 if (i<nmTiles)
    {tiles[i].uses++;
     if (tiles[i].flags!=TILEFLAG_VDP2)
	tiles[i].flags|=flags;
     return i;
    }
 strcpy(tiles[nmTiles].filename,filename);
 tiles[nmTiles].uses=1;
 tiles[nmTiles].flags=flags;
 {char buff[80];
  strcpy(buff,"tiles\\");
  strcat(buff,filename);
  readBmp(buff);
 }
 tiles[nmTiles].width=picWidth;
 tiles[nmTiles].height=picHeight;
 tiles[nmTiles].number=0;
 tiles[nmTiles].palNm=mapPal();
 if (checkIfVDP2Tile(filename))
    {tiles[nmTiles].flags=TILEFLAG_VDP2;
     tiles[nmTiles].nmSubTiles=1;
    }
 else
    {/* add sub tile info */
     int w,h,n;
     n=0;
     for (h=0;h<picHeight;h+=64)
	for (w=0;w<picWidth;w+=64)
	   {tiles[nmTiles].subx[n]=w;
	    tiles[nmTiles].suby[n]=h;
	    n++;
	   }
     tiles[nmTiles].nmSubTiles=n;
    }
 if (nmTiles>0)
    tiles[nmTiles].number=tiles[nmTiles-1].number+tiles[nmTiles-1].nmSubTiles;
 else
    tiles[nmTiles].number=0;
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
	    picdata[y1][x1]=0;
	    continue;
	   }
	RLEBuffer[pos++]=picdata[y1][x1];
	picdata[y1][x1]=0;
       }
 size=writeRLE(RLEBuffer,pos,NULL);
 writeShort(size);
 writeRLE(RLEBuffer,pos,ofile);
 bytesWritten+=size;
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
	    picdata[y1][x1]=0;
	    continue;
	   }
	writeChar(picdata[y1][x1]);
	picdata[y1][x1]=0;
       }
 return 64*64;
}

int writeTile(int tileNm)
{int s,x,y,size,dim;
 if (tiles[tileNm].flags==TILEFLAG_VDP2)
    {writeShort(tiles[tileNm].flags);
     writeShort(tiles[tileNm].subx[0]);
     writeShort(tiles[tileNm].suby[0]);
     writeShort(tiles[tileNm].width);
     writeShort(tiles[tileNm].height);
     return 10;
    }
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
 if ((tiles[tileNm].flags & TILEFLAG_8BPP) &&
     (tiles[tileNm].flags & TILEFLAG_64x64))
    tiles[tileNm].flags|=TILEFLAG_RLE;
 for (s=0;s<tiles[tileNm].nmSubTiles;s++)
    {tilesWrote++;
     writeShort(tiles[tileNm].flags);
     writeShort(tiles[tileNm].palNm);
     dim=0;
     if (tiles[tileNm].flags & TILEFLAG_64x64)
	dim=64;
     if (tiles[tileNm].flags & TILEFLAG_32x32)
	dim=32;
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
{if (tiles[tileNm].flags==TILEFLAG_VDP2)
    return 10;
 else
    return tiles[tileNm].nmSubTiles*(64*64+4);
}

#define MAXNMSOUNDS 100

short outSoundMap[ST_NMSTATICSOUNDGROUPS];
int nmSounds=0;
char soundList[MAXNMSOUNDS][80];

int addSound(char *filename)
{assert(nmSounds<MAXNMSOUNDS);
 printf("Adding sound %s\n",filename);
 strcpy(soundList[nmSounds],filename);
 return nmSounds++;
}

int maybeAddSound(char *filename)
{int i;
 for (i=0;i<nmSounds;i++)
    if (!strcmp(filename,soundList[i]))
       return i;
 return addSound(filename);
}

#define MAXNMTHINGS 1000
short sequenceIndex[MAXNMTHINGS],frameList[MAXNMTHINGS],
   frameFlags[MAXNMTHINGS];
short chunkx[MAXNMTHINGS],chunky[MAXNMTHINGS],chunkPic[MAXNMTHINGS],
   chunkFlags[MAXNMTHINGS];
short frameSounds[MAXNMTHINGS];
int tileMap[MAXNMTHINGS];
char soundMap[MAXNMTHINGS][80];

int loadSequenceSet(char *filename)
{int i,c,s;
 FILE *ifile;
 char buff[160];
 char *p;
 short nmSounds,nmTiles,nmSeq,nmFrm,nmChnk;
 int retVal;
 retVal=nmSequences;
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

 /* load tiles */
 fread(&nmTiles,2,1,ifile);
 assert(nmTiles<MAXNMTHINGS);
 for (i=0;i<nmTiles;i++)
    {p=buff;
     while (1)
	{fread(p,1,1,ifile);
	 if (*p=='\n')
	    break;
	 p++;
	}
     *p=0;
     tileMap[i]=addTile(buff,TILEFLAG_64x64|TILEFLAG_8BPP|TILEFLAG_PALLETE);
    }
 fread(&nmSeq,2,1,ifile);
 assert(nmSeq<MAXNMTHINGS);
 fread(sequenceIndex,nmSeq,2,ifile);
 for (i=0;i<nmSeq;i++)
    {outSequenceList[nmSequences+i]=nmFrames+sequenceIndex[i];
    }
 nmSequences+=nmSeq;

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

 fread(frameSounds,nmFrm,2,ifile);

 for (i=0;i<nmFrm;i++)
    {outFrames[nmFrames].chunkIndex=nmChunks;
     outFrames[nmFrames].flags=frameFlags[i];
     if (!frameSounds[i])
	outFrames[nmFrames].sound=-1;
     else
	outFrames[nmFrames].sound=
	   maybeAddSound(soundMap[(frameSounds[i]&0xfff)-1]);
     nmFrames++;
     {int frameSize;
      TileInfo *ti;
      if (i<nmFrm-1)
	 frameSize=frameList[i+1]-frameList[i];
      else
	 frameSize=nmChnk-frameList[i];
      for (c=frameList[i];c<frameList[i]+frameSize;c++)
	 {ti=tiles+tileMap[chunkPic[c]];
	  assert(c<nmChnk);
	  for (s=0;s<ti->nmSubTiles;s++)
	     {outChunk[nmChunks].chunkx=chunkx[c]+ti->subx[s];
	      outChunk[nmChunks].chunky=chunky[c]+ti->suby[s];
	      outChunk[nmChunks].tile=ti->number+s;
	      outChunk[nmChunks].flags=chunkFlags[c];
	      if (outChunk[nmChunks].flags & 1)
		 {/* flip across x axis */
		  if (checkIfVDP2Tile(ti->filename))
		     {char buff[160];
		      if (strstr(ti->filename,"3197"))
			 {strcpy(buff,"x-");
			  strcat(buff,ti->filename);
			  outChunk[nmChunks].tile=
			     addTile(buff,TILEFLAG_64x64|TILEFLAG_8BPP|
				     TILEFLAG_PALLETE);
			 }
		     }
		  else
		     {outChunk[nmChunks].chunkx=
			 chunkx[c]+
			    ti->width-(ti->subx[s]+64);
		     }
		 }
	      /*printf("#%d: x%d y%d tile%d %s\n",nmChunks,
		outChunk[nmChunks].chunkx,
		outChunk[nmChunks].chunky,
		outChunk[nmChunks].tile,
		tiles[outChunk[nmChunks].tile].filename);*/
	      nmChunks++;
	     }
	 }
     }
    }
 return retVal;
}

int loadSequences(void)
{int size;
 nmSequences=0;
 nmFrames=0;
 nmChunks=0;
 addTile("x-3197.bmp",TILEFLAG_64x64|TILEFLAG_8BPP|TILEFLAG_PALLETE);
 addTile("2422.bmp",TILEFLAG_64x64|TILEFLAG_8BPP|TILEFLAG_PALLETE);
 addTile("2424.bmp",TILEFLAG_64x64|TILEFLAG_8BPP|TILEFLAG_PALLETE);
 loadSequenceSet("sword.seq");
 loadSequenceSet("pistol.seq");
 loadSequenceSet("m60_new.seq");
 loadSequenceSet("grenade.seq");
 loadSequenceSet("flamer.seq");
 loadSequenceSet("cobra.seq");
 loadSequenceSet("ravolt.seq");
 loadSequenceSet("ezring.seq");

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
	  nmChunks*sizeof(sChunkType);
 return size;
}


void writeSequences(int size)
{int i;
 assert(size==sizeof(struct seqHeader)+
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
     /*assert(outFrames[i].sound==-1);*/
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
}



char waveBuff[500000];
int waveSize,waveSampleRate,waveBPS;
int waveLoopStart;

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
 writeInt(((octave<<11)&0x7800)|(fns&0x3ff));
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
    {for (i=0;i<waveSize;i++)
	writeChar(waveBuff[i] ^ 0x80);
    }
 return waveSize+12;
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
 fread(waveBuff,4,1,ifile);
 waveBuff[4]=0;
 if (strcmp(waveBuff,"data"))
    {printf("Wave file '%s' half bad! %s\n",name,waveBuff);
     exit(-1);
    }
 fread(&waveSize,4,1,ifile);
 if (!waveSize)
    {printf("Wave size==0!\n");
     exit(-1);
    }
 fread(waveBuff,1,waveSize,ifile);
 waveLoopStart=-1;
 {char *match="smpl\074";
  char *pos=match;
  char c;
  while (!feof(ifile))
     {fread(&c,1,1,ifile);
      if (c==*pos)
	 {pos++;
	  if (!*pos)
	     break;
	 }
      else
	 pos=match;
     }
  if (!feof(ifile))
     {/* found match */
      char crap[100];
      int i;
      fread(crap,47,1,ifile);
      fread(&i,4,1,ifile);
      if (!feof(ifile))
	 {if (i==-1)
	     printf("Sample %s has no loop\n",name);
	  else
	     printf("Sample %s has a loop @%d\n",name,i);
	  waveLoopStart=i;
	 }
     }
 }
 fclose(ifile);
}

int convertTile(TileInfo *tile)
{int x,y,x1,y1;
 /* find a place for tile in ram */
 for (y=0;y<512-tile->height;y++)
    for (x=0;x<512-tile->width;x++)
       {for (y1=0;y1<tile->height;y1++)
	   for (x1=0;x1<tile->width;x1++)
	      if (vdp2PicMap[y+y1][x+x1])
		 goto Fail;
	goto Score;
     Fail:
	x=x;
       }
 return 0;
 Score:
 /* x & y hold a valid position */
 {char buff[80];
  strcpy(buff,"tiles\\");
  strcat(buff,tile->filename);
  readBmp(buff);
  printf("Converting %s\n",buff);
 }
 for (y1=0;y1<tile->height;y1++)
    for (x1=0;x1<tile->width;x1++)
       {vdp2PicData[y+y1][x+x1]=picdata[y1][x1];
	vdp2PicMap[y+y1][x+x1]=1;
       }
 tile->subx[0]=x;
 tile->suby[0]=y;
 return 1;
}

void convertTiles(void)
{int x,y;
 int t;
 for (y=0;y<512;y++)
    for (x=0;x<512;x++)
       vdp2PicMap[y][x]=0;

 for (y=0;y<512;y++)
    for (x=0;x<512;x++)
       vdp2PicData[y][x]=255;
 for (t=0;t<nmTiles;t++)
    {if (tiles[t].flags==TILEFLAG_VDP2)
	if (!convertTile(tiles+t))
	   {printf("Can't fit tile into vdp2 ram.\n");
	    exit(-1);
	   }
    }
}

void writeVDP2Data(void)
{int x,y;
 for (y=0;y<512;y++)
    for (x=0;x<512;x++)
       {if (vdp2PicData[y][x]==255)
	   {writeChar(0);
	    continue;
	   }
	if (vdp2PicData[y][x]==0)
	   {writeChar(255);
	    continue;
	   }
	writeChar(vdp2PicData[y][x]);
       }
}


void writeSoundSet(void)
{int i;
 char buff[160];
 writeInt(ST_NMSTATICSOUNDGROUPS);
 for (i=0;i<ST_NMSTATICSOUNDGROUPS;i++)
    writeShort(outSoundMap[i]);
 writeInt(nmSounds);
 for (i=0;i<nmSounds;i++)
    {strcpy(buff,"sounds\\");
     strcat(buff,soundList[i]);
     loadWave(buff);
     writeSound();
    }
}

void writeSounds(void)
{int i;
 for (i=0;i<ST_NMSTATICSOUNDGROUPS;i++)
    outSoundMap[i]=-2;

 outSoundMap[ST_JOHN]=nmSounds;
 addSound("jon-jump.wav");
 addSound("jon-die4.wav");
 addSound("splash-b.wav");
 addSound("jon-hit2.wav");
 addSound("jon-hit3.wav");
 addSound("scuba.wav");
 addSound("jon_gasp.wav");
 addSound("sizzle1.wav");

 outSoundMap[ST_ITEM]=nmSounds;
 addSound("item_key.wav");
 addSound("ammo.wav");
 addSound("health1.wav");
 addSound("ammofull.wav");
 addSound("health2.wav");
 addSound("item_wrn.wav");

 outSoundMap[ST_FLAMER]=nmSounds;
 addSound("ft-on2.wav");
 addSound("ft-off2.wav");
 addSound("ft-run3.wav");

 outSoundMap[ST_PUSHBLOCK]=nmSounds;
 addSound("doormove.wav");
 addSound("doorshut.wav");
 addSound("elevator.wav");

 outSoundMap[ST_INTERFACE]=nmSounds;
 addSound("select.wav");
 addSound("toggle.wav");
 addSound("new-item.wav");

 outSoundMap[ST_RING]=nmSounds;
 addSound("ringfire.wav");

 outSoundMap[ST_BLOWPOT]=nmSounds;
 addSound("pot_pc1.wav");
 addSound("pot_pc2.wav");
 addSound("pot_pc3.wav");
 addSound("pot_pow.wav");

 outSoundMap[ST_MANACLE]=nmSounds;
 addSound("fist2.wav");
 addSound("drone.wav");

 writeSoundSet();
}



/* args 1: output file
 */
int main(int argc,char **argv)
{int seqSize;
 int i;
 int mark;
 if (argc!=2)
    {printf("Args bad.\n");
     exit(-1);
    }
 ofile=fopen(argv[1],"wb");
 if (!ofile)
    printf("Can't open output file.\n");

 /* load static tiles */
 addTile("shadow.bmp",TILEFLAG_64x64|TILEFLAG_8BPP|TILEFLAG_PALLETE);

 /* load sequences */
 seqSize=loadSequences();

 /* convert some tiles to vdp2 tiles */
 convertTiles();

 writeVDP2Data();
 mark=bytesWritten;
 /* write sounds */
 writeSounds();
 printf("%d bytes of sound data\n",bytesWritten-mark);
 /* write swapable tiles */
 {int nm=0;
  for (i=0;i<nmTiles;i++)
     nm+=tiles[i].nmSubTiles;
  printf("%d tiles, ",nm);
  writeInt(nm);
 }
 mark=bytesWritten;
 for (i=0;i<nmTiles;i++)
    writeTile(i);
 printf("%d bytes.\n",bytesWritten-mark);

 /* write sequences */
 writeInt(seqSize);
 mark=bytesWritten;
 writeSequences(seqSize);
 fclose(ofile);
 printf("%d bytes of sequence data.\n",bytesWritten-mark);

 return 0;
}
