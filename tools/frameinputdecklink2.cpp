#include "frameinputdecklink2.h"

#define LOCAL_DEBUG 0

frameinputdecklink2::frameinputdecklink2()
: frameinput()
{
	ifmt_ctx = NULL;
	in_stream = NULL;
	in_codecpar = NULL;

	avdevice_register_all();

	dec = avcodec_find_decoder(AV_CODEC_ID_V210);
	if (!dec) {
		fprintf(stderr, "unable to alloc decoder\n");
	}
	codec = avcodec_alloc_context3(dec);
	if (!codec) {
		fprintf(stderr, "unable to alloc codec\n");
	}

	if (avcodec_open2(codec, dec, NULL) < 0) {
		fprintf(stderr, "unable to open2 codec\n");
	}
}

frameinputdecklink2::~frameinputdecklink2()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	if (codec) {
		avcodec_close(codec);
		//av_free(codec);
	}
}

int frameinputdecklink2::hardware_open(int portnr)
{
#if LOCAL_DEBUG
	printf("%s(portnr=%d)\n", __func__, portnr);
#endif
	char name[64];
	sprintf(name, "FrameSync Input #%d", portnr);
	setName(name);

	AVInputFormat *inpF = av_find_input_format("decklink");

	AVDictionary *opts = NULL;
	av_dict_set(&opts, "raw_format", "yuv422p10", 0);
	//av_dict_set(&opts, "format_code", "hp59", 0);

	char devname[64];
	sprintf(devname, "DeckLink Duo (%d)", portnr + 1);
	int ret = avformat_open_input(&ifmt_ctx, devname, inpF, &opts);
	if (ret < 0) {
		fprintf(stderr, "Could not open input device '%s'", name);
		exit(1);
	}

	ret = avformat_find_stream_info(ifmt_ctx, 0);
	if (ret < 0) {
		fprintf(stderr, "Failed to retrieve input stream information");
		exit(1);
	}

	av_dump_format(ifmt_ctx, 0, devname, 0);
printf("Human name: %s\n", ifmt_ctx->iformat->long_name);

#if LOCAL_DEBUG
	printf("number of streams: %d\n", ifmt_ctx->nb_streams);
#endif

	for (unsigned int i = 0; i < ifmt_ctx->nb_streams; i++) {
		AVStream *is = ifmt_ctx->streams[i];
		in_codecpar = is->codecpar;
		if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
#if LOCAL_DEBUG
			printf("stream %d: VIDEO\n", i);
#endif
			in_stream = is;
			setWidth(in_codecpar->width);
			setHeight(in_codecpar->height);
			setTimebase(is->r_frame_rate);
			switch(in_codecpar->field_order) {
			case AV_FIELD_UNKNOWN:
			case AV_FIELD_PROGRESSIVE:
				setProgressive(true);
				break;
			default:
				setProgressive(false);
			}
			break;
		}
	}

	if (in_stream == NULL) {
		printf("No video stream found\n");
		return -1;
	}

	codec->width = in_codecpar->width;
	codec->height = in_codecpar->height;

    threadStart();

#if LOCAL_DEBUG
	printf("%s() ok\n", __func__);
#endif

	return 0; /* Success */
}

void frameinputdecklink2::hardware_close()
{
}

int frameinputdecklink2::threadRun()
{
#if LOCAL_DEBUG
	printf("decklink2 thread starts\n");
#endif
	uint32_t lastFrameNr = 0;
	while (!terminate) {
		AVPacket *pkt = av_packet_alloc();
		int ret = av_read_frame(ifmt_ctx, pkt);
		if (ret < 0)
			break;

		if (pkt->stream_index != in_stream->index) {
			av_packet_free(&pkt);
			continue;
		}
#if LOCAL_DEBUG
		printf("stream_index = %d, pts = %" PRIu64 ", size = %d\n",
			pkt->stream_index,
			pkt->pts, pkt->size);
#endif

		uint32_t frameNr = 0;
		struct timeval ts = { 0 };
		if (getReadMetadataOnArrival()) {
			int instride = 0;
			if (getWidth() == 1920) {
				instride = 5120; /* V210 Input stride */
			} else
			if (getWidth() == 1280) {
				instride = 3456; /* V210 Input stride 1280 */
			} else 
			if (getWidth() == 720) {
				instride = 2160; /* V210 Input stride 720 */
			} else {
				printf("Unsupported\n");
				exit(1);
			}

			double sf = getFrameScaleFactor();
			frameNr = V210_read_32bit_value(pkt->data, instride, V210_BOX_HEIGHT * 0 /* LineNr */, sf);
			ts.tv_sec = V210_read_32bit_value(pkt->data, instride, V210_BOX_HEIGHT * 1 /* LineNr */, sf);
			ts.tv_usec = V210_read_32bit_value(pkt->data, instride, V210_BOX_HEIGHT * 2 /* LineNr */, sf);
			uint32_t marker = V210_read_32bit_value(pkt->data, instride, V210_BOX_HEIGHT * 3 /* LineNr */, sf);

			if (marker != 0xA0018003) {
				setMissingMetadata(1);
			} else {
				setMissingMetadata(0);
			}

			if (lastFrameNr && lastFrameNr == frameNr) {
				incDuplicateFrameCount();
			}
			if (lastFrameNr == 0 && frameNr > 0) {
				lastFrameNr = frameNr - 1;
			} else
			if (lastFrameNr + 1 != frameNr) {
					addLostFrameCount(frameNr - lastFrameNr);
					lastFrameNr = frameNr;
			}
			lastFrameNr = frameNr;

#if 0
printf("input frameNr = %d, sec %lu, usec %lu\n", frameNr, ts.tv_sec, ts.tv_usec);
#endif

			if (frameNr == 0) {
				zeroIdFrames++;
				if (getLosTerminate() && zeroIdFrames >= (getFPS() * 2)) {
					printf("LOS on input\n");
					exit(1);
				}
			} else {
				zeroIdFrames = 0;
			}
		}

		int decoded = 0;
		ret = avcodec_send_packet(codec, (const AVPacket *)pkt);
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
			/* TODO, check leaks */
			fprintf(stderr, "unable to send avpacket, ret = %d\n", ret);
		}
		if (!decoded) {
			av_packet_free(&pkt);
			fprintf(stderr, "No decoded frame yet\n");
			continue;
		}

		/* Take a standard V210 pixel format, convert into 422-10bit planer. */
		AVFrame *frm = av_frame_alloc();

		ret = avcodec_receive_frame(codec, frm);
		if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
			av_frame_free(&frm);
			av_packet_free(&pkt);
			continue;
		}

		setMetadata(frameNr, &ts);

/* TODO: Put the metadata on the frame itself, don't use opaque, use side_data */
		//frm->side_data = (AVFrameSideData **)av_malloc(sizeof(AVFrameSideData *));
		//frm->nb_side_data = 1;
		//frm->side_data[0] = getSideDataAlloc();
#if 0
		frm->opaque = getSideDataAlloc();
#endif

		incrementFramesProcessed();
		av_packet_free(&pkt);

		/* Put this new frame on our output q. */
		q.push(frm, 0);

	}
	complete();
#if LOCAL_DEBUG
	printf("decklink2 output stopped\n");
#endif
	return 0;
}

