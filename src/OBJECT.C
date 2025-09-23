#include "level.h"
#include "sprite.h"
#include "object.h"
#include "util.h"
#include "ai.h"
#include "sruins.h"
#include "sound.h"

#define MAXOBJECTS 350

Object objects[MAXOBJECTS];

Object* objectRunList; /* these lists have head nodes */
Object* objectIdleList;
Object* objectFreeList;

#define MAXNMMOVES 100
static struct
{
    Object *o, *toList;
} objMove[MAXNMMOVES];
static sint32 nmMoves;
void delay_moveObject(Object* o, Object* toList)
{
    assert(nmMoves < MAXNMMOVES);
    objMove[nmMoves].o = o;
    objMove[nmMoves].toList = toList;
    nmMoves++;
}

void moveObject(Object* o, Object* toList)
{
    assert(o);
    assert(o != objectRunList);
    assert(o != objectIdleList);
    assert(o != objectFreeList);
    assert(toList == objectRunList || toList == objectFreeList || toList == objectIdleList);

    assert(o->prev);
    o->prev->next = o->next;
    if (o->next)
        o->next->prev = o->prev;

    o->next = toList->next;
    if (o->next)
        o->next->prev = o;
    toList->next = o;
    o->prev = toList;
}

void processDelayedMoves(void)
{
    sint32 i;
    for (i = 0; i < nmMoves; i++)
        moveObject(objMove[i].o, objMove[i].toList);
    nmMoves = 0;
}

void initObjects(void)
{
    sint32 i;
    resetLimitCounters();
    objectRunList = objects + 0;
    objectIdleList = objects + 1;
    objectFreeList = objects + 2;

    objectRunList->next = NULL;
    objectIdleList->next = NULL;
    objectRunList->prev = NULL;
    objectIdleList->prev = NULL;

    for (i = 3; i < MAXOBJECTS - 1; i++)
    {
        objects[i].type = OT_DEAD;
        objects[i].next = &(objects[i + 1]);
        objects[i].prev = &(objects[i - 1]);
    }
    objectFreeList->prev = NULL;
    objectFreeList->next = objects + 3;
    objects[i].prev = &(objects[i - 1]);
    objects[i].next = NULL;
    objects[i].type = OT_DEAD;
    nmMoves = 0;
}

Object* getFreeObject(messHandler handler, sint32 type, sint32 class)
{
    if (!objectFreeList->next)
        return NULL;
    assert(objectFreeList->next);
    /* assert(objectFreeList->next->type==OT_DEAD); */
    objectFreeList->next->type = type;
    objectFreeList->next->class = class;
    objectFreeList->next->func = handler;
    return objectFreeList->next;
}

void signalObject(Object* object, sint32 message, sint32 param1, sint32 param2)
{
#ifndef NDEBUG
    checkStack();
    assert(object);
    assert(object != objectRunList);
    assert(object != objectIdleList);
    assert(object != objectFreeList);
#if 1
    assert(object->type != OT_DEAD); /* here */
    assert(object->type < OT_NMTYPES);
    assert(object->type >= 0);
#endif
#endif
    object->func(object, message, param1, param2);
}

void signalList(Object* head, sint32 message, sint32 param1, sint32 param2)
{
    Object* o;
    for (o = head->next; o; o = o->next)
    {
        assert(o->type >= 0);
        o->func(o, message, param1, param2);
    }
}

void null_func(Object* _this, sint32 message, sint32 param1, sint32 param2)
{
    return;
}

void delayKill(Object* o)
{
    sint32 oldClass;
    sint32 oldType;
    checkStack();
    assert(o->class != CLASS_DEAD);
    assert(o->type != OT_DEAD);
    signalObject(o, SIGNAL_OBJECTDESTROYED, (sint32)o, 0);
    oldType = o->type;
    o->type = OT_DEAD;
    o->func = null_func;
    oldClass = o->class;
    o->class = CLASS_DEAD;
    delay_moveObject(o, objectFreeList);
    if (oldClass == CLASS_MONSTER || (oldType >= OT_SANDALS && oldType <= OT_FEATHER)) /* just an optimization */
    {
        signalList(objectRunList, SIGNAL_OBJECTDESTROYED, (sint32)o, 0);
        signalList(objectIdleList, SIGNAL_OBJECTDESTROYED, (sint32)o, 0);
    }
}

