#include "frameformat.h"

struct types_s
{
	frameformat_t ff;
	const char *name;
	int isProgressive;
	int activeWidth;
	int activeHeight;
	int rateN;
	int rateD;
	double frameRate;
} formattypes[] = {
	{ FF_UNDEFINED,      "undefined",       0,    0,    0,     0,    0, 0.0, },

	/* 1280 */
	{ FF_1280x720p5994,  "1280x720p59.94",  1, 1280,  720, 60000, 1001, 0.0, },
	{ FF_1280x720p60  ,  "1280x720p60",     1, 1280,  720, 60000, 1000, 0.0, },

	/* 1920 */
	{ FF_1920x1080i2997, "1920x1080i29.97", 0, 1920, 1080, 30000, 1001, 0.0, },
};

frameformat::frameformat()
{
	for (unsigned int i = 0; i < (sizeof(formattypes) / sizeof(struct types_s)); i++) {
		struct types_s *t = &formattypes[i];
		t->frameRate = (double)t->rateN / (double)t->rateD;
	}
}

frameformat::~frameformat()
{
}

int frameformat::getActiveWidth(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return 0;
	return formattypes[ff].activeWidth;
}

int frameformat::getActiveHeight(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return 0;
	return formattypes[ff].activeHeight;
}

int frameformat::isProgressive(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return 0;
	return formattypes[ff].isProgressive;
}

const char *frameformat::getName(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return NULL;
	return formattypes[ff].name;
}

int frameformat::getFrameRateNum(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return 0;
	return formattypes[ff].rateN;
}

int frameformat::getFrameRateDen(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return 0;
	return formattypes[ff].rateD;
}

double frameformat::getFrameRate(frameformat_t ff)
{
	if (ff >= FF_MAX)
		return 0;
	return formattypes[ff].frameRate;
}
