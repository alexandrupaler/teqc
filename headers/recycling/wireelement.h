#ifndef WIREELEMENT_CPP_
#define WIREELEMENT_CPP_

#include <stdio.h>
#include <bitset>

#include "recycling/wireorder.h"

class wireelement
{
public:
	long threshold;
	wireelement* ngh[2];

	wireelement* prev;
	wireelement* next;

	wireelement* getUpdatedWire(long value);

	wireelement();

	void linkForward(wireelement* fwd);

	wireelement* cutForward();
	wireelement* cutBackward();

//	size_t number;
	long number;

//	wireorder* start;
//	wireorder* stop;

	std::bitset<100000> bits;
//	std::vector<bool> bits;
};


#endif /* WIREELEMENT_CPP_ */
