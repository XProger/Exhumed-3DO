#ifndef __INCLUDEDpicseth
#define __INCLUDEDpicseth

uint8* loadPic(sint32* width, sint32* height);
sint32 loadPicSet(uint16** palletes, uint32** datas, sint32 maxNmPics);
void skipPicSet();

#endif
