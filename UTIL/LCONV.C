#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>

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

#define PLAYER_TYPE 1

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
{int v0,v1,v2,v3; /*/index into vertex array for each vertex of face*/
 signed char tile; /*index of tile (texture)*/
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


#include LEVELFILE
FILE *ofile;
void printNorm(walltype *w)
{fprintf(ofile,"{%f %f %f} ",
	 w->nx/16384.0,
	 w->ny/16384.0,
	 w->nz/16384.0);
}

void printVert(int vn)
{fprintf(ofile,"{%d %d %d} ",
	 vertex[vn].x,
	 vertex[vn].y,
	 vertex[vn].z);
}


int main(int argc,char **argv)
{int s,w,f,i;
 int lightPatches=0;
 char buffer[160];
 if (argc!=2)
    {printf("Barf!\n");
     exit(-1);
    }
 sprintf(buffer,"%s.pat",argv[1]);
 ofile=fopen(buffer,"w");
 if (!ofile)
    {printf("Barf!\n");
     exit(-1);
    }
 i=0;
 for (s=0;s<sectorcount;s++)
    for (w=sector[s].firstwall;w<=sector[s].lastwall+2;w++)
       {if (wall[w].firstface==-1)
	   continue;
	i++;
       }
 fprintf(ofile,"Number objects %d\n",i);

 for (s=0;s<sectorcount;s++)
    for (w=sector[s].firstwall;w<=sector[s].lastwall+2;w++)
       {if (wall[w].firstface==-1)
	   continue;
	fprintf(ofile,"Object foo%d mesh {\n",w);
	fprintf(ofile,"  OWMatrix foo {1 0 0 0 1 0 0 0 1 0 0 0}");
	if (s==1 /*wall[w].nz==-16384*/) /* make ceilings luminous */
	   {fprintf(ofile,"  Prop foo {E{30.0 30.0 30.0} p{0.3 0.3 0.3} "
		    "Kd{1.0} Ks{0.0}}\n");
	    lightPatches+=(wall[w].lastface-wall[w].firstface+1);
	   }
	else
	   fprintf(ofile,"  Prop foo {E{0.0 0.0 0.0} p{0.7 0.7 0.7} "
		   "Kd{1.0} Ks{0.0}}\n");
	/* used to be .7 reflectance */
	fprintf(ofile,"  NumMeshes 1\n");
	fprintf(ofile,"  Mesh foo%d %d {\n",
		w,wall[w].lastface-wall[w].firstface+1);
	for (f=wall[w].firstface;f<=wall[w].lastface;f++)
	   {fprintf(ofile,"  Patch norm 4 {");
	    for (i=0;i<4;i++)
	       printNorm(wall+w);
	    fprintf(ofile,"}\n");
	    fprintf(ofile,"  Patch vert 4 {");
	    printVert(face[f].v0);
	    printVert(face[f].v1);
	    printVert(face[f].v2);
	    printVert(face[f].v3);
	    fprintf(ofile,"}\n");
	   }
	fprintf(ofile,"  }}\n");
       }
 fclose(ofile);
 printf("%d light patches in world\n",lightPatches);
 return 0;
}
