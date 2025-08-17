#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>

#include "world.h"
#include "main.h"
#include "runlist.h"
#include "sprite.h"
#include "ai.h"
#include "seq.h"

#define EDITMODE
int menuseq;

short freesprite;
spritetype sprite[MAXSPRITES];
short targetcount;
short target[MAXTARGETS];

short monstertypes[20];
short monstercount2;

char *monstername[]=
{
	"Player",

	"Anubis",
	"Bastet",
	"Hawk",
	"Queen",
	"Mantis",
	"Mummy",
	"Omenwasp",
	"Piranha",
	"Selkis",
	"Sentry",
	"Set",
	"Spider",
	"Spiny Ball",

	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"X Key",
	"Bug Key",
	"Time Key",
	"Plant Key",
	"Shawl",
	"Feather",
	"Mask",
	"Prism",
	"Sandals",
	"Scepter",
	"Anklets",
	"Health",
	"Ammo",
	"Pistol",
	"M-60",
	"Orb",
	"Flamer",
	"Staff",
	"Ring",
	"Manacle",
	"Pot",
	"Full Ammo",
	"Full Health",
	"Vessel 1",
	"Vessel 2",
	"Vessel 3",
	"Vessel 4",
	"Vessel 5",
	"Camel",

	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"Contain 1",
	"Contain 2",
	"Contain 3",
	"Contain 4",
	"Contain 5",
	"Contain 6",
	"Contain 7",
	"Contain 8",
	"Contain 9",
	"Contain 10",
	"Contain 11",
	"Contain 12",
	"Contain 13",
	"Contain 14",
	"Contain 15",
	"Contain 16",
	"Contain 17",

	"Torch 1",
	"Torch 2",
	"Torch 3",
	"Torch 4",
	"Torch 5",
	"Torch 6",
	"Torch 7",
	"Torch 8",
	"Torch 9",
	"Torch 10",
	"Torch 11",
	"Torch 12",
	"Torch 13",
	"Torch 14",
	"Torch 15",
	"Torch 16",
	"Torch 17",
	"Torch 18",
	"Torch 19",
	"Torch 20",
	"Torch 21",
	"Torch 22",
	"Torch 23",
	"Torch 24",
	"Torch 25",
	"Torch 26",
	"Torch 27",
	"Torch 28",
	"Torch 29",
	"Torch 30",
	"Torch 31",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

#define MAXMENUSEQS 9
char *menuseqfile[MAXMENUSEQS]=
{
	"sub1.pss",
	"ibomb.pss",
	"isand.pss",
	"ishawl.pss",
	"iank.pss",
	"ifeath.pss",
	"imask.pss",
	"iscept.pss",
	"imap.pss",
};

char *weaponfile[MAXWEAPONS]=
{
	"swordd.pss",
	"pistoll.pss",
	"m600.pss",
	"grenadee.pss",
	"flamerr.pss",
	"cobraa.pss",
	"ringg.pss",
	"ravoltt.pss"
};

void moveSlotToEnd(short i)
{
	nextcacheslot[prevcacheslot[i]]=nextcacheslot[i];
	prevcacheslot[nextcacheslot[i]]=prevcacheslot[i];
	nextcacheslot[i]=LASTCACHESLOT;
	prevcacheslot[i]=prevcacheslot[LASTCACHESLOT];
	nextcacheslot[prevcacheslot[i]]=i;
	prevcacheslot[LASTCACHESLOT]=i;
}

void decompressTile(short newtile)
{
	int i,j;
	int k,l;
	int w,h;
	int len;

	i=0;
	j=animtileaddr[newtile];

	w=animtilewidth[newtile];
	h=abs(animtileheight[newtile]);

	len=w*h;

	while(i<len)
	{
		for (k=seqtiles[j];k>0;k--,i++)
		{
			tilebuffer[i]=255;
		}
		j++;

		l=j+1;
		for (k=seqtiles[j];k>0;k--,l++,i++)
		{
			tilebuffer[i]=seqtiles[l];
		}
		j+=(seqtiles[j]+1);
	}

//	i=tilecacheslot[newtile];
//	monstertile[i].tpage=LoadTPage((u_long *)tilebuffer,1,0,mtilex[i],mtiley[i],w,h);
//	DrawSync(0);

}

void countBlanks()
{
	int i,j,k;
	int w,h,len;

	for (i=0;i<basetile;i++)
	{
		w=animtilewidth[i];
		h=animtileheight[i];

		len=w*abs(h);

		if (h<0)
		{
			decompressTile(i);
			j=0;
			for (k=0;k<len;k++)
				if (tilebuffer[k]!=255) j=1;
			if (!j) blankcount++;
		}
		else
		{
			j=0;
			for (k=0;k<len;k++)
				if (seqtiles[animtileaddr[i]+k]!=255) j=1;
			if (!j) blankcount++;
		}
	}

}

void getCacheSlot(short newtile)
{
	int	i;
	int	x,y;
//	int w,h;

	i=nextcacheslot[FIRSTCACHESLOT];
	if (cachetile[i]>=0)
		tilecacheslot[cachetile[i]]=-1;	//previous tile in slot	now has no slot
	cachetile[i]=newtile;
	tilecacheslot[newtile]=i;
	moveSlotToEnd(i);

//	w=0;//(64-animtilewidth[newtile])&4;
//	monstertile[i].u0=w;
//	h=0;//(64-animtileheight[newtile])&4;
//	monstertile[i].v0=(monstertile[i].v0&0xc0)+h;

//	x=mtilex[i]+w;
//	y=mtiley[i]+h;

	x=mtilex[i];
	y=mtiley[i];

	monstertile[i].w=animtilewidth[newtile];
	monstertile[i].h=abs(animtileheight[newtile]);
	monstertile[i].clut=animtilepal[newtile];

	if(animtileheight[newtile]<0)
	{
		decompressTile(newtile);
		monstertile[i].tpage=LoadTPage((u_long *)tilebuffer,1,0,x,y,animtilewidth[newtile],-animtileheight[newtile]);
		DrawSync(0);
	}
	else
		monstertile[i].tpage=LoadTPage((u_long *)(seqtiles+animtileaddr[newtile]),1,0,x,y,animtilewidth[newtile],animtileheight[newtile]);

	cachecount++;
}

void rotMonster(int m, int x0, int y0, int z0)
{
	long	p;
	long flag;
	long z;
	short xy[2];
	SVECTOR	sv;

	sv.vx=(sprite[m].x>>16)-x0;
	sv.vy=(sprite[m].y>>16)-y0;
	sv.vz=(sprite[m].z>>16)-z0;
	z=RotTransPers(&sv,(long *)xy,&p,&flag);
	lmonster[m].vx=xy[0];
	lmonster[m].vy=xy[1];
	lmonster[m].vz=z;
}

void rotMonster2(int m, int x0, int y0, int z0)
{
	long flag;
	SVECTOR	sv;

	sv.vx=(sprite[m].x>>16)-x0;
	sv.vy=(sprite[m].y>>16)-y0;
	sv.vz=(sprite[m].z>>16)-z0;
	RotTrans(&sv,&lmonster[m],&flag);
}

void addMonsterpoints(long x, long y, long z, short s, short r, short height, short spr, short m)
{
	int i;
	int	mr;

	i=sector[s].firstsprite;

	while (i>=0)
	{
		if ((i!=m)&&(i!=spr)&&(!sprite[i].nonblocking))
		{
			mr=(sprite[i].radius+r)*2;
			if ( (mr>abs((sprite[i].x>>16)-x)) && (mr>abs((sprite[i].y>>16)-y)) )
			{
				monsterpoint[monsterpoints]=i;
				monsterpoints++;
			}
		}
		i=sprite[i].nextsprite;
	}
}

int hitscan(int *x,int *y,int *z,short *s,short dist, short yaw, short pitch, short skipsprite)
{
		spritetype m;
		int hit;

		m.x=*x;
		m.y=*y;
		m.z=*z;
		m.s=*s;

		if ((pitch<=1024)||(pitch>=3072))
		{
			m.angle=yaw;
			m.stepz=pitch;
		}
		else
		{
			m.angle=(yaw+2048)&4095;
			m.stepz=(2048-pitch)&4095;
		}

		m.radius=(dist*costable[m.stepz>>1])>>14;
		m.dx=(m.radius*costable[m.angle>>1])<<2;
		m.dy=(m.radius*sintable[m.angle>>1])<<2;
		m.dz=(dist*sintable[m.stepz>>1])<<2;

		hit=hitScan2(&m, skipsprite);
		*x=m.x;
		*y=m.y;
		*z=m.z;
		*s=m.s;
		return hit;
}

int hitScan2(spritetype *m, int skipsprite)
{
	//hitscan uses the sector,x,y,z,angle,dx,dy,dz from a
	//spritetype record	directly.

	int s=m->s;
	int x=m->x>>16;
	int y=m->y>>16;
	int z=m->z>>16;
	int yaw=m->angle>>1;
	int yaw2=(-yaw)&2047;
	int pitch=m->stepz>>1;
	int pitch2=(-pitch)&2047;
	int dx=m->dx>>16;
	int dy=m->dy>>16;
	int dz=m->dz>>16;
	int distxy=m->radius;

	int v1,v2,v3,v4;
	int i;
	int firstwall;
	int lastwall;
	int x1,y1,x2,y2;
	int z1,z2,z3,z4;
	int rx,ry,rz;
	int ry1,ry2,rx1,rx2;
	int rz2;
	int hit;

	int minz=0, minsprite=-1;
	int minxy=distxy+1;

	if (distxy)
	{
		i=sector[s].firstsprite;

		while (i>=0)
		{
			if ((i!=skipsprite)&&(!sprite[i].nonblocking))
			{
				x1=(sprite[i].x>>16)-x;
				y1=(sprite[i].y>>16)-y;

				rx=rotatex(x1,y1,yaw2);

				if ((rx>=0)&&(rx<=distxy)&&(rx<minxy))
				{
					ry=rotatey(x1,y1,yaw2);

					if (((ry+sprite[i].radius)>=0)
						&&((ry-sprite[i].radius)<=0))
					{
						rz=(rx*dz)/distxy;
						rz+=z;
						z1=sprite[i].z>>16;
						if ((rz>z1)&&(rz<(z1+sprite[i].height)))
						{
							minxy=rx;
							minz=rz;
							minsprite=i;
						}
					}
				}
			}
			i=sprite[i].nextsprite;
		}

		// since sectors are assumed convex, return if sprite hit

		if (minxy<=distxy)
		{
			m->x=(x+rotatexy0(minxy,0,yaw))<<16;
			m->y=(y+rotateyy0(minxy,0,yaw))<<16;
			m->z=minz<<16;
			return((HIT_SPRITE<<16)+minsprite);
		}

		// do all vertical walls

		firstwall=sector[s].firstwall;
		lastwall=sector[s].lastwall+1;

		for (i=firstwall;i<lastwall;i++)
		{
			v1=wall[i].v0;
			v2=v1+1;

			x1=vertex[v1].x-x;
			y1=vertex[v1].y-y;
			x2=vertex[v2].x-x;
			y2=vertex[v2].y-y;

			ry1=rotatey(x1,y1,yaw2);
			ry2=rotatey(x2,y2,yaw2);

			if ((ry1>=0)&&(ry2<=0)&&(ry1!=ry2))
			{
				rx1=rotatex(x1,y1,yaw2);
				rx2=rotatex(x2,y2,yaw2);
//				if (ry1==ry2)
//				{
//					if (rx1<rx2)
//						rx=rx1;
//					else
//						rx=rx2;
//				}
//				else
					rx=rx2+((rx1-rx2)*(-ry2))/(ry1-ry2);

				if (rx<=distxy)
				{
					if (rx<0) rx=0;

					rz=z+((rx*dz)/distxy);

					v3=v1+2;
					v4=v1+3;

					z1=vertex[v1].z;
					z2=vertex[v2].z;
					z3=vertex[v3].z;
					z4=vertex[v4].z;

					if (!(((z1<rz)&&(z2<rz))||((z3>rz)&&(z4>rz))))
					{
						if((z1>=rz)&&(z2>=rz)&&(z3<=rz)&&(z4<=rz))
						{
							hit=1;
						}
						else
						{
							hit=0;
							if ((z1<rz)||(z2<rz))
							{
								//do triangle above
								rz2=z2+((z1-z2)*(-ry2))/(ry1-ry2);
								if (rz<=rz2)
								{
									hit=1;
								}
							}
							else
							{
								//do triangle below
								rz2=z3+((z4-z3)*(-ry2))/(ry1-ry2);
								if (rz>=rz2)
								{
									hit=1;
								}
							}
						}

						if (hit)
						{
							if ((wall[i].nextsector>=0)&&(!wall[i].isblocked))
							{
								m->s=wall[i].nextsector;
								return hitScan2(m,skipsprite);
							}
							else
							{
								m->x=(x+rotatexy0(rx,0,yaw))<<16;
								m->y=(y+rotateyy0(rx,0,yaw))<<16;
								m->z=rz<<16;
								return((HIT_WALL<<16)+i);
							}
						}
					}
				}
			}
		}

		//hit no walls or sprites, so might have hit floor or ceiling

		x2=x+dx;
		y2=y+dy;

		i=sector[s].lastwall+FLOOR;
		z1=getFloor(x,y,z,s,distxy)-z;
		z2=getFloor(x2,y2,z+dz,s,distxy)-z;

		ry1=rotatey(distxy,z2,pitch2);
		ry2=rotateyx0(0,z1,pitch2);
		if ((ry1>=0)&&(ry2<=0))
		{
			rx1=rotatex(distxy,z2,pitch2);
			rx2=rotatexx0(0,z1,pitch2);

			if (ry1==ry2)
			{
				if (rx1<rx2)
					rx=rx1;
				else
					rx=rx2;
			}
			else
				rx=rx2+((rx1-rx2)*(-ry2))/(ry1-ry2);

			rx=rotatexy0(rx,0,pitch);
			if (rx<=distxy)
			{
				rz=(rx*dz)/distxy;
				rz+=z;
				if ((wall[i].nextsector>=0)&&(!wall[i].isblocked))
				{
					m->s=wall[i].nextsector;
					return hitScan2(m,skipsprite);
				}
				else
				{
					m->x=(x+rotatexy0(rx,0,yaw))<<16;
					m->y=(y+rotateyy0(rx,0,yaw))<<16;
					m->z=rz<<16;
					return((HIT_FLOOR<<16)+i);
				}
			}
		}

		i=sector[s].lastwall+CEILING;
		z1=getCeiling(x,y,z,s,distxy)-z;
		z2=getCeiling(x2,y2,z+dz,s,distxy)-z;

		ry1=rotateyx0(0,z1,pitch2);
		ry2=rotatey(distxy,z2,pitch2);
		if ((ry1>=0)&&(ry2<=0))
		{
			rx1=rotatexx0(0,z1,pitch2);
			rx2=rotatex(distxy,z2,pitch2);

			if (ry1==ry2)
			{
				if (rx1<rx2)
					rx=rx1;
				else
					rx=rx2;
			}
			else
				rx=rx2+((rx1-rx2)*(-ry2))/(ry1-ry2);

			rx=rotatexy0(rx,0,pitch);
			if (rx<=distxy)
			{
				rz=(rx*dz)/distxy;
				rz+=z;
				if ((wall[i].nextsector>=0)&&(!wall[i].isblocked))
				{
					m->s=wall[i].nextsector;
					return hitScan2(m,skipsprite);
				}
				else
				{
					m->x=(x+rotatexy0(rx,0,yaw))<<16;
					m->y=(y+rotateyy0(rx,0,yaw))<<16;
					m->z=rz<<16;
					return((HIT_CEILING<<16)+i);
				}
			}
		}
	}
	else
	{
		if (dz<0)
		{
			z1=getFloor(x,y,z,s,distxy);
			if ((z+dz)<=z1)
			{
				i=sector[s].lastwall+FLOOR;
				if ((wall[i].nextsector>=0)&&(!wall[i].isblocked))
				{
					m->s=wall[i].nextsector;
					return hitScan2(m,skipsprite);
				}
				else
				{
					m->z=z1<<16;
					return((HIT_FLOOR<<16)+sector[s].lastwall+FLOOR);
				}
			}
		}
		else if (dz>0)
		{
			z1=getCeiling(x,y,z,s,distxy);
			if ((z+dz)>=z1)
			{
				i=sector[s].lastwall+CEILING;
				if ((wall[i].nextsector>=0)&&(!wall[i].isblocked))
				{
					m->s=wall[i].nextsector;
					return hitScan2(m,skipsprite);
				}
				else
				{
					m->z=z1<<16;
					return((HIT_CEILING<<16)+sector[s].lastwall+CEILING);
				}
			}
		}
	}

	//ray died in air, return end of ray...
	m->x=m->x+m->dx;
	m->y=m->y+m->dy;
	m->z=m->z+m->dz;
	return(HIT_NOTHING);
}


void sInR(int x0,int y0,int z0,short s,short r,short m0)
{
	short m;
	int x,y,z;
	int d;
	int last;
	int i;
	int dist;

	if (sector[s].clipcount!=clipcount)
	{
		sector[s].clipcount=clipcount;

		m=sector[s].firstsprite;

		while (m>=0)
		{
			if (m!=m0)
			{
				if ((x=abs(sprite[m].x-x0)>>16)<r)
				{
					if ((y=abs(sprite[m].y-y0)>>16)<r)
					{
						if ((z=abs(sprite[m].z+(sprite[m].height>>1)-z0)>>16)<r)
						{
							if ((d=x*x+y*y+z*z)<(r*r))
							{
								monsterpoint[monsterpoints]=m;
								intbuffer[monsterpoints]=d;
								monsterpoints++;
							}
						}
					}
				}
			}
			m=sprite[m].nextsprite;
		}

		x=x0>>16;
		y=y0>>16;
		z=z0>>16;
		last=sector[s].lastwall+FLOOR+1;
		for (i=sector[s].firstwall;i<last;i++)
		{
			if (wall[i].nextsector>=0)
			{
				dist=((long)(x-vertex[wall[i].v0].x)*wall[i].nx
						+(long)(y-vertex[wall[i].v0].y)*wall[i].ny
						+(long)(z-vertex[wall[i].v0].z)*wall[i].nz)>>14;
				if (dist<r) sInR(x0,y0,z0,wall[i].nextsector,r,m0);
			}
		}
	}
}

int spritesInRadius(int x0,int y0,int z0,short s,short r,short m0)
{
	monsterpoints=0;
	clipcount++;

	sInR(x0,y0,z0,s,r,m0);

	return monsterpoints;
}

int moveSprite(int m,int m2, long gravity)
{
	int	sect,newsect;
	int	i;
	long	x,x2,y,y2;
	long	z,dz;
	long	z0,z1;

	x=sprite[m].x;
	y=sprite[m].y;
	sprite[m].x+=sprite[m].dx;
	sprite[m].y+=sprite[m].dy;
	z0=sprite[m].z;

	if (!radCheck(m,m2))
	{
		sprite[m].x=x;
		sprite[m].y=y;
		sprite[m].z=z0;
	}
	else
	{
		x=sprite[m].x;
		y=sprite[m].y;
	}

	rotateTranswalls(sprite[m].x>>16,sprite[m].y>>16);

	sect=sprite[m].s;
	newsect=sprite[m].s;

	x2=32767;
	for (i=0;i<transwalls;i++)
	{
		if (transsect[i]==sect)
		{
			x=transx[i];
			y=transy1[i];
			y2=transy2[i];

			if ((x>=0)&&(x<x2)&&(y>=0)&&(y2<=0))
			{
				x2=x;
			}
		}
	}

	for (i=0;i<transwalls;i++)
	{
		if (transsect[i]!=sect)
		{
			x=transx[i];
			y=transy1[i];
			y2=transy2[i];

			if ((x>0)&&(x<x2)&&(y>=0)&&(y2<=0))
			{
				x2=x;
				newsect=transsect[i];
			}
		}
	}

	if (newsect!=sprite[m].s) changeSpriteSector(m,newsect);

	z0=(getCeiling(sprite[m].x>>16,sprite[m].y>>16,sprite[m].z>>16,sprite[m].s,sprite[m].radius)-sprite[m].height)<<16;
//	z1=minFloor(sprite[m].x>>16,sprite[m].y>>16,sprite[m].z>>16,sprite[m].s,sprite[m].radius,sprite[m].height)<<16;
	z1=getFloor(sprite[m].x>>16,sprite[m].y>>16,sprite[m].z>>16,sprite[m].s,sprite[m].radius)<<16;

	dz=sprite[m].dz;
	z=sprite[m].z;

	z+=dz;

	dz+=gravity;

	if (dz==0) dz=-1;
	else if (dz<MAXFALL) dz=MAXFALL;

	if ((z<=z1)&&(dz<=0))
	{
		z=z1;
		dz=0;
		if (!hittype)
			hittype=(HIT_FLOOR<<16)+sector[sprite[m].s].lastwall+FLOOR;
	}
	else if	(z>z0)
	{
		z=z0;
		dz=-1;
		if (!hittype)
			hittype=(HIT_CEILING<<16)+sector[sprite[m].s].lastwall+CEILING;
	}

	sprite[m].dz=dz;
	sprite[m].z=z;

	return (hittype);
}

void animateObjects()
{
	int i;

	i=runList;

	while((i=getNextObject(i))>=0)
		if ((aicount&1)||(objectChain[i].type==PLAYER_TYPE))
			signalObject(i,SIGNAL_MOVE,0,0);
}

void plotChunks(short spr,short fbase,short xang, short yang, short r,short g,short b)
{
	short firstchunk;
	short lastchunk;
	short	frame,flags;
	short	t;
	int w, h;
//	int width, height;
	int ws, hs;
//	int	wsin,wcos,hsin,hcos;
	short briter;
	short briteg;
	short briteb;
	int z;
	int	x1,x2,x3,x4,y1,y2,y3,y4;
	int u1,u2,u3,u4,v1,v2,v3,v4;
	int xoff,yoff;
	int xoff2,yoff2;

	firstchunk=frameindex[fbase];
	lastchunk=frameindex[fbase+1];

	z=lmonster[spr].vz;
	if (z<20) z=20;
	ws=(VIEWZ>>2)*sprite[spr].wscale/z;
	hs=(VIEWZ>>2)*sprite[spr].hscale/z;

	for (frame=firstchunk;frame<lastchunk;frame++)
	{
		flags=chunkflags[frame];
		t=chunkpict[frame];		//tile number

//////////////////////////////
// CACHE should happen when it is decided tile is on screen
//////////////////////////////

		if (tilecacheslot[t]>=0)
			moveSlotToEnd(tilecacheslot[t]);
		else
			getCacheSlot(t);
		t=tilecacheslot[t];

		w=monstertile[t].w;
		h=monstertile[t].h;
		xoff=(chunkx[frame]*ws+128)>>8;
		yoff=(chunky[frame]*hs+128)>>8;

//		if (yang)
//		{
//			width=(ws*w+128)>>8;
//			height=(hs*h+128)>>8;
//			wsin=width*sintable[yang>>1]>>14;
//			wcos=width*costable[yang>>1]>>14;
//			hsin=height*sintable[yang>>1]>>14;
//			hcos=height*costable[yang>>1]>>14;
//			x1=lmonster[spr].vx+rotatex(xoff,yoff,yang>>1);
//			x2=x1+wcos;
//			x3=x1-hsin;
//			x4=x3+wcos;
//			y1=lmonster[spr].vy+rotatey(xoff,yoff,yang>>1);
//			y2=y1+wsin;
//			y3=y1+hcos;
//			y4=y3+wsin;
//		}
//		else
		{
			xoff2=((chunkx[frame]+w)*ws+128)>>8;
			x1=lmonster[spr].vx+xoff;
			x3=x1;
			x2=lmonster[spr].vx+xoff2;
			x4=x2;
			yoff2=((chunky[frame]+h)*hs+128)>>8;
			y1=lmonster[spr].vy+yoff;
			y2=y1;
			y3=lmonster[spr].vy+yoff2;
			y4=y3;
		}

		if (sprite[spr].islit)
		{
			briter=255;
			briteg=255;
			briteb=255;
		}
		else
		{
			briter=r-(z>>briteshift);
			briteg=g-(z>>briteshift);
			briteb=b-(z>>briteshift);
		}

		if (briter<0) briter=0;
		if (briteg<0) briteg=0;
		if (briteb<0) briteb=0;

		u1=monstertile[t].u0;
		u2=monstertile[t].u0+w-1;
		u3=u1;
		u4=u2;

		v1=monstertile[t].v0;
		v2=v1;
		v3=monstertile[t].v0+h-1;
		v4=v3;

		setRGB0(&polywall[polycount],briter,briteg,briteb);
		setRGB1(&polywall[polycount],briter,briteg,briteb);
		setRGB2(&polywall[polycount],briter,briteg,briteb);
		setRGB3(&polywall[polycount],briter,briteg,briteb);
		if (flags&1)
		{
			if (flags&2)
				setUV4(&polywall[polycount],u4,v4,u3,v3,u2,v2,u1,v1);
			else
				setUV4(&polywall[polycount],u2,v2,u1,v1,u4,v4,u3,v3);
		}
		else
		{
			if (flags&2)
				setUV4(&polywall[polycount],u3,v3,u4,v4,u1,v1,u2,v2);
			else
				setUV4(&polywall[polycount],u1,v1,u2,v2,u3,v3,u4,v4);
		}
		setXY4(&polywall[polycount],x1,y1,x2,y2,x3,y3,x4,y4);
		polywall[polycount].tpage=monstertile[t].tpage;
		polywall[polycount].clut=monstertile[t].clut;
		AddPrim(&ot[z],&polywall[polycount]);
		faceindex[polycount]=-(spr+1);
		polycount++;
		if (polycount>=MAXPOLYS) sendScreen();
	}
}

void addTarget(short spr)
{
	if (targetcount<MAXTARGETS)
	{
		target[targetcount]=spr;
		targetcount++;
	}
}

void doBurn(short pass,short spr, int x0,int y0,int z0,short xang, short yang, creaturetype *dude)
{
	int f;
	int z;
	int zoff;
	int i;

	int x=lmonster[spr].vx;

	f=seqloc[FLAMEBALL_TYPE];
	f=seq[f].base;
	f=seqindex[f+1];

	z=lmonster[spr].vz;
	if (z<20) z=20;

	for (i=0;i<4;i++)
	{
		lmonster[spr].vx=(VIEWZ>>2)*dude->burnx[i]/z+x;
		zoff=dude->burnz[i];
		if ((zoff>0)&&(pass>0))
		{
			zoff=(VIEWZ>>2)*zoff/z;
			lmonster[spr].vy-=zoff;
			plotChunks(spr,f+((dude->burntime+(i<<1))&7),xang,yang,128,128,128);
			lmonster[spr].vy+=zoff;
		}
		else if ((zoff<0)&&(pass<0))
		{
			zoff=(VIEWZ>>2)*zoff/z;
			lmonster[spr].vy+=zoff;
			plotChunks(spr,f+((dude->burntime+(i<<1))&7),xang,yang,128,128,128);
			lmonster[spr].vy-=zoff;
		}
	}
	lmonster[spr].vx=x;
}

void plotSprites(int s, int x0, int y0, int z0, short xang, short yang)
{
	int angle, facing;
	int m;
	int	frame;
	int	o;
	int r,g,b;
	short index,type;
	short sloc;
	short sbase;
	short light;
	int i;

	m=sector[s].firstsprite;

	while (m>=0)
	{
		rotMonster(m,x0,y0,z0);

		if (lmonster[m].vz>4) //16
		{
			light=sector[s].light;
			o=sprite[m].object;
			getObjectType(o,&type,&index);

			if (type==ITEM_TYPE) sloc=seqloc[((itemtype*)&creatureList[index])->type];
			else if (type==PART_TYPE) sloc=seqloc[((parttype *)&creatureList[index])->type];
			else if (type==ONCE_TYPE) sloc=seqloc[((oncetype *)&creatureList[index])->type];
			else if (type==TORCH_TYPE) sloc=seqloc[((torchtype*)&creatureList[index])->type];
			else sloc=seqloc[type];

			if (sloc>=0)
			{
				frame=-1;
				sbase=seq[sloc].base;
				switch(type)
				{
					default:
						break;
					case TORCH_TYPE:
						frame=seqindex[sbase]+sprite[m].frame;
						plotChunks(m,frame,xang,yang,light,light,light);
						break;
					case ITEM_TYPE:
						if (!((itemtype*)&creatureList[index])->delay)
						{
							frame=seqindex[sbase]+sprite[m].frame;
							plotChunks(m,frame,xang,yang,light,light,light);
						}
						break;
					case ONCE_TYPE:
						if (!((oncetype*)&creatureList[index])->delay)
						{
							frame=seqindex[sbase]+sprite[m].frame;
							plotChunks(m,frame,xang,yang,light+((oncetype *)&creatureList[index])->r,light+((oncetype *)&creatureList[index])->g,light+((oncetype *)&creatureList[index])->b);
						}
						break;
					case PART_TYPE:
						switch (((parttype *)&creatureList[index])->type)
						{
							case SPIDER_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_SPIDER_PART;
								break;
							case BASTET_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_BASTET_PART;
								break;
							case MUMMY_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_MUMMY_PART;
								break;
							case ANUBIS_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_ANUBIS_PART;
								break;
							case PIRHANA_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_PIRHANA_PART;
								break;
							case OMENWASP_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_OMENWASP_PART;
								break;
							case HAWK_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_HAWK_PART;
								break;
							case SENTRY_TYPE:
								frame=sbase+sprite[m].sequence+SEQ_SENTRY_PART;
								break;
							case CONTAIN1_TYPE:
							case CONTAIN2_TYPE:
							case CONTAIN3_TYPE:
							case CONTAIN4_TYPE:
							case CONTAIN5_TYPE:
							case CONTAIN6_TYPE:
							case CONTAIN7_TYPE:
							case CONTAIN8_TYPE:
							case CONTAIN9_TYPE:
							case CONTAIN10_TYPE:
							case CONTAIN11_TYPE:
							case CONTAIN12_TYPE:
							case CONTAIN13_TYPE:
							case CONTAIN14_TYPE:
							case CONTAIN15_TYPE:
							case CONTAIN16_TYPE:
							case CONTAIN17_TYPE:
								frame=sbase;
								break;
						}
						frame=seqindex[frame]+sprite[m].frame;
						plotChunks(m,frame,xang,yang,light,light,light);
						break;
					case SPIDER_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (SpiderSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+SpiderSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case ANUBIS_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (AnubisSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+AnubisSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case SENTRY_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (SentrySeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+SentrySeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case SENTRYBALL_TYPE:
						if (SentryballSeq[((sentryballtype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+SentryballSeq[((sentryballtype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						break;
					case CLOUD_TYPE:
						if (CloudSeq[((cloudtype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+CloudSeq[((cloudtype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						break;
					case MUMMY_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (MummySeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+MummySeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case SET_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (SetSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+SetSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case MANTIS_TYPE:
						if (((creaturetype *)&creatureList[index])->state!=AI_MANTIS_IDLE)
						{
							if (MantisSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
								facing=0;
							else
							{
								angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
								facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
							}
							frame=sbase+facing+MantisSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

							frame=seqindex[frame]+sprite[m].frame;

							plotChunks(m,frame,xang,yang,light,light,light);

							if((((creaturetype *)&creatureList[index])->state==AI_MANTIS_RISE)
							|| (((creaturetype *)&creatureList[index])->state==AI_MANTIS_SHOOT)
							|| (((creaturetype *)&creatureList[index])->state==AI_MANTIS_SINK)
							|| (((creaturetype *)&creatureList[index])->state==AI_MANTIS_WAIT))
							addTarget(m);
						}
						break;
					case SELKIS_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (SelkisSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+SelkisSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case QUEEN_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (QueenSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+QueenSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case BASTET_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (BastetSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+BastetSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case OMENWASP_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (OmenwaspSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+OmenwaspSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case PIRHANA_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (PirhanaSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+PirhanaSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case HAWK_TYPE:
						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);

						if (HawkSeq[((creaturetype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+HawkSeq[((creaturetype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						if (((creaturetype*)&creatureList[index])->burntime>=0)
							doBurn(-1,m,x0,y0,z0,xang,yang,(creaturetype*)&creatureList[index]);
						addTarget(m);
						break;
					case FIREBALL_TYPE:
						if (FireballSeq[((fireballtype *)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+FireballSeq[((fireballtype *)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,128,128,128);

						break;
					case ORB_TYPE:
						if (OrbSeq[((orbtype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+OrbSeq[((orbtype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						break;
					case METEOR_TYPE:
						if (MeteorSeq[((meteortype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+MeteorSeq[((meteortype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						break;
					case SOUL_TYPE:
						if (SoulSeq[((soultype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+SoulSeq[((soultype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,light,light,light);

						break;
					case RABALL_TYPE:
						if (RaballSeq[((raballtype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+RaballSeq[((raballtype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						plotChunks(m,frame,xang,yang,128,128,128);

						break;
					case FLAMEBALL_TYPE:
						if (FlameballSeq[((flameballtype*)&creatureList[index])->state].bOneFrame)
							facing=0;
						else
						{
							angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
							facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
						}
						frame=sbase+facing+FlameballSeq[((flameballtype*)&creatureList[index])->state].nBaseSeq;

						frame=seqindex[frame]+sprite[m].frame;

						i=((flameballtype*)&creatureList[index])->time;
						if (i<5)
						{
							r=128+(i<<4);
							g=128+(i<<4);
							b=192-(i<<5);
						}
						else
						{
							r=192-((i-4)<<1);
							g=192-((i-4)<<2);
							b=64;
						}

						plotChunks(m,frame,xang,yang,r,g,b);

						break;
					case COBRABALL_TYPE:
					case REDCOBRA_TYPE:
						if (m!=((cobraballtype*)&creatureList[index])->sprite)
						{
							if (m!=((cobraballtype*)&creatureList[index])->head)
								facing=8;
							else
							{
								angle=getAngle((sprite[m].x>>16)-x0,(sprite[m].y>>16)-y0);
								facing=((angle-(sprite[m].angle>>1)+128+1024)&2047)>>8;
							}

							frame=seqindex[sbase+facing];

							plotChunks(m,frame,xang,yang,light,light,light);
						}
						break;
					case PLAYER_TYPE:
						break;
				}
			}
		}
		if (sprite[m].islit) sprite[m].islit=0;
		m=sprite[m].nextsprite;
	}
}

void changeSpriteSector(int m, int dest)
{
	int i;
	int	s;

	s=sprite[m].s;
	sprite[m].s=dest;
	i=sector[s].firstsprite;

	if (i==m)
	{
		sector[s].firstsprite=sprite[m].nextsprite;
		if ((dest>=sectorcount)||(dest<0))
//		if ((dest>=MAXSECTORS)||(dest<0))
		{
			sprite[m].nextsprite=freesprite;
			freesprite=m;
			return;
		}
		else
		{
			sprite[m].nextsprite=sector[dest].firstsprite;
			sector[dest].firstsprite=m;
			return;
		}
	}

	while(i>=0)
	{
		if (sprite[i].nextsprite==m)
		{
			sprite[i].nextsprite=sprite[m].nextsprite;
			if ((dest>=sectorcount)||(dest<0))
//			if ((dest>=MAXSECTORS)||(dest<0))
			{
				sprite[m].nextsprite=freesprite;
				freesprite=m;
				return;
			}
			else
			{
				sprite[m].nextsprite=sector[dest].firstsprite;
				sector[dest].firstsprite=m;
				return;
			}
		}
		else
			i=sprite[i].nextsprite;
	}
}

int insertSprite(int s)
{
	int m;

	m=freesprite;

	if (m>=0)
	{
		freesprite=sprite[m].nextsprite;
		sprite[m].nextsprite=sector[s].firstsprite;
		sector[s].firstsprite=m;
		sprite[m].s=s;
	}

	return m;
}

void deleteSprite(int m)
{
	changeSpriteSector(m,-1);
//	changeSpriteSector(m,MAXSECTORS);
	sprite[m].object=-1;
}


int spriteSeesPoint(int x2,int y2,int z2,int m1,int w)
{
	short thing;
	short s1=sprite[m1].s;
	int x1=sprite[m1].x;
	int y1=sprite[m1].y;
	int z1=sprite[m1].z+(sprite[m1].height<<15);

	int dx=x2-(x1>>16);
	int dy=y2-(y1>>16);
	int dz=z2-(z1>>16);

	short yaw=getAngle(dx,dy)<<1;
	int dxy=lsquareroot(abs(dx)*abs(dx)+abs(dy)*abs(dy));
	short pitch=getAngle(dxy,dz)<<1;

	int i;

	i=hitscan(&x1,&y1,&z1,&s1,abs(dx)+abs(dy)+abs(dz),yaw,pitch,m1);

	thing=i>>16;
	if (thing==HIT_NOTHING) return 1;
	if ((thing==HIT_WALL)||(thing==HIT_FLOOR)||(thing==HIT_CEILING))
	{
		i=i&0xffff;
		if (i==w)
			return 1;
		else
		{
			if (((long)wall[i].nx*wall[w].nx+
					wall[i].ny*wall[w].ny+
					wall[i].nz*wall[w].nz)>(14000<<14))
//			if ((abs(wall[i].nx-wall[w].nx)<256)
//				&&(abs(wall[i].ny-wall[w].ny)<256)
//				&&(abs(wall[i].nz-wall[w].nz)<256))
			{
				dx=abs(dx);
				dy=abs(dy);
				dz=abs(dz);
				x1=abs((x1-sprite[m1].x)>>16);
				y1=abs((y1-sprite[m1].y)>>16);
				z1=abs((z1-sprite[m1].z)>>16);

				if ((x1>=(dx-64))&&(y1>=(dy-64))&&(z1>=(dz-64)))
					return 1;
			}
		}
	}

	return 0;
}

int pointSeesSprite(int x1,int y1,int z1,short s1,int m2)
{
	int x2=sprite[m2].x>>16;
	int y2=sprite[m2].y>>16;
	int z2=(sprite[m2].z>>16)+(sprite[m2].height>>1);

	int dx=x2-(x1>>16);
	int dy=y2-(y1>>16);
	int dz=z2-(z1>>16);

	short yaw=getAngle(dx,dy)<<1;
	int dxy=lsquareroot(abs(dx)*abs(dx)+abs(dy)*abs(dy));
	short pitch=getAngle(dxy,dz)<<1;

	int i;

	int d=lsquareroot(dx*dx+dy*dy+dz*dz);

	i=hitscan(&x1,&y1,&z1,&s1,d,yaw,pitch,-1);
	if ((((i>>16)==HIT_SPRITE)&&((i&0xffff)==m2))||((i>>16)==HIT_NOTHING))
	{
		return 1; //(((dx<<14)/d*wall[w].nx)+((dy<<14)/d*wall[w].ny)+((dz<<14)/d*wall[w].nz))>>12;
	}

	return 0;
}

int canSee(int m1, int dz1, int m2, int dz2, short checkarc)
{
	short ang=sprite[m1].angle;
	short s1=sprite[m1].s;
	int x1=sprite[m1].x;
	int y1=sprite[m1].y;
	int z1=sprite[m1].z;

	short s2=sprite[m2].s;
	int x2=sprite[m2].x;
	int y2=sprite[m2].y;
	int z2=sprite[m2].z;

	int dx,dy,dz;
	int dxy;
	short yaw,pitch;

	int i, inarc;

	hitscan(&x1,&y1,&z1,&s1,dz1,0,1024,m1);
	hitscan(&x2,&y2,&z2,&s2,dz2,0,1024,m2);

	dx=(x2-x1)>>16;
	dy=(y2-y1)>>16;
	dz=(z2-z1)>>16;

	dxy=lsquareroot(dx*dx+dy*dy);
	yaw=getAngle(dx,dy)<<1;
	pitch=getAngle(dxy,dz)<<1;

	if ((checkarc)&&((pitch>3072+512)||(pitch<512)))
	{
		if (ang>yaw)
			inarc=ang-yaw;
		else
			inarc=yaw-ang;
		if ((inarc>3072+512)||(inarc<512))
		{
			i=hitscan(&x1,&y1,&z1,&s1,abs(dx)+abs(dy)+abs(dz),yaw,pitch,m1);
			if (((i>>16)==HIT_SPRITE)&&((i&0xffff)==m2))
				return 1;
		}
	}
	else
	{
		i=hitscan(&x1,&y1,&z1,&s1,abs(dx)+abs(dy)+abs(dz),yaw,pitch,m1);
		if (((i>>16)==HIT_SPRITE)&&((i&0xffff)==m2))
			return 1;
	}

	return 0;
}

void centerMonster(short m)
{
	int i;

	i=sector[monster[m].s].lastwall+FLOOR;
	monster[m].x=(vertex[wall[i].v0].x+vertex[wall[i].v0+2].x)<<15;
	monster[m].y=(vertex[wall[i].v0].y+vertex[wall[i].v0+2].y)<<15;
	monster[m].z=getFloor(monster[m].x>>16,monster[m].y>>16,monster[m].z>>16,monster[m].s,32)<<16;
}

long buildMonster(short type,int x,int y,int z,short s,short ang)
{
	switch (type)
	{
		case PLAYER_TYPE:
			return ReBuildPlayer(x,y,z,s,ang);
			break;
		case SENTRY_TYPE:
			return buildSentry(x,y,z,s,ang);
			break;
		case SPIDER_TYPE:
			return BuildSpider(x,y,z,s,ang);
			break;
		case HAWK_TYPE:
			return BuildHawk(x,y,z,s,ang);
			break;
		case ANUBIS_TYPE:
			return buildAnubis(x,y,z,s,ang);
			break;
		case MUMMY_TYPE:
			return buildMummy(x,y,z,s,ang);
			break;
		case SET_TYPE:
			return buildSet(x,y,z,s,ang);
			break;
		case SELKIS_TYPE:
			return buildSelkis(x,y,z,s,ang);
			break;
		case MANTIS_TYPE:
			return buildMantis(x,y,z,s,ang);
			break;
		case QUEEN_TYPE:
			return buildQueen(x,y,z,s,ang);
			break;
		case BASTET_TYPE:
			return buildBastet(x,y,z,s,ang);
			break;
		case OMENWASP_TYPE:
			return buildOmenwasp(x,y,z,s,ang);
			break;
		case PIRHANA_TYPE:
			return buildPirhana(x,y,z,s,ang);
			break;

		case XKEY_TYPE:
		case BUGKEY_TYPE:
		case TIMEKEY_TYPE:
		case PLANTKEY_TYPE:
		case SHAWL_TYPE:
		case FEATHER_TYPE:
		case MASK_TYPE:
		case PRISM_TYPE:
		case SANDALS_TYPE:
		case SCEPTER_TYPE:
		case ANKLETS_TYPE:
		case HEALTH_TYPE:
		case AMMO_TYPE:
		case PISTOL_TYPE:
		case M60_TYPE:
		case GRENADE_TYPE:
		case FLAMER_TYPE:
		case COBRA_TYPE:
		case RING_TYPE:
		case MANACLE_TYPE:
		case POT_TYPE:
		case FULLAMMO_TYPE:
		case FULLHEALTH_TYPE:
		case VESSEL1_TYPE:
		case VESSEL2_TYPE:
		case VESSEL3_TYPE:
		case VESSEL4_TYPE:
		case VESSEL5_TYPE:
		case CAMEL_TYPE:
		case CONTAIN1_TYPE:
		case CONTAIN2_TYPE:
		case CONTAIN3_TYPE:
		case CONTAIN4_TYPE:
		case CONTAIN5_TYPE:
		case CONTAIN6_TYPE:
		case CONTAIN7_TYPE:
		case CONTAIN8_TYPE:
		case CONTAIN9_TYPE:
		case CONTAIN10_TYPE:
		case CONTAIN11_TYPE:
		case CONTAIN12_TYPE:
		case CONTAIN13_TYPE:
		case CONTAIN14_TYPE:
		case CONTAIN15_TYPE:
		case CONTAIN16_TYPE:
		case CONTAIN17_TYPE:
		case PLANT1_TYPE:
		case PLANT2_TYPE:
		case PLANT3_TYPE:
		case PLANT4_TYPE:
		case BUBLE1_TYPE:
		case FISH_TYPE:
		case FALS_TYPE:
		case WTRBUBS_TYPE:
		case BRANCH1_TYPE:
		case GLOW1_TYPE:
		case BUSH1_TYPE:
		case BUSH2_TYPE:
		case BUSH3_TYPE:
		case BUSH4_TYPE:
		case BUSH5_TYPE:
		case CHAIN_TYPE:
		case CORAL_TYPE:
		case LOG1_TYPE:
		case TREE_TYPE:
			return buildItem(type,x,y,z,s,ang);
			break;

		case TORCH1_TYPE:
		case TORCH2_TYPE:
		case TORCH3_TYPE:
		case TORCH4_TYPE:
		case TORCH5_TYPE:
		case TORCH6_TYPE:
		case TORCH7_TYPE:
		case TORCH8_TYPE:
		case TORCH9_TYPE:
		case TORCH10_TYPE:
		case TORCH11_TYPE:
		case TORCH12_TYPE:
		case TORCH13_TYPE:
		case TORCH14_TYPE:
		case TORCH15_TYPE:
		case TORCH16_TYPE:
		case TORCH17_TYPE:
		case TORCH18_TYPE:
		case TORCH19_TYPE:
		case TORCH20_TYPE:
		case TORCH21_TYPE:
		case TORCH22_TYPE:
		case TORCH23_TYPE:
		case TORCH24_TYPE:
		case TORCH25_TYPE:
		case TORCH26_TYPE:
		case TORCH27_TYPE:
		case TORCH28_TYPE:
		case TORCH29_TYPE:
		case TORCH30_TYPE:
		case TORCH31_TYPE:
			return buildTorch(type,x,y,z,s,ang);
			break;
	}
	return -1;
}

void initSprites()
{
	int object;
	int i;

	for (i=0;i<sectorcount;i++)
		sector[i].firstsprite=-1;

	freesprite=-1;

	for (i=0;i<MAXSPRITES;i++)
	{
		sprite[i].nextsprite=freesprite;
		freesprite=i;
	}

	initCreatures();

	for (i=0;i<monstercount;i++)
	{
		if (monster[i].s>=0)
		{
			centerMonster(i);

			object=buildMonster(monster[i].type,monster[i].x,monster[i].y,
									monster[i].z,monster[i].s,monster[i].ang);
#ifdef EDITMODE
			{
				short type,index;
				getObjectType(object,&type,&index);
				if (type!=PLAYER_TYPE)
				moveObject(object,idleList);
			}
#endif
		}
	}
}

void initSeqs()
{
	int i;

	monstercount2=0;

	clearSeq();

	addSeq(WALLHIT_TYPE,"kapoww.pss");
	addSeq(BOOM_TYPE,"boom1.pss");
	addSeq(GUTS1_TYPE,"gutss1.pss");
	addSeq(GUTS2_TYPE,"gutss2.pss");
	addSeq(REDBALL_TYPE,"redd.pss");
	addSeq(BLUEBALL_TYPE,"bluee.pss");
	addSeq(FLAMEBALL_TYPE,"flamed.pss");
	addSeq(COBRABALL_TYPE,"cobballl.pss");
	addSeq(ORB_TYPE,"grenroll.pss");
	addSeq(RABALL_TYPE,"ringoo.pss");
	addSeq(CLOUD_TYPE,"eyehitt.pss");
	addSeq(GRENPOW_TYPE,"grenpoww.pss");
	addSeq(HEALTH_TYPE,"healthh.pss");
	addSeq(AMMO_TYPE,"ammoo.pss");

	for (i=0;i<monstercount;i++)
	{
		if (monster[i].s<0)
		{
			monstertypes[monstercount2]=monster[i].type;
			monstercount2++;
		}

		switch (monster[i].type)
		{
			case PLAYER_TYPE:
				break;
			case SENTRY_TYPE:
				addSeq(SENTRY_TYPE,"roachh.pss");
				seqloc[SENTRYBALL_TYPE]=seqloc[SENTRY_TYPE]; //sentry ball is last sequence of sentry
				break;
			case SPIDER_TYPE:
				addSeq(SPIDER_TYPE,"spiderr.pss");
				break;
			case HAWK_TYPE:
				addSeq(HAWK_TYPE,"hawkk.pss");
				break;
			case ANUBIS_TYPE:
				addSeq(ANUBIS_TYPE,"anubiss.pss");
				addSeq(FIREBALL_TYPE,"anuballl.pss");
				break;
			case MUMMY_TYPE:
				addSeq(MUMMY_TYPE,"mummyy.pss");
				addSeq(REDCOBRA_TYPE,"cobrared.pss");
				break;
			case SET_TYPE:
				addSeq(SET_TYPE,"sett.pss");
				addSeq(SOUL_TYPE,"soul.pss");
				break;
			case SELKIS_TYPE:
				addSeq(SELKIS_TYPE,"selkiss.pss");
				addSeq(FIREBALL_TYPE,"anuballl.pss");
				break;
			case MANTIS_TYPE:
				addSeq(MANTIS_TYPE,"mantiss.pss");
				addSeq(METEOR_TYPE,"meteor.pss");
				break;
			case QUEEN_TYPE:
				addSeq(QUEEN_TYPE,"sett.pss");
				break;
			case BASTET_TYPE:
				addSeq(BASTET_TYPE,"bastett.pss");
				break;
			case OMENWASP_TYPE:
				addSeq(OMENWASP_TYPE,"waspp.pss");
				break;
			case PIRHANA_TYPE:
				addSeq(PIRHANA_TYPE,"pirhanaa.pss");
				break;
			case XKEY_TYPE:
				addSeq(XKEY_TYPE,"xkey.pss");
				break;
			case BUGKEY_TYPE:
				addSeq(BUGKEY_TYPE,"bugkey.pss");
				break;
			case TIMEKEY_TYPE:
				addSeq(TIMEKEY_TYPE,"timekey.pss");
				break;
			case PLANTKEY_TYPE:
				addSeq(PLANTKEY_TYPE,"plantkey.pss");
				break;
			case SHAWL_TYPE:
				addSeq(SHAWL_TYPE,"cape.pss");
				break;
			case FEATHER_TYPE:
				addSeq(FEATHER_TYPE,"feather.pss");
				break;
			case MASK_TYPE:
				addSeq(MASK_TYPE,"mask.pss");
				break;
			case PRISM_TYPE:
				addSeq(PRISM_TYPE,"pyramid.pss");
				break;
			case SANDALS_TYPE:
				addSeq(SANDALS_TYPE,"sandals.pss");
				break;
			case SCEPTER_TYPE:
				addSeq(SCEPTER_TYPE,"scepter.pss");
				break;
			case ANKLETS_TYPE:
				addSeq(ANKLETS_TYPE,"anklets.pss");
				break;
			case HEALTH_TYPE:
				addSeq(HEALTH_TYPE,"healthh.pss");
				break;
			case AMMO_TYPE:
				addSeq(AMMO_TYPE,"ammoo.pss");
				break;
			case PISTOL_TYPE:
				addSeq(PISTOL_TYPE,"pistol.pss");
				break;
			case M60_TYPE:
				addSeq(M60_TYPE,"m-60.pss");
				break;
			case GRENADE_TYPE:
				addSeq(GRENADE_TYPE,"bomb.pss");
				break;
			case FLAMER_TYPE:
				addSeq(FLAMER_TYPE,"flamer.pss");
				break;
			case COBRA_TYPE:
				addSeq(COBRA_TYPE,"cobra.pss");
				break;
			case RING_TYPE:
				addSeq(RING_TYPE,"ring.pss");
				break;
			case MANACLE_TYPE:
				addSeq(MANACLE_TYPE,"manacle.pss");
				break;
			case POT_TYPE:
				addSeq(POT_TYPE,"bugkey.pss");
				break;
			case FULLAMMO_TYPE:
				addSeq(FULLAMMO_TYPE,"fullammo.pss");
				break;
			case FULLHEALTH_TYPE:
				addSeq(FULLHEALTH_TYPE,"fullhelt.pss");
				break;
			case VESSEL1_TYPE:
			case VESSEL2_TYPE:
			case VESSEL3_TYPE:
			case VESSEL4_TYPE:
			case VESSEL5_TYPE:
				addSeq(monster[i].type,"vessel.pss");
				break;
			case CAMEL_TYPE:
				addSeq(CAMEL_TYPE,"camel.pss");
				break;
			case CONTAIN1_TYPE:
				addSeq(monster[i].type,"tomb1.pss");
				break;
			case CONTAIN2_TYPE:
				addSeq(monster[i].type,"tbang.pss");
				break;
			case CONTAIN3_TYPE:
				addSeq(monster[i].type,"blow3.pss");
				break;
			case CONTAIN4_TYPE:
				addSeq(monster[i].type,"blow8.pss");
				break;
			case CONTAIN5_TYPE:
				addSeq(monster[i].type,"sbang.pss");
				break;
			case CONTAIN6_TYPE:
				addSeq(monster[i].type,"blow1.pss");
				break;
			case CONTAIN7_TYPE:
				addSeq(monster[i].type,"blow7.pss");
				break;
			case CONTAIN8_TYPE:
				addSeq(monster[i].type,"vase1.pss");
				break;
			case CONTAIN9_TYPE:
				addSeq(monster[i].type,"pbang.pss");
				break;
			case CONTAIN10_TYPE:
				addSeq(monster[i].type,"blow4.pss");
				break;
			case CONTAIN11_TYPE:
				addSeq(monster[i].type,"mbang.pss");
				break;
			case CONTAIN12_TYPE:
				addSeq(monster[i].type,"blow5.pss");
				break;
			case CONTAIN13_TYPE:
				addSeq(monster[i].type,"dog.pss");
				break;
			case CONTAIN14_TYPE:
				addSeq(monster[i].type,"blow6.pss");
				break;
			case CONTAIN15_TYPE:
				addSeq(monster[i].type,"blow2.pss");
				break;
			case CONTAIN16_TYPE:
				addSeq(monster[i].type,"kbang.pss");
				break;
			case CONTAIN17_TYPE:
				addSeq(monster[i].type,"cbang.pss");
				break;

			case TORCH1_TYPE:
				addSeq(monster[i].type,"tom1.pss");
				break;
			case TORCH2_TYPE:
				addSeq(monster[i].type,"tom2.pss");
				break;
			case TORCH3_TYPE:
				addSeq(monster[i].type,"thoth1.pss");
				break;
			case TORCH4_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH5_TYPE:
				addSeq(monster[i].type,"thoth3.pss");
				break;
			case TORCH6_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH7_TYPE:
				addSeq(monster[i].type,"set1.pss");
				break;
			case TORCH8_TYPE:
				addSeq(monster[i].type,"set2.pss");
				break;
			case TORCH9_TYPE:
				addSeq(monster[i].type,"set3.pss");
				break;
			case TORCH10_TYPE:
				addSeq(monster[i].type,"set4.pss");
				break;
			case TORCH11_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH12_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH13_TYPE:
				addSeq(monster[i].type,"quar1.pss");
				break;
			case TORCH14_TYPE:
				addSeq(monster[i].type,"peril1.pss");
				break;
			case TORCH15_TYPE:
				addSeq(monster[i].type,"marsh1.pss");
				break;
			case TORCH16_TYPE:
				addSeq(monster[i].type,"magma1.pss");
				break;
			case TORCH17_TYPE:
				addSeq(monster[i].type,"magma2.pss");
				break;
			case TORCH18_TYPE:
				addSeq(monster[i].type,"town1.pss");
				break;
			case TORCH19_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH20_TYPE:
				addSeq(monster[i].type,"hant1.pss");
				break;
			case TORCH21_TYPE:
				addSeq(monster[i].type,"hant2.pss");
				break;
			case TORCH22_TYPE:
				addSeq(monster[i].type,"hant3.pss");
				break;
			case TORCH23_TYPE:
				addSeq(monster[i].type,"hant4.pss");
				break;
			case TORCH24_TYPE:
				addSeq(monster[i].type,"col1.pss");
				break;
			case TORCH25_TYPE:
				addSeq(monster[i].type,"col2.pss");
				break;
			case TORCH26_TYPE:
				addSeq(monster[i].type,"col3.pss");
				break;
			case TORCH27_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH28_TYPE:
				addSeq(monster[i].type,"thoth2.pss");
				break;
			case TORCH29_TYPE:
				addSeq(monster[i].type,"camp1.pss");
				break;
			case TORCH30_TYPE:
				addSeq(monster[i].type,"camp2.pss");
				break;
			case TORCH31_TYPE:
				addSeq(monster[i].type,"camp3.pss");
				break;
			case PLANT1_TYPE:
				addSeq(monster[i].type,"plant1.pss");
				break;
			case PLANT2_TYPE:
				addSeq(monster[i].type,"plant2.pss");
				break;
			case PLANT3_TYPE:
				addSeq(monster[i].type,"plant3.pss");
				break;
			case PLANT4_TYPE:
				addSeq(monster[i].type,"plant4.pss");
				break;
			case BUBLE1_TYPE:
				addSeq(monster[i].type,"buble1.pss");
				break;
			case FISH_TYPE:
				addSeq(monster[i].type,"fish.pss");
				break;
			case FALS_TYPE:
				addSeq(monster[i].type,"fals.pss");
				break;
			case WTRBUBS_TYPE:
				addSeq(monster[i].type,"wtrbubs.pss");
				break;
			case BRANCH1_TYPE:
				addSeq(monster[i].type,"branch1.pss");
				break;
			case GLOW1_TYPE:
				addSeq(monster[i].type,"glow1.pss");
				break;
			case BUSH1_TYPE:
				addSeq(monster[i].type,"bush1.pss");
				break;
			case BUSH2_TYPE:
				addSeq(monster[i].type,"bush2.pss");
				break;
			case BUSH3_TYPE:
				addSeq(monster[i].type,"bush3.pss");
				break;
			case BUSH4_TYPE:
				addSeq(monster[i].type,"bush4.pss");
				break;
			case BUSH5_TYPE:
				addSeq(monster[i].type,"bush5.pss");
				break;
			case CHAIN_TYPE:
				addSeq(monster[i].type,"chain.pss");
				break;
			case CORAL_TYPE:
				addSeq(monster[i].type,"coral.pss");
				break;
			case LOG1_TYPE:
				addSeq(monster[i].type,"log1.pss");
				break;
			case TREE_TYPE:
				addSeq(monster[i].type,"tree.pss");
				break;
		}
	}

	for (i=0;i<MAXWEAPONS;i++)
		weaponseq[i]=readSeq(weaponfile[i]);
}


void initMenuSeqs()
{
	int i;

	menuseq=readSeq(menuseqfile[0]);

	for (i=1;i<MAXMENUSEQS;i++)
		readSeq(menuseqfile[i]);
}
