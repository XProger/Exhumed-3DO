#include <machine.h>
#include "sega_int.h"
#include "sega_mth.h"
#include "sega_sys.h"
#include "sega_per.h"
#include "file.h"
#include "util.h"

#include "slevel.h"
#include "sound.h"
#include "sruins.h"

static sint32 soundTop = 0; /* first unused byte of sound memory */
static uint8 soundFrame = 1;

struct soundSaveData
{
    sint32 owner;
    sint16 sound;
    sint16 pan, vol;
} soundSave[32];

static sint32 slotOwner[32]; /* -1 if empty */
static sint16 slotSound[32];
static sint8 slotDirty[32];

#define SCSP_REG_SET 0x0200
#define ADR_SCSP_REG ((uint8*)0x25b00400)

sint16 level_objectSoundMap[OT_NMTYPES];
sint16 level_staticSoundMap[ST_NMSTATICSOUNDGROUPS];

typedef struct
{
    sint32 startAddr;
    sint32 loopStart; /* or -1 if no loop */
    uint16 sampleRate;
    uint16 size;
    uint8 lastFrameUsed, bps;
} SoundRec;

#define MAXNMSOUNDS 80

SoundRec sounds[MAXNMSOUNDS];
static sint32 nmSounds;

#define SILENCE 8 /* must be power of 2 */
static sint8 silentQ[SILENCE];
static sint32 qHead, qTail;

void setMasterVolume(sint32 vol)
{
#ifdef TODO // sound
    POKE_W(SNDBASE + 0x100400, 0x0200 + (vol & 0xf));
    /* set dac18, mem4mb & master volume */
#endif
}

sint32 getSoundTop(void)
{
    return soundTop;
}

static void silenceVoice(void)
{
    static sint32 slot = 0;
    while (((qHead + 1) & (SILENCE - 1)) != qTail)
    { /* q has space for more silence */
        while (1)
        {
            slot = (slot + 1) & 0x1f;
            if (slot == 16 || slot == 17)
                continue;
            if (slotOwner[slot] != -1 && sounds[slotSound[slot]].loopStart != -1)
                continue;
            break;
        }
        silentQ[qHead] = slot;
        qHead = (qHead + 1) & (SILENCE - 1);
        POKE_W(0x20 * slot + SNDBASE + 0x100000 + 0, 0x1000);
        slotOwner[slot] = -1;
    }
}

void stopAllLoopedSounds(void)
{
    sint32 i;
    for (i = 0; i < 32; i++)
        if (sounds[slotSound[i]].loopStart != -1)
        {
#ifdef TODO // sound
            POKE_W(0x20 * i + SNDBASE + 0x100000, PEEK_W(0x20 * i + SNDBASE + 0x100000) & 0x7ff);
#endif
            /* set key off on all voices without disturbing other settings */
            slotOwner[i] = -1;
        }
#ifdef TODO // sound
    /* konex */
    POKE_W(SNDBASE + 0x100000 + 0, PEEK_W(0x20 * i + SNDBASE + 0x100000) | 0x1000);
#endif
}

void stopAllSound(sint32 source)
{
    sint32 i;
    for (i = 0; i < 32; i++)
    {
        if (slotOwner[i] == source)
        {
#ifdef TODO // sound
            POKE_W(0x20 * i + SNDBASE + 0x100000, PEEK_W(0x20 * i + SNDBASE + 0x100000) & 0x7ff);
#endif
            /* set key off on all voices without disturbing other settings */
            slotOwner[i] = -1;
        }
    }
    /* konex */
#ifdef TODO // sound
    POKE_W(SNDBASE + 0x100000 + 0, PEEK_W(0x20 * i + SNDBASE + 0x100000) | 0x1000);
#endif
}

void stopSound(sint32 source, sint32 sNm)
{
#ifdef TODO // sound
    sint32 i;
    for (i = 0; i < 32; i++)
    {
        if (slotOwner[i] == source && slotSound[i] == sNm)
        {
            POKE_W(0x20 * i + SNDBASE + 0x100000, PEEK_W(0x20 * i + SNDBASE + 0x100000) & 0x7ff);
            slotOwner[i] = -1;
            return;
        }
    }
    POKE_W(SNDBASE + 0x100000, PEEK_W(0x20 * i + SNDBASE + 0x100000) | 0x1000);
#endif
}

static sint32 deQVoice(void)
{
    sint32 retVal;
    assert(qHead != qTail);
    retVal = silentQ[qTail];
    qTail = (qTail + 1) & (SILENCE - 1);
    silenceVoice();
    return retVal;
}

