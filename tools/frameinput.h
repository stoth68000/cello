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

private:
	bool m_losTerminate;

	int m_lostFrames;
	int m_dupFrames;
	int m_missingMetadata;
	int m_lostCodes;
};

#endif /* FRAMEINPUT_H */
