#ifndef FRAMEINPUTDECKLINK_H
#define FRAMEINPUTDECKLINK_H

#include "frameinput.h"

#include <DeckLinkAPI.h>

class frameinputdecklink : public frameinput, IDeckLinkInputCallback
{
public:
	frameinputdecklink();
	~frameinputdecklink();

public:
	//virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
	virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame *, IDeckLinkAudioInputPacket *);
	virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events,
		IDeckLinkDisplayMode *p_display_mode, BMDDetectedVideoInputFormatFlags);

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) {
		return E_NOINTERFACE;
	};

	virtual ULONG STDMETHODCALLTYPE AddRef(void) {
		uintptr_t new_ref;
		pthread_mutex_lock(&ref_mutex);
		new_ref = ++ref_;
		pthread_mutex_unlock(&ref_mutex);
		return new_ref;
	};

	virtual ULONG STDMETHODCALLTYPE Release(void) {
		uintptr_t new_ref;
		pthread_mutex_lock(&ref_mutex);
		new_ref = --ref_;
		pthread_mutex_unlock(&ref_mutex);
		if (new_ref == 0)
			delete this;
		return new_ref;
	};

private:
	int threadRun() { return 0; }

private:
	pthread_mutex_t ref_mutex;
	uintptr_t ref_;

private:
	IDeckLink *pCard;
	IDeckLinkInput *pInput;
	IDeckLinkConfiguration *pConfig;

	AVCodecContext *codec;
	AVCodec        *dec;

public:
	int hardware_open(int portnr);
	void hardware_close();

};

#endif /* FRAMEINPUTDECKLINK_H */
