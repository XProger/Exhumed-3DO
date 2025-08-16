#include "level.h"
#include "sprite.h"
#include "object.h"
#include "util.h"
#include "route.h"

static short fromWall[MAXNMSECTORS];
static short fromSector[MAXNMSECTORS];

typedef struct __qNode
{struct __qNode *next;
 short s;
} QNode;

#define MAXFRONTIER 100
QNode nodes[MAXFRONTIER];

QNode *freeNodes;

void initRoutePlotter(void)
{int i,s,w;
 MthXyz t;
 freeNodes=NULL;
 for (i=0;i<MAXFRONTIER;i++)
    {nodes[i].next=freeNodes;
     freeNodes=nodes+i;
    }
 
 /* compute stepability for all doorways */
 for (s=0;s<level_nmSectors;s++)
    {for (w=level_sector[s].firstWall;w<=level_sector[s].lastWall;w++)
	{if (level_wall[w].normal[1]!=0)
	    continue;
	 if (level_wall[w].nextSector==-1)
	    break;
	 if (findFloorDistance(s,getVertex(level_wall[w].v[3],&t))>F(16))
	    level_wall[w].flags|=WALLFLAG_NOTSTEPABLE;
	}
    }
}

short routeBuffer[MAXNMSECTORS];

int plotRouteToObject(MonsterObject *from,SpriteObject *to,int floater)
{int s,w,nextSector,i,c;
#ifndef NDEBUG
 int loopCount=0;
#endif
 QNode *frontier,*ftail,*n,*q;
 for (s=0;s<level_nmSectors;s++)
    fromWall[s]=-1;
 frontier=freeNodes;
 freeNodes=freeNodes->next;
 frontier->s=from->sprite->s;
 fromWall[from->sprite->s]=-2;
 frontier->next=NULL;
 ftail=frontier;
 while (frontier)
    {/* remove all nodes from frontier */
     assert(loopCount++<5000);
     if (frontier->s==to->sprite->s)
	{/* unroll route */
	 c=0;
	 s=to->sprite->s;
	 while (s!=from->sprite->s)
	    {assert(fromWall[s]!=-1);
	     assert(fromWall[s]!=-2);
	     routeBuffer[c++]=fromWall[s];
	     s=fromSector[s];
	    }
	 /* write last n steps into object's route buffer */
	 for (i=0;i<ROUTESIZE && c>0;i++)
	    from->route[i]=routeBuffer[--c];
	 from->route[i]=-1;
	 from->routePos=0;
	 while (frontier)
	    {q=frontier;
	     frontier=frontier->next;
	     q->next=freeNodes;
	     freeNodes=q;
	    }
	 return 1;
	}
     for (w=level_sector[frontier->s].firstWall;
	  w<=level_sector[frontier->s].lastWall;
	  w++)
	{if (level_wall[w].nextSector==-1)
	    break;
	 if ((level_wall[w].flags & from->sprite->flags) & WALLFLAG_BLOCKBITS)
	    continue;	 
	 if (!floater && (level_wall[w].flags & WALLFLAG_NOTSTEPABLE))
	    continue;
	 if (level_wall[w].flags & WALLFLAG_DOORWALL)
	    continue;
	 nextSector=level_wall[w].nextSector;
	 if (fromWall[nextSector]==-1)
	    {fromWall[nextSector]=w;
	     assert(w>=0);
	     fromSector[nextSector]=frontier->s;
	     assert(freeNodes);
	     q=freeNodes;
	     freeNodes=freeNodes->next;
	     q->s=nextSector;
	     q->next=NULL;
	     assert(ftail);
	     ftail->next=q;
	     ftail=q;
	    }
	}
     n=frontier;
     frontier=frontier->next;
     n->next=freeNodes;
     freeNodes=n;
    }
 /* no route to destination */
 from->routePos=-1;
 return 0;
}
