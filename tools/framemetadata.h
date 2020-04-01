#ifndef FRAMEMETADATA_H
#define FRAMEMETADATA_H

#include "frameprocessor.h"

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
private:
	int readMetadataOnArrival;
	int writeMetadataOnArrival;

        uint32_t metadataFrameNr;
        struct timeval metadataTimestamp;
};

#endif /* FRAMEMETADATA_H */
