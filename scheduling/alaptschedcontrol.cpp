#include "scheduling/alaptschedcontrol.h"

void alaptschedcontrol::updateControl(bfsState& state, heuristicparameters& heuristic, boxworld2& boxworld, connectionManager& connmanager)
{
	assumedInputTimeCoordinate = state.getMaximumInputLevel();

	if(state.isCircuitFinished())
	{
		assumedInputTimeCoordinate = state.getMinimumLevel();
	}

	timeWhenPoolEnds = assumedInputTimeCoordinate - heuristic.timeBeforePoolEnd;

	long boxLength[2];
	for(int boxType = 0; boxType < 2; boxType++)
	{
		int available = connmanager.getUnusedReservedNr(boxType);

		triggerSchedule[boxType] = (available < state.toScheduleInputs[boxType].size());

		boxLength[boxType] =  boxworld.currentConfig.boxSize[boxType][CIRCUITDEPTH] / DELTA;

		//asta trebuie de doua feluri
		//mai trebuie si randul pentru a sti unde sa plasez
		boxStartTimeCoordinate[boxType] = timeWhenPoolEnds - boxLength[boxType] - heuristic.connectionBufferTimeLength;
	}
}

bool alaptschedcontrol::shouldTriggerScheduling()
{
	return triggerSchedule[0] || triggerSchedule[1];
}

bool alaptschedcontrol::shouldTriggerScheduling(int boxType)
{
	return triggerSchedule[boxType];
}

void alaptschedcontrol::cancelTrigger()
{
	return;//do nothing
}

long alaptschedcontrol::getBoxStartTimeCoordinate(int boxType)
{
	return boxStartTimeCoordinate[boxType];
}

long alaptschedcontrol::getTimeWhenPoolEnds()
{
	return timeWhenPoolEnds;
}

long alaptschedcontrol::getAssumedInputTimeCoordinate()
{
	return assumedInputTimeCoordinate;
}
