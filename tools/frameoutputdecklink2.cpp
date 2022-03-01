#include "frameoutputdecklink2.h"
#include <sys/time.h>

#define LOCAL_DEBUG 1

void close_stream(AVFormatContext *oc, OutputStream *ost)
{
    avcodec_free_context(&ost->enc);
    av_frame_free(&ost->frame);
    av_frame_free(&ost->tmp_frame);
    sws_freeContext(ost->sws_ctx);
    swr_free(&ost->swr_ctx);
}

void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *c = ost->enc;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /* open the codec */
    //ret = avcodec_open2(c, codec, &opt);
    ret = avcodec_open2(c, codec, &opt_arg);
    av_dict_free(&opt);
    if (ret < 0) {
        fprintf(stderr, "Could not open video codec\n");
        exit(1);
    }

#if 0
    /* allocate and init a re-usable frame */
    ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!ost->frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    ost->tmp_frame = NULL;
    if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
        //ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
printf("Alloc RGBA\n");
        ost->tmp_frame = alloc_picture(AV_PIX_FMT_RGBA, c->width, c->height);
        if (!ost->tmp_frame) {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }
#endif

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(ost->st->codecpar, c);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}

/* Add an output stream. */
void add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id, int width, int height, AVRational timebase)
{
    AVCodecContext *c;
    int i;

    /* find the encoder */
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec)) {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }

    ost->st = avformat_new_stream(oc, NULL);
    if (!ost->st) {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }
    ost->st->id = oc->nb_streams-1;
    c = avcodec_alloc_context3(*codec);
    if (!c) {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    ost->enc = c;

    switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
        c->sample_fmt  = (*codec)->sample_fmts ?
            (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        c->bit_rate    = 64000;
        c->sample_rate = 44100;
        if ((*codec)->supported_samplerates) {
            c->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++) {
                if ((*codec)->supported_samplerates[i] == 44100)
                    c->sample_rate = 44100;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        c->channel_layout = AV_CH_LAYOUT_STEREO;
        if ((*codec)->channel_layouts) {
            c->channel_layout = (*codec)->channel_layouts[0];
            for (i = 0; (*codec)->channel_layouts[i]; i++) {
                if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    c->channel_layout = AV_CH_LAYOUT_STEREO;
            }
        }
        c->channels        = av_get_channel_layout_nb_channels(c->channel_layout);
        ost->st->time_base = (AVRational){ 1, c->sample_rate };
        break;

    case AVMEDIA_TYPE_VIDEO:
        c->codec_id = codec_id;

        c->bit_rate = 400000;
        /* Resolution must be a multiple of two. */
        c->width    = width;
        c->height   = height;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
         * of which frame timestamps are represented. For fixed-fps content,
         * timebase should be 1/framerate and timestamp increments should be
         * identical to 1. */
        ost->st->time_base = { timebase.den, timebase.num };
        c->time_base       = ost->st->time_base;

        c->gop_size      = 12; /* emit one intra frame every twelve frames at most */
        c->pix_fmt       = AV_PIX_FMT_YUV422P10;
        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
            /* just for testing, we also add B-frames */
            c->max_b_frames = 2;
        }
        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
            /* Needed to avoid using macroblocks in which some coeffs overflow.
             * This does not happen with normal video, it just happens here as
             * the motion of the chroma plane does not match the luma plane. */
            c->mb_decision = 2;
        }
    break;

    default:
        break;
    }

    /* Some formats want stream headers to be separate. */
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}


frameoutputdecklink2::frameoutputdecklink2(frameinput *input)
: frameoutput(input)
, codec(NULL)
, enc(NULL)
, ofmt_ctx(NULL)
, ofmt(NULL)
, video_codec(NULL)
, audio_codec(NULL)
, have_video(0)
, encode_video(0)
, have_audio(0)
, encode_audio(0)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif

	memset(&video_st, 0, sizeof(video_st));
	memset(&audio_st, 0, sizeof(audio_st));

	avdevice_register_all();
	//av_log_set_level(AV_LOG_TRACE);

	enc = avcodec_find_encoder(AV_CODEC_ID_V210);
	if (!enc) {
		fprintf(stderr, "unable to alloc encoder\n");
	}

}