void registerPBObject(sint32 pbNm, Object* me)
{
    sint32 w;
    /* dPrint("registering pb object %d (ot=%d)\n",pbNm,me->type); */
    assert(pbNm >= 0);
    assert(pbNm < level_nmPushBlocks);
    for (w = level_pushBlock[pbNm].startWall; w <= level_pushBlock[pbNm].endWall; w++)
    {
        assert(level_PBWall[w] >= 0);
        assert(level_PBWall[w] < level_nmWalls);
        level_wall[level_PBWall[w]].object = me;
        /* dPrint("  has wall %d\n",level_PBWall[w]); */
    }
}

static sint32 objectPPos;

sint16 suckShort(void)
{
    sint16 ret;
    ret = (level_objectParams[objectPPos] << 8) | (level_objectParams[objectPPos + 1]);
    objectPPos += 2;
    return ret;
}

sint32 suckInt(void)
{
    sint32 ret;
    ret = (level_objectParams[objectPPos] << 24) | (level_objectParams[objectPPos + 1] << 16) | (level_objectParams[objectPPos + 2] << 8) | (level_objectParams[objectPPos + 3] << 0);
    objectPPos += 4;
    return ret;
}

void suckSpriteParams(Sprite* s)
{
    if (!s)
    {
        suckShort();
        suckShort();
        suckShort();
        suckShort();
        return;
    }
    s->pos.x = F(suckShort()); /* mirror <<<< */
    s->pos.y = F(suckShort());
    s->pos.z = F(suckShort());
    s->angle = normalizeAngle(((sint32)suckShort()) * 5760);
}

