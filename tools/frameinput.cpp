#include "frameinput.h"

frameinput::frameinput()
: frameprocessor()
, framemetadata()
, threadbase((char *)"frame input")
{
	m_losTerminate = true;
	m_lostFrames = 0;
	m_dupFrames = 0;
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

int frameinput::getLostFrameCount()
{
        return m_lostFrames;
}

int frameinput::getDuplicateFrameCount()
{
        return m_dupFrames;
}

void frameinput::addLostFrameCount(int n)
{
        m_lostFrames += n;
}

void frameinput::incDuplicateFrameCount()
{
        m_dupFrames++;
}


