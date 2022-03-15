#ifndef FRAMEOUTPUTDECKLINK_H
#define FRAMEOUTPUTDECKLINK_H

#include <DeckLinkAPI.h>

#include "frameoutput.h"

class frameoutputdecklink : public frameoutput, public IDeckLinkVideoOutputCallback, public IDeckLinkAudioOutputCallback
{
public:
	frameoutputdecklink(frameinput *input);
	~frameoutputdecklink();

public:
	int hardware_open(int portnr);
	void hardware_close();

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
        virtual ULONG STDMETHODCALLTYPE AddRef() { return 1; }
        virtual ULONG STDMETHODCALLTYPE Release() { return 1; }
        virtual HRESULT STDMETHODCALLTYPE ScheduledFrameCompleted(IDeckLinkVideoFrame* completedFrame, BMDOutputFrameCompletionResult result);
        virtual HRESULT STDMETHODCALLTYPE ScheduledPlaybackHasStopped();
        virtual HRESULT STDMETHODCALLTYPE RenderAudioSamples(bool preroll);

private:
	void ScheduleNextFrame(bool prerolling, IDeckLinkMutableVideoFrame *frame);
	void ScheduleNextPrerollFrame();

	int threadRun();

	IDeckLink *pCard;
	IDeckLinkOutput *pOutput;
	IDeckLinkConfiguration *pConfig;
	IDeckLinkDisplayMode *pDisplayMode;
	IDeckLinkVideoFrame *pFrame;
	//IDeckLinkMutableVideoFrame *referenceFrame;
	uint64_t m_totalFramesScheduled;

protected:
	int m_running;

private:
	AVCodecContext *codec;
	AVCodec        *enc;
};

#endif /* FRAMEOUTPUTDECKLINK_H */
