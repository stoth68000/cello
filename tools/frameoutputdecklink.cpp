#include "frameoutputdecklink.h"
#include <sys/time.h>

#include "videoformats.h"

#define LOCAL_DEBUG 0

frameoutputdecklink::frameoutputdecklink(frameinput *input)
: frameoutput(input)
, pCard(NULL)
, pOutput(NULL)
, pConfig(NULL)
, pDisplayMode(NULL)
, pFrame(NULL)
//, referenceFrame(NULL)
, m_totalFramesScheduled(0)
, m_running(1)
, codec(NULL)
, enc(NULL)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
//	pthread_mutex_init(&ref_mutex, NULL);
}

frameoutputdecklink::~frameoutputdecklink()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

int frameoutputdecklink::hardware_open(int portnr)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	char name[64];
	sprintf(name, "FrameSync Output #%d", portnr);
	setName(name);

	enc = avcodec_find_encoder(AV_CODEC_ID_V210);
	if (!enc) {
		fprintf(stderr, "unable to alloc encoder\n");
	}

	IDeckLinkIterator *pIter = CreateDeckLinkIteratorInstance();
	if (!pIter) {
		fprintf(stderr, "Unable to initialize decklink hardware.\n");
		return -1;
	}

	//IDeckLinkDisplayModeIterator *pDispIter = NULL;

	int r = E_FAIL;
	for (int i = 0; i <= portnr; i++) {
		if (pCard)
			pCard->Release();
		r = pIter->Next(&pCard);
		if (r != S_OK)
			break;
	}

	if (r != S_OK) {
		fprintf(stderr, "unable to find decklink port %d\n", portnr);
		return -1;
	}

	const char *modelName;
	pCard->GetModelName(&modelName);
	printf("Found Decklink model %s, using output port #%d\n", modelName, portnr);
	free((char *)modelName);

	if (pCard->QueryInterface(IID_IDeckLinkOutput, (void**)&pOutput) != S_OK) {
		fprintf(stderr, "unable to query link\n");
	}

	if (pCard->QueryInterface(IID_IDeckLinkConfiguration, (void**)&pConfig) != S_OK) {
		fprintf(stderr, "unable to query configuration\n");
	}

	if (pConfig->SetInt(bmdDeckLinkConfigVideoOutputConnection, bmdVideoConnectionSDI) != S_OK) {
		fprintf(stderr, "unable to set link to SDI\n");
	}
#if 0
	int bytesPerPixel = 4;
	if (pOutput->CreateVideoFrame(getWidth(), getHeight(), getWidth() * bytesPerPixel, bmdFormat10BitYUV,
		bmdFrameFlagDefault, &referenceFrame) != S_OK) {
		fprintf(stderr, "Failed to create video frame\n");
	}
#endif
	pOutput->SetScheduledFrameCompletionCallback(this);
	pOutput->SetAudioCallback(NULL);
	//if (pOutput->EnableVideoOutput(bmdModeHD1080i5994, bmdVideoOutputFlagDefault) != S_OK) {

	/* TODO: Improve this. Needs a lookup table for all supported modes and related declink modes. */
	struct videoFormat_s q;
	q.width = getWidth();
	q.height = getHeight();
	q.progressive = getProgressive();
	q.fps = getFPS();
	const struct videoFormat_s *fmt = findVideoFormatByFormat(&q);
	if (fmt == NULL) {
		dumpVideoFormat(&q);
		printf("TODO: decklink output lookup, fix me\n");
		exit(1);
	}

	if (pOutput->EnableVideoOutput(fmt->BMDDisplayMode, bmdVideoOutputFlagDefault) != S_OK) {
		fprintf(stderr, "Unable to enable output\n");
	}

	pOutput->StartScheduledPlayback(0, 100, 1.0);

	for (int i = 0; i < 7; i++)
		ScheduleNextPrerollFrame();

	threadStart();

#if LOCAL_DEBUG
	printf("%s() ok\n", __func__);
#endif
	return 0;
}

void frameoutputdecklink::hardware_close()
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

