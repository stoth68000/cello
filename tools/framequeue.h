#ifndef FRAMEQUEUE_H
#define FRAMEQUEUE_H

#include "framebase.h"

struct framequeue_item_s
{
	struct xorg_list list;

	uint64_t id; /* Burned in frame counter, guaranteed to be uniq. */
	AVFrame *frm;
};

class framequeue
{
public:
	framequeue();
	~framequeue();

public:
	int push(AVFrame *frm, uint64_t id);
	int pop(AVFrame **frm, uint64_t *id);
	int count();
	int waitForFrame();

private:
	pthread_mutex_t mutex;
	struct framequeue_item_s q;
	pthread_cond_t cv;
	int entries;
};

#endif /* FRAMEQUEUE_H */
