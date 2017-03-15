#ifndef ASAPSCHEDCONTROL_H_
#define ASAPSCHEDCONTROL_H_

#include "bfsstate.h"
#include "heuristicparameters.h"
#include "boxworld2.h"
#include "connectionmanager.h"

class asapschedcontrol
{
public:
	asapschedcontrol();

	void updateControl(bfsState& state, heuristicparameters& heuristic, boxworld2& boxworld);

	bool shouldTriggerScheduling(int boxType);

	bool shouldTriggerScheduling();

	void cancelTrigger();

	long getBoxStartTimeCoordinate();

	long getSchedulingRoundLength();

	long getTimeWhenPoolEnds();

	long getAssumedInputTimeCoordinate();

protected:
	long boxStartTimeCoordinate;

	long schedulingRoundLength;// = heuristicParam.timeBeforePoolEnd + 10 + heuristicParam.connectionBufferTimeLength;;//boxworld.getDistillationRoundTimeLength();
	long timeWhenPoolEnds;// = iterationState.getMinimumLevel() - heuristicParam.timeBeforePoolEnd;//connManager.getTimeWhenPoolEnds(iterationState);

	long assumedInputTimeCoordinate;// = timeWhenPoolEnds;//iterationState.getMinimumLevel() - roundLength;
	bool triggerSchedule;// = (assumeInputTimeCoordinate > previousTimeCoordinate + roundLength);
};


#endif /* ASAPSCHEDCONTROL_H_ */
