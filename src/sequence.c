#include "sequence.h"
#include "util.h"
#include "spr.h"
#include "pic.h"
#include "sound.h"

#include "grenpal.h"
#include "manpal.h"

sint32 level_nmSequences;
sint32 level_nmFrames;
sint32 level_nmChunks;

sint16 *level_sequence;
sFrameType* level_frame;
sChunkType* level_chunk;
sint16* level_sequenceMap;

static sint8 level_seq_data[80 * 1024];
static sint8 weapon_seq_data[16 * 1024];

sint32 loadSequences(sint32 tileBase, sint32 soundBase)
{
    sint32 size, i;
    sint8* buffer = level_seq_data;
    struct seqHeader* head;

    fs_read(&size, 4);

    size = FS_INT(&size);

    assert(size > 0 && size < sizeof(level_seq_data));
    fs_read(buffer, size);

    head = (struct seqHeader*)buffer;

    head->nmSequences = FS_INT(&head->nmSequences);
    head->nmFrames = FS_INT(&head->nmFrames);
    head->nmChunks = FS_INT(&head->nmChunks);

    level_nmSequences = head->nmSequences;
    level_nmFrames = head->nmFrames;
    level_nmChunks = head->nmChunks;

    assert(level_nmSequences >= 0);
    assert(level_nmFrames >= 0);
    assert(level_nmChunks >= 0);

    assert(((uint32)size) == sizeof(struct seqHeader) + level_nmSequences * sizeof(sint16) + level_nmFrames * sizeof(sFrameType) + level_nmChunks * sizeof(sChunkType) + OT_NMTYPES * sizeof(sint16));

    level_frame = (sFrameType*)(buffer + sizeof(struct seqHeader));

    for (i = 0; i < level_nmFrames; i++)
    {
        sFrameType *v = level_frame + i;
        v->chunkIndex = FS_SHORT(&v->chunkIndex);
        v->flags = FS_SHORT(&v->flags);
        v->sound = FS_SHORT(&v->sound);
    }

#ifndef NDEBUG
    {
        sint32 i;
        for (i = 0; i < level_nmFrames; i++)
        {
            assert(level_frame[i].pad[0] == 0);
            assert(level_frame[i].pad[1] == 0);
        }
    }
#endif

    level_chunk = (sChunkType*)(level_frame + level_nmFrames);

    for (i = 0; i < level_nmChunks; i++)
    {
        sChunkType *v = level_chunk + i;

        v->chunkx = FS_SHORT(&v->chunkx);
        v->chunky = FS_SHORT(&v->chunky);
        v->tile = FS_SHORT(&v->tile);
    }

#ifndef NDEBUG
    {
        sint32 i;
        for (i = 0; i < level_nmChunks; i++)
        {
            assert(level_chunk[i].pad == 0);
        }
    }
#endif

    level_sequence = (sint16*)(level_chunk + level_nmChunks);

    for (i = 0; i < level_nmSequences; i++)
    {
        level_sequence[i] = FS_SHORT(&level_sequence[i]);
    }

    level_sequenceMap = level_sequence + level_nmSequences;

    for (i = 0; i < OT_NMTYPES; i++)
    {
        level_sequenceMap[i] = FS_SHORT(&level_sequenceMap[i]);
    }

    for (i = 0; i < level_nmChunks; i++)
        level_chunk[i].tile += tileBase;
    for (i = 0; i < level_nmFrames; i++)
        if (level_frame[i].sound != -1)
            level_frame[i].sound += soundBase;

    assert(level_sequence[0] == 0);
    /* paranoia checks */
    assert(!(((sint32)level_sequence) & 3));
    assert(!(((sint32)level_frame) & 3));
    assert(!(((sint32)level_chunk) & 3));
    return 1;
}

sint16* level_wSequence;
sFrameType* level_wFrame;
sChunkType* level_wChunk;

