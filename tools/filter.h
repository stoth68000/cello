#ifndef FILTER_H
#define FILTER_H

#include "framebase.h"
#include "framequeue.h"
#include "threadbase.h"

class filter : public framebase, public threadbase
{
public:
	filter();
	~filter();

	framequeue *getQueue();

	const char *getName();
	int setName(const char *name);

	virtual int filter_open(int portnr) = 0;
	virtual void filter_close() = 0;

protected:
	char *name;

protected:
	/* Queue */
	framequeue q;

private:
	const AVFilter *buffersrc;
	const AVFilter *buffersink;
	AVFilterInOut *outputs;
	AVFilterInOut *inputs;
};

#endif /* FILTER_H */
