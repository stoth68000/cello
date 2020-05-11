#include "frameinput.h"

frameinput::frameinput()
: frameprocessor()
, framemetadata()
, threadbase((char *)"frame input")
{
	m_losTerminate = true;
}

frameinput::~frameinput()
{
}

void frameinput::setLosTerminate(bool v)
{
	m_losTerminate = v;
}

bool frameinput::getLosTerminate()
{
	return m_losTerminate;
}
