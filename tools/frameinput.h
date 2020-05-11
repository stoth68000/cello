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

private:
	bool m_losTerminate;
};

#endif /* FRAMEINPUT_H */
