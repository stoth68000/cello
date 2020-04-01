#ifndef FRAMEPIPE_H
#define FRAMEPIPE_H

/* framepipes attach to a single source q, reference count
 * the frames in the source q and make referenced copies
 * of those frames, adding those copies to any output q's
 * that have been attached to the class.
 * a 1-to-many relationship.
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
