#ifndef SEQSCHEDCONTROL_H_
#define SEQSCHEDCONTROL_H_

#include "bfsstate.h"
#include "heuristicparameters.h"
#include "boxworld2.h"
#include "connectionmanager.h"

class seqschedulercontrol
{
public:
	void updateControl(bfsState& state, heuristicparameters& heuristic, boxworld2& boxworld, connectionManager& connManager);

	bool shouldTriggerScheduling(int boxType);

	bool shouldTriggerScheduling();

	long getBoxStartTimeCoordinate();

	long getAchievedCost();

	long getTimeWhenPoolEnds();

	long getAssumedInputTimeCoordinate();

	void cancelTrigger();

protected:
	long boxStartTimeCoordinate;

	long timeWhenPoolEnds;
	long assumedInputTimeCoordinate;
	bool triggerSchedule[2];

	long achievedCost;
};

#endif /* SEQSCHEDCONTROL_H_ */
