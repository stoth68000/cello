#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "frameinputdecklink.h"
#include "frameinputdecklink2.h"
#include "frameoutputndi.h"
#include "frameoutputdecklink.h"
#include "frameoutputdecklink2.h"
#include "framepipe_burnwriter.h"
#include "filtervectorscope.h"

#include <list>
using namespace std;

std::list<class frameoutput *> m_outputs;
std::list<class frameinput *> m_inputs;
std::list<class filter *> m_filters;
pthread_t gtid = 0;

static time_t g_startupTime = 0;
static char g_startupTimeASCII[64];

struct thread_monitor_s
{
	int terminate;
	frameoutput *fo;
	frameinput *fi;
	framepipe_burnwriter *fpbw;
};

int _timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
     /* Perform the carry for the later subtraction by updating y. */
     if (x->tv_usec < y->tv_usec)
     {
         int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
         y->tv_usec -= 1000000 * nsec;
         y->tv_sec += nsec;
     }
     if (x->tv_usec - y->tv_usec > 1000000)
     {
         int nsec = (x->tv_usec - y->tv_usec) / 1000000;
         y->tv_usec += 1000000 * nsec;
         y->tv_sec -= nsec;
     }

     /* Compute the time remaining to wait. tv_usec is certainly positive. */
     result->tv_sec = x->tv_sec - y->tv_sec;
     result->tv_usec = x->tv_usec - y->tv_usec;

     /* Return 1 if result is negative. */
     return x->tv_sec < y->tv_sec;
}

uint64_t _timediff_to_msecs(struct timeval *tv)
{
	return (tv->tv_sec * 1000) + (tv->tv_usec / 1000);
}

static frameinput *findInputById(int id)
{
	list<class frameinput *>::iterator ei;
	frameinput *fi = NULL;

	int i = 0;
	for (ei = m_inputs.begin(); ei != m_inputs.end(); ++ei) {
		if (i++ == id) {
			fi = *ei;
			break;
		}
	}
	if (fi == NULL) {
		printf("No input found.\n");
	} else {
		printf("selected_input = %d '%s'\n", id, fi->getName());
	}
	return fi;
}

static frameoutput *findOutputById(int id)
{
	list<class frameoutput *>::iterator eo;
	frameoutput *fo = NULL;

	int i = 0;
	for (eo = m_outputs.begin(); eo != m_outputs.end(); ++eo) {
		if (i++ == id) {
			fo = *eo;
			break;
		}
	}
	if (fo == NULL) {
		printf("No output found.\n");
	} else {
		printf("selected_output = %d '%s'\n", id, fo->getName());
	}
	return fo;
}

static void *threadfunc(void *p)
{
	struct thread_monitor_s *ctx = (struct thread_monitor_s *)p;

	int colx = 8;
	time_t lastConsoleUpdate = 0;
	while (!ctx->terminate) {
		usleep(100 * 1000);

		int line = 6;

		uint32_t frameNr[3];
		struct timeval ts[3];
		ctx->fo->getMetadata(&frameNr[0], &ts[0]);
		ctx->fi->getMetadata(&frameNr[1], &ts[1]);

		frameNr[2] = frameNr[1] - frameNr[0];
		_timeval_subtract(&ts[2], &ts[0], &ts[1]);
		int64_t ms = _timediff_to_msecs(&ts[2]);

		ctx->fpbw->clearMessages();

		/* Basic I/O connection information */
		char msg[256], msg2[256], msg3[256], msg4[256];

		sprintf(msg2, "Cello Version: %s", GIT_VERSION);
		ctx->fpbw->addMessage(msg2, colx, line++);
		line++;

		/* Date / time */
		time_t now;
		time(&now);
		char str[128];
		sprintf(str, "%s", ctime(&now));
		str[ strlen(str) - 1] = 0;
		ctx->fpbw->addMessage(str, colx, line++);

		line++;

		/* Signal formats */
		sprintf(msg2, "    started: %s", g_startupTimeASCII);
		ctx->fpbw->addMessage(msg2, colx, line++);

		sprintf(msg2, "  -> output: %s", ctx->fo->humanFormatDescription());
		ctx->fpbw->addMessage(msg2, colx, line++);

		sprintf(msg2, "  <-  input: %s", ctx->fi->humanFormatDescription());
		ctx->fpbw->addMessage(msg2, colx, line++);

		int los = 0;
		if (frameNr[0] && frameNr[1] == 0) {
			los = 1;
		}

		if (ctx->fi->isMissingMetadata()) {
			los = 1;
		}

		if (!los)
			sprintf(msg, "    latency: %dms", ms);
		else
			sprintf(msg, "    latency: No signal");
		ctx->fpbw->addMessage(msg, colx, line++);

		line++;

		sprintf(msg2, " last error: %s", ctx->fi->getLastErrorTimeASCII());
		ctx->fpbw->addMessage(msg2, colx, line++);

		sprintf(msg2, "lost frames: %d", ctx->fi->getLostFrameCount());
		ctx->fpbw->addMessage(msg2, colx, line++);

		sprintf(msg3, " dup frames: %d", ctx->fi->getDuplicateFrameCount());
		ctx->fpbw->addMessage(msg3, colx, line++);

		sprintf(msg4, " lost codes: %d", ctx->fi->getLostCodesCount());
		ctx->fpbw->addMessage(msg4, colx, line++);

		if (ctx->fi->isMissingMetadata()) {
			line++;
			ctx->fpbw->addMessage("Metadata missing, check signal path", colx, line++);
		}

		if (now != lastConsoleUpdate) {
			lastConsoleUpdate = now;

			char ts[64];
			sprintf(ts, ctime(&now));
			ts[ strlen(ts) - 1] = 0;
			printf("%s: %s", ts, msg);
			printf("\t%s %s %s\n", msg2, msg3, msg4);
		}
	}

	return NULL;
}

