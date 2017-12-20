#include <stdio.h>
#include <vector>
#include <algorithm>

#include "numberandcoordinate.h"
#include "convertcoordinate.h"

void numberandcoordinate::sortEntries()
{
	std::sort(inputList.begin(), inputList.end(), numberandcoordinate::compareWidthCooord);
}

void numberandcoordinate::addEntry(pinpair& numberAndCoords)
{
	int itype = numberAndCoords[TYPE];
	//increase the number of Astates by one or zero
	nrAStates += (1 - itype);
	nrYStates += itype;

	//compute min/max inj coordinate
	if (numberAndCoords[ROWCOORD] < minInjRow)
	{
		minInjRow = numberAndCoords[ROWCOORD];
	}
	if (numberAndCoords[ROWCOORD] > maxInjRow)
	{
		maxInjRow = numberAndCoords[ROWCOORD];
	}

	inputList.push_back(numberAndCoords);
}

int numberandcoordinate::size()
{
	return inputList.size();
}

int numberandcoordinate::getIOType(int idx)
{
	return inputList[idx][TYPE];
}

pinpair::pinpair()
{
	id = FAKEID;
	type = ATYPE;
};

pinpair::pinpair(const pinpair& other)
{
	pins[0]= other.pins[0];
	pins[1]= other.pins[1];
	id = other.id;
	type = other.type;
};

pinpair& pinpair::operator=(const pinpair other)
{
	pins[0] = other.pins[0];
	pins[1] = other.pins[1];
	id = other.id;
	type = other.type;

	return *this;
};

long& pinpair::operator[](int i)
{
	if(i == TYPE)
	{
		return type;
	}
	else if(i == INJNR)
	{
		return id;
	}
	int coordnr = (i - OFFSETNONCOORD) / 3;
	int coordidx = (i - OFFSETNONCOORD) % 3;
	return pins[coordnr].coord[coordidx];
};

void pinpair::setPinDetail(int idx, pindetails& c)
{
	pins[idx] = c;
};

pindetails& pinpair::getPinDetail(int idx)
{
	return pins[idx];
}

long pinpair::minDistBetweenPins()
{
	return pins[0].coord.manhattanDistance(pins[1].coord);
}

bool pinpair::isColinear()
{
	return pins[0].coord.isColinear(pins[1].coord);
}

int pinpair::getType()
{
	return type;
}

//void pindetails::addPinBlock(int offset, int direction, int distance)
//{
//	addPinBlock(offset, direction, distance, false);
//}
