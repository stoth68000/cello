#ifndef FRAMEOUTPUT_H
#define FRAMEOUTPUT_H

#include "frameprocessor.h"
#include "framemetadata.h"
#include "threadbase.h"
#include "hires-av-debug.h"

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

	void frameTransmit();

	uint32_t getFifoDepth();
	uint32_t getFifoDepthMS();

private:
	uint64_t lastRenderedFrameId;
	struct hires_av_ctx_s m_iomonitor;

protected:
	int minQueueDepth;
	int playing;
	uint32_t m_fifodepth;
};

#endif /* FRAMEOUTPUT_H */
