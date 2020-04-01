#ifndef FRAMEBASE_H
#define FRAMEBASE_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
//#include <libavresample/avresample.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
};

#include "xorg-list.h"
#include "v210.h"
#include "frameformat.h"

void avframe_dump_console(AVFrame *frm);

class framebase
{
public:
	framebase();
	~framebase();

	uint64_t getFramesProcessed();
	void incrementFramesProcessed();

private:
	uint64_t framesProcessed;
};

#endif /* FRAMEBASE_H */
