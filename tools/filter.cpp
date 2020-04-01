#include "filter.h"

filter::filter()
: framebase()
, threadbase((char *)"filter")
, name(NULL)
{
	buffersrc = avfilter_get_by_name("buffer");
	buffersink = avfilter_get_by_name("buffersink");
	outputs = avfilter_inout_alloc();
	inputs = avfilter_inout_alloc();
}

filter::~filter()
{
}

framequeue *filter::getQueue()
{
        return &q;
}

int filter::setName(const char *name)
{
	this->name = strdup(name);
	return 0;
}

const char *filter::getName()
{
	return name;
}