int frameoutputdecklink::threadRun()
{
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
				codec->time_base.den = getTimebaseNum();
			} else
			if (frm->width == 1280) {
				codec->time_base.den = getTimebaseNum();
			} else {
				printf("Not implemented\n");
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
			fprintf(stderr, "unable to allocate avpacket, ret = %d\n", ret);
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

		IDeckLinkMutableVideoFrame *referenceFrame = NULL;
		int bytesPerPixel = 4;
		if (pOutput->CreateVideoFrame(getWidth(), getHeight(), getWidth() * bytesPerPixel, bmdFormat10BitYUV,
			bmdFrameFlagDefault, &referenceFrame) != S_OK) {
			fprintf(stderr, "Failed to create video frame\n");
		}

		/* Extract the pixels with a different stride, write to the decklink output frame. */
		/* Take a 422-10bit planer and repacket into standard V210 pixel format, adjust the stride
		 * for the decklink output.
		 */
		uint8_t *ptr;
		referenceFrame->GetBytes((void **)&ptr);

		int instride = 0;
		if (getWidth() == 1920) {
			instride = 5120; /* V210 Input stride */
		} else
		if (getWidth() == 1280) {
			instride = 3456; /* V210 Input stride 1280 */
		} else {
			printf("Unsupported\n");
			exit(1);
		}
		for (int i = 0; i < frm->height; i++) {
			uint8_t *dst = ptr + (i * referenceFrame->GetRowBytes());
			uint8_t *src = pkt->data + (i * instride);
			memcpy(dst, src, instride);
		}

		unsigned int burnval = (unsigned int)getFramesProcessed();
		if (getWriteMetadataOnArrival()) {
			/* Line 0 - Frame counter */
			V210_write_32bit_value((void *)ptr, referenceFrame->GetRowBytes(), burnval, V210_BOX_HEIGHT * 0, 0);

			/* Lines 1/2 - Walltime seconds and useconds */
			struct timeval ts;
			gettimeofday(&ts, NULL);
			V210_write_32bit_value((void *)ptr, referenceFrame->GetRowBytes(), ts.tv_sec, V210_BOX_HEIGHT * 1, 0);
			V210_write_32bit_value((void *)ptr, referenceFrame->GetRowBytes(), ts.tv_usec, V210_BOX_HEIGHT * 2, 0);
			V210_write_32bit_value((void *)ptr, referenceFrame->GetRowBytes(), 0xA0018003, V210_BOX_HEIGHT * 3, 0);

			setMetadata(burnval, &ts);
		}

		setLastRenderedFrameId(burnval);
#if 0
		printf("output frameNr = %d\n", burnval);
#endif
		ScheduleNextFrame(false, referenceFrame);
		incrementFramesProcessed();

		/* Cleanup */
		av_frame_free(&frm);
		av_packet_free(&pkt);
	}
	complete();
#if LOCAL_DEBUG
	printf("decklink output stopped\n");
#endif
	return 0;
}

HRESULT frameoutputdecklink::RenderAudioSamples(bool preroll)
{
	return E_FAIL;
}

HRESULT frameoutputdecklink::ScheduledPlaybackHasStopped()
{
	printf("%s()\n", __func__);
        return S_OK;
}

HRESULT frameoutputdecklink::ScheduledFrameCompleted(IDeckLinkVideoFrame *completedFrame, BMDOutputFrameCompletionResult result)
{
	completedFrame->Release();

	switch (result) {
		case bmdOutputFrameCompleted:
			break;
		case bmdOutputFrameDisplayedLate:
			printf("%s() frame displayed late\n", __func__);
			break;
		case bmdOutputFrameDropped:
			printf("%s() frame dropped\n", __func__);
			break;
		case bmdOutputFrameFlushed:
			printf("%s() frame flushed\n", __func__);
			break;
		default:
			printf("%s() default, check me result 0x%x\n", __func__, result);
	} 

	return S_OK;
}

void frameoutputdecklink::ScheduleNextPrerollFrame()
{
	IDeckLinkMutableVideoFrame *frame = NULL;

	int bytesPerPixel = 4;
	if (pOutput->CreateVideoFrame(getWidth(), getHeight(), getWidth() * bytesPerPixel, bmdFormat10BitYUV, bmdFrameFlagDefault, &frame) != S_OK) {
		fprintf(stderr, "Failed to create preroll video frame\n");
	}

	ScheduleNextFrame(true, frame);
}

void frameoutputdecklink::ScheduleNextFrame(bool prerolling, IDeckLinkMutableVideoFrame *frame)
{
	if (prerolling == false)
	{
		// If not prerolling, make sure that playback is still active
		if (m_running == false)
			return;
	}

	uint32_t v;
	pOutput->GetBufferedVideoFrameCount(&v);
	if (v <= 2) {
		printf("\t\t%d\n", v);
	}

	if (pOutput->ScheduleVideoFrame(frame, (m_totalFramesScheduled * getTimebaseDen()), getTimebaseDen(), getTimebaseNum()) != S_OK) {
		fprintf(stderr, "Send failed\n");
		return;
	}

	//printf("Frame send to hw\n");
	m_totalFramesScheduled += 1;
}
