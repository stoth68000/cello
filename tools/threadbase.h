
#ifndef THREADBASE_H
#define THREADBASE_H

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#define THREADBASE_DEBUG 0

class threadbase {
public:
	threadbase(char *name) {
		has_started = 0;
		ok_complete = 0;
		terminate = 0;
		snprintf(this->name, sizeof(this->name), "%s", name);
	};

	virtual ~threadbase() {
	};

	void complete() {
		ok_complete = 1;
	};

	int completed() {
		return ok_complete ? 1 : 0;
	}

	void setThreadName(char *name) {
		snprintf(this->name, sizeof(this->name), "%s", name);
	};

	int threadStart() {
#if THREADBASE_DEBUG
		printf("Thread Starting [%s]\n", name);
#endif
		terminate = 0;
		ok_complete = 0;
#if 0
		pthread_attr_t tattr;
		pthread_attr_init(&tattr);
		pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
		int ret = pthread_create(&tid, &tattr, threadbase::helper, this);
#else
		int ret = pthread_create(&tid, 0, threadbase::helper, this);
#endif
		return ret;
	};

	int isThreadStarted() {
		if (has_started)
			return 1;
		else
			return 0;
	}

	virtual void threadStop() {
		// signal the thread, wait until stopped then exit
		terminate = 1;
		while (!ok_complete && has_started) {
			usleep(20 * 1000);
		}
		has_started = 0;
#if THREADBASE_DEBUG
		printf("Thread Stopped  [%s]\n", name);
#endif
	};

	static void *helper(void *arg) {
		threadbase *t = reinterpret_cast<threadbase*>(arg);

#ifdef __APPLE__
		pthread_setname_np(t->threadName());
#endif
#ifdef __linux__
		pthread_setname_np(t->tid, t->threadName());
#endif
		//signal(SIGPIPE, SIG_IGN);
		t->has_started = 1;
		t->threadRun();
		pthread_exit(0);
	};

	const char *threadName() {
		return name;
	};

private:
	virtual int threadRun() = 0;

private:
	pthread_t tid;
	int has_started;
	char name[256];

public:
	int terminate;
	int ok_complete;
};

#endif /* THREADBASE_H */