#ifdef TODO // sound
static void initSlot(sint32 i)
{
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 2, 0x0000); /* start address */
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 4, 0x0000); /* loop start address */
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 6, 0x0000); /* loop end address */
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 8, 0x001f); /* adsr */
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x0a, 0x000f);
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x0c, 0x0000); /* direct out */
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x0e, 0x0000);
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x10, 0x0000);
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x12, 0x8000);
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x14, 0x0000);
    POKE_W(0x20 * i + SNDBASE + 0x100000 + 0x16, 0xe000);
}
#endif

void initSound(void)
{
#ifdef TODO // sound
    sint32 i;

    uint8* SMPC_SF = (uint8*)0x20100063;
    uint8* SMPC_COM = (uint8*)0x2010001f;

    /* aaaaaaaaaaaarrrrrrrrghhhhhhhhhh!!!!!!!! */
    while ((*SMPC_SF & 0x01) == 0x01)
        ;
    *SMPC_SF = 1;
    *SMPC_COM = 7; /*SMPC_SNDOFF;*/

    POKE_W(ADR_SCSP_REG, SCSP_REG_SET);

    while ((*SMPC_SF & 0x01) == 0x01)
        ;
#if 0
 *SMPC_SF=1;
 *SMPC_COM=6; /*SMPC_SNDON;*/
 /* must wait 30us for sound cpu to init */
#endif

    soundTop = 0;
    nmSounds = 0;

    setMasterVolume(15);
    /* initialize slots */
    for (i = 0; i < 32; i++)
    {
        POKE_W(0x20 * i + SNDBASE + 0x100000 + 0, 0x0000); /* koff */
        initSlot(i);
    }
    POKE_B(SNDBASE + 0x100000 + 0x217, 0xe0); /* make cd audible */
    POKE_B(SNDBASE + 0x100000 + 0x220, 0xe0); /* make cd audible */

    for (i = 0; i < 32; i++)
    {
        slotOwner[i] = -1;
        slotDirty[i] = 0;
    }

    qHead = 0;
    qTail = 0;
    silenceVoice();
    silenceVoice();
    silenceVoice();
    silenceVoice();
#endif
}

void sound_nextFrame(void)
{
    sint32 i;
    soundFrame++;
    if (!soundFrame)
    {
        soundFrame = 1;
        for (i = 0; i < nmSounds; i++)
            sounds[i].lastFrameUsed = 0;
    }
}

void loadSound(sint32 fd)
{
    sint32 size, rate, bps, loopStart;
    //sint32 i;
    //uint16* buffer;
    fs_read(fd, (sint8*)&size, 4);
    fs_read(fd, (sint8*)&rate, 4);
    fs_read(fd, (sint8*)&bps, 4);
    fs_read(fd, (sint8*)&loopStart, 4);

    size = FS_INT(&size);
    rate = FS_INT(&rate);
    bps = FS_INT(&bps);
    loopStart = FS_INT(&loopStart);
#ifdef TODO // sound
    buffer = (uint16*)mem_malloc(0, size);
    assert(buffer);
    fs_read(fd, (sint8*)buffer, size);
    assert(nmSounds < MAXNMSOUNDS);
    sounds[nmSounds].startAddr = soundTop;
    sounds[nmSounds].sampleRate = rate;
    sounds[nmSounds].size = size;
    sounds[nmSounds].bps = bps;
    sounds[nmSounds].loopStart = loopStart;
    sounds[nmSounds].lastFrameUsed = 0;
    nmSounds++;
    assert(soundTop + size < 1024 * 512);
    assert(!(soundTop & 1));
    for (i = 0; i < size / 2; i++)
    {
        POKE_W(SNDBASE + soundTop, buffer[i]);
        soundTop += 2;
    }
    mem_free(buffer);
#ifndef NDEBUG
    extraStuff = (512 * 1024 - soundTop) / 1024;
#endif

#else
    fs_skip(fd, size);
#endif
}

sint32 loadSoundSet(sint32 fd, sint16* out_map, sint32 mapSize)
{
    sint32 nmSounds, i;
    fs_read(fd, (sint8*)&nmSounds, 4);
    nmSounds = FS_INT(&nmSounds);
    assert(nmSounds == mapSize);
    fs_read(fd, (sint8*)out_map, 2 * mapSize);

    for (i = 0; i < mapSize; i++)
    {
        out_map[i] = FS_SHORT(out_map + i);
    }

    fs_read(fd, (sint8*)&nmSounds, 4);

    nmSounds = FS_INT(&nmSounds);

    assert(nmSounds >= 0 && nmSounds < MAXNMSOUNDS);
    for (i = 0; i < nmSounds; i++)
        loadSound(fd);
    return nmSounds;
}

