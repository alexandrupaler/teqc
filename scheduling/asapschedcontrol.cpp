#include "scheduling/asapschedcontrol.h"

asapschedcontrol::asapschedcontrol()
{
	triggerSchedule = true;
}

void asapschedcontrol::updateControl(bfsState& state, heuristicparameters& heuristic, boxworld2& boxworld)
{
	/*this scheduler should call a single time the cancelTrigger
	 * thus, after triggerSchedule was once set false, no other schedule is triggered*/

	if(triggerSchedule == true)
	{
		long boxLength = -1;
		for(int boxType = 0; boxType < 2; boxType++)
		{
			if(boxLength < boxworld.currentConfig.boxSize[boxType][CIRCUITDEPTH] / DELTA)
			{
				boxLength =  boxworld.currentConfig.boxSize[boxType][CIRCUITDEPTH] / DELTA;
			}
		}

		/*Assuming input is at zero*/
		timeWhenPoolEnds = - heuristic.timeBeforePoolEnd;

		boxStartTimeCoordinate = -boxLength - heuristic.connectionBufferTimeLength + timeWhenPoolEnds;
	}
}

bool asapschedcontrol::shouldTriggerScheduling(int boxType)
{
	return triggerSchedule;
}

void asapschedcontrol::cancelTrigger()
{
	triggerSchedule = false;
}

bool asapschedcontrol::shouldTriggerScheduling()
{
	return shouldTriggerScheduling(0);
}

long asapschedcontrol::getBoxStartTimeCoordinate()
{
	return boxStartTimeCoordinate;
}

long asapschedcontrol::getAssumedInputTimeCoordinate()
{
	return 0;
}

long asapschedcontrol::getTimeWhenPoolEnds()
{
	return timeWhenPoolEnds;
}