static void usage()
{
	printf("-L <0|1> - Enable or disable process termination when decklink LOS condition occurs. [def: 1]\n");
	printf("           Used when you have a static workflow where the input frames are not expected to change format.\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	list<class frameinput *>::iterator ei;
	list<class frameoutput *>::iterator eo;
	list<class filter *>::iterator ef;
	frameoutput *fo = NULL;
	frameinput *fi = NULL;
	int selected_input = -1;
	int selected_output = -1;
	struct thread_monitor_s ctx;
	bool opt_losTerminate = true;

	printf("cello - a tool to measure SDI video latency\n");
	printf("version: %s\n", GIT_VERSION);
	printf("\n");

	int opt;
	while ((opt = getopt(argc, argv, "?hL:")) != -1) {
		switch (opt) {
		case 'L':
			opt_losTerminate = atoi(optarg);
			break;
		case '?':
		case 'h':
			usage();
			exit(0);
		default:
			usage();
			exit(0);
		}
	}

	time(&g_startupTime);
	sprintf(g_startupTimeASCII, "%s", ctime(&g_startupTime));
	g_startupTimeASCII[ strlen(g_startupTimeASCII) - 1] = 0;

	/* Decklink port #0 - Input - source material, we'll metadata stamp this and output it */
	frameinputdecklink2 *fid2 = new frameinputdecklink2();
	//fid2->setDebug(true);
	fid2->hardware_open(0);
	m_inputs.push_back(fid2);

	/* Decklink port #1 - Output to workflow - source material plus metadata/timestamps */
	frameoutputdecklink *fod2 = new frameoutputdecklink(fid2);
	fod2->setReadMetadataOnArrival(true);
	fod2->setWriteMetadataOnArrival(true);
	fod2->hardware_open(1);
	m_outputs.push_back(fod2);

	/* Framepipe connecting to decklink port #1 */
	/* Connected Decklink port #1 input to decklink port #3 output */
	framepipe *fp = new framepipe();
	fp->setSourceQueue( fid2->getQueue() );
	fp->addOutputQueue( fod2->getQueue() );
	fp->threadStart();

// while 1

	/* Decklink port #3 - Input - source material post workflow, we'll analyze the metadata */
	fid2 = new frameinputdecklink2();
	fid2->setDebug(true);
	fid2->setReadMetadataOnArrival(true);
	fid2->setWriteMetadataOnArrival(true);
	fid2->setLosTerminate(opt_losTerminate);
	fid2->hardware_open(3);
	m_inputs.push_back(fid2);

	ctx.terminate = 0;
	ctx.fo = fod2;
	ctx.fi = fid2;

	/* If we detect a resolution difference between the metadata output signal
	 * to the blackbox, and the returned SDI signal from the black box,
	 * calculate the scaling difference between the two and pass this
	 * into the input object, so it can correctly decode the metadata.
	 */
	double scalingfactor = (double)fid2->getWidth() / (double)fod2->getWidth();

	printf("o->i %.02f  fod2:%d / fid2:%d\n", scalingfactor, fod2->getWidth(), fid2->getWidth());

	/* Adjust the metadata reader if the input and output resolutions don't match */
	fid2->setFrameScaleFactor(scalingfactor);

	frameoutputdecklink2 *fod = new frameoutputdecklink2(fid2);
	fod->setReadMetadataOnArrival(false);
	fod->setWriteMetadataOnArrival(false);
	fod->hardware_open(2);
	m_outputs.push_back(fod);

	/* Connect the workflow output (out last input) to the frame analyzer */
	framepipe_burnwriter *fpbw = new framepipe_burnwriter();
	fpbw->setSourceQueue( fid2->getQueue() );
	fpbw->addOutputQueue( fod->getQueue() );
	fpbw->threadStart();

	ctx.fpbw = fpbw;
	if (gtid == 0) {
		printf("Creating monitoring thread\n");
		pthread_create(&gtid, 0, threadfunc, &ctx);
	}

	while (1) {
		char *line = readline("> ");
		add_history(line);

		if (strcasecmp(line, "pause") == 0) {
			for (eo = m_outputs.begin(); eo != m_outputs.end(); ++eo) {
				printf("Pausing %s\n", (*eo)->getName());
				(*eo)->pause();
			}
		}

		if (strcasecmp(line, "play") == 0) {
			for (eo = m_outputs.begin(); eo != m_outputs.end(); ++eo) {
				printf("Playing %s\n", (*eo)->getName());
				(*eo)->play();
			}
		}


		if (strcasecmp(line, "!") == 0) {
			system("/bin/bash");
		}

		if (strncasecmp(line, "sync with output ", 17) == 0) {
			int master_output = atoi(&line[17]);
			if (!fo) {
				printf("No current output active.\n");
				continue;
			}

			frameoutput *mo = findOutputById(master_output);
			if (!mo) {
				printf("No target output found.\n");
				continue;
			}

			uint64_t liveLastFrame = fo->getLastRenderedFrameId();
			uint64_t masterLastFrame = mo->getLastRenderedFrameId();

			int delayFrames = liveLastFrame - masterLastFrame;

			fo->setMinimumQueueDepth(delayFrames);

			printf("Syncing output %d/%" PRIu64 " to output %d by frameId %" PRIu64 ", delay %d frames\n",
				selected_output,
				liveLastFrame,
				master_output,
				masterLastFrame,
				delayFrames);

		}

		if (strncasecmp(line, "select output ", 14) == 0) {
			selected_output = atoi(&line[14]);
			fo = findOutputById(selected_output);
		}
		if (strncasecmp(line, "select input ", 13) == 0) {
			selected_input = atoi(&line[13]);
			fi = findInputById(selected_input);
		}

		if ((strcasecmp(line, "quit") == 0) || (strcasecmp(line, "q") == 0) || (strcasecmp(line, "exit") == 0))
			break;

		if (strcasecmp(line, "l") == 0) {
			if (!fi)
				continue;
			if (!fo)
				continue;

			ctx.terminate = 0;
			ctx.fo = fo;
			ctx.fi = fi;
			ctx.fpbw = fpbw; 
			if (gtid == 0) {
				printf("Creating monitoring thread\n");
				pthread_create(&gtid, 0, threadfunc, &ctx);
			}
		}

		if (strcasecmp(line, "s") == 0) {
			int i = 0;
			for (ei = m_inputs.begin(); ei != m_inputs.end(); ++ei) {
				printf("input[%d].framesProcessed = %" PRIu64 " '%s'\n", i++,
					(*ei)->getFramesProcessed(),
					(*ei)->getName());

				uint32_t frameNr;
				struct timeval ts;
				if ((*ei)->getMetadata(&frameNr, &ts) == 0) {
					printf("\t-> frameNr = %d, ts = %010lu.%06lu\n", frameNr, ts.tv_sec, ts.tv_usec);
				}

				AVRational tb = (*ei)->getTimebase();
				printf("\t-> width = %d, height = %d, timebase = { %d / %d }\n",
					(*ei)->getWidth(),
					(*ei)->getHeight(),
					tb.num, tb.den);
			}
			i = 0;
			for (eo = m_outputs.begin(); eo != m_outputs.end(); ++eo) {
				printf("output[%d].framesProcessed = %" PRIu64 " '%s' frameId = %" PRIu64 "\n", i++,
					(*eo)->getFramesProcessed(),
					(*eo)->getName(),
					(*eo)->getLastRenderedFrameId());

				uint32_t frameNr;
				struct timeval ts;
				if ((*eo)->getMetadata(&frameNr, &ts) == 0) {
					printf("\t-> frameNr = %d, ts = %010lu.%06lu\n", frameNr, ts.tv_sec, ts.tv_usec);
				}
				AVRational tb = (*eo)->getTimebase();
				printf("\t-> width = %d, height = %d, timebase = { %d / %d }\n",
					(*eo)->getWidth(),
					(*eo)->getHeight(),
					tb.num, tb.den);
			}
			i = 0;
			for (ef = m_filters.begin(); ef != m_filters.end(); ++ef) {
				printf("filter[%d].framesProcessed = %" PRIu64 " '%s'\n", i++,
					(*ef)->getFramesProcessed(),
					(*ef)->getName());
			}
		}
	}
	printf("Terminating, please wait.\n");

	ctx.terminate = 1;
	fp->threadStop();

	for (ei = m_inputs.begin(); ei != m_inputs.end(); ++ei) {
		(*ei)->hardware_close();
	}

	for (eo = m_outputs.begin(); eo != m_outputs.end(); ++eo) {
		(*eo)->hardware_close();
	}

	delete fp;
	//delete fon[0];
	//delete fon[1];
	//delete fid;

	return 0;
}
