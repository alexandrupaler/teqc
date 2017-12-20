#ifndef ALAPTSCHEDCONTROL_H_
#define ALAPTSCHEDCONTROL_H_

#include "recycling/bfsstate.h"
#include "heuristicparameters.h"
#include "boxworld2.h"
#include "connectionmanager.h"

class alaptschedcontrol
{
public:
	void updateControl(bfsState& state, heuristicparameters& heuristic, boxworld2& boxworld, connectionManager& connmanager);

	bool shouldTriggerScheduling(int boxType);

	bool shouldTriggerScheduling();

	void cancelTrigger();

	long getBoxStartTimeCoordinate(int boxType);

	long getTimeWhenPoolEnds();

	long getAssumedInputTimeCoordinate();

protected:
	long boxStartTimeCoordinate[2];

	long timeWhenPoolEnds;

	long assumedInputTimeCoordinate;
	bool triggerSchedule[2];
};

#endif /* ALAPTSCHEDCONTROL_H_ */
