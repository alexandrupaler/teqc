#include "bfsstate.h"
#include "circconvert.h"
#include "numberandcoordinate.h"

bool bfsState::isSomethingToDo()
{
	return toDraw.size() != 0
			|| toScheduleInputs[ATYPE].size() != 0
			|| toScheduleInputs[YTYPE].size() != 0;
}

long bfsState::getRequiredMaximumInputLevel()
{
//	printf("get %ld\n", requiredMaximumInputLevel);
	return requiredMaximumInputLevel;
}

void bfsState::setRequiredMaximumInputLevel(long value, string who)
{
//	printf("%s set %ld\n", who.c_str(),value);
	requiredMaximumInputLevel = value;
}

long bfsState::getMaximumInputLevel()
{
	return maximumInputLevel;
}

void bfsState::setMaximumInputLevel(long value)
{
	maximumInputLevel = value;
}

long bfsState::getMinimumLevel()
{
	return minimumLevel;
}

void bfsState::setMinimumLevel(long value)
{
	minimumLevel = value;
}

bool bfsState::isCircuitFinished()
{
	return circuitFinished;
}

void bfsState::setCircuitFinished(bool value)
{
	circuitFinished = value;
}

void bfsState::resetLevels()
{
	setMinimumLevel(NOLEVEL);
	setMaximumInputLevel(NOLEVEL);
	setRequiredMaximumInputLevel(NOLEVEL, "resetLevels");
}

void bfsState::saveMaxLevel(long newLevel)
{
	setMaximumInputLevel(newLevel);
	setRequiredMaximumInputLevel(getMaximumInputLevel(), "saveMaxLevel");
}

void bfsState::init(vector<int>& inputs)
{
	resetLevels();

	nrLines = inputs.size();

	if(currentIdProWire.size() == 0)
	{
		currentIdProWire.resize(nrLines, -1);

		for(int i=0; i<nrLines; i++)
		{
			//take the id of the input operations
			currentIdProWire[i] = inputs[i];
		}
	}
}

void bfsState::initLinesToCheck()
{
	minLevelLinesToCheck.clear();
	for(int i=0; i<nrLines; i++)
	{
		minLevelLinesToCheck.insert(i);
	}
}

void bfsState::removeFromLinesToCheck(int line)
{
	minLevelLinesToCheck.erase(line);
}

void bfsState::getMinLevelFromLinesToCheck(vector<recyclegate>& circuit, long& minLevel, int& minLevelPos)
{
	minLevel = LONG_MAX;
	minLevelPos = -1;
	for(set<int>::iterator it = minLevelLinesToCheck.begin(); it != minLevelLinesToCheck.end(); it++)
	{
		if(currentIdProWire[*it] != -1
				 && minLevel > circuit[currentIdProWire[*it]].level)
		{
			minLevel = circuit[currentIdProWire[*it]].level;
			minLevelPos = *it;
		}
	}

//	return minLevelPos;
}

void bfsState::resetToDrawAndToSchedule()
{
	/*
	 * Prepare the next stage:
	 * Clean the elements to be drawn and the ones to be scheduled
	 */
	for(int i = 0; i < 2; i++)
	{
		scheduledInputs.insert(toScheduleInputs[i].begin(), toScheduleInputs[i].end());
		toScheduleInputs[i].clear();
	}
	toDraw.clear();
}
