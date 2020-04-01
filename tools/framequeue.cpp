#include "framequeue.h"

#include <unistd.h>
#include <sys/time.h>

framequeue::framequeue()
{
	pthread_mutex_init(&mutex, NULL);
	xorg_list_init(&q.list);
	pthread_cond_init(&cv, NULL);
	entries = 0;
}

framequeue::~framequeue()
{
	pthread_cond_broadcast(&cv);

	/* TODO: Erase the entire list. */
	AVFrame *frm;
	while (pop(&frm, NULL) == 0) {
		av_frame_free(&frm);
	}

	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cv);
}

int framequeue::push(AVFrame *frm, uint64_t id)
{
	struct framequeue_item_s *item = (struct framequeue_item_s *)malloc(sizeof(*item));
	if (!item)
		return -1;

	pthread_mutex_lock(&mutex);
	item->id = id;
	item->frm = frm;
	xorg_list_append(&item->list, &q.list);
	entries++;
//printf("%s() count = %d\n", __func__, entries);
	pthread_cond_signal(&cv);
	pthread_mutex_unlock(&mutex);

	return -1;
}

int framequeue::pop(AVFrame **frm, uint64_t *id)
{
	int ret = -1;
	*frm = NULL;
	if (id)
		*id = 0;

	struct framequeue_item_s *item = NULL;

	pthread_mutex_lock(&mutex);
	if (!xorg_list_is_empty(&q.list)) {
		item = xorg_list_first_entry(&q.list, struct framequeue_item_s, list);
		if (item) {
			xorg_list_del(&item->list);
			entries--;
//printf("%s() count = %d\n", __func__, entries);

			*frm = item->frm;
			if (id)
				*id = item->id;
		}
		ret = 0;
	}
	pthread_mutex_unlock(&mutex);

	return ret;
}

int framequeue::count()
{
	int ret;
	pthread_mutex_lock(&mutex);
	ret = entries;
	pthread_mutex_unlock(&mutex);
	return ret;
}

int framequeue::waitForFrame()
{
	struct timeval now;
	gettimeofday(&now, NULL);

	struct timespec to;
	to.tv_sec = now.tv_sec + 1;
	to.tv_nsec = now.tv_usec * 1000UL;

	int ret = 0;
	pthread_mutex_lock(&mutex);
	ret = pthread_cond_timedwait(&cv, &mutex, &to);
	if (ret != 0)
		ret = -1;
	pthread_mutex_unlock(&mutex);

	return ret;
}

