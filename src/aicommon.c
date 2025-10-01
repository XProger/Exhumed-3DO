#include "mth.h"

#include "slevel.h"
#include "level.h"
#include "sprite.h"
#include "ai.h"
#include "util.h"
#include "sequence.h"
#include "hitscan.h"
#include "sound.h"
#include "sruins.h"
#include "route.h"
#include "walls.h"
#include "pic.h"
#include "gamestat.h"
#include "bigmap.h"

#define GRAVITY (6 << 12)
#define HB 0x8000

#define AISLOT(x) ((this->aiSlot & (x)) == (aicount & (x)))

sint32 aicount = 0, nextAiSlot = 0;

void movePlayerToSector(sint32 playerSec)
{
    MthXyz pos;
    assert(camera);
    /* move player to position */
    pos.x = F(level_sector[playerSec].center[0]);
    pos.y = F(level_sector[playerSec].center[1]);
    pos.z = F(level_sector[playerSec].center[2]);
    pos.y += camera->radius - findFloorDistance(playerSec, &pos);
    moveSpriteTo(camera, playerSec, &pos);
}

void markSectorFloor(sint32 sec, Object* this)
{
    sint32 i;
    for (i = level_sector[sec].firstWall; i <= level_sector[sec].lastWall; i++)
    {
        if (level_wall[i].normal[1] > 0)
        {
            level_wall[i].object = this;
        }
    }
}

void setSectorBrightness(sint32 s, sint32 level)
{
    sint32 w, i;
    for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
    {
        if (level_wall[w].flags & WALLFLAG_PARALLELOGRAM)
        {
            sint32 nm;
            nm = (level_wall[w].tileHeight + 1) * (level_wall[w].tileLength + 1);
            for (i = level_wall[w].firstLight; i < level_wall[w].firstLight + nm; i++)
                level_vertexLight[i] = level;
        }
        else
        {
            for (i = level_wall[w].firstVertex; i <= level_wall[w].lastVertex; i++)
                level_vertex[i].light = level;
        }
    }
}

sint32 getFacingAngle(Sprite* from, Sprite* to)
{
    sint32 angle;
    angle = getAngle(from->pos.x - to->pos.x, from->pos.z - to->pos.z);
    angle -= from->angle + F(180);
    if (angle > F(180))
        angle -= F(360);
    if (angle < F(-180))
        angle += F(360);

    if (angle < 0)
    {
        if (angle > F(-23))
            return 0;
        if (angle > F(-23 - 45))
            return 7;
        if (angle > F(-23 - 90))
            return 6;
        if (angle > F(-23 - 135))
            return 5;
        return 4;
    }
    if (angle < F(23))
        return 0;
    if (angle < F(23 + 45))
        return 1;
    if (angle < F(23 + 90))
        return 2;
    if (angle < F(23 + 135))
        return 3;
    return 4;
}

sint32 PlotCourseToObject(SpriteObject* from, SpriteObject* to)
{
    sint32 xdiff, zdiff;
    assert(from);
    assert(to);
    assert(from != to);

    xdiff = to->sprite->pos.x - from->sprite->pos.x;
    zdiff = to->sprite->pos.z - from->sprite->pos.z;

    from->sprite->angle = getAngle(xdiff, zdiff);
    /*  return(ksqrt((xdiff*xdiff)+(ydiff*ydiff))); */
    return 0;
}

fix32 spriteDist2(Sprite* s1, Sprite* s2)
{
    fix32 dx, dy, dz;
    dx = s1->pos.x - s2->pos.x;
    dy = s1->pos.y - s2->pos.y;
    dz = s1->pos.z - s2->pos.z;
    return f(dx) * dx + f(dy) * dy + f(dz) * dz;
}

fix32 spriteDistApprox(Sprite* s1, Sprite* s2)
{
    return approxDist(s1->pos.x - s2->pos.x, s1->pos.y - s2->pos.y, s1->pos.z - s2->pos.z);
}

