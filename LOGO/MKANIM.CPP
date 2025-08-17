#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

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

void writeChar(int i)
{assert(i<127 && i>-127);
 fputc(i,ofile);
}

void writeUChar(int i)
{assert(i<256 && i>=0);
 fputc(i,ofile);
}

#define GRAVITY 7.0
#define K 2.0
#define D 0.3

typedef struct
{double x,y,z;
} Point;

Point basis[3];
Point eyePos;
void setCamera(double pitch,double eyeX,double eyeY,double eyeZ)
{basis[0].x=-1.0; basis[0].y=0; basis[0].z=0;
 basis[1].x=0; basis[1].y=cos(pitch); basis[1].z=-sin(pitch);
 basis[2].x=0; basis[2].y=sin(pitch); basis[2].z=cos(pitch);
 eyePos.x=eyeX; eyePos.y=eyeY; eyePos.z=eyeZ;
}

Point &transformPoint(Point p)
{static Point tfrm;
 p.x-=eyePos.x; p.y-=eyePos.y; p.z-=eyePos.z;

 tfrm.x=basis[0].x*p.x+basis[0].y*p.y+basis[0].z*p.z;
 tfrm.y=basis[1].x*p.x+basis[1].y*p.y+basis[1].z*p.z;
 tfrm.z=basis[2].x*p.x+basis[2].y*p.y+basis[2].z*p.z;
 assert(tfrm.z>1);
 tfrm.x=tfrm.x*160.0/tfrm.z;
 tfrm.y=tfrm.y*160.0/tfrm.z;
 return tfrm;
}

class Mass
{public:
 Mass(double x,double y,double z,double _mass)
    {p.x=x; p.y=y; p.z=z; mass=_mass; next=NULL;
     v.x=0; v.y=0; v.z=0;
     forceAccum=v;
     lastPos[0]=x; lastPos[1]=y; lastPos[2]=z;}

 void step(double time);
 void force(double x,double y,double z)
    {forceAccum.x+=x/mass; forceAccum.y+=y/mass; forceAccum.z+=z/mass;}
 Point p,v,forceAccum;
 int lastPos[3];
 int number;
 double mass;
 Mass *next;
};

void Mass::step(double time)
{v.x+=forceAccum.x; v.y+=forceAccum.y; v.z+=forceAccum.z;
 forceAccum.x=0; forceAccum.y=0; forceAccum.z=0;
 p.x+=v.x*time;
 p.y+=v.y*time;
 p.z+=v.z*time;


 v.y+=GRAVITY*time;
 if (p.y>1200)
   {p.y=1200;
    if (v.y>0)
      v.y=-0.3*v.y;
   }
}

class Spring
{public:
 Spring(Mass *_m1,Mass *_m2,double _k,double _d)
    {m1=_m1; m2=_m2; k=_k; d=_d; next=NULL;
     r=sqrt((m1->p.x-m2->p.x)*(m1->p.x-m2->p.x)+
	    (m1->p.y-m2->p.y)*(m1->p.y-m2->p.y)+
	    (m1->p.z-m2->p.z)*(m1->p.z-m2->p.z));
    };
 void recalcRestLen(void)
    {r=sqrt((m1->p.x-m2->p.x)*(m1->p.x-m2->p.x)+
	    (m1->p.y-m2->p.y)*(m1->p.y-m2->p.y)+
	    (m1->p.z-m2->p.z)*(m1->p.z-m2->p.z));
    }
 void step(double time);
 Mass *m1,*m2;
 double r,k,d;
 Spring *next;
};

void Spring::step(double time)
{double len;
 Point v;
 v.x=m2->p.x-m1->p.x;  v.y=m2->p.y-m1->p.y;  v.z=m2->p.z-m1->p.z;
 len=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
 if (len<0.0001)
    return;
 v.x/=len; v.y/=len; v.z/=len;
 double f=k*(len-r);
 /* damp */
 f+=d*(v.x*(m2->v.x-m1->v.x)+
       v.y*(m2->v.y-m1->v.y)+
       v.z*(m2->v.z-m1->v.z));
 f*=time;
 m1->force(v.x*f,v.y*f,v.z*f);
 m2->force(-v.x*f,-v.y*f,-v.z*f);
}