sint32 loadWeaponSequences(void)
{
    sint32 size, i;
    sint32 level_nmSequences, level_nmFrames, level_nmChunks;
    sint8* buffer = weapon_seq_data;
    struct seqHeader* head;

    fs_read(&size, 4);

    size = FS_INT(&size);

    assert(size > 0 && size < sizeof(weapon_seq_data));
    fs_read(buffer, size);

    head = (struct seqHeader*)buffer;

    head->nmSequences = FS_INT(&head->nmSequences);
    head->nmFrames = FS_INT(&head->nmFrames);
    head->nmChunks = FS_INT(&head->nmChunks);

    level_nmSequences = head->nmSequences;
    level_nmFrames = head->nmFrames;
    level_nmChunks = head->nmChunks;

    assert(level_nmSequences >= 0);
    assert(level_nmFrames >= 0);
    assert(level_nmChunks >= 0);

    level_wFrame = (sFrameType*)(buffer + sizeof(struct seqHeader));

    for (i = 0; i < level_nmFrames; i++)
    {
        sFrameType *v = level_wFrame + i;
        v->chunkIndex = FS_SHORT(&v->chunkIndex);
        v->flags = FS_SHORT(&v->flags);
        v->sound = FS_SHORT(&v->sound);
    }

#ifndef NDEBUG
    {
        sint32 i;
        for (i = 0; i < level_nmFrames; i++)
        {
            assert(level_wFrame[i].pad[0] == 0);
            assert(level_wFrame[i].pad[1] == 0);
        }
    }
#endif

    level_wChunk = (sChunkType*)(level_wFrame + level_nmFrames);

    for (i = 0; i < level_nmChunks; i++)
    {
        sChunkType *v = level_wChunk + i;

        v->chunkx = FS_SHORT(&v->chunkx);
        v->chunky = FS_SHORT(&v->chunky);
        v->tile = FS_SHORT(&v->tile);
    }

    level_wSequence = (sint16*)(level_wChunk + level_nmChunks);

    for (i = 0; i < level_nmSequences; i++)
    {
        level_wSequence[i] = FS_SHORT(&level_wSequence[i]);
    }

    assert(level_wSequence[0] == 0);
    /* paranoia checks */
    assert(!(((sint32)level_wSequence) & 3));
    assert(!(((sint32)level_wFrame) & 3));
    assert(!(((sint32)level_wChunk) & 3));
    return 1;
}

#define QSIZE 8
typedef struct
{
    sint32 seq, seqWith;
    sint32 centerx, centery;
} QType;

static QType sequenceQ[QSIZE];
static sint32 qHead, qTail;
static sint32 frame;
static sint32 clock;
static sint32 sequenceOver;

void initWeaponQ(void)
{
    sequenceQ[0].seq = -1;
    qHead = 0;
    qTail = 0;
    frame = 0;
    clock = 0;
    sequenceOver = 1;
}

void queueWeaponSequence(sint32 seqNm, sint32 cx, sint32 cy)
{
    qHead = (qHead + 1) & 0x7;
    assert(qHead != qTail);
    sequenceQ[qHead].seq = seqNm;
    sequenceQ[qHead].centerx = cx;
    sequenceQ[qHead].centery = cy;
    sequenceQ[qHead].seqWith = -1;
    sequenceOver = 0;
}

void addWeaponSequence(sint32 seqNm)
{
    sequenceQ[qHead].seqWith = seqNm;
}

sint32 getWeaponSequenceQSize(void)
{
    return (qHead - qTail) & 0x7;
}

sint32 getCurrentWeaponSequence(void)
{
    return sequenceQ[qTail].seq;
}

sint32 weaponSequenceQEmpty(void)
{
    return sequenceOver;
}

sint32 getWeaponFrame(void)
{
    return frame;
}

void setWeaponFrame(sint32 f)
{
    frame = f;
}

