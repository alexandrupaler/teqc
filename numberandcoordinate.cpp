#include <stdio.h>
#include <vector>

#include "numberandcoordinate.h"

using namespace std;

void numberandcoordinate::addSingleTypeEntries(int nrInputs, int type, int startCoord, int stopCoord, int circuitRowCoordIncrement)
{
	bool reverseDirection = false;
	if(stopCoord < startCoord)
	{
		circuitRowCoordIncrement *= -1;
		reverseDirection = true;
	}

	int circuitRowCoordinate = startCoord;

	for(int i=0; i<nrInputs; i++)
	{
		inputpin fakeInput;
		fakeInput[TYPE] = type;//A state
		//fakeInput[INJNR] = 10000 + i;//INJNR, start with huge number...should not be needed
		fakeInput[ROWCOORD] =  circuitRowCoordinate;

		addEntry(fakeInput);
		//inputList.push_back(distparam);

		circuitRowCoordinate += circuitRowCoordIncrement;
		if(reverseDirection)
		{
			if(circuitRowCoordinate < stopCoord)
				circuitRowCoordinate = startCoord;
		}
		else
		{
			if(circuitRowCoordinate > stopCoord)
				circuitRowCoordinate = startCoord;
		}
	}
}

void numberandcoordinate::addEntry(inputpin& numberAndCoords)
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

//void numberandcoordinate::read(char* fname)
//{
//	FILE* fin = fopen(fname, "r");
//
//
//	fclose(fin);
//}

int numberandcoordinate::size()
{
	return inputList.size();
}

int numberandcoordinate::getIOType(int idx)
{
	return inputList[idx][TYPE];
}