class Face
{public:
 Face(Mass *_m1,Mass *_m2,Mass *_m3,Mass *_m4,int _owner)
    {m[0]=_m1; m[1]=_m2; m[2]=_m3; m[3]=_m4; next=NULL; owner=_owner;}
 Mass *m[4];
 Face *next;
 int owner;
};

class Object
{public:
 Object(void) {masses=NULL; faces=NULL; springs=NULL; next=NULL;}
 void addMass(Mass *m) {assert(m); m->next=masses; masses=m;}
 void addSpring(Spring *s) {assert(s); s->next=springs; springs=s;}
 void addFace(Face *f) {assert(f); f->next=faces; faces=f;}
 void draw(void);
 void step(double time);
 int countFaces(void)
    {int i;
     Face *f;
     for (i=0,f=faces;f;i++,f=f->next) ;
     return i;
    }
 void translate(double dx,double dy,double dz)
    {Mass *p;
     for (p=masses;p;p=p->next)
	{p->p.x+=dx;
	 p->p.y+=dy;
	 p->p.z+=dz;
	 p->lastPos[0]=p->p.x;
	 p->lastPos[1]=p->p.y;
	 p->lastPos[2]=p->p.z;
	}
    }
 int countPoints(void)
    {int i;
     Mass *p;
     for (i=0,p=masses;p;i++,p=p->next) ;
     return i;
    }
 int numberPoints(int base)
    {Mass *p;
     for (p=masses;p;p=p->next)
	p->number=base++;
     return base;
    }
 void writeFaces(void)
    {Face *f;
     for (f=faces;f;f=f->next)
	{writeShort(f->m[0]->number);
	 writeShort(f->m[1]->number);
	 writeShort(f->m[2]->number);
	 writeShort(f->m[3]->number);
	 writeShort(f->owner);
	 writeShort(0);
	}
    }
 void writePoints(void)
    {Mass *p;
     for (p=masses;p;p=p->next)
	{Point t=transformPoint(p->p);
	 writeShort(t.x);
	 writeShort(t.y);
	 writeShort(t.z);
	 writeShort(0);
	 p->lastPos[0]=t.x;
	 p->lastPos[1]=t.y;
	 p->lastPos[2]=t.z;
	}
    }
 void recalcRestLengths(void)
    {Spring *s;
     for (s=springs;s;s=s->next)
	s->recalcRestLen();
     Mass *p;
     for (p=masses;p;p=p->next)
	{p->lastPos[0]=p->p.x;
	 p->lastPos[1]=p->p.y;
	 p->lastPos[2]=p->p.z;
	}
    }
 void setLightDir(double x,double y,double z)
    {lightDir.x=x; lightDir.y=y; lightDir.z=z;
     double len=sqrt(lightDir.x*lightDir.x+lightDir.y*lightDir.y+
		     lightDir.z*lightDir.z);
     lightDir.x/=len; lightDir.y/=len; lightDir.z/=len;
    }

