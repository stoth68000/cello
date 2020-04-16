#ifndef V210_H
#define V210_H

#include <stdint.h>

#define V210_BOX_HEIGHT 12 /* must be a multiple of six (represents pixel width of each box drawn ) */

#ifdef __cplusplus
extern "C" {
#endif

/* KL paint 6 pixels in a single point */
__inline__ void V210_draw_6_pixels(uint32_t *addr, uint32_t *coloring);
__inline__ void V210_draw_box(uint32_t *frame_addr, uint32_t stride, int color, int interlaced);
__inline__ void V210_draw_box_at(uint32_t *frame_addr, uint32_t stride, int color, int x, int y, int interlaced);
void V210_write_32bit_value(void *frame_bytes, uint32_t stride, uint32_t value, uint32_t lineNr, int interlaced);

uint32_t V210_read_32bit_value(void *frame_bytes, uint32_t stride, uint32_t lineNr, double scalefactor);

#ifdef __cplusplus
};
#endif

#endif /* V210_H */
