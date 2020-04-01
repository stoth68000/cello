#ifndef FRAMEINPUTDECKLINK2_H
#define FRAMEINPUTDECKLINK2_H

#include "frameinput.h"

extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

class frameinputdecklink2 : public frameinput
{
public:
	frameinputdecklink2();
	~frameinputdecklink2();

private:
	AVFormatContext   *ifmt_ctx;
	AVCodecContext    *codec;
	AVCodec           *dec;
	AVStream          *in_stream;
	AVCodecParameters *in_codecpar;

public:
	int hardware_open(int portnr);
	void hardware_close();

private:
	int threadRun();

	int zeroIdFrames;
};

#endif /* FRAMEINPUTDECKLINK2_H */
