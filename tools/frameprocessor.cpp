#include "frameprocessor.h"

frameprocessor::frameprocessor()
: framebase()
, name(NULL)
, debug(0)
{
	width = - 1;
	height = -1;
	progressive = false;
	timebase = { 0 };
	frameScaleFactor = 1;
	asciiFormat[0] = 0;
}

frameprocessor::~frameprocessor()
{
}

framequeue *frameprocessor::getQueue()
{
	return &q;
}

int frameprocessor::setName(const char *name)
{
	this->name = strdup(name);
	return 0;
}

const char *frameprocessor::getName()
{
	return name;
}

const char *frameprocessor::humanFormatDescription()
{
	sprintf(asciiFormat, "%dx%d %03.02f(%c) { %d, %d }",
		width, height,
		(double)timebase.num / (double)timebase.den,
		progressive ? 'p' : 'i',
		timebase.num, timebase.den);
	return asciiFormat;
}