void loadDynamicSounds(sint32 fd)
{
    sint32 i;
    sint32 nmStaticSounds = nmSounds;
    loadSoundSet(fd, level_objectSoundMap, OT_NMTYPES);
    for (i = 0; i < OT_NMTYPES; i++)
        level_objectSoundMap[i] += nmStaticSounds;
}

sint32 loadStaticSounds(sint32 fd)
{
    return loadSoundSet(fd, level_staticSoundMap, ST_NMSTATICSOUNDGROUPS);
}

#if 0
void testSound(void)
(sint32 i,j;
 for (i=0;i<nmSounds;i++)
    {dPrint("Testing sound %d\n",i);
     playSound(0,i);
     for (j=0;j<300;j++)
	SCL_DisplayFrame();
    }
}
#endif

void initSoundRegs(sint32 sNm, sint32 vol, sint32 pan, struct soundSlotRegister* ssr)
{
    assert(vol >= 0 && vol <= 255);
    assert(pan >= 0 && pan <= 31);
    if (!enable_stereo)
        pan = 0;
    ssr->reg[0] = 0x1800 | (sounds[sNm].startAddr >> 16);
    if (sounds[sNm].bps == 8)
        ssr->reg[0] |= 0x10;
    ssr->reg[1] = sounds[sNm].startAddr; /* start address */
    if (sounds[sNm].loopStart != -1)
    {
        ssr->reg[2] = sounds[sNm].loopStart; /* loop start */
        ssr->reg[0] |= 1 << 5; /* normal loop */
    }
    else
        ssr->reg[2] = 0x0000; /* loop start address */
    if (sounds[sNm].bps == 8)
        ssr->reg[3] = sounds[sNm].size;
    else
        ssr->reg[3] = sounds[sNm].size >> 1;
    ssr->reg[4] = 0x001f; /* adsr */
    ssr->reg[5] = 0x000f;
    ssr->reg[6] = vol;
    ssr->reg[7] = 0x0000;
    ssr->reg[8] = sounds[sNm].sampleRate;
    ssr->reg[9] = 0x8000;
    ssr->reg[10] = 0x0000;
    ssr->reg[11] = (7 << 13) | (pan << 8);
    ssr->soundNm = sNm;
}

struct soundSlotRegister* playSoundMegaE(sint32 source, struct soundSlotRegister* ssr)
{
    sint32 i, slot, base;
    slot = deQVoice();
    slotOwner[slot] = source;
    slotSound[slot] = ssr->soundNm;
    base = 0x20 * slot + SNDBASE + 0x100000;
    for (i = 1; i < 12; i++)
        POKE_W(base + (i << 1), ssr->reg[i]);
    POKE_W(base, ssr->reg[0]);
    slotDirty[slot] = 1;
    return (struct soundSlotRegister*)(base);
}

void playSoundE(sint32 source, sint32 sNm, sint32 vol, sint32 pan)
{
#ifdef TODO // sound
    sint32 slot, i;
    uint16 zeroReg;
    sint32 base;
    assert(sNm < nmSounds);
    assert(sNm >= 0);
    if (vol > 255)
        return;
    /* if sound is a looping sound, make sure that no other copies of it are
       playing */
    if (sounds[sNm].loopStart != -1)
    {
        for (i = 0; i < 32; i++)
            if (slotOwner[i] == source && slotSound[i] == sNm)
                return;
    }
    else
    {
        if (sounds[sNm].lastFrameUsed == soundFrame)
            return;
        sounds[sNm].lastFrameUsed = soundFrame;
    }

    if (!enable_stereo)
        pan = 0;

    slot = deQVoice();
    if (slotDirty[slot])
    {
        initSlot(slot);
        slotDirty[slot] = 0;
    }
    slotOwner[slot] = source;
    slotSound[slot] = sNm;
    base = 0x20 * slot + SNDBASE + 0x100000;
    POKE_W(base + 2, sounds[sNm].startAddr); /* start address */
    /* loop end address */
    if (sounds[sNm].bps == 8)
        POKE_W(base + 6, sounds[sNm].size);
    else
        POKE_W(base + 6, sounds[sNm].size >> 1);
    POKE_W(base + 0x10, sounds[sNm].sampleRate); /* not really the sample rate */

    assert(vol >= 0 && vol <= 255);
    assert(pan >= 0 && pan <= 31);
    POKE_W(base + 0x16, (7 << 13) | (pan << 8));
    POKE_W(base + 0x0c, vol);
    zeroReg = 0x1800 | (sounds[sNm].startAddr >> 16);

    if (sounds[sNm].loopStart != -1)
    {
        POKE_W(base + 0x4, sounds[sNm].loopStart); /* loop start */
        zeroReg |= 1 << 5; /* normal loop */
    }
    if (sounds[sNm].bps == 8)
        zeroReg |= 0x10;
    POKE_W(base, zeroReg); /* kon & ex */
#endif
}