 void writeDeltas(void)
    {Mass *p;
     int i,d,j;
     for (p=masses;p;p=p->next)
	{Point t=transformPoint(p->p);
	 writeChar(((int)t.x)-p->lastPos[0]);
	 writeChar(((int)t.y)-p->lastPos[1]);
	 writeChar(((int)t.z)-p->lastPos[2]);
	 p->lastPos[0]=t.x;
	 p->lastPos[1]=t.y;
	 p->lastPos[2]=t.z;
	}
     Face *f;
     for (f=faces;f;f=f->next)
	{/* write out color of each face */
	 Point a,b,n;
	 double len;
	 /* find normal of face */
	 n.x=0; n.y=0; n.z=0;
	 for (i=0;i<4;i++)
	    {j=i+1; if (j>=4) j=0;
	     n.x+=(f->m[i]->p.y-f->m[j]->p.y)*(f->m[i]->p.z+f->m[j]->p.z);
	     n.y+=(f->m[i]->p.z-f->m[j]->p.z)*(f->m[i]->p.x+f->m[j]->p.x);
	     n.z+=(f->m[i]->p.x-f->m[j]->p.x)*(f->m[i]->p.y+f->m[j]->p.y);
	    }
	 len=sqrt(n.x*n.x+n.y*n.y+n.z*n.z);
	 n.x/=len; n.y/=len; n.z/=len;
	 /* compute light value */
	 double light=n.x*lightDir.x+n.y*lightDir.y+
	    n.z*lightDir.z;
	 if (light<0) light=0;
	 i=light*24.0+8.0;
	 if (i<0) i=0;
	 if (i>31) i=31;
	 /* mark */
	 if (basis[2].x*n.x+
	     basis[2].y*n.y+
	     basis[2].z*n.z<-0.2)
	    i=-1;
	 writeChar(i);
	}
    }
 Point lightDir;
 Spring *springs;
 Mass *masses;
 Face *faces;
 Object *next;
};

void Object::step(double time)
{Spring *s;
 for (s=springs;s;s=s->next)
    s->step(time);
 Mass *m;
 for (m=masses/*->next*/;m;m=m->next)
    m->step(time);
}

double projectX(Point *p)
{return p->x/10+320 + p->z/100;
}

double projectY(Point *p)
{return p->y/10+240 + p->z/100;
}


#if 0
void Object::draw(void)
{Spring *s;
 double o;
 for (s=springs;s;s=s->next)
    {line(projectX(&s->m1->p),projectY(&s->m1->p),
	  projectX(&s->m2->p),projectY(&s->m2->p));
    }
}
#endif

void extrude(Object *o)
{Mass *m,*n,*newList,*original,*newTail;
 Face *f,*newFaces;
 int i,owner;
 Spring *s;
 newList=NULL;
 newTail=NULL;
 newFaces=NULL;
 for (m=o->masses;m;m=m->next)
    {n=new Mass(m->p.x,m->p.y,m->p.z+200.0,m->mass);
     n->next=NULL;
     if (!newTail)
	{newList=n;
	 newTail=n;
	}
     else
	{newTail->next=n;
	 newTail=n;
	}
    }
 for (m=o->masses,n=newList;m;m=m->next,n=n->next)
    {/* for each mass, see which ones it is connected to in the original
	mesh, and connect it up in the new mesh */
     for (s=o->springs;s;s=s->next)
	{Mass *m1,*n1;
	 if (s->m1==m)
	    {for (m1=o->masses,n1=newList;
		  m1 && m1!=s->m2;
		  m1=m1->next,n1=n1->next) ;
	     assert(m1 && n1);

	     o->addSpring(new Spring(m,n,s->k,s->d));
	     o->addSpring(new Spring(n,n1,s->k,s->d));
	     o->addSpring(new Spring(m,n1,s->k,s->d));
	     /* if this edge is bounded by exactly one face, then
		we need to add faces to connect it to the other side */
	     int nmBound=0;
	     for (f=o->faces;f;f=f->next)
		{if ((f->m[0]==m && (f->m[1]==m1 || f->m[3]==m1)) ||
		     (f->m[1]==m && (f->m[2]==m1 || f->m[0]==m1)) ||
		     (f->m[2]==m && (f->m[3]==m1 || f->m[1]==m1)) ||
		     (f->m[3]==m && (f->m[0]==m1 || f->m[2]==m1)))
		    {nmBound++;
		     owner=f->owner;
		    }
		}
	     if (nmBound!=1)
		continue;
	     f=new Face(m,n,n1,m1,owner);
	     f->next=newFaces;
	     newFaces=f;
	    }
	 if (s->m2==m)
	    {for (m1=o->masses,n1=newList;
		  m1 && m1!=s->m1;
		  m1=m1->next,n1=n1->next) ;
	     assert(m1 && n1);

	     o->addSpring(new Spring(m,n,s->k,s->d));
	     /*o->addSpring(new Spring(n,n1,K,D));*/ /* doubles */
	     o->addSpring(new Spring(m,n1,s->k,s->d));
	    }

	}
    }

 /* make copies of front faces */
 for (f=o->faces;f;f=f->next)
    {Mass *t[4];
     for (i=0;i<4;i++)
	{for (m=o->masses,n=newList;m!=f->m[i];m=m->next,n=n->next) ;
	 t[i]=n;
	}
     o->addFace(new Face(t[3],t[2],t[1],t[0],f->owner));
    }

 Mass *next;
 for (n=newList;n;n=next)
    {next=n->next;
     o->addMass(n);
    }

 Face *fnext;
 for (f=newFaces;f;f=fnext)
    {fnext=f->next;
     o->addFace(f);
    }
}

