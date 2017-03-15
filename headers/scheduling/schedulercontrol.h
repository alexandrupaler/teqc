#ifndef SCHEDULERCONTROL_H_
#define SCHEDULERCONTROL_H_

#include "bfsstate.h"
#include "heuristicparameters.h"
#include "boxworld2.h"

class greedyschedulercontrol
{
public:
	greedyschedulercontrol();

	void updateControl(bfsState& state, heuristicparameters& heuristic, boxConfiguration& configuration);

	bool shouldTriggerScheduling();

	bool shouldTriggerScheduling(int boxType);

	long getBoxStartTimeCoordinate();

	long getSchedulingRoundLength();

	long getTimeWhenPoolEnds();

	long getAssumedInputTimeCoordinate();

	void cancelTrigger();

protected:
	long boxStartTimeCoordinate;

	long schedulingRoundLength;
	long timeWhenPoolEnds;

	long assumedInputTimeCoordinate;
	bool triggerSchedule;

	long previousTimeCoordinate;
};
#endif /* SCHEDULERCONTROL_H_ */
