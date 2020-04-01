#ifndef FRAMEFORMAT_H
#define FRAMEFORMAT_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "framebase.h"

/* This type ABOLSUTELY must match the static array of formattypes.
 * If you edit this, make sure you insert the correct entry into formattypes in .cpp
 * else the index ordinals will be off.
 */
typedef enum frameformat_e {
	FF_UNDEFINED = 0,
	FF_1280x720p5994,
	FF_1280x720p60,
	FF_1920x1080i2997,
	FF_MAX
} frameformat_t;

class frameformat
{
	frameformat();
	~frameformat();

	const char *getName(frameformat_t ff);
	int         getActiveWidth(frameformat_t ff);
	int         getActiveHeight(frameformat_t ff);
	int         isProgressive(frameformat_t ff);

	int         getFrameRateNum(frameformat_t ff);
	int         getFrameRateDen(frameformat_t ff);
	double      getFrameRate(frameformat_t ff);

};

#endif /* FRAMEFORMAT_H */