int currentObject=0;
void addSquare(Object *o,Mass *sm1,Mass *sm2,double len,Mass **om1,Mass **om2,
	       double k=K,double damp=D)
{Point v,p;
 Mass *m1,*m2;
 double d;
 v.x=sm1->p.x-sm2->p.x;
 v.y=sm1->p.y-sm2->p.y;
 v.z=sm1->p.z-sm2->p.z;
 d=sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
 v.x/=d; v.y/=d; v.z/=d;
 p.x=v.y; p.y=-v.x; p.z=v.z;
 m1=new Mass(sm1->p.x+p.x*len,
	     sm1->p.y+p.y*len,
	     sm1->p.z+p.z*len,
	     sm1->mass);
 m2=new Mass(sm2->p.x+p.x*len,
	     sm2->p.y+p.y*len,
	     sm2->p.z+p.z*len,
	     sm2->mass);
 o->addMass(m1);
 o->addMass(m2);
 o->addSpring(new Spring(m1,sm1,k,damp));
 o->addSpring(new Spring(sm2,m2,k,damp));
 o->addSpring(new Spring(sm1,m2,k,damp));
 o->addSpring(new Spring(sm2,m1,k,damp));
 o->addSpring(new Spring(m2,m1,k,damp));
 o->addFace(new Face(m1,sm1,sm2,m2,currentObject));
 if (om1)
    *om1=m1;
 if (om2)
    *om2=m2;
}

void addSquareE(Object *o,Mass *sm1,Mass *sm2,Mass *om1,Mass *om2,
		double k=K,double d=D)
{Mass *m1,*m2;
 m1=om1;
 m2=om2;
 o->addSpring(new Spring(m1,sm1,k,d));
 o->addSpring(new Spring(sm2,m2,k,d));
 o->addSpring(new Spring(sm1,m2,k,d));
 o->addSpring(new Spring(sm2,m1,k,d));
/* o->addSpring(new Spring(m2,m1,k,d));*/
 o->addFace(new Face(m1,sm1,sm2,m2,currentObject));
}


void addL(Object *o,double startX,double startY)
{Point p;
 Mass *m[4];

 o->addMass(m[0]=new Mass(startX-100,startY,0,1.0));
 o->addMass(m[1]=new Mass(startX+100,startY,0,1.0));

 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]));
 o->addSpring(new Spring(m[0],m[1],K,D));
// addSquare(o,m[1],m[0],200,NULL,NULL); /* extend L */

 addSquare(o,m[2],m[3],200,&(m[0]),&(m[1]));
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]));
 addSquare(o,m[0],m[2],200,NULL,NULL);
 currentObject++;
}

