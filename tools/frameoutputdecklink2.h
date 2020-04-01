#ifndef FRAMEOUTPUTDECKLINK2_H
#define FRAMEOUTPUTDECKLINK2_H

#include <DeckLinkAPI.h>

#include "frameoutput.h"

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

typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

class frameoutputdecklink2 : public frameoutput
{
public:
	frameoutputdecklink2(frameinput *input);
	~frameoutputdecklink2();

public:
	int hardware_open(int portnr);
	void hardware_close();

private:
	int threadRun();

private:
	AVCodecContext  *codec;
	AVCodec         *enc;
	AVFormatContext *ofmt_ctx;
	AVOutputFormat  *ofmt;

	OutputStream video_st;
	OutputStream audio_st;
	AVCodec *video_codec;
	AVCodec *audio_codec;

	int have_video;
	int encode_video;
	int have_audio;
	int encode_audio;
};

#endif /* FRAMEOUTPUTDECKLINK2_H */
