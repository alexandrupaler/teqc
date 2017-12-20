#ifndef WIREORDER_H__
#define WIREORDER_H__

#include <set>
#include <vector>
#include <stdlib.h>

class wireorder
{
public:
	wireorder* prev;
	wireorder* next;

	size_t number;

	wireorder();

	void makeNext(wireorder* first, wireorder* last);

	std::set<wireorder*> areInIntervalStartingFromHere(wireorder* first, wireorder* last, wireorder* stop);

	std::vector<wireorder*> joinWithInterval(wireorder* thisEnd, wireorder* startInterval, wireorder* stopInterval);

	wireorder* cutFromNext();

	wireorder* cutFromPrev();

	void linkNext(wireorder* newNext);

	void linkPrev(wireorder* newPrev);

	bool operator< (wireorder const& rhs);
};

#endif
