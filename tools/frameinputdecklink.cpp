#include "frameinputdecklink.h"

#include <dlfcn.h>
//#include <DeckLinkAPIDispatch.cpp>

#define LOCAL_DEBUG 1

frameinputdecklink::frameinputdecklink()
: frameinput()
, pCard(NULL)
, pInput(NULL)
, pConfig(NULL)
{
	pthread_mutex_init(&ref_mutex, NULL);

	dec = avcodec_find_decoder(AV_CODEC_ID_V210);
	if (!dec) {
		fprintf(stderr, "unable to alloc decoder\n");
	}
	codec = avcodec_alloc_context3(dec);
	if (!codec) {
		fprintf(stderr, "unable to alloc codec\n");
	}

	AVDictionary *opts = NULL;
	av_dict_set(&opts, "refcounted_frames", "1", 0);
	if (avcodec_open2(codec, dec, &opts) < 0) {
		fprintf(stderr, "unable to open2 codec\n");
	}
}

frameinputdecklink::~frameinputdecklink()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif

	if (codec) {
		avcodec_close(codec);
		//av_free(codec);
	}

#if LOCAL_DEBUG
	printf("%s() done\n", __func__);
#endif
}

int frameinputdecklink::hardware_open(int portnr)
{
#if LOCAL_DEBUG
	printf("%s(%d)\n", __func__, portnr);
#endif
	char name[64];
	sprintf(name, "FrameSync Input #%d", portnr);
	setName(name);

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
	printf("Found Decklink model %s, using  input port #%d\n", modelName, portnr);
	free((char *)modelName);

	if (pCard->QueryInterface(IID_IDeckLinkInput, (void**)&pInput) != S_OK) {
		fprintf(stderr, "unable to query link\n");
	}

	if (pCard->QueryInterface(IID_IDeckLinkConfiguration, (void**)&pConfig) != S_OK) {
		fprintf(stderr, "unable to query configuration\n");
	}

	if (pConfig->SetInt(bmdDeckLinkConfigVideoInputConnection, bmdVideoConnectionSDI) != S_OK) {
		fprintf(stderr, "unable to set link to SDI\n");
	}

	setWidth(1920);
	setHeight(1080);

	AVRational ab = { 30000, 1001 };
	setTimebase(ab);

	printf("Hardcoded for 720p\n");
	if (pInput->EnableVideoInput(bmdModeHD1080i5994, bmdFormat10BitYUV, bmdVideoInputEnableFormatDetection) != S_OK) {
	//if (pInput->EnableVideoInput(bmdModeHD720p5994, bmdFormat10BitYUV, bmdVideoInputEnableFormatDetection) != S_OK) {
	//if (pInput->EnableVideoInput(bmdModeHD1080p2398, bmdFormat10BitYUV, bmdVideoInputEnableFormatDetection) != S_OK) {
		fprintf(stderr, "unable to enable video input\n");
	}

	if (pInput->EnableAudioInput(48000, bmdAudioSampleType32bitInteger, 16) != S_OK) {
		fprintf(stderr, "unable to enable audio input\n");
	}

	pInput->SetCallback(this);

	if (pInput->StartStreams() != S_OK) {
		fprintf(stderr, "unable to start streams\n");
	}

#if LOCAL_DEBUG
	printf("%s() done\n", __func__);
#endif
	return 0; /* Success */
}

void frameinputdecklink::hardware_close()
{
	if (pInput) {
		pInput->StopStreams();
	}

	if (pConfig)
		pConfig->Release();
	if (pInput)
		pInput->Release();
	if (pCard)
		pCard->Release();
}

HRESULT STDMETHODCALLTYPE frameinputdecklink::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events,
	IDeckLinkDisplayMode *p_display_mode, BMDDetectedVideoInputFormatFlags)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	BMDDisplayMode mode_id = p_display_mode->GetDisplayMode();

	printf("%s(%s) detected '%c%c%c%c'\n", __func__, getName(),
		mode_id >> 24, mode_id >> 16, mode_id >> 8, mode_id);

	return S_OK;
}

HRESULT frameinputdecklink::VideoInputFrameArrived(IDeckLinkVideoInputFrame *videoFrame, IDeckLinkAudioInputPacket *audioFrame)
{
#if LOCAL_DEBUG
	printf("%s(video=%p, audio=%p)\n", __func__, videoFrame, audioFrame);
#endif

	if (!videoFrame) {
		return S_OK;
	}

	unsigned char *ptr;
	videoFrame->GetBytes((void **)&ptr);

	uint32_t frameNr = 0;
	struct timeval ts = { 0 };
	if (getReadMetadataOnArrival()) {
		/* assume no V210 scaling, so we'll pass a value of 1 (no scaling) */
		frameNr = V210_read_32bit_value(ptr, videoFrame->GetRowBytes(), V210_BOX_HEIGHT * 0 /* LineNr */, 1);
		ts.tv_sec = V210_read_32bit_value(ptr, videoFrame->GetRowBytes(), V210_BOX_HEIGHT * 1 /* LineNr */, 1);
		ts.tv_usec = V210_read_32bit_value(ptr, videoFrame->GetRowBytes(), V210_BOX_HEIGHT * 2 /* LineNr */, 1);
	}

	AVPacket *pkt = av_packet_alloc();
	pkt->data = (uint8_t *)ptr;
	pkt->size = videoFrame->GetRowBytes() * videoFrame->GetHeight();

	codec->width   = videoFrame->GetWidth();
	codec->height  = videoFrame->GetHeight();

	if (getDebug()) {
		printf("%s(portnr = %s) width = %lu, height = %lu, stride = %lu\n",
			__func__,
			getName(),
			videoFrame->GetWidth(), videoFrame->GetHeight(),
			videoFrame->GetRowBytes());
		if (getReadMetadataOnArrival()) {
			printf("%s(portnr = %s) frameNr = %012d, ts = %010ld.%06ld\n",
				__func__,
				getName(),
				frameNr,
				ts.tv_sec, ts.tv_usec);
		}
	}

	int decoded = 0;
	int ret = avcodec_send_packet(codec, (const AVPacket *)pkt);
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
		fprintf(stderr, "unable to allocate avframe, ret = %d\n", ret);
	}
	if (!decoded) {
		av_packet_free(&pkt);
		fprintf(stderr, "No decoded frame yet\n");
		return S_OK;
	}

	/* Take a standard V210 pixexl format, convert into 422-10bit planer. */
	AVFrame *frm = av_frame_alloc();

	ret = avcodec_receive_frame(codec, frm);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
		av_frame_free(&frm);
		return S_OK;
	}

	BMDTimeValue time, f;
	videoFrame->GetStreamTime(&time, &f, 27000000LL);

	frm->key_frame = 1;
	frm->pts = time;

	//avframe_dump_console(frm);
	setMetadata(frameNr, &ts);

	/* Drop frames if we're only looking at metadata */
//	if (getReadMetadataOnArrival()) {
//		av_frame_unref(frm);
//	} else {
		q.push(frm, frameNr);
//	}

	incrementFramesProcessed();

	av_packet_free(&pkt);

	return S_OK;
}