void placeObjects(void)
{
    sint32 o, p1, p2, p3, p4, i;

    for (o = 0; o < level_nmObjects; o++)
    {
        if (level_object[o].type == OT_PLAYER)
        {
            objectPPos = level_object[o].firstParam;
            player = constructPlayer(suckShort(), 1);
            camera = player->sprite;
            break;
        }
    }
    objectPPos = 0;

    for (o = 0; o < level_nmObjects; o++)
    {
        assert(level_object[o].type < OT_NMTYPES);
        assert(level_object[o].firstParam == objectPPos);
        switch (level_object[o].type)
        {
            case OT_SECTORSWITCH:
                constructSectorSwitch();
                break;
            case OT_SW1:
            case OT_SW2:
            case OT_SW3:
            case OT_SW4:
                constructSwitch(level_object[o].type);
                break;
            case OT_SHOOTER1:
            case OT_SHOOTER2:
            case OT_SHOOTER3:
                constructShooter(level_object[o].type);
                break;
            case OT_TELEPSECTOR:
                constructTeleporter();
                break;
            case OT_TELEPRETURN:
                constructTeleportReturn();
                break;
            case OT_PLAYER:
                for (i = 0; i < 5; i++)
                    suckShort();
                break;
            case OT_BLOB:
                constructBlob(suckShort());
                break;
            case OT_QUEEN:
                constructQueen(suckShort());
                break;
            case OT_RAMSESTRIGGER:
                p1 = suckShort();
                p2 = suckShort();
                constructRamsesTrigger(p1, p2);
                break;
            case OT_CAMEL:
                constructCamel(suckShort());
                break;
            case OT_FORCEFIELD:
                constructForceField(suckShort());
                break;
            case OT_FLOORSWITCH:
                constructFloorSwitch(suckShort());
                break;
            case OT_NORMALELEVATOR:
            case OT_STUCKDOWNELEVATOR:
            case OT_STARTSDOWNELEVATOR:
                p1 = suckShort();
                p2 = suckShort();
                p3 = suckShort();
                constructElevator(p1, level_object[o].type, p2, p3);
                break;
            case OT_DOWNTHENUPELEVATOR:
                p1 = suckShort();
                p2 = suckShort();
                p3 = suckShort();
                constructUpDownElevator(p1, level_object[o].type, p2, p3);
                break;
            case OT_NORMALDOOR:
            case OT_BUGDOOR:
            case OT_TIMEDOOR:
            case OT_XDOOR:
            case OT_PLANTDOOR:
            case OT_STUCKUPDOOR:
                constructDoor(level_object[o].type, suckShort());
                break;
            case OT_STUCKDOWNDOOR:
                constructDownDoor(suckShort());
                break;
            case OT_RAMSESLID:
                constructRamsesLid(suckShort());
                break;
            case OT_EARTHQUAKEBLOCK:
                constructEarthQuakeBlock(suckShort());
                break;
            case OT_SINKINGBLOCK:
                constructSinkBlock(suckShort());
                break;
            case OT_BOBBINGBLOCK:
                constructBobBlock(suckShort());
                break;
            case OT_SPIDER:
                constructSpider(suckShort(), 1, NULL, NULL);
                break;
            case OT_ANUBIS:
                constructAnubis(suckShort());
                break;
            case OT_WASP:
                constructWasp(suckShort());
                break;
            case OT_FISH:
                constructFish(suckShort());
                break;
            case OT_HAWK:
                constructHawk(suckShort());
                break;
            case OT_MUMMY:
                constructMummy(suckShort());
                break;
            case OT_BASTET:
                constructBastet(suckShort());
                break;
            case OT_SENTRY:
                constructSentry(suckShort());
                break;
            case OT_MAGMANTIS:
                constructMagmantis(suckShort());
                break;
            case OT_SET:
                constructSet(suckShort());
                break;
            case OT_SELKIS:
                constructSelkis(suckShort());
                break;
            case OT_TORCH1:
            case OT_TORCH2:
            case OT_TORCH3:
            case OT_TORCH4:
            case OT_TORCH5:
            case OT_TORCH6:
            case OT_TORCH7:
            case OT_TORCH8:
            case OT_TORCH9:
            case OT_TORCH10:
            case OT_TORCH11:
            case OT_TORCH12:
            case OT_TORCH13:
            case OT_TORCH14:
            case OT_TORCH15:
            case OT_TORCH16:
            case OT_TORCH17:
            case OT_TORCH18:
            case OT_TORCH19:
            case OT_TORCH20:
            case OT_TORCH21:
            case OT_TORCH22:
            case OT_TORCH23:
            case OT_TORCH24:
            case OT_TORCH25:
            case OT_TORCH26:
            case OT_TORCH27:
            case OT_TORCH28:
            case OT_TORCH29:
            case OT_TORCH30:
            case OT_TORCH31:
            case OT_TORCH32:
            case OT_TORCH33:
            case OT_TORCH34:
            case OT_TORCH35:
            case OT_TORCH36:
            case OT_TORCH37:
            case OT_TORCH38:
                constructTorch(suckShort(), level_object[o].type);
                break;
            case OT_CONTAIN1:
            case OT_CONTAIN2:
            case OT_CONTAIN3:
            case OT_CONTAIN4:
            case OT_CONTAIN5:
            case OT_CONTAIN6:
            case OT_CONTAIN7:
            case OT_CONTAIN8:
            case OT_CONTAIN9:
            case OT_CONTAIN10:
            case OT_CONTAIN11:
            case OT_CONTAIN12:
            case OT_CONTAIN13:
            case OT_CONTAIN14:
            case OT_CONTAIN15:
            case OT_CONTAIN16:
            case OT_CONTAIN17:
            case OT_BOOMPOT1:
            case OT_BOOMPOT2:
                constructBlowPot(suckShort(), level_object[o].type);
                break;
            case OT_RAMSIDEMUMMY:
                suckShort();
                suckShort();
                suckShort();
                suckShort();
                suckShort();
                break;
            case OT_BUGKEY:
            case OT_TIMEKEY:
            case OT_XKEY:
            case OT_PLANTKEY:
            case OT_M60:
            case OT_COBRASTAFF:
            case OT_FLAMER:
            case OT_GRENADEAMMO:
            case OT_MANACLE:
            case OT_PISTOL:
            case OT_HEALTHBALL:
            case OT_AMMOBALL:
            case OT_HEALTHORB:
            case OT_AMMOORB:
            case OT_HEALTHSPHERE:
            case OT_AMMOSPHERE:
            case OT_CAPE:
            case OT_FEATHER:
            case OT_MASK:
            case OT_SANDALS:
            case OT_SCEPTER:
            case OT_PYRAMID:
            case OT_RING:
            case OT_ANKLETS:
            case OT_BLOODBOWL:
            case OT_TRANSMITTER:
            case OT_CHOPPER:
            case OT_RAMMUMMY:
            case OT_COMM_BATTERY:
            case OT_COMM_BOTTOM:
            case OT_COMM_DISH:
            case OT_COMM_HEAD:
            case OT_COMM_KEYBOARD:
            case OT_COMM_MOUSE:
            case OT_COMM_SCREEN:
            case OT_COMM_TOP:
            case OT_DOLL1:
            case OT_DOLL2:
            case OT_DOLL3:
            case OT_DOLL4:
            case OT_DOLL5:
            case OT_DOLL6:
            case OT_DOLL7:
            case OT_DOLL8:
            case OT_DOLL9:
            case OT_DOLL10:
            case OT_DOLL11:
            case OT_DOLL12:
            case OT_DOLL13:
            case OT_DOLL14:
            case OT_DOLL15:
            case OT_DOLL16:
            case OT_DOLL17:
            case OT_DOLL18:
            case OT_DOLL19:
            case OT_DOLL20:
            case OT_DOLL21:
            case OT_DOLL22:
            case OT_DOLL23:
                p1 = suckShort();
                p2 = suckShort();
                p3 = suckShort();
                p4 = suckShort();
                constructThing(p1, F(p2), F(p3), F(p4), level_object[o].type);
                suckShort(); /* discard angle */
                break;
            default:
                dPrint("unknown object type %d\n", level_object[o].type);
                assert(0);
                break;
        }
    }

    shiftSprites();

#if 0
 (sint32 i;
  /* put objects on the ground */
  for (i=0;i<MAXOBJECTS;i++)
     {if (objects[i].type!=OT_DEAD)
	 if ((objects[i].type!=OT_BLOB && objects[i].type!=OT_HAWK &&
	      objects[i].class==CLASS_MONSTER) ||
	     objects[i].type==OT_CAMEL)
	    {Sprite *s=((SpriteObject *)&(objects[i]))->sprite;
	     sint32 d=findFloorDistance(s->s,&(s->pos));
	     s->pos.y+=s->radius-d;
	    }
     }
 }
#endif

    /* put the map in a random pot */
    {
        sint32 potCount = 0;
        sint32 c;
        Object* o;
        for (o = objectIdleList; o; o = o->next)
        {
            if (o->type >= OT_CONTAIN1 && o->type <= OT_CONTAIN17)
                potCount++;
        }
        if (potCount > 0)
        {
            c = getNextRand() % potCount;
            for (o = objectIdleList; o; o = o->next)
            {
                if (o->type >= OT_CONTAIN1 && o->type <= OT_CONTAIN17)
                {
                    potCount--;
                    if (!potCount)
                        break;
                }
            }
            assert(o);
            ((BlowPotObject*)o)->iAmMapHolder = 1;
        }
    }
}

