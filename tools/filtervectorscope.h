#ifndef FILTERVECTORSCOPE_H
#define FILTERVECTORSCOPE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "filter.h"

class filtervectorscope : public filter
{
public:
	filtervectorscope();
	~filtervectorscope();

public:
	int filter_open(int portnr);
	void filter_close();

private:
	int threadRun();
};

#endif /* FILTERVECTORSCOPE_H */
