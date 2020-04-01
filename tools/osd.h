//
//  OSD.h
//  DecklinkVideoGenerator
//
//  Created by Steven Toth on 7/11/12.
//  Copyright (c) 2012 Kernel Labs Inc. All rights reserved.
//

#ifndef DecklinkVideoGenerator_OSD_h
#define DecklinkVideoGenerator_OSD_h

#ifdef __cplusplus
extern "C" {
#endif

struct vc8x0_display_context
{
	unsigned char *frame, *ptr; /* top left of render image and a working ptr */
	int stride;
	int width;
    
	int plotwidth, plotheight;
    
	int currx, curry;
    
	/* Rendered font fg and bg colors */
	unsigned char bg[2], fg[2];
};

int vc8x0_display_init(struct vc8x0_display_context *ctx);
int vc8x0_display_render_reset(struct vc8x0_display_context *ctx, unsigned char *ptr, int width, long stride);
int vc8x0_display_render_string(struct vc8x0_display_context *ctx, const char *s, int len, int x, int y);

#ifdef __cplusplus
};
#endif

#endif
