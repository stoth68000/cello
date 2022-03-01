#if 0

#include "frameoutputndi.h"

#define LOCAL_DEBUG 0

frameoutputndi::frameoutputndi(frameinput *input)
: frameoutput(input)
, pSWS(NULL)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

frameoutputndi::~frameoutputndi()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

int frameoutputndi::hardware_open(int portnr)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	if (!NDIlib_initialize())
		return -1;

	NDIlib_send_create_t NDI_send_create_desc;

	char name[64];
	sprintf(name, "FrameSync Output #%d", portnr);
	setName(name);

	NDI_send_create_desc.p_ndi_name = getName();
	pNDI_send = NDIlib_send_create(&NDI_send_create_desc);

	NDIlib_metadata_frame_t NDI_connection_type;
	NDI_connection_type.p_data = (char *)"<ndi_product long_name=\"FrameSync Video Source.\" "
		"             short_name=\"FrameSync Source\" "
		"             manufacturer=\"Kernel Labs Inc.\" "
		"             version=\"1.000.000\" "
		"             session=\"default\" "
		"             model_name=\"M1\" "
		"             serial=\"123456789\"/>";

	
	NDIlib_send_add_connection_metadata(pNDI_send, &NDI_connection_type);
	NDI_video_frame.p_data = NULL;

	threadStart();

#if LOCAL_DEBUG
	printf("%s() ok\n", __func__);
#endif
	return 0;
}

void frameoutputndi::hardware_close()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	threadStop();
#if LOCAL_DEBUG
	printf("%s() thread stopped\n", __func__);
#endif

	free(NDI_video_frame.p_data);
	NDIlib_send_destroy(pNDI_send);
#if LOCAL_DEBUG
	printf("%s() done\n", __func__);
#endif
}

int frameoutputndi::threadRun()
{
#if LOCAL_DEBUG
	printf("NDI thread starts\n");
#endif
	int ret;
	while (!terminate) {
		ret = q.waitForFrame();
		if (ret < 0) {
			usleep(2 * 1000);
			continue;
		}
		AVFrame *frm;
		uint64_t id;
		ret = q.pop(&frm, &id);

		if (!playing) {
			av_frame_free(&frm);
			continue;
		}

		if (NDI_video_frame.p_data == NULL) {
			/* Initialize out NDI context based on incoming frame data, first time only. */
			NDI_video_frame.xres = frm->width;
			NDI_video_frame.yres = frm->height;
			NDI_video_frame.FourCC = NDIlib_FourCC_type_BGRX;
			if (frm->width == 1920) {
				NDI_video_frame.frame_rate_N = 30000;
				NDI_video_frame.frame_rate_D = 1001;
			} else
			if (frm->width == 1280) {
				NDI_video_frame.frame_rate_N = 60000;
				NDI_video_frame.frame_rate_D = 1001;
			} else {
				printf("Not supported.\n");
				exit(1);
			}
			NDI_video_frame.p_data = (uint8_t*)malloc(NDI_video_frame.xres * NDI_video_frame.yres * 4);
		}

		if (frm->format == AV_PIX_FMT_YUV422P10 && NDI_video_frame.FourCC == NDIlib_FourCC_type_BGRX) {
			/* On a Haswell, encoding the B&W flash is 30%, adding this CSC and encoding basketball, 52% */

			if (pSWS == NULL) {
				pSWS = sws_getContext(
					frm->width, frm->height, AV_PIX_FMT_YUV422P10, /* src */
					frm->width, frm->height, AV_PIX_FMT_BGRA, /* dst */
					0, /* flags */
					NULL, /* srcfilter */
					NULL, /* dstfilter */
					NULL /* double param */);
			}

			const uint8_t *const srcplane[] = { frm->data[0], frm->data[1], frm->data[2] };
			const int      srcstride[] = { frm->linesize[0], frm->linesize[1], frm->linesize[2] };

			uint8_t *p = NDI_video_frame.p_data;
			uint8_t *const dstplane[] = { p, 0, 0 };
			const int      dststride[] = { frm->width * 4, 0, 0 };

			ret = sws_scale(pSWS, srcplane, &srcstride[0], 0, frm->height, dstplane, &dststride[0]);
		} else {
			/* Fill the image with a B&W flash */
			static int val = 0;
			memset((void*)NDI_video_frame.p_data,
				(val++ & 1) ? 255 : 0,
				NDI_video_frame.xres * NDI_video_frame.yres * 4);
		}

		setLastRenderedFrameId(id);
		NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame);

		incrementFramesProcessed();

		av_frame_free(&frm);
	}
	complete();
#if LOCAL_DEBUG
	printf("NDI output stopped\n");
#endif
	return 0;
}
#endif