void hurtSprite(Sprite* sprite, Object* hurter, sint32 damage)
{
    assert(sprite->owner->class != CLASS_DEAD);
    assert(sprite->owner->type != OT_DEAD);
    signalObject(sprite->owner, SIGNAL_HURT, damage, (sint32)hurter);
}

#define MAXPERSECTOR 20
void radialDamage(Object* this, MthXyz* center, sint32 sector, sint32 damage, fix32 radius)
{
    sint8 processed[MAXNMSECTORS]; /* 0=not, 1=need to be, 2=has been */
    fix32 dist;
    MthXyz normal;
    Sprite* sp;
    SpriteObject* list[MAXPERSECTOR];
    sint32 nmToHurt;
    sint32 w, i, s, dam, push;
    sint32 done;
    for (i = 0; i < MAXNMSECTORS; i++)
        processed[i] = 0;
    processed[sector] = 1;
    do
    {
        done = 1;
        for (s = 0; s < MAXNMSECTORS; s++)
            if (processed[s] == 1)
            { /* hurt all the sprites in the sector */
                nmToHurt = 0;
                for (sp = sectorSpriteList[s]; sp; sp = sp->next)
                {
                    if (!sp->owner)
                        continue;
                    if (sp->owner->class != CLASS_MONSTER)
                        continue;
                    if (nmToHurt >= MAXPERSECTOR)
                        continue;
                    list[nmToHurt++] = (SpriteObject*)sp->owner;
                }
                for (nmToHurt--; nmToHurt >= 0; nmToHurt--)
                {
                    if (list[nmToHurt]->class == CLASS_DEAD)
                        continue;
                    sp = list[nmToHurt]->sprite;
                    if (!sp)
                        continue;
                    dist = approxDist(center->x - sp->pos.x, center->y - sp->pos.y, center->z - sp->pos.z);
                    if (dist > radius)
                        continue;
                    dam = MTH_Mul(damage, F(1) - MTH_Div(dist, radius));
                    {
                        sint32 pdam = dam;
                        if (pdam > 50)
                            pdam = 50;
                        push = MTH_Div((pdam << 14), dist);
                        normal.x = MTH_Mul(-center->x + sp->pos.x, push);
                        normal.y = MTH_Mul(-center->y + sp->pos.y, push);
                        normal.z = MTH_Mul(-center->z + sp->pos.z, push);
                        sp->vel.x += normal.x;
                        sp->vel.y += normal.y;
                        sp->vel.z += normal.z;
                    }
                    signalObject((Object*)list[nmToHurt], SIGNAL_HURT, dam, (sint32)this);
                }
                /* add close adjacent sectors */
                for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
                {
                    if (level_wall[w].nextSector == -1)
                        break;
                    getVertex(level_wall[w].v[0], &normal);
                    normal.x = center->x - normal.x;
                    normal.y = center->y - normal.y;
                    normal.z = center->z - normal.z;
                    dist = MTH_Product((fix32*)level_wall[w].normal, (fix32*)&normal);
                    if (dist < radius)
                        if (processed[level_wall[w].nextSector] == 0)
                        {
                            processed[level_wall[w].nextSector] = 1;
                            done = 0;
                        }
                }
                processed[s] = 2;
            }
    } while (!done);
}

