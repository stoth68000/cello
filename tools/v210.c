
#include <stdio.h>
#include <stdint.h>

#include "v210.h"

#define LOCAL_DEBUG 0

#define  y_white 0x3ff
#define  y_black 0x000
#define cr_white 0x200
#define cb_white 0x200

/* Six pixels */
uint32_t white[] = {
	 cr_white << 20 |  y_white << 10 | cb_white,
	  y_white << 20 | cb_white << 10 |  y_white,
	 cb_white << 20 |  y_white << 10 | cr_white,
	  y_white << 20 | cr_white << 10 |  y_white,
};

uint32_t black[] = {
	 cr_white << 20 |  y_black << 10 | cb_white,
	  y_black << 20 | cb_white << 10 |  y_black,
	 cb_white << 20 |  y_black << 10 | cr_white,
	  y_black << 20 | cr_white << 10 |  y_black,
};

/* KL paint 6 pixels in a single point */
__inline__ void V210_draw_6_pixels(uint32_t *addr, uint32_t *coloring)
{
	for (int i = 0; i < (V210_BOX_HEIGHT / 6); i++) {
		addr[0] = coloring[0];
		addr[1] = coloring[1];
		addr[2] = coloring[2];
		addr[3] = coloring[3];
		addr += 4;
	}
}

__inline__ void V210_draw_box(uint32_t *frame_addr, uint32_t stride, int color, int interlaced)
{
	uint32_t *coloring;
	if (color == 1)
		coloring = white;
	else
		coloring = black;

	int interleaved = interlaced ? 2 : 1;
	interleaved = 1;
	for (uint32_t l = 0; l < V210_BOX_HEIGHT; l++) {
		uint32_t *addr = frame_addr + ((l * interleaved) * (stride / 4));
		V210_draw_6_pixels(addr, coloring);
	}
}

__inline__ void V210_draw_box_at(uint32_t *frame_addr, uint32_t stride, int color, int x, int y, int interlaced)
{
	uint32_t *addr = frame_addr + (y * (stride / 4));
	addr += ((x / 6) * 4);
	V210_draw_box(addr, stride, color, interlaced);
}

void V210_write_32bit_value(void *frame_bytes, uint32_t stride, uint32_t value, uint32_t lineNr, int interlaced)
{
	for (int p = 31, sh = 0; p >= 0; p--, sh++) {
		V210_draw_box_at(((uint32_t *)frame_bytes), stride,
			(value & (1 << sh)) == (uint32_t)(1 << sh), p * V210_BOX_HEIGHT, lineNr, interlaced);
	}
}

uint32_t V210_read_32bit_value(void *frame_bytes, uint32_t stride, uint32_t lineNr, double scalefactor)
{
	double pixheight = V210_BOX_HEIGHT * scalefactor;
	double newlinenr = lineNr * scalefactor;

	int xpos = 0;
	uint32_t bits = 0;
	for (int i = 0; i < 32; i++) {
		xpos = (i * pixheight) + (pixheight / 2);
		/* Sample the pixel two lines deeper than the initial line, and eight pixels in from the left */
		uint32_t *addr = ((uint32_t *)frame_bytes) + (((int)newlinenr + 2) * (stride / 4));
		addr += ((xpos / 6) * 4);

		bits <<= 1;

		/* Sample the pixel.... Compressor will decimate, we'll need a luma threshold for production. */
		if ((addr[1] & 0x3ff) > 0x080)
			bits |= 1;
	}
#if LOCAL_DEBUG
	printf("%s(%p) = 0x%08x\n", __func__, frame_bytes, bits);
#endif
	return bits;
}
