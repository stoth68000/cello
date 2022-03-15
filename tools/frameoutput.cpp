#include "frameoutput.h"
#include "frameinput.h"

frameoutput::frameoutput(frameinput *input)
: frameprocessor()
, framemetadata()
, threadbase((char *)"frame output")
, lastRenderedFrameId(0)
, m_fifodepth(0xffff)
{
	minQueueDepth = 0;
	playing = 1;

	if (input) {
		setWidth(input->getWidth());
		setHeight(input->getHeight());
		setTimebase(input->getTimebase());
		setProgressive(input->getProgressive());
	}
	/* TODO: Hardcoded framerate */
	hires_av_init(&m_iomonitor, 60000.0, 1001.0, 48000.0);

}

frameoutput::~frameoutput()
{
}

uint32_t frameoutput::getFifoDepth()
{
	return m_fifodepth;
}

uint32_t frameoutput::getFifoDepthMS()
{
	double latency = m_fifodepth * getTimebaseMs();
	return latency;
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

void frameoutput::frameTransmit()
{
	hires_av_rx(&m_iomonitor, HIRES_AV_STREAM_VIDEO, 1);
	hires_av_tx(&m_iomonitor, HIRES_AV_STREAM_VIDEO, 1);
	//hires_av_summary_per_second(&m_iomonitor, 0);
}
