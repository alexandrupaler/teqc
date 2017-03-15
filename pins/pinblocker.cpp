#include "pins/pinblocker.h"

long pinblocker::getNextAvailableId()
{
	//mechanism similar to connection pool
	if(pinblocker::availableIds.size() == 0)
	{
		pinblocker::availableIds.push_back(pinblocker::maxId++);
	}

	//take the first available id
	long id = pinblocker::availableIds.back();
	pinblocker::availableIds.pop_back();

	return id;
}

vector<long> pinblocker::availableIds;
long pinblocker::maxId = 1;