#if 0
void radialDamage(Object *this,MthXyz *center,sint32 damage,fix32 radius)
(sint32 nmList,dist,dam,push;
 MthXyz normal;
 Object *o,*list[MAXOBJECTS];
 Sprite *s;

 checkStack();
 nmList=0;
 for (o=objectRunList->next;o;o=o->next)
    if (o->class==CLASS_MONSTER)
       list[nmList++]=o;
 for (o=objectIdleList->next;o;o=o->next)
    if (o->class==CLASS_MONSTER)
       list[nmList++]=o;
 for (nmList--;nmList>=0;nmList--)
    {s=((MonsterObject *)list[nmList])->sprite;
     if (s->owner->class==CLASS_DEAD)
	continue;
     dist=approxDist(center->x-s->pos.x,
		     center->y-s->pos.y,
		     center->z-s->pos.z);
     if (dist>radius)
	continue;
     dam=MTH_Mul(damage,F(1)-MTH_Div(dist,radius));
     push=MTH_Div((dam<<14),dist);
     normal.x=MTH_Mul(-center->x+s->pos.x,push);
     normal.y=MTH_Mul(-center->y+s->pos.y,push);
     normal.z=MTH_Mul(-center->z+s->pos.z,push);
     s->vel.x+=normal.x;
     s->vel.y+=normal.y;
     s->vel.z+=normal.z;
     /* these asserts can fail if an object blows up from radial damage
	and kills something that is still in an earlier explosion's list */
     assert(s->owner->class!=CLASS_DEAD);
     assert(s->owner->type!=OT_DEAD);
     signalObject(s->owner,SIGNAL_HURT,dam,(sint32)this);
    }
}
#endif

void pushBlockMakeSound(PushBlockObject* source, sint32 sndNm)
{
    MthXyz pos;
    sint32 s = source->pbNum;
    pos.x = F(level_sector[level_pushBlock[s].enclosingSector].center[0]);
    pos.y = F(level_sector[level_pushBlock[s].enclosingSector].center[1]);
    pos.z = F(level_sector[level_pushBlock[s].enclosingSector].center[2]);
    posMakeSound((sint32)(source), &pos, (sndNm));
}

void pushBlockAdjustSound(PushBlockObject* source)
{
    MthXyz pos;
    sint32 s = source->pbNum;
    pos.x = F(level_sector[level_pushBlock[s].enclosingSector].center[0]);
    pos.y = F(level_sector[level_pushBlock[s].enclosingSector].center[1]);
    pos.z = F(level_sector[level_pushBlock[s].enclosingSector].center[2]);
    posAdjustSound((sint32)(source), &pos);
}
