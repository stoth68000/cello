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
	strcpy(m_lastErrorTimeASCII, "N/A");
	m_lastErrorTime = 0;
}

frameinput::~frameinput()
{
}

void frameinput::updateErrorTime()
{
	time(&m_lastErrorTime);
	sprintf(&m_lastErrorTimeASCII[0], "%s", ctime(&m_lastErrorTime));
	m_lastErrorTimeASCII[ strlen(m_lastErrorTimeASCII) - 1 ] = 0;
}

const char *frameinput::getLastErrorTimeASCII()
{
	return &m_lastErrorTimeASCII[0];
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
		updateErrorTime();
}

void frameinput::incDuplicateFrameCount()
{
        m_dupFrames++;
		updateErrorTime();
}

int frameinput::getLostCodesCount()
{
	return m_lostCodes;
}

void frameinput::setMissingMetadata(int n)
{
	if (m_missingMetadata == 0 && n == 1) {
		m_lostCodes++;
		updateErrorTime();
	}

	if (m_missingMetadata == 1 && n == 0) {
		m_dupFrames = 0;
		m_lostFrames = 0;
		updateErrorTime();
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