frameoutputdecklink2::~frameoutputdecklink2()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

int frameoutputdecklink2::hardware_open(int portnr)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	char name[64];
	sprintf(name, "FrameSync Output #%d", portnr);
	setName(name);

	AVDictionary *opt = NULL;

	char devname[64];
	sprintf(devname, "DeckLink Duo (%d)", portnr + 1);
printf("Attempting to open .... %s\n", devname);
	int ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, "decklink", devname);
	if (ret < 0) {
		fprintf(stderr, "Unable to open output %s, ret = %d\n", devname, ret);
		return -1; 
	}

	ofmt = ofmt_ctx->oformat;

	/* Add the audio and video streams using the default format codecs
	 * and initialize the codecs. */
	if (ofmt->video_codec != AV_CODEC_ID_NONE) {
		//add_stream(&video_st, ofmt_ctx, &video_codec, ofmt->video_codec);
		add_stream(&video_st, ofmt_ctx, &video_codec, AV_CODEC_ID_V210, getWidth(), getHeight(), getTimebase());
		have_video = 1;
		encode_video = 1;
	}
#if 0
	if (ofmt->audio_codec != AV_CODEC_ID_NONE) {
		//add_stream(&audio_st, ofmt_ctx, &audio_codec, ofmt->audio_codec);
		have_audio = 0;
		encode_audio = 0;
	}
#endif

	/* Now that all the parameters are set, we can open the audio and
	 * video codecs and allocate the necessary encode buffers.
	 */
	if (have_video)
		open_video(ofmt_ctx, video_codec, &video_st, opt);

	av_dump_format(ofmt_ctx, 0, devname, 1);

	/* Write the stream header, if any. */
	av_dict_set(&opt, "preroll", "0.1", 0);
	ret = avformat_write_header(ofmt_ctx, &opt);
	if (ret < 0) {
		fprintf(stderr, "Error occurred writing output stream header\n");
		return 1;
	}

	threadStart();

#if LOCAL_DEBUG
	printf("%s() ok\n", __func__);
#endif
	return 0;
}

void frameoutputdecklink2::hardware_close()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	threadStop();
#if LOCAL_DEBUG
	printf("%s() thread stopped\n", __func__);
#endif

#if LOCAL_DEBUG
	printf("%s() done\n", __func__);
#endif
}

