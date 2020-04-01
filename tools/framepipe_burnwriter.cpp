#include "framepipe_burnwriter.h"

#include <unistd.h>
#include <sys/time.h>

#define LOCAL_DEBUG 0

framepipe_burnwriter::framepipe_burnwriter()
: framepipe()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
	vc8x0_display_init(&dc);

	clearMessages();
}

framepipe_burnwriter::~framepipe_burnwriter()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
}

int framepipe_burnwriter::transformInPlaceFrame(AVFrame *frm)
{
	if (frm->format != AV_PIX_FMT_YUV422P10)
		return -1;

	av_frame_make_writable(frm);
	vc8x0_display_render_reset(&dc, frm->data[0], frm->width, frm->linesize[0]);
	for (int i = 0; i < FPBW_MAX_MESSAGES; i++) {
		if (!msgs[i].enabled)
			continue;
		vc8x0_display_render_string(&dc, msgs[i].msg, strlen(msgs[i].msg), msgs[i].colx, msgs[i].coly);
	}

	incrementFramesProcessed();

	return 0;
}

void framepipe_burnwriter::clearMessages()
{
	memset(&msgs[0], 0, sizeof(msgs));
}

int framepipe_burnwriter::addMessage(const char *msg, int colx, int coly)
{
	int ret = -1;
	for (int i = 0; i < FPBW_MAX_MESSAGES; i++) {
		if (msgs[i].enabled)
			continue;
		msgs[i].enabled = true;
		strcpy(msgs[i].msg, msg);
		msgs[i].colx = colx;
		msgs[i].coly = coly;
		return 0;
	}

	return ret;
}
