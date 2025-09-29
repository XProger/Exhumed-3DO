#ifndef __INCLUDEDsequenceh
#define __INCLUDEDsequenceh

#include "slevel.h"

extern sint32 level_nmSequences;
extern sint32 level_nmFrames;
extern sint32 level_nmChunks;

extern sint16* level_sequence;
extern sFrameType* level_frame;
extern sChunkType* level_chunk;
extern sint16* level_sequenceMap;

/* sequences for the weapons */
extern sint16* level_wSequence;
extern sFrameType* level_wFrame;
extern sChunkType* level_wChunk;

sint32 loadSequences(sint32 fd, sint32 tileBase, sint32 soundBase);
sint32 loadWeaponSequences(sint32 fd);

void initWeaponQ(void);
sint32 weaponSequenceQEmpty(void);
sint32 getCurrentWeaponSequence(void);
sint32 getWeaponFrame(void);
void setWeaponFrame(sint32 f);
void queueWeaponSequence(sint32 seqNm, sint32 cx, sint32 cy);
sint32 advanceWeaponSequence(sint32 xbase, sint32 ybase, sint32 hack);
sint32 getWeaponSequenceQSize(void);
void addWeaponSequence(sint32 seqNm);
void clearWeaponQ(void);
#endif
