#ifndef FRAMEINPUT_H
#define FRAMEINPUT_H

/*
 * frameinput abstracts a port on a SDI receiver device.
 * It:
 * has a thread.
 * configures the hardware.
 * collects the video from the hardware and creates AVFrames.
 * AVFrames are stored within a queue.
 * Places AVFrames on an internal list
 * Signals any input 'readers' that the receive list has been inpdated.
 */
#include "frameprocessor.h"
#include "framemetadata.h"
#include "threadbase.h"
#include "hires-av-debug.h"
#include "kl-lineartrend.h"

class frameinput : public frameprocessor, public framemetadata, public threadbase
{
public:
	frameinput();
	~frameinput();

	void setLosTerminate(bool v);
	bool getLosTerminate();

	int getLostFrameCount();
	int getDuplicateFrameCount();
	int getLostCodesCount();

	void addLostFrameCount(int n);
	void incDuplicateFrameCount();
	void setMissingMetadata(int n);
	int isMissingMetadata();
	void resetErrorCounts();

	const char *getLastErrorTimeASCII();

	void frameArrived();

	void setPrintIOMonitor(bool v) { m_printIOMonitor = v; };

private:
	void updateErrorTime();

	time_t m_lastErrorTime = 0;
	char m_lastErrorTimeASCII[64];

	bool m_losTerminate;

	int m_lostFrames;
	int m_dupFrames;
	int m_missingMetadata;
	int m_lostCodes;
	struct hires_av_ctx_s m_iomonitor;
	bool m_printIOMonitor;

	/* Trending */
	double m_trendCounter;
	time_t m_lastTrendUpdate;
	struct kllineartrend_context_s *m_iotrend;
};

#endif /* FRAMEINPUT_H */
