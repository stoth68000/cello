#include "filtervectorscope.h"

#define LOCAL_DEBUG 1

filtervectorscope::filtervectorscope()
: filter()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

filtervectorscope::~filtervectorscope()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

int filtervectorscope::filter_open(int portnr)
{
#if LOCAL_DEBUG
	printf("%s(portnr=%d)\n", __func__, portnr);
#endif
	char name[64];
	sprintf(name, "Filter VectorScope #%d", portnr);
	setName(name);

	threadStart();

#if LOCAL_DEBUG
	printf("%s() ok\n", __func__);
#endif
	return 0;
}

void filtervectorscope::filter_close()
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

int filtervectorscope::threadRun()
{
printf("vectorscope thread starts\n");
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
#if 0
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

		NDIlib_send_send_video_v2(pNDI_send, &NDI_video_frame);
#endif
		incrementFramesProcessed();

		av_frame_free(&frm);
	}
printf("vectorscope thread stopping\n");
	complete();
	printf("vectorscope output stopped\n");
	return 0;
}
