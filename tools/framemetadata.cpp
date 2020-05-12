#include "framemetadata.h"

framemetadata::framemetadata()
{
	setReadMetadataOnArrival(false);
	setWriteMetadataOnArrival(false);
}

framemetadata::~framemetadata()
{
}

void framemetadata::setReadMetadataOnArrival(bool tf)
{
	readMetadataOnArrival = tf;
}

bool framemetadata::getReadMetadataOnArrival(void)
{
	return readMetadataOnArrival;
}

void framemetadata::setWriteMetadataOnArrival(bool tf)
{
	writeMetadataOnArrival = tf;
}

bool framemetadata::getWriteMetadataOnArrival(void)
{
	return writeMetadataOnArrival;
}

int framemetadata::getMetadata(uint32_t *frameNr, struct timeval *when)
{
	if (!writeMetadataOnArrival) {
		if (!readMetadataOnArrival)
			return -1;
	}

	*frameNr = metadataFrameNr;
	*when = metadataTimestamp;

	return 0;
}

int framemetadata::setMetadata(uint32_t frameNr, struct timeval *when)
{
	if (!writeMetadataOnArrival)
		return -1;

	metadataFrameNr = frameNr;
	metadataTimestamp = *when;

	return 0;
}

struct frame_metadata_s *framemetadata::getSideDataAlloc()
{
	struct frame_metadata_s *data = (struct frame_metadata_s *)av_malloc(sizeof(*data));
	data->frameNr = metadataFrameNr;
	data->ts.tv_sec = metadataTimestamp.tv_sec;
	data->ts.tv_usec = metadataTimestamp.tv_usec;

	return data;
}

