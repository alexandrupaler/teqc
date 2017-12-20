#ifndef PINBLOCKER_H_
#define PINBLOCKER_H_

#include <vector>

#include "pins/pinblockjournalentry.h"

struct pinblocker
{
	int offset;
	int axis;
	int distance;
//	bool constant;

//	int priority;

	pinblockjournalentry journal;

	int blockId;
	int blockType;

	static std::vector<long> availableIds;
	static long maxId;

	long getNextAvailableId();
};


#endif /* PINBLOCKER_H_ */
