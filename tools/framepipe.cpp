#include "framepipe.h"

#include <unistd.h>

#define LOCAL_DEBUG 0

framepipe::framepipe()
: threadbase((char *)"frame pipe")
, src(NULL)
, dstCount(0)
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif

	for (int i = 0; i < FRAMEPIPE_MAX_DEST_COUNT; i++)
		dst[i] = NULL;

	pthread_mutex_init(&mutex, NULL);
}

framepipe::~framepipe()
{
#if LOCAL_DEBUG
	printf("%s()\n", __func__);
#endif
#if 0
	if (src)
		pthread_cond_broadcast(src);
	if (dst)
		pthread_cond_broadcast(dst);
#endif
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
#if LOCAL_DEBUG
	printf("%s() done\n", __func__);
#endif
}

framequeue *framepipe::getOutputQueue(int nr)
{
	if (nr >= dstCount)
		return NULL;

	return dst[nr];
}

int framepipe::setSourceQueue(framequeue *q)
{
	src = q;
	return 0;
}

int framepipe::addOutputQueue(framequeue *q)
{
	if (dstCount >= FRAMEPIPE_MAX_DEST_COUNT)
		return -1;

	dst[dstCount++] = q;

	return 0;
}

int framepipe::threadRun()
{
	int ret;
	while (!terminate) {
		if (!src) {
			usleep(2 * 1000);
			continue;
		}
		ret = src->waitForFrame();
		if (ret < 0) {
			usleep(2 * 1000);
			continue;
		}
		AVFrame *frm;
		uint64_t ident;
		ret = src->pop(&frm, &ident);

		ret = transformInPlaceFrame(frm);

		for (int i = 0; i < FRAMEPIPE_MAX_DEST_COUNT; i++) {
			if (dst[i]) {
				AVFrame *n = av_frame_alloc();
				av_frame_ref(n, frm);
				ret = dst[i]->push(n, ident);
			}
		}
		av_frame_unref(frm);
	}
	complete();

#if LOCAL_DEBUG
	printf("framepipe thread terminated\n");
#endif
	return 0;
}

