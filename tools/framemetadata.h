#ifndef FRAMEMETADATA_H
#define FRAMEMETADATA_H

#include "frameprocessor.h"

struct frame_metadata_s
{
	uint32_t frameNr;
	struct timeval ts;
};

class framemetadata
{
public:
	framemetadata();
	~framemetadata();

	void setReadMetadataOnArrival(bool tf);
	bool getReadMetadataOnArrival(void);
	void setWriteMetadataOnArrival(bool tf);
	bool getWriteMetadataOnArrival(void);

	int getMetadata(uint32_t *frameNr, struct timeval *when);
	int setMetadata(uint32_t frameNr, struct timeval *when);

	struct frame_metadata_s *getSideDataAlloc();

private:
	int readMetadataOnArrival;
	int writeMetadataOnArrival;

        uint32_t metadataFrameNr;
        struct timeval metadataTimestamp;
};

#endif /* FRAMEMETADATA_H */