void playSound(sint32 source, sint32 sNm)
{
    playSoundE(source, sNm, 0, 0);
}

enum
{
    S_KARNAK = 2,
    S_TRIBAL,
    S_SELKIS,
    S_SWAMP,
    S_ROCKIN,
    S_QUARRY,
    S_MAGMA,
    S_SANCTUM,
    S_ENDCREDIT,
    S_KILMAT1,
    S_KILMAT2,
    S_WATER2,
    S_MAP
};

static sint8 trackMap[] = { S_KARNAK, S_TRIBAL, S_QUARRY, S_KARNAK, S_SELKIS, S_WATER2, S_TRIBAL, S_ROCKIN, S_WATER2, S_SELKIS, S_MAGMA, S_ROCKIN, S_MAGMA, S_KILMAT1, S_QUARRY, S_ROCKIN, S_MAGMA, S_QUARRY, S_SWAMP, S_WATER2, S_TRIBAL, S_QUARRY, S_TRIBAL, S_KILMAT1, S_KILMAT1, S_KILMAT1, S_KILMAT1, S_KILMAT1, S_KILMAT1, S_KILMAT2, S_KARNAK };

sint8 mapMusic = S_MAP;
sint8 titleMusic = S_SANCTUM;
sint8 endMusic = S_ENDCREDIT;
sint8 firstVoiceTrack = S_MAP + 1;

void playCDTrackForLevel(sint32 lev)
{
    if (enable_music)
        playCDTrack(trackMap[lev], 1);
}

void posGetSoundParams(MthXyz* pos, sint32* vol, sint32* pan)
{
    sint32 angle;
    *vol = f(approxDist(pos->x - camera->pos.x, pos->y - camera->pos.y, pos->z - camera->pos.z));
    *vol = (*vol >> 5) - 15;
    if (*vol > 255)
        return;
    if (*vol < 0)
        *vol = 0;

    angle = getAngle(camera->pos.x - pos->x, camera->pos.z - pos->z);
    angle = -normalizeAngle(F(90) + angle - camera->angle);
    /* fold back half onto front half */
    if (angle > F(90))
        angle = F(180) - angle;
    if (angle < F(-90))
        angle = F(-180) - angle;
    *pan = ((uint32)(abs(angle))) >> 19;
    if (angle < 0)
        *pan |= 0x10;
}

void posMakeSound(sint32 source, MthXyz* pos, sint32 sndNm)
{
    sint32 vol, pan;
    assert(sndNm >= 0);
    posGetSoundParams(pos, &vol, &pan);
    if (vol > 255)
        return;
    playSoundE(source, sndNm, vol, pan);
}

void posAdjustSound(sint32 source, MthXyz* pos)
{
    sint32 vol, pan;
    posGetSoundParams(pos, &vol, &pan);
    if (vol > 255)
        return;
    adjustSounds(source, vol, pan);
}

void adjustSounds(sint32 source, sint32 vol, sint32 pan)
{
    sint32 s, base;
    for (s = 0; s < 32; s++)
        if (slotOwner[s] == source)
        {
            base = 0x20 * s + SNDBASE + 0x100000;
            POKE_W(base + 0x0c, vol);
            POKE_W(base + 0x16, (7 << 13) | (pan << 8));
        }
}

void saveSoundState(void)
{
    sint32 i;
    for (i = 0; i < 32; i++)
    {
        if (sounds[slotSound[i]].loopStart != -1)
        {
            sint32 base = 0x20 * i + SNDBASE + 0x100000;
            soundSave[i].owner = slotOwner[i];
            soundSave[i].sound = slotSound[i];
            soundSave[i].pan = (PEEK_W(base + 0x16) >> 8) & 0x1f;
            soundSave[i].vol = PEEK_W(base + 0x0c) & 0xff;
        }
        else
            soundSave[i].sound = -1;
    }
}

void restoreSoundState(void)
{
    sint32 i;
    for (i = 0; i < 32; i++)
    {
        if (soundSave[i].sound != -1)
        {
            playSoundE(soundSave[i].owner, soundSave[i].sound, soundSave[i].vol, soundSave[i].pan);
        }
        else
            soundSave[i].sound = -1;
    }
}
