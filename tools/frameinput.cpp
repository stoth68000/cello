#include "frameinput.h"

#include "logger.h"
extern void *g_log;

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
	m_printIOMonitor = false;
	m_lastTrendUpdate = 0;
	m_trendCounter = 0;

	/* TODO: Hardcoded framerate */
	hires_av_init(&m_iomonitor, 60000.0, 1001.0, 48000.0);

	m_iotrend = kllineartrend_alloc(60 * 60 * 60, "SDI Input Frame Arrival - Video Drift Trend");
}

frameinput::~frameinput()
{
	kllineartrend_free(m_iotrend);
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

void frameinput::resetErrorCounts()
{
	m_lostFrames = 0;
	m_dupFrames = 0;
	m_lostCodes = 0;
	updateErrorTime();
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

void frameinput::frameArrived()
{
	hires_av_rx(&m_iomonitor, HIRES_AV_STREAM_VIDEO, 1);
	hires_av_tx(&m_iomonitor, HIRES_AV_STREAM_VIDEO, 1);

	if (m_printIOMonitor) {
		hires_av_summary_per_second(&m_iomonitor, 0);

		/* Once per minute, show the trend calculations for any drift between measured frame
		 * counts vs actual frames received. In other words, predict the SDI frame
		 * arrival times as a trend.
		 */
		time_t now = time(NULL);

		if (now >= (m_lastTrendUpdate + 60)) {
			m_lastTrendUpdate = now;

			m_trendCounter++;
			if (m_trendCounter > 1) {
				printf("Updating trend with %f\n", m_iomonitor.stream[HIRES_AV_STREAM_VIDEO].expected_actual_deficit_ms);
				kllineartrend_add(m_iotrend, m_trendCounter, m_iomonitor.stream[HIRES_AV_STREAM_VIDEO].expected_actual_deficit_ms);

				kllineartrend_printf(m_iotrend);

				double slope, intersect, deviation;
				kllineartrend_calculate(m_iotrend, &slope, &intersect, &deviation);
				//printf(" *******************                           Slope %15.5f Deviation is %12.2f\n", slope, deviation);
				// TODO: Write this into the cello log.
				cello_logger_printf(g_log, "frameinput:: ********** Slope %15.5f Deviation is %12.2f\n", slope, deviation);
			}
		}
	}
}