MonsterObject* findPlayer(Sprite* sprite, sint32 dist)
{
    assert(sprite);
    assert(player->sprite);
    if (spriteDistApprox(sprite, player->sprite) > dist)
        return NULL;
    if (canSee(sprite, player->sprite))
        return player;
    return NULL;
}

sint32 randomAngle(sint32 spreadlog2)
{
    return ((sint32)(getNextRand() << 16)) >> (16 - spreadlog2);
}

void setSequence(SpriteObject* this)
{
    uint16 seqBase = this->sequenceMap[this->state];
    assert(this->type >= 0);
    assert(this->type < OT_NMTYPES);
    assert(level_sequenceMap[this->type] >= 0);
    if (seqBase & 0x8000)
        this->sprite->sequence = level_sequenceMap[this->type] + (seqBase & 0x7fff) + getFacingAngle(this->sprite, camera);
    else
        this->sprite->sequence = level_sequenceMap[this->type] + seqBase;
}

void spriteObject_makeZorch(SpriteObject* this)
{
    if (getNextRand() & 1)
        constructZorch(this->sprite->s, this->sprite->pos.x, this->sprite->pos.y + F(32), this->sprite->pos.z, (getNextRand() & 0x40) ? OT_REDZORCH : OT_BLUEZORCH);
}

sint32 monsterObject_signalHurt(MonsterObject* this, sint32 param1, sint32 param2)
{
    Object* hurter = (Object*)param2;
    assert(this->class == CLASS_MONSTER);
    this->sprite->flags |= SPRITEFLAG_FLASH;
    this->health -= param1;
    if (hurter->class == CLASS_MONSTER && hurter != (Object*)this)
        this->enemy = (MonsterObject*)hurter;
    if (hurter->class == CLASS_PROJECTILE)
    {
        ProjectileObject* po = (ProjectileObject*)hurter;
        if (po->owner && po->owner->class == CLASS_MONSTER && po->owner != (Object*)this)
            this->enemy = (MonsterObject*)(po->owner);
    }
    if (this->health <= 0 && this->health + param1 > 0)
    {
        return 1;
    }
    return 0;
}

void monsterObject_signalDestroyed(MonsterObject* this, sint32 param1)
{
    Object* killed = (Object*)param1;
    if (killed == (Object*)this)
        freeSprite(this->sprite);
    if (killed == (Object*)this->enemy)
        this->enemy = NULL;
}

MthXyz followRoute(MonsterObject* this)
{
    sint32 v, w;
    fix32 planeDist;
    sWallType* targetWall;
    MthXyz doorPos, wallP;

    assert(this->class == CLASS_MONSTER);
    if (this->route[this->routePos] == -1)
    { /* end of route */
        this->routePos = -1;
        doorPos.x = 0;
        return doorPos;
    }

    /* see if we've passed the doorway we were heading for */
    targetWall = level_wall + this->route[this->routePos];
    getVertex(targetWall->v[0], &wallP);
    planeDist = (f(this->sprite->pos.x - wallP.x)) * targetWall->normal[0] + (f(this->sprite->pos.y - wallP.y)) * targetWall->normal[1] + (f(this->sprite->pos.z - wallP.z)) * targetWall->normal[2];
    if (planeDist <= F(1))
    /* we have passed the wall */
    {
        this->routePos++;
        doorPos = followRoute(this);
        /* make monster move perp to doorway to try to get him to
           go around corners better */
        this->sprite->angle = getAngle(-targetWall->normal[0], -targetWall->normal[2]);
        return doorPos;
    }
    /* we haven't passed the wall */

    /* find centroid of door we are going towards */
    doorPos.x = 0;
    doorPos.y = 0;
    doorPos.z = 0;
    w = this->route[this->routePos];
    for (v = 0; v < 4; v++)
    {
        getVertex(targetWall->v[v], &wallP);
        doorPos.x += f(wallP.x);
        doorPos.y += f(wallP.y);
        doorPos.z += f(wallP.z);
    }
    doorPos.x = doorPos.x << (16 - 2);
    doorPos.y = doorPos.y << (16 - 2);
    doorPos.z = doorPos.z << (16 - 2);

    this->sprite->angle = getAngle(doorPos.x - this->sprite->pos.x, doorPos.z - this->sprite->pos.z);

    return doorPos;
}

