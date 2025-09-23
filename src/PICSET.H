#ifndef __INCLUDEDpicseth
#define __INCLUDEDpicseth

uint8* loadPic(sint32 fd, sint32* width, sint32* height);
sint32 loadPicSet(sint32 fd, uint16** palletes, uint32** datas, sint32 maxNmPics);
void skipPicSet(sint32 fd);

#endif