int frameoutputdecklink2::threadRun()
{
#if LOCAL_DEBUG
	printf("%s() decklink2 thread starting\n", __func__);
#endif
	uint32_t lastFrameNr = 0;

	int ret;
	while (!terminate) {
		ret = q.waitForFrame();
		if (ret < 0) {
			usleep(2 * 1000);
			continue;
		}
		if (q.count() < minQueueDepth)
			continue;

		AVFrame *frm;
		uint64_t id;
		ret = q.pop(&frm, &id);

		if (!playing) {
			av_frame_free(&frm);
			continue;
		}

		if (frm->format != AV_PIX_FMT_YUV422P10) {
			/* We only support output of 422P10 */
			av_frame_free(&frm);
			continue;
		}

		if (codec == NULL) {

			codec = avcodec_alloc_context3(enc);
			if (!codec) {
				fprintf(stderr, "unable to alloc codec\n");
				break;
			}
			codec->codec_type = AVMEDIA_TYPE_VIDEO;
			codec->codec_id = AV_CODEC_ID_V210;
			codec->bits_per_raw_sample = 10;
			codec->width = frm->width;
			codec->height = frm->height;
			codec->pix_fmt = AV_PIX_FMT_YUV422P10LE;
			codec->gop_size = 1;
			if (frm->width == 1920) {
				codec->time_base.den = getTimebaseDen();
			} else
			if (frm->width == 1280) {
				codec->time_base.den = getTimebaseDen();
			} else
			if (frm->width == 720) {
				codec->time_base.den = getTimebaseDen();
			} else {
				printf("%s() Not implemented\n", __func__);
				exit(1);
			}
			codec->time_base.num = getTimebaseDen();

			if (avcodec_open2(codec, enc, NULL) < 0) {
				fprintf(stderr, "unable to open2 codec\n");
			}
		}

		/* Decode the frame from avcodec 422p10le back to V210 */
		int decoded = 0;
		int ret = avcodec_send_frame(codec, frm);
		switch (ret) {
		case 0:
			decoded = 1; /* Success */
			break;
		case AVERROR(EINVAL):
			fprintf(stderr, "EINVAL, ret = %d\n", ret);
			break;
		case AVERROR(EAGAIN):
			fprintf(stderr, "EAGAIN, ret = %d\n", ret);
			break;
		case AVERROR(ENOMEM):
			fprintf(stderr, "ENOMEM, ret = %d\n", ret);
			break;
		default:
			fprintf(stderr, "unable to send avframe, ret = %d\n", ret);
		}
		if (!decoded) {
			fprintf(stderr, "No decoded packet yet\n");
			return S_OK;
		}

		AVPacket *pkt = av_packet_alloc();

		ret = avcodec_receive_packet(codec, pkt);
		if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
			/* Error, cleanup */
			av_packet_free(&pkt);
			av_frame_free(&frm);
			continue;
		}

		unsigned int burnval = (unsigned int)getFramesProcessed();
		if (encode_video &&
			(!encode_audio || av_compare_ts(video_st.next_pts, video_st.enc->time_base,
                                                        audio_st.next_pts, audio_st.enc->time_base) <= 0)) {

			/* Burn any V210 as necessary */
			if (getWriteMetadataOnArrival()) {
				/* Line 0 - Frame counter */
				/* Calculate the V210 stride as size / height */
				int stride = pkt->size / frm->height;

				V210_write_32bit_value((void *)pkt->data, stride, burnval, V210_BOX_HEIGHT * 0, 0);

				/* Validate the binary counter mechanism, read the data back */
				uint32_t v = V210_read_32bit_value((void *)pkt->data, stride, V210_BOX_HEIGHT * 0, 1);
				if (v != burnval) {
					printf("Bug: v %d burnval %d\n", v, burnval);
					exit(1);
				}

				/* Lines 1/2 - Walltime seconds and useconds */
				struct timeval ts;
				gettimeofday(&ts, NULL);
				V210_write_32bit_value((void *)pkt->data, stride, ts.tv_sec, V210_BOX_HEIGHT * 1, 0);
				V210_write_32bit_value((void *)pkt->data, stride, ts.tv_usec, V210_BOX_HEIGHT * 2, 0);
				V210_write_32bit_value((void *)pkt->data, stride, 0xA0018003, V210_BOX_HEIGHT * 3, 0);

				setMetadata(burnval, &ts);

				/* Validate the binary counter mechanism, read the data back */
				v = V210_read_32bit_value((void *)pkt->data, stride, V210_BOX_HEIGHT * 1, 1);
				if (v != ts.tv_sec) {
					printf("Bug: v %d tvsec %lu\n", v, ts.tv_sec);
					exit(1);
				}
				//printf("OUTPUT sec %lu v %d, frameNr %d\n", ts.tv_sec, v, burnval);
			}

			av_packet_rescale_ts(pkt, video_st.enc->time_base, video_st.st->time_base);
			pkt->stream_index = video_st.st->index;
			/* Write the compressed frame to the media file. */
			int cnt = getFramesProcessed();
			pkt->pts = cnt; /* libavformat doesn't actualy want real PTSs. It wants a frame counter. */
			pkt->dts = pkt->pts;
			av_interleaved_write_frame(ofmt_ctx, pkt);

			//write_frame(ofmt_ctx, &video_st.enc->time_base, video_st.st, pkt);
		}

		setLastRenderedFrameId(burnval);
		incrementFramesProcessed();

		/* Cleanup */
		av_frame_free(&frm);
		av_packet_free(&pkt);
	}
	av_write_trailer(ofmt_ctx);
	if (have_video)
		close_stream(ofmt_ctx, &video_st);
	if (have_audio)
		close_stream(ofmt_ctx, &audio_st);
	if (!(ofmt->flags & AVFMT_NOFILE))
		avio_closep(&ofmt_ctx->pb);

	avformat_free_context(ofmt_ctx);

	complete();
#if LOCAL_DEBUG
	printf("decklink output stopped\n");
#endif
	return 0;
}
