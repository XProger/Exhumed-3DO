#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define floor my_floor

/* stuff thats not used */
typedef enum {
  SR_STANDARDVOLUME = 0,
  SR_DOORWAY        = 1,
  SR_LIFTZONE       = 2
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

typedef struct
{
	long	x,y,z;
	short	s;
	short type;
	short wscale;
	short hscale;
	short ang;
	short frame;
	short clock;
	short radius;
	short height;
	long	dx,dy,dz;
	int		isfloor:1;
}	monstertype;

/* stuff that is used */
typedef struct
{short x,y,z;
} vertextype;

typedef struct
{short v0,v1,v2,v3; /*/index into vertex array for each vertex of face*/
 signed char tile; /*index of tile (texture)*/
} facetype;

typedef struct
{short firstface; /*index into face array of first face of wall
		    (-1, no visible face) */
 short lastface; /*index of last face */
 short v0; /*end points of wall in vertex array*/
 short nextsector; /*index of sector on other side of wall, no nextsector=-1*/
 short normal; /*normal angle of wall (facing into sector, 0-4095)*/
 short nx,ny,nz;
 int isblocked:1; /*if wall has a nextsector, set this bit if wall should
		    block motion anyway */
} walltype;


typedef struct
{short firstwall; /*index into wall array of first wall of sector*/
 short lastwall; /*index of last wall*/
 short firstsector; /*index into sectorlist array for head of visible
		      sector list */
 short lastsector; /*index for end of visible sector list*/
 short light; /*lighting level (0-255)*/
 short cielz,floorz,cielh,floorh,ceils,floors;
} sectortype;


#include LEVELFILE

#include "..\slevel.h"

#define MAXNMVERT 10000
#define MAXNMFACES 5000
#define MAXNMTEXTURES 10000
#define MAXNMVISIB 5000
#define MAXNMWALLS 5000
#define MAXNMSECTORS 200

int vertexMap[MAXNMVERT];
sVertexType outVert[MAXNMVERT];
int nmVertex;
sFaceType outFace[MAXNMFACES];
int nmFaces;
char outTexture[MAXNMTEXTURES];
int nmTextures;
/*short outVisibList[MAXNMVISIB];*/
int nmVisib;
sWallType outWalls[MAXNMWALLS];
int nmWalls;
sSectorType outSector[MAXNMSECTORS];
int nmSectors;
char outCutPlane[MAXNMSECTORS][MAXNMSECTORS];


typedef struct
{int x,y,z;} MthXyz;

int addVertex(short oldIndex)
{assert(nmVertex<MAXNMVERT);
 if (vertexMap[oldIndex]!=-1)
    return vertexMap[oldIndex];
 if (nmVertex==MAXNMVERT)
    {printf("Yarg!\n");
     exit(-1);
    }
 outVert[nmVertex].x=vertex[oldIndex].x<<15;
 outVert[nmVertex].z=vertex[oldIndex].y<<15;
 outVert[nmVertex].y=vertex[oldIndex].z<<15;
 vertexMap[oldIndex]=nmVertex;
 return nmVertex++;
}

void clearVertexMap(void)
{int i;
 for (i=0;i<MAXNMVERT;i++)
    vertexMap[i]=-1;
}

typedef
struct {double x,y,z;} fVert;

void getVertex(sVertexType *v,fVert *out)
{out->x=v->x/65536.0;
 out->y=v->y/65536.0;
 out->z=v->z/65536.0;
}

int addWall(walltype *wall)
{
 int i;
 assert(nmWalls<MAXNMWALLS);
 outWalls[nmWalls].flags=WALLFLAG_RECTANGLE;
 outWalls[nmWalls].nextSector=wall->nextsector;
/*
 if (!rectangle)
     clearVertexMap();
 */
 outWalls[nmWalls].v[0]=addVertex(wall->v0);
 outWalls[nmWalls].v[1]=addVertex(wall->v0+1);
 outWalls[nmWalls].v[2]=addVertex(wall->v0+2);
 outWalls[nmWalls].v[3]=addVertex(wall->v0+3);


 outWalls[nmWalls].textures=nmTextures;
 if (wall->firstface==-1)
    outWalls[nmWalls].flags|=WALLFLAG_INVISIBLE;
 else
    /* really should only be for rectangular walls */
    for (i=wall->firstface;i<=wall->lastface;i++)
       outTexture[nmTextures++]=face[i].tile;

 {fVert v0,v1,v2;
  double len;
  getVertex(outVert+outWalls[nmWalls].v[0],&v0);
  getVertex(outVert+outWalls[nmWalls].v[1],&v1);
  getVertex(outVert+outWalls[nmWalls].v[2],&v2);
  len=sqrt((v0.x-v1.x)*(v0.x-v1.x)+
	   (v0.y-v1.y)*(v0.y-v1.y)+
	   (v0.z-v1.z)*(v0.z-v1.z));
  outWalls[nmWalls].pixelLength=len;
  outWalls[nmWalls].tileLength=(len+TILESIZE/2)/TILESIZE;
  if (outWalls[nmWalls].tileLength<=0)
     outWalls[nmWalls].tileLength=1;
  len=sqrt((v2.x-v1.x)*(v2.x-v1.x)+
	   (v2.y-v1.y)*(v2.y-v1.y)+
	   (v2.z-v1.z)*(v2.z-v1.z));
  outWalls[nmWalls].tileHeight=(len+TILESIZE/2)/TILESIZE;
  if (outWalls[nmWalls].tileHeight<=0)
     outWalls[nmWalls].tileHeight=1;
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
 return nmWalls++;
}


void addSector(sectortype *sector)
{int w;
 assert(nmSectors<MAXNMSECTORS);
 outSector[nmSectors].firstWall=nmWalls;
 for (w=sector->firstwall;w<=sector->lastwall+2;w++)
    addWall(wall+w);
 outSector[nmSectors].lastWall=nmWalls-1;
 outSector[nmSectors].firstSector=sector->firstsector;
 outSector[nmSectors].lastSector=sector->lastsector;
 outSector[nmSectors].light=sector->light;
 nmSectors++;
}

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

void sortWalls(void)
{int s,w,i,j,wallCount,firstWall;
 sWallType swap;
 for (s=0;s<nmSectors;s++)
    {firstWall=outSector[s].firstWall;
     wallCount=outSector[s].lastWall-outSector[s].firstWall+1;
     for (w=1;w<wallCount;w++)
	{for (i=w;i>0;i--)
	    if (outWalls[firstWall+i-1].nextSector==-1 &&
		outWalls[firstWall+i].nextSector!=-1)
	       {swap=outWalls[firstWall+i-1];
		outWalls[firstWall+i-1]=outWalls[firstWall+i];
		outWalls[firstWall+i]=swap;
	       }
	}
    }
}

#define f(x) (((double)(x))/65536.0)
/* check to see if all the points in 'sector' are on the back side of
   'wall' */
int testWall(sWallType *testWall,int sector)
{int w,v;
 sSectorType *s=outSector+sector;
 sVertexType *testVert=outVert+testWall->v[0];
 sWallType *wall;
 for (w=s->firstWall;w<=s->lastWall;w++)
    {wall=outWalls+w;
     for (v=0;v<4;v++)
	{if ((f(outVert[wall->v[v]].x)-f(testVert->x))*f(testWall->normal[0])+
	     (f(outVert[wall->v[v]].y)-f(testVert->y))*f(testWall->normal[1])+
             (f(outVert[wall->v[v]].z)-f(testVert->z))*f(testWall->normal[2])
	     >0.001)
	    return 0;
	}
    }
 return 1;
}

void findCutPlanes(void)
{int s1,s2,w;
 for (s1=0;s1<nmSectors;s1++)
    for (s2=0;s2<nmSectors;s2++)
       {if (s1==s2)
	   continue;
	for (w=outSector[s1].firstWall;w<=outSector[s1].lastWall;w++)
	   if (testWall(outWalls+w,s2))
	      {outCutPlane[s1][s2]=w+1-outSector[s1].firstWall;
	       goto Continue;
	      }
	for (w=outSector[s2].firstWall;w<=outSector[s2].lastWall;w++)
	   if (testWall(outWalls+w,s1))
	      {outCutPlane[s1][s2]=-(w+1-outSector[s2].firstWall);
	       goto Continue;
	      }
        printf("Bad!\n");
        outCutPlane[s1][s2]=1;
     Continue:
	continue;
       }
}

int main(int argc,char **argv)
{int s,w,i;
 if (argc!=2)
    {printf("Barf\n");
     exit(0);
    }
 ofile=fopen(argv[1],"wb");
 if (!ofile)
    {printf("Harf!\n");
     exit(0);
    }
 nmVertex=0;
 nmFaces=0;
 nmTextures=0;
 nmVisib=sectorlistcount;
 nmWalls=0;
 nmSectors=0;
 clearVertexMap();

 for (s=0;s<sectorcount;s++)
    addSector(sector+s);
 for (s=0;s<MAXNMSECTORS;s++)
    for (w=0;w<MAXNMSECTORS;w++)
       outCutPlane[s][w]=99;

/* sortWalls();*/
 findCutPlanes();
 printf("NmCut=%d\n",nmSectors*nmSectors);
 for (s=0;s<nmSectors;s++)
    for (w=0;w<nmSectors;w++)
       writeChar(outCutPlane[s][w]);
 return 0;

 nmSectors=(nmSectors+3)&0xfffffffc;
 nmWalls=(nmWalls+3)&0xfffffffc;
 nmFaces=(nmFaces+3)&0xfffffffc;
 nmVertex=(nmVertex+3)&0xfffffffc;
 nmTextures=(nmTextures+3)&0xfffffffc;
 nmVisib=(nmVisib+3)&0xfffffffc;
 nmFaces=(nmFaces+3)&0xfffffffc;

 writeInt(nmSectors);
 writeInt(nmWalls);
 writeInt(nmVertex);
 writeInt(nmFaces);
 writeInt(nmTextures);
 writeInt(nmVisib);

 printf("NmSectors=%d\n",nmSectors);
 for (s=0;s<nmSectors;s++)
    {writeShort(outSector[s].firstWall);
     writeShort(outSector[s].lastWall);
     writeShort(outSector[s].firstSector);
     writeShort(outSector[s].lastSector);
     writeShort(outSector[s].light);
     writeShort(0);
    }
 printf("NmWalls=%d\n",nmWalls);
 for (s=0;s<nmWalls;s++)
    {writeInt(outWalls[s].normal[0]);
     writeInt(outWalls[s].normal[1]);
     writeInt(outWalls[s].normal[2]);
     writeInt(outWalls[s].d);
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
     writeShort(outWalls[s].light);
     writeShort(outWalls[s].pixelLength);
     writeChar(outWalls[s].tileLength);
     writeChar(outWalls[s].tileHeight);
    }
 printf("NmVert=%d\n",nmVertex);
 for (s=0;s<nmVertex;s++)
    {writeInt(outVert[s].x);
     writeInt(outVert[s].y);
     writeInt(outVert[s].z);
    }
 printf("NmFaces=%d\n",nmFaces);
 for (s=0;s<nmFaces;s++)
    {writeShort(outFace[s].v[0]);
     writeShort(outFace[s].v[1]);
     writeShort(outFace[s].v[2]);
     writeShort(outFace[s].v[3]);
     writeChar(outFace[s].tile);
     writeChar(outFace[s].light);
    }
 printf("NmTextures=%d\n",nmTextures);
 for (s=0;s<nmTextures;s++)
    writeChar(outTexture[s]);
 printf("NmVisib=%d\n",nmVisib);
 for (s=0;s<nmVisib;s++)
    writeShort(sectorlist[s]);
 printf("NmCut=%d\n",nmSectors*nmSectors);
 for (s=0;s<nmSectors;s++)
    for (w=0;w<nmSectors;w++)
       writeChar(outCutPlane[s][w]);

 return 0;

}