void addT(Object *o,double startX,double startY)
{Point p;
 const double kmult=1.0;
 Mass *m[4];

 o->addMass(m[0]=new Mass(startX-100,startY,0,1.0));
 o->addMass(m[1]=new Mass(startX+100,startY,0,1.0));

 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]),K*kmult);
 o->addSpring(new Spring(m[0],m[1],K*kmult,D));

 addSquare(o,m[0],m[2],200,NULL,NULL,K*kmult);
 addSquare(o,m[3],m[1],200,NULL,NULL,K*kmult);

 addSquare(o,m[2],m[3],200,&(m[0]),&(m[1]),K*kmult);
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]),K*kmult);
 currentObject++;
}

void addY(Object *o,double startX,double startY)
{Point p;
 const double k=K*1.0;
 Mass *m[4];
 Mass *y[2];

 o->addMass(m[0]=new Mass(startX-100,startY,0,1.0));
 o->addMass(m[1]=new Mass(startX+100,startY,0,1.0));

 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]));
 m[0]->p.x+=90;
 m[1]->p.x-=90;
 m[0]->p.y+=60;
 m[1]->p.y+=60;

 o->addSpring(new Spring(m[0],m[1],K,D));

 addSquare(o,m[0],m[2],200,y,y+1);
// addSquare(o,y[0],y[1],200,NULL,NULL);
 addSquare(o,m[3],m[1],200,y,y+1);
// addSquare(o,y[0],y[1],200,NULL,NULL);

 addSquare(o,m[2],m[3],200,&(m[0]),&(m[1]),k);
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]),k);
 currentObject++;
}

void addM(Object *o,double startX,double startY)
{Point p;
 Mass *m[4];
 Mass *mid1,*mid2;

 o->addMass(m[0]=new Mass(startX+100,startY,0,1.0));
 o->addMass(m[1]=new Mass(startX-100,startY,0,1.0));

 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]));
 o->addSpring(new Spring(m[0],m[1],K,D));

 addSquare(o,m[2],m[3],200,&(m[0]),&(m[1]));
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]));
 addSquare(o,m[0],m[2],100,&(m[0]),&(m[1]));
 mid1=m[0]; mid2=m[1];
 addSquare(o,m[0],m[1],100,&(m[0]),&(m[1]));
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3]));
 addSquare(o,m[0],m[2],200,&(m[0]),&(m[1]));
 addSquare(o,m[0],m[1],200,&(m[0]),&(m[1]));
 mid1->p.y+=50;
 mid2->p.y+=50;
 currentObject++;
}

#define CORNERIN 60
void addO(Object *o,double startX,double startY,int bug=0)
{Point p;
 Mass *m[4];
 Mass *corner[4];
 Mass *link[2];

 o->addMass(m[0]=new Mass(startX-100,startY,0,1.0));
 o->addMass(m[1]=new Mass(startX+100,startY,0,1.0));

 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3])); /* 0 */
 link[0]=m[2];
 link[1]=m[0];
 corner[0]=m[1];
 o->addSpring(new Spring(m[0],m[1],K,D));

 addSquare(o,m[2],m[3],200,&(m[0]),&(m[1])); /* 1 */
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3])); /* 2 */
 corner[1]=m[3];
 addSquare(o,m[0],m[2],100,&(m[0]),&(m[1])); /* 3 */
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3])); /* 4 */
 corner[2]=m[3];
 addSquare(o,m[0],m[2],200,&(m[0]),&(m[1])); /* 5 */
 addSquare(o,m[0],m[1],200,&(m[2]),&(m[3])); /* 6 */
 corner[3]=m[3];
 addSquareE(o,m[0],m[2],link[0],link[1]); /* 7 */
 if (bug)
    o->addSpring(new Spring(link[0],link[1],K,D));
 corner[0]->p.x-=CORNERIN; corner[0]->p.y+=CORNERIN;
 corner[1]->p.x-=CORNERIN; corner[1]->p.y-=CORNERIN;
 corner[2]->p.x+=CORNERIN; corner[2]->p.y-=CORNERIN;
 corner[3]->p.x+=CORNERIN; corner[3]->p.y+=CORNERIN;
 currentObject++;
}

