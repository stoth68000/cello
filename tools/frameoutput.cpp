#include "frameoutput.h"
#include "frameinput.h"

frameoutput::frameoutput(frameinput *input)
: frameprocessor()
, framemetadata()
, threadbase((char *)"frame output")
, lastRenderedFrameId(0)
{
	minQueueDepth = 0;
	playing = 1;

	if (input) {
		setWidth(input->getWidth());
		setHeight(input->getHeight());
		setTimebase(input->getTimebase());
		setProgressive(input->getProgressive());
	}
}

frameoutput::~frameoutput()
{
}

uint64_t frameoutput::getLastRenderedFrameId()
{
	return lastRenderedFrameId;
}

void frameoutput::setLastRenderedFrameId(uint64_t id)
{
	lastRenderedFrameId = id;
}

int frameoutput::setMinimumQueueDepth(int count)
{
	minQueueDepth = count;
	return 0;
}

int frameoutput::getMinimumQueueDepth()
{
	return minQueueDepth;
}

int frameoutput::pause()
{
	playing = 0;
	return 0;
}

int frameoutput::play()
{
	playing = 1;
	return 0;
}