void spriteObject_makeSound(SpriteObject* this, sint32 sndNm)
{
    assert(this->type < OT_NMTYPES);
    assert(sndNm >= 0);
    spriteMakeSound(this->sprite, level_objectSoundMap[this->type] + sndNm);
}

void spriteHome(SpriteObject* this, SpriteObject* enemy, sint32 fudge, sint32 step)
{
    sint32 angle = getAngle(enemy->sprite->pos.x - this->sprite->pos.x, enemy->sprite->pos.z - this->sprite->pos.z);
    angle = normalizeAngle(angle - this->sprite->angle);

    if (angle < -fudge)
        this->sprite->angle = normalizeAngle(this->sprite->angle - step);
    if (angle > fudge)
        this->sprite->angle = normalizeAngle(this->sprite->angle + step);
}

void setState(SpriteObject* this, sint32 state)
{
    assert(level_sequenceMap[this->type] != -2);
    this->state = state;
    this->sprite->sequence = level_sequenceMap[this->type] + (this->sequenceMap[state] & 0x7fff);
    this->sprite->frame = 0;
}

/* returns 1 if found enemy, 0 if seeking */
sint32 monster_seekEnemy(MonsterObject* this, sint32 floater, sint32 speed2)
{
    assert(this->class == CLASS_MONSTER);
    if (!this->enemy)
    {
        setState((SpriteObject*)this, 0);
        this->routePos = -1;
        return 0;
    }
    if (canSee(this->sprite, this->enemy->sprite))
    {
        this->routePos = -1;
        return 1;
    }
    else
    {
        if (this->routePos == -1)
            return 1;
        {
            MthXyz p = followRoute(this);
            if (this->routePos == -1)
                return 0;
            this->sprite->vel.x = MTH_Cos(this->sprite->angle) << speed2;
            this->sprite->vel.z = MTH_Sin(this->sprite->angle) << speed2;
            if (floater)
            {
                if (this->sprite->pos.y < p.y - F(10))
                    this->sprite->vel.y = F(1);
                else if (p.y > this->enemy->sprite->pos.y + F(10))
                    this->sprite->vel.y = -F(1);
                else
                    this->sprite->vel.y = 0;
            }
        }
    }
    return 0;
}

enum
{
    STATE_IDLE,
    STATE_WALK,
    STATE_SRA,
    STATE_LRA,
    STATE_HIT,
    STATE_SEEK,
    STATE_NMBASICSTATES
};

void normalMonster_idle(MonsterObject* this, sint32 speed, sint32 icuSound)
{
    if ((this->aiSlot & 0x1f) == (aicount & 0x1f))
    {
        this->sprite->vel.x = 0;
        this->sprite->vel.z = 0;
        if (!this->enemy)
        {
            if (this->type == OT_BASTET)
                this->enemy = findPlayer(this->sprite, F(600));
            else
                this->enemy = findPlayer(this->sprite, F(1600));
        }
        if (this->enemy)
        {
            if (icuSound >= 0)
                spriteObject_makeSound((SpriteObject*)this, icuSound);
            setState((SpriteObject*)this, STATE_WALK);
            PlotCourseToObject((SpriteObject*)this, (SpriteObject*)this->enemy);
            this->sprite->vel.x = MTH_Cos(this->sprite->angle) << speed;
            this->sprite->vel.z = MTH_Sin(this->sprite->angle) << speed;
        }
    }
}