/* returns cumulative OR of the flags of the new frames passed and displayed */
sint32 advanceWeaponSequence(sint32 xbase, sint32 ybase, sint32 hack)
{
    //sint32 j;
    sint32 gframe, chunk;
    sint32 sequence, flip;
    sint32 VDP2PicOn;
    sint32 retVal = 0;
    sint32 sound;
    sint32 overlay;
    sint32 xo, yo;
    static sint32 loadedPal = 0;

    XyInt pos;

    sequence = sequenceQ[qTail].seq;
    if (sequence < 0)
    {
        if (qTail != qHead)
        {
            qTail = (qTail + 1) & 0x7;
            sequence = sequenceQ[qTail].seq;
            frame = 0;
            retVal |= level_wFrame[frame + level_wSequence[sequence]].flags;
        }
        else
        {
            frame = 0;
            return 0;
        }
    }
    clock--;
    while (clock < 0)
    {
        clock += 2;
        frame++;
        if (frame + level_wSequence[sequence] >= level_wSequence[sequence + 1])
        {
            if (qTail != qHead)
            {
                qTail = (qTail + 1) & 0x7;
                sequence = sequenceQ[qTail].seq;
                if (sequence < 0)
                    return retVal;
                assert(sequence >= 0);
                frame = 0;
            }
            else
            {
                frame = 0;
                /*frame=level_wSequence[sequence+1];*/
                sequenceOver = 1;
            }
        }
        if (sequence < 0)
            continue;
        retVal |= level_wFrame[frame + level_wSequence[sequence]].flags;

        sound = level_wFrame[frame + level_wSequence[sequence]].sound;
        if (sound != -1)
            playSound(0, sound);
    }

    if (sequence < 0)
        return retVal;

    xo = xbase + sequenceQ[qTail].centerx;
    yo = ybase + sequenceQ[qTail].centery;

    VDP2PicOn = 0;
    overlay = 0;
    if (sequence >= 50)
    {
        overlay = 0x4000;
    }

    {
        XyInt pos[2];
        pos[0].x = 0;
        pos[0].y = 0;
        pos[1].x = 320;
        pos[1].y = 210;
        EZ_userClip(pos);
    }

    while (1)
    {
        gframe = frame + level_wSequence[sequence];
        for (chunk = level_wFrame[gframe].chunkIndex; chunk < level_wFrame[gframe + 1].chunkIndex; chunk++)
        {
            sChunkType* c = level_wChunk + chunk;
            if (getPicClass(c->tile) == TILEVDP)
            {
#ifdef TODO // weapon palette
                uint16* colorRam = (uint16*)SCL_COLRAM_ADDR;
                SCL_SET_N0CAOS(0);
                if (sequence >= 30 && sequence < 35)
                {
                    if (loadedPal != 1)
                    {
                        for (j = 0; j < 256; j++)
                            colorRam[(NMOBJECTPALLETES + 1) * 256 + j] = ((uint16*)grenadePal)[j];
                        loadedPal = 1;
                    }
                    SCL_SET_N0CAOS(6);
                }
                if (sequence >= 44 && sequence < 50)
                {
                    if (loadedPal != 2)
                    {
                        for (j = 0; j < 256; j++)
                            colorRam[(NMOBJECTPALLETES + 1) * 256 + j] = ((uint16*)manaclePal)[j];
                        loadedPal = 2;
                    }
                    SCL_SET_N0CAOS(6);
                }
#endif

#ifdef TODO // weapon draw
                displayVDP2Pic(c->tile, xo + c->chunkx, yo + c->chunky);
#endif
                VDP2PicOn = 1;
                if (hack)
                    break;
            }
            else
            {
                pos.x = xo - 320 / 2 + c->chunkx;
                pos.y = yo - 240 / 2 + c->chunky;
                flip = 0;
                if (c->flags & 1)
                    flip |= DIR_LRREV;
                if (c->flags & 2)
                    flip |= DIR_TBREV;
                assert(getPicClass(c->tile) == TILE8BPP);
                EZ_normSpr(flip, UCLPIN_ENABLE | COLOR_4 | HSS_ENABLE | ECD_DISABLE, overlay, mapPic(c->tile), &pos, NULL);
            }
        }
        if (sequenceQ[qTail].seqWith == -1 || sequence == sequenceQ[qTail].seqWith)
            break;
        sequence = sequenceQ[qTail].seqWith;
        overlay = 0x4000;
        if (hack)
            break;
    }
    if (!VDP2PicOn)
        delay_dontDisplayVDP2Pic();
    return retVal;
}

void clearWeaponQ(void)
{
    initWeaponQ();
}
