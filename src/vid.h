#ifndef VID_H
#define VID_H

void vid_init(void);
void vid_resize(sint32 width, sint32 height);
void vid_clear(void);
void vid_blit(void);
void vid_center(sint32 x, sint32 y);
void vid_spr(sint32 *a, sint32 *c, uint16 color);
void vid_poly(sint32 *points, uint16 *colors);

#endif