enum
{
    DO_RANDOMWANDER,
    DO_CHARGE,
    DO_FOLLOWROUTE,
    DO_GOIDLE
};
sint32 decideWhatToDo(MonsterObject* this, sint32 route, sint32 floater)
{
    sint32 distance;
#if 0
 if ((this->sprite->flags ^ this->enemy->sprite->flags) &
     SPRITEFLAG_UNDERWATER)
    /* we and our enemy are on different sides of the water */
    return DO_RANDOMWANDER;
#endif
    if (this->sprite->flags & SPRITEFLAG_UNDERWATER)
    {
        if (!(this->enemy->sprite->flags & SPRITEFLAG_UNDERWATER))
            return DO_RANDOMWANDER;
    }
    else
    {
        if (level_sector[this->enemy->sprite->s].flags & SECFLAG_WATER)
            return DO_RANDOMWANDER;
    }
    if ((this->enemy != player || !invisibleCounter) && canSee(this->sprite, this->enemy->sprite))
        return DO_CHARGE;
    distance = spriteDistApprox(this->sprite, this->enemy->sprite);
    if (distance > F(1600))
        return DO_GOIDLE;
    if (!invisibleCounter && route)
    {
        if (plotRouteToObject(this, (SpriteObject*)this->enemy, floater))
            return DO_FOLLOWROUTE;
    }
    if (distance > F(1000))
        return DO_GOIDLE;
    return DO_RANDOMWANDER;
}

void normalMonster_walking(MonsterObject* this, sint32 collide, sint32 fflags, sint32 speed)
{
    assert(this->enemy);
    if ((collide & COLLIDE_SPRITE) && sprites[collide & 0xffff].owner == (Object*)this->enemy)
    { /* bite them! */
        setState((SpriteObject*)this, STATE_SRA);
        this->sprite->vel.x = 0;
        this->sprite->vel.z = 0;
    }
    if (collide & COLLIDE_WALL)
    {
        this->sprite->angle = normalizeAngle(this->sprite->angle + /*F(180)+*/
            randomAngle(8));
        this->sprite->vel.x = MTH_Cos(this->sprite->angle) << speed;
        this->sprite->vel.z = MTH_Sin(this->sprite->angle) << speed;
    }
    if (AISLOT(0xf))
    {
        sint32 action = decideWhatToDo(this, 1, 0);
        switch (action)
        {
            case DO_GOIDLE:
                setState((SpriteObject*)this, STATE_IDLE);
                this->enemy = NULL;
                break;
            case DO_RANDOMWANDER:
                if (AISLOT(0x1f))
                {
                    this->sprite->angle = ((sint16)getNextRand()) * 360;
                    this->sprite->vel.x = MTH_Cos(this->sprite->angle) << speed;
                    this->sprite->vel.z = MTH_Sin(this->sprite->angle) << speed;
                }
                break;
            case DO_FOLLOWROUTE:
                this->sprite->vel.x = 0;
                this->sprite->vel.z = 0;
                setState((SpriteObject*)this, STATE_SEEK);
                return;
            case DO_CHARGE:
                PlotCourseToObject((SpriteObject*)this, (SpriteObject*)this->enemy);
                /* if we get here, we know we can see the player */
                if (getNextRand() & 0x20)
                { /* shoot player */
                    setState((SpriteObject*)this, STATE_LRA);
                    this->sprite->vel.x = 0;
                    this->sprite->vel.z = 0;
                }
                else
                {
                    this->sprite->angle = normalizeAngle(this->sprite->angle + randomAngle(6));
                    this->sprite->vel.x = MTH_Cos(this->sprite->angle) << speed;
                    this->sprite->vel.z = MTH_Sin(this->sprite->angle) << speed;
                }
                break;
        }
    }
}

void initProjectile(MthXyz* from, MthXyz* to, MthXyz* outPos, MthXyz* outVel, sint32 height, sint32 speed2)
{
    fix32 d;
    d = approxDist(from->x - to->x, from->y - to->y, from->z - to->z);
    d = d >> speed2;
    *outPos = *from;
    outPos->y += height;
    outVel->x = MTH_Div(to->x - outPos->x, d);
    outVel->y = MTH_Div(to->y - outPos->y, d);
    outVel->z = MTH_Div(to->z - outPos->z, d);
    outPos->x += outVel->x << 3;
    outPos->y += outVel->y << 3;
    outPos->z += outVel->z << 3;
}

