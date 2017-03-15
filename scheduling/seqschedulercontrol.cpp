#include "scheduling/seqschedcontrol.h"

void seqschedulercontrol::updateControl(bfsState& state, heuristicparameters& heuristic, boxworld2& boxworld, connectionManager& connManager)
{
	for(int boxType = 0; boxType < 2; boxType++)
	{
		int necessary = state.toScheduleInputs[boxType].size();
		int available = connManager.getUnusedReservedNr(boxType);

		triggerSchedule[boxType] = (available < necessary);
	}

	assumedInputTimeCoordinate = state.getMaximumInputLevel();// timeWhenPoolEnds;//iterationState.getMinimumLevel() - roundLength;

	if(triggerSchedule[0] || triggerSchedule[1])
	{
		/*get max boxTimeDepth*/
		long boxTimeDepth = -1;
		for(int boxType = 0; boxType < 2; boxType++)
		{
			if(triggerSchedule[boxType])
			{
				if(boxTimeDepth < boxworld.currentConfig.boxSize[boxType][CIRCUITDEPTH]/DELTA)
				{
					boxTimeDepth = boxworld.currentConfig.boxSize[boxType][CIRCUITDEPTH]/DELTA;
				}
			}
		}

		boxStartTimeCoordinate = assumedInputTimeCoordinate - boxTimeDepth - heuristic.timeBeforePoolEnd;

		achievedCost = heuristic.connectionBufferTimeLength/*HEURISTIC*/ + (boxStartTimeCoordinate + boxTimeDepth);
	}
	else
	{
		if(state.isCircuitFinished())
			achievedCost = state.getMinimumLevel();
		else
			achievedCost = state.getMaximumInputLevel();
	}

	timeWhenPoolEnds = achievedCost - heuristic.timeBeforePoolEnd;//connManager.getTimeWhenPoolEnds(iterationState);
}

long seqschedulercontrol::getAchievedCost()
{
	return achievedCost;
}

long seqschedulercontrol::getAssumedInputTimeCoordinate()
{
	return assumedInputTimeCoordinate;
}

bool seqschedulercontrol::shouldTriggerScheduling(int boxType)
{
	return triggerSchedule[boxType];
}

bool seqschedulercontrol::shouldTriggerScheduling()
{
	return triggerSchedule[0] || triggerSchedule[1];
}

long seqschedulercontrol::getBoxStartTimeCoordinate()
{
	return boxStartTimeCoordinate;
}

long seqschedulercontrol::getTimeWhenPoolEnds()
{
	return timeWhenPoolEnds;
}

void seqschedulercontrol::cancelTrigger()
{
	return;//do nothing
}

