#ifndef FRAMEPIPE_BURNWRITER_H
#define FRAMEPIPE_BURNWRITER_H

#include "framepipe.h"
#include "osd.h"

#define FPBW_MAX_MESSAGES 24

class framepipe_burnwriter : public framepipe
{
public:
	framepipe_burnwriter();
	~framepipe_burnwriter();

	void clearMessages();
	int addMessage(const char *msg, int colx, int coly);

protected:
	int transformInPlaceFrame(AVFrame *frm);

private:
	struct vc8x0_display_context dc;
	struct {
		bool enabled;
		int colx, coly;
		char msg[256];
	} msgs[FPBW_MAX_MESSAGES];
};

#endif /* FRAMEPIPE_BURNWRITER_H */
