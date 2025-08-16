#ifndef __INCLUDEDdmah
#define __INCLUDEDdmah

int dmaActive(void);
void startNextDma(void);
void qDMA(void *from,void *to,int size);
void initDMA(void);
void dmaMemCpy(void *from,void *to,int size);

#endif
