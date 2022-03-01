#include "frameinput.h"

frameinput::frameinput()
: frameprocessor()
, framemetadata()
, threadbase((char *)"frame input")
{
	m_losTerminate = true;
	m_lostFrames = 0;
	m_dupFrames = 0;
	m_missingMetadata = 1;
	m_lostCodes = 0;
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

int frameinput::getLostCodesCount()
{
	return m_lostCodes;
}

void frameinput::setMissingMetadata(int n)
{
	if (m_missingMetadata == 0 && n == 1) {
		m_lostCodes++;
	}

	if (m_missingMetadata == 1 && n == 0) {
		m_dupFrames = 0;
		m_lostFrames = 0;
	}

    m_missingMetadata = n ? 1 : 0;
	
	if (m_missingMetadata) {
		m_lostCodes++;
	}
}

int frameinput::isMissingMetadata()
{
        return m_missingMetadata;
}
