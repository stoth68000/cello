#if 0

#ifndef FRAMEOUTPUTNDI_H
#define FRAMEOUTPUTNDI_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <Processing.NDI.Lib.h>

#include "frameoutput.h"

class frameoutputndi : public frameoutput
{
public:
	frameoutputndi(frameinput *input);
	~frameoutputndi();

public:
	int hardware_open(int portnr);
	void hardware_close();

private:
	NDIlib_send_instance_t pNDI_send;
	NDIlib_video_frame_v2_t NDI_video_frame;

private:
	int threadRun();
	struct SwsContext *pSWS;
};

#endif /* FRAMEOUTPUTNDI_H */

#endif
