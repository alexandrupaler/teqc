#include "scheduling/schedulercontrol.h"

#include <climits>

greedyschedulercontrol::greedyschedulercontrol()
{
	previousTimeCoordinate = INT_MIN;
}

void greedyschedulercontrol::updateControl(bfsState& state, heuristicparameters& heuristic, boxConfiguration& configuration)
{
	/*
	 * state.getMinimumLevel() is the latest time coordinate
	 */

	assumedInputTimeCoordinate = state.getMinimumLevel();

	timeWhenPoolEnds = state.getMinimumLevel() - heuristic.timeBeforePoolEnd;

	long boxTimeDepth = configuration.boxSize[0 /*assuming that it is A*/][CIRCUITDEPTH]/DELTA;

	schedulingRoundLength = boxTimeDepth + /*heuristic.timeBeforePoolEnd + */heuristic.connectionBufferTimeLength;

	boxStartTimeCoordinate = assumedInputTimeCoordinate - boxTimeDepth - heuristic.timeBeforePoolEnd - heuristic.connectionBufferTimeLength;

	triggerSchedule = (assumedInputTimeCoordinate > previousTimeCoordinate + schedulingRoundLength);

	if(triggerSchedule)
	{
		previousTimeCoordinate = assumedInputTimeCoordinate;
	}
}

void greedyschedulercontrol::cancelTrigger()
{
//	triggerSchedule = false;
}

bool greedyschedulercontrol::shouldTriggerScheduling(int boxType)
{
	return shouldTriggerScheduling();//influence on grand scheme of things?
}

bool greedyschedulercontrol::shouldTriggerScheduling()
{
	return triggerSchedule;
}

long greedyschedulercontrol::getBoxStartTimeCoordinate()
{
	return boxStartTimeCoordinate;
}

long greedyschedulercontrol::getSchedulingRoundLength()
{
	return schedulingRoundLength;
}

long greedyschedulercontrol::getTimeWhenPoolEnds()
{
	return timeWhenPoolEnds;
}

long greedyschedulercontrol::getAssumedInputTimeCoordinate()
{
	return assumedInputTimeCoordinate;
}