void addB(Object *o,double startX,double startY)
{Point p;
 const double bk=K*1.0;
 const double bd=D*1.0;
 Mass *m[4],*push[4];
 Mass *hook[2],*stiff[2];
 Mass *inflec,*middle;
 int i;

 o->addMass(m[0]=new Mass(startX-100,startY,0,1.0));
 o->addMass(m[1]=new Mass(startX+100,startY,0,1.0));
 stiff[0]=m[0]; stiff[1]=m[1];
 o->addSpring(new Spring(m[0],m[1],bk,D));
 hook[0]=m[0];
 addSquare(o,m[0],m[1],100,&(m[0]),&(m[1]),bk,bd);
 hook[1]=m[0];
 addSquare(o,m[0],m[1],100,&(m[0]),&(m[1]),bk,bd);
 addSquare(o,m[0],m[1],100,&(m[0]),&(m[1]),bk,bd);
 middle=m[0];
 addSquare(o,m[0],m[1],100,&(m[0]),&(m[1]),bk,bd);
 addSquare(o,m[0],m[1],100,&(m[0]),&(m[1]),bk,bd);
 addSquare(o,m[0],m[1],100,&(m[2]),&(m[3]),bk,bd);
 o->addSpring(new Spring(stiff[0],m[3],K,D));
 o->addSpring(new Spring(stiff[1],m[2],K,D));

 addSquare(o,m[0],m[2],200,&(m[2]),&(m[3]),bk,bd);
 o->addSpring(new Spring(hook[0],m[2],bk,bd));

 push[0]=m[0]; push[1]=m[2];
 addSquare(o,m[0],m[2],100,&(m[2]),&(m[3]),bk,bd);
 push[2]=m[2]; push[3]=m[3];
 o->addMass(inflec=new Mass(m[3]->p.x,m[3]->p.y-100,m[3]->p.z,1.0));
 addSquareE(o,m[2],m[3],middle,inflec,bk,bd);
 for (i=0;i<4;i++)
    push[i]->p.x-=60;

 addSquare(o,hook[0],hook[1],200,&(m[2]),&(m[3]),bk,bd);
 push[0]=m[3];
 push[1]=hook[1];
 addSquare(o,m[3],hook[1],100,&(m[2]),&(m[3]),bk,bd);
 push[2]=m[2];
 push[3]=m[3];
 addSquareE(o,m[2],m[3],inflec,middle,bk,bd);
 for (i=0;i<4;i++)
    push[i]->p.x-=60;
 currentObject++;
}


#define X 1900
#define Y -900
void main(void)
{int frame,i;
 double xpos;
 int nmFrames=10*60;
 Mass *o1,*o2,*o0;
 Object world;
 xpos=X+400;
 addL(&world,xpos,Y-600);
 xpos-=500;
 addO(&world,xpos,Y-600);
 xpos-=650;
 o0=world.masses;
 addB(&world,xpos,Y-600);
 xpos-=560;
 addO(&world,xpos,Y-600);
 xpos-=750;
 o1=world.masses;
 addT(&world,xpos,Y-600);
 xpos-=450;
 addO(&world,xpos,Y-600,1);
 xpos-=950;
 o2=world.masses;
 addM(&world,xpos,Y);
 xpos-=550;
 addY(&world,xpos,Y-600);
 world.recalcRestLengths();
 extrude(&world);

 world.setLightDir(1,1,4);
 ofile=fopen("anim.anm","wb");
 writeShort(world.countFaces());
 writeShort(nmFrames);
 writeShort(world.countPoints());

 setCamera(15.0/180.0*3.1415926,0,0,-2300.0);

 world.numberPoints(0);
 world.writeFaces();
 world.writePoints();

 o2->p.z-=150;
// o1->p.z+=50; o1->next->p.z+=50; o1->next->next->p.z+=50;
 for (frame=0;frame<nmFrames;frame++)
    {for (i=0;i<5;i++)
	world.step(0.10);
     world.writeDeltas();
     printf("frame:%d\n",frame);
    }
 /* clean up */
 fclose(ofile);
}
