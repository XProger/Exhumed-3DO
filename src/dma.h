#ifndef __INCLUDEDdmah
#define __INCLUDEDdmah

sint32 dmaActive(void);
void startNextDma(void);
void qDMA(void* from, void* to, sint32 size);
void initDMA(void);
void dmaMemCpy(void* from, void* to, sint32 size);

#endif
