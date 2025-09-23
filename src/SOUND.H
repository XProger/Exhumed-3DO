#ifndef __INCLUDEDsoundh
#define __INCLUDEDsoundh

#define SNDBASE 0x05a00000

struct soundSlotRegister
{
    uint16 reg[12];
    sint32 soundNm;
};
void initSoundRegs(sint32 sNm, sint32 vol, sint32 pan, struct soundSlotRegister* ssr);
/* returns pointer to actual sound regs for aftertouch */
struct soundSlotRegister* playSoundMegaE(sint32 source, struct soundSlotRegister* ssr);

extern sint16 level_objectSoundMap[];
extern sint16 level_staticSoundMap[];

enum
{
    ST_JOHN,
    ST_ITEM,
    ST_FLAMER,
    ST_PUSHBLOCK,
    ST_INTERFACE,
    ST_RING,
    ST_BLOWPOT,
    ST_MANACLE,
    ST_NMSTATICSOUNDGROUPS
};

void initSound(void);
void loadSound(sint32 fd);
void loadDynamicSounds(sint32 fd);
sint32 loadStaticSounds(sint32 fd);
void playSound(sint32 source, sint32 sNm);
void playSoundE(sint32 source, sint32 sNm, sint32 vol, sint32 pan);

void stopSound(sint32 source, sint32 sNm);
void stopAllSound(sint32 source);
void stopAllLoopedSounds(void);
void adjustSounds(sint32 source, sint32 vol, sint32 pan);
sint32 getSoundTop(void);

extern sint8 mapMusic, titleMusic, endMusic, firstVoiceTrack;

void playCDTrackForLevel(sint32 lev);

#define playStaticSound(group, offs) playSound(0, level_staticSoundMap[group] + (offs))

#define stopStaticSound(group, offs) stopSound(0, level_staticSoundMap[group] + (offs))

void testSound(void);

#ifdef SEGA_MTH_H
void posGetSoundParams(MthXyz* pos, sint32* vol, sint32* pan);
void posMakeSound(sint32 source, MthXyz* pos, sint32 sndNm);
void posAdjustSound(sint32 source, MthXyz* pos);
#endif
void sound_nextFrame(void);
void setMasterVolume(sint32 vol);

void restoreSoundState(void);
void saveSoundState(void);
#endif
