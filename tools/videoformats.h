#ifndef VIDEOFORMATS_H
#define VIDEOFORMATS_H

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct videoFormat_s
{
	int width;
	int height;
	int progressive;
	double fps;
	unsigned int BMDDisplayMode;
	const char *asciiDescription;
};

void dumpVideoFormat(const struct videoFormat_s *fmt);

const struct videoFormat_s *findVideoFormatByDecklinkMode(uint32_t mode);

/* Find by width, height, progressive, fps */
const struct videoFormat_s *findVideoFormatByFormat(struct videoFormat_s *q);

#ifdef __cplusplus
};
#endif

#endif /* VIDEOFORMATS_H */
