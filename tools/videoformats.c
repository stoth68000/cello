#include "videoformats.h"

#define LOCAL_DEBUG 0

typedef uint32_t BMDDisplayMode;
enum _BMDDisplayMode {

    /* SD Modes */

    bmdModeNTSC                                                  = /* 'ntsc' */ 0x6E747363,
    bmdModeNTSC2398                                              = /* 'nt23' */ 0x6E743233,	// 3:2 pulldown
    bmdModePAL                                                   = /* 'pal ' */ 0x70616C20,
    bmdModeNTSCp                                                 = /* 'ntsp' */ 0x6E747370,
    bmdModePALp                                                  = /* 'palp' */ 0x70616C70,

    /* HD 1080 Modes */

    bmdModeHD1080p2398                                           = /* '23ps' */ 0x32337073,
    bmdModeHD1080p24                                             = /* '24ps' */ 0x32347073,
    bmdModeHD1080p25                                             = /* 'Hp25' */ 0x48703235,
    bmdModeHD1080p2997                                           = /* 'Hp29' */ 0x48703239,
    bmdModeHD1080p30                                             = /* 'Hp30' */ 0x48703330,
    bmdModeHD1080i50                                             = /* 'Hi50' */ 0x48693530,
    bmdModeHD1080i5994                                           = /* 'Hi59' */ 0x48693539,
    bmdModeHD1080i6000                                           = /* 'Hi60' */ 0x48693630,	// N.B. This _really_ is 60.00 Hz.
    bmdModeHD1080p50                                             = /* 'Hp50' */ 0x48703530,
    bmdModeHD1080p5994                                           = /* 'Hp59' */ 0x48703539,
    bmdModeHD1080p6000                                           = /* 'Hp60' */ 0x48703630,	// N.B. This _really_ is 60.00 Hz.

    /* HD 720 Modes */

    bmdModeHD720p50                                              = /* 'hp50' */ 0x68703530,
    bmdModeHD720p5994                                            = /* 'hp59' */ 0x68703539,
    bmdModeHD720p60                                              = /* 'hp60' */ 0x68703630,

    /* 2k Modes */

    bmdMode2k2398                                                = /* '2k23' */ 0x326B3233,
    bmdMode2k24                                                  = /* '2k24' */ 0x326B3234,
    bmdMode2k25                                                  = /* '2k25' */ 0x326B3235,

    /* Special Modes */

    bmdModeUnknown                                               = /* 'iunk' */ 0x69756E6B
};

struct videoFormat_s videoFormats[] =
{
	{ 1280,  720, 1,    50,    bmdModeHD720p50,       "1280x720p50" },
	{ 1280,  720, 1, 59.94,  bmdModeHD720p5994,    "1280x720p59.94" },
	{ 1280,  720, 1,    60,    bmdModeHD720p60,       "1280x720p60" },

	{ 1920, 1080, 0,    25,   bmdModeHD1080i50,      "1920x1080i25" },
	{ 1920, 1080, 0, 29.97, bmdModeHD1080i5994,   "1920x1080i29.97" },
	{ 1920, 1080, 0,    30, bmdModeHD1080i6000,      "1920x1080i30" },

	{ 1920, 1080, 1, 23.98, bmdModeHD1080p2398,   "1920x1080p23.98" },
	{ 1920, 1080, 1,    24,   bmdModeHD1080p24,      "1920x1080p24" },
	{ 1920, 1080, 1,    25,   bmdModeHD1080p25,      "1920x1080p25" },
	{ 1920, 1080, 1, 29.97, bmdModeHD1080p2997,      "1920x1080p29.97" },
	{ 1920, 1080, 1,    30,   bmdModeHD1080p30,      "1920x1080p30" },
	{ 1920, 1080, 1,    50,   bmdModeHD1080p50,      "1920x1080p50" },
	{ 1920, 1080, 1, 59.94, bmdModeHD1080p5994,    "1920x1080p5994" },
	{ 1920, 1080, 1,    60, bmdModeHD1080p6000,      "1920x1080p60" },
};

void dumpVideoFormat(const struct videoFormat_s *fmt)
{
	printf("%s() width %d height %d progressive %d fps %03.02f bmmode 0x%x name %s\n",
		__func__,
		fmt->width,
		fmt->height,
		fmt->progressive,
		fmt->fps,
		fmt->BMDDisplayMode,
		fmt->asciiDescription);
}

const struct videoFormat_s *findVideoFormatByDecklinkMode(uint32_t mode)
{
	for (int i = 0; i < sizeof(videoFormats) / sizeof(struct videoFormat_s); i++) {
		const struct videoFormat_s *fmt = &videoFormats[i];
		if (fmt->BMDDisplayMode == mode)
			return fmt;
	}
	return NULL;
}

const struct videoFormat_s *findVideoFormatByFormat(struct videoFormat_s *q)
{
	for (int i = 0; i < sizeof(videoFormats) / sizeof(struct videoFormat_s); i++) {
		const struct videoFormat_s *fmt = &videoFormats[i];
//		dumpVideoFormat(fmt);
		if (q->width != fmt->width)
			continue;
		if (q->height != fmt->height)
			continue;
		if (q->progressive != fmt->progressive)
			continue;
		if (q->fps != fmt->fps)
			continue;

		return fmt;
	}
	return NULL;
}

