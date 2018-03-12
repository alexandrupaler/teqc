#include "recycling/bfsstate.h"
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

void bfsState::setRequiredMaximumInputLevel(long value, std::string who)
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

size_t bfsState::getNrLines()
{
//	return currentGateProWire.size();

	return septNrLines;
}

void bfsState::init(std::vector<recyclegate*>& inputs)
{
	resetLevels();

//	if(currentGateProWire.size() == 0)
//	{
//		for(size_t i=0; i<inputs.size(); i++)
//		{
//			currentGateProWire[inputs[i]->wirePointers[0]] = inputs[i];
//		}
//	}
}

void bfsState::septInit(std::vector<recyclegate*>& bfs, size_t nrLines)
{
	resetLevels();

	septBfs = bfs;
	septLastIndex = 0;
	septNrLines = nrLines;
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

	septNrAssignedInputs = 0;
}

bfsState::~bfsState()
{
	resetToDrawAndToSchedule();
	septBfs.clear();
	scheduledInputs.clear();
}
