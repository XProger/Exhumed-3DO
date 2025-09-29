#ifndef VID_H
#define VID_H

#define VID_NO_CLUT 255

#define SPR_FLIP_V 0x0020 // == DIR_TBREV
#define SPR_FLIP_H 0x0010 // == DIR_LRREV

void vid_init(void);
void vid_resize(sint32 width, sint32 height);
void vid_clear(void);
void vid_blit(void);
void vid_origin(sint32 x, sint32 y);
void vid_tex_reset(void);
void vid_tex_set(sint32 tex_index, sint32 format, const void *data, sint32 width, sint32 height);
void vid_clut_set(sint32 clut_index, const uint16 *data, sint32 length);
void vid_sprite(sint32 *a, sint32 *c, uint16 color, sint32 dir, sint32 tex_index, sint32 clut_index);
void vid_poly(sint32 *points, uint16 *colors, sint32 tex_index, sint32 clut_index);

#endif
