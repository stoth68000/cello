#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

/* frameprocessor is a base class.
 * Its inherited to become frameinput or frameoutput new super classes.
 * decklink, NDI then inherit the frameinput frameoutput approrpiate superclass.
 *
 * A frameprocessor has a frame q.
 * frameinput classes deposit video frames into the q.
 * frameoutput classes extract video frames from the q.
 */

#include "framebase.h"
#include "framequeue.h"
#include "v210.h"

#include <unistd.h>

class frameprocessor : public framebase
{
public:
	frameprocessor();
	~frameprocessor();

	framequeue *getQueue();

	const char *getName();
	int setName(const char *name);

	void setDebug(bool tf) { debug = tf; };
	bool getDebug(void) { return debug; };

	virtual int hardware_open(int portnr) = 0;
	virtual void hardware_close() = 0;

	int getWidth() { return width; };
	int getHeight() { return height; };
	bool getProgressive() { return progressive; };
	AVRational getTimebase() { return timebase; };
	int getTimebaseNum() { return timebase.num; };
	int getTimebaseDen() { return timebase.den; };
	double getFPS() { return (double)timebase.num / (double)timebase.den; };

	void setFrameScaleFactor(double f) { frameScaleFactor = f; };
	double getFrameScaleFactor() { return frameScaleFactor; };

	const char *humanFormatDescription();

private:
	char *name;
	int debug;

	int width;
	int height;
	int progressive;
	AVRational timebase;

	double frameScaleFactor;

	char asciiFormat[128];

protected:
	/* Queue */
	framequeue q;

	void setWidth(int width) { this->width = width; };
	void setHeight(int height) { this->height = height; };
	void setProgressive(bool tf) { this->progressive = progressive; };
	void setTimebase(AVRational timebase) { this->timebase = timebase; };
};

#endif /* FRAMEPROCESSOR_H */
