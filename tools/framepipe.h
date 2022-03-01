#ifndef FRAMEPIPE_H
#define FRAMEPIPE_H

/* framepipes attach to a single source q, clone copies of
 * frames and put them on all of the output queues.
 * I tried initially using frame reference counting and had issues
 * related to _unref not correctly tidying up.
 * I switch back to non-referenced frames March 2022.
 */
#include "framequeue.h"
#include "threadbase.h"

class framepipe : public threadbase, public framebase
{
public:
	framepipe();
	~framepipe();

	int setSourceQueue(framequeue *q);
	int addOutputQueue(framequeue *q);
	framequeue *getOutputQueue(int nr);

protected:
	virtual int transformInPlaceFrame(AVFrame *frm) { return 0; };

private:
	pthread_mutex_t mutex;

private:
	int threadRun();

	framequeue *src;

#define FRAMEPIPE_MAX_DEST_COUNT 4
	int dstCount;
	framequeue *dst[FRAMEPIPE_MAX_DEST_COUNT];
};

#endif /* FRAMEPIPE_H */