void doh_func(Object* o, sint32 i, sint32 j, sint32 k)
{
    assert(0);
}

void runObjects(void)
{
    signalList(objectRunList, SIGNAL_MOVE, 0, 0);
    aicount++;
}

void signalAllObjects(sint32 signal, sint32 param1, sint32 param2)
{
    signalList(objectRunList, signal, param1, param2);
    signalList(objectIdleList, signal, param1, param2);
}

void pbObject_move(PushBlockObject* pbo, fix32 dy)
{
    sint32 nowPos;
    nowPos = f(pbo->offset);
    pbo->offset += dy;
    movePushBlock(pbo->pbNum, 0, f(pbo->offset) - nowPos, 0);
}

void pbObject_moveTo(PushBlockObject* pbo, sint32 y)
{
    sint32 nowPos;
    nowPos = f(pbo->offset);
    pbo->offset = F(y);
    movePushBlock(pbo->pbNum, 0, f(pbo->offset) - nowPos, 0);
}

void explodeMaskedWall(sint32 wallNm)
{
    sint32 size;
    sint32 i, sector;
    MthXyz v1, v2, origin, p;
    fix32 u, v;
    sWallType* wall = level_wall + wallNm;
    assert(wall->flags & WALLFLAG_EXPLODABLE);
    sector = findWallsSector(wallNm);
    wall->flags &= ~(WALLFLAG_BLOCKED | WALLFLAG_EXPLODABLE | WALLFLAG_BLOCKSSIGHT);
    wall->flags |= WALLFLAG_INVISIBLE;
    if (wall->flags & WALLFLAG_PARALLELOGRAM)
        size = wall->tileHeight * wall->tileLength;
    else
        size = wall->lastFace - wall->firstFace + 1;
    size /= 4;
    getVertex(wall->v[0], &origin);
    getVertex(wall->v[1], &v1);
    getVertex(wall->v[3], &v2);
    v1.x -= origin.x;
    v1.y -= origin.y;
    v1.z -= origin.z;
    v2.x -= origin.x;
    v2.y -= origin.y;
    v2.z -= origin.z;
    for (i = 0; i <= size; i++)
    {
        u = getNextRand() & 0xffff;
        v = getNextRand() & 0xffff;
        p.x = origin.x + MTH_Mul(u, v1.x) + MTH_Mul(v, v2.x);
        p.y = origin.y + MTH_Mul(u, v1.y) + MTH_Mul(v, v2.y);
        p.z = origin.z + MTH_Mul(u, v1.z) + MTH_Mul(v, v2.z);
        constructOneShot(sector, p.x, p.y, p.z, OT_GRENPOW, F(2), 0, i);
        constructHardBit(sector, &p, level_sequenceMap[OT_BRICK], 0, i % 4);
    }
    /* explode other side of wall if appilcable */
    {
        sint32 w;
        sint32 s;
        s = wall->nextSector;
        assert(s != -1);
        for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
        {
            if (level_wall[w].nextSector == sector && (level_wall[w].flags & WALLFLAG_EXPLODABLE))
            {
                level_wall[w].flags &= ~(WALLFLAG_BLOCKED | WALLFLAG_EXPLODABLE | WALLFLAG_BLOCKSSIGHT);
                level_wall[w].flags |= WALLFLAG_INVISIBLE;
            }
        }
    }
}

void explodeAllMaskedWallsInSector(sint32 s)
{
    sint32 w;
    for (w = level_sector[s].firstWall; w <= level_sector[s].lastWall; w++)
        if (level_wall[w].flags & WALLFLAG_EXPLODABLE)
            explodeMaskedWall(w);
    if (level_sector[s].flags & SECFLAG_EXPLODINGWALLS)
    {
        level_sector[s].flags &= ~SECFLAG_EXPLODINGWALLS;
        for (w = 0; w < level_nmSectors; w++)
            if ((level_sector[w].flags & SECFLAG_EXPLODINGWALLS) && level_sector[w].cutChannel == level_sector[s].cutChannel)
                explodeAllMaskedWallsInSector(w);
    }
}
