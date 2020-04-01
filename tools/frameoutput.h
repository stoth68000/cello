#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include "frameprocessor.h"
#include "framemetadata.h"
#include "threadbase.h"

class frameinput;

class frameoutput : public frameprocessor, public framemetadata, public threadbase
{
public:
	frameoutput(frameinput *input);
	~frameoutput();

	uint64_t getLastRenderedFrameId();
	void     setLastRenderedFrameId(uint64_t);

	int setMinimumQueueDepth(int count);
	int getMinimumQueueDepth();
	int pause();
	int play();

private:
	uint64_t lastRenderedFrameId;

protected:
	int minQueueDepth;
	int playing;

};

#endif /* FRAMEOUTPUT_H */
