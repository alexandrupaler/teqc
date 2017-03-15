#ifndef BFSSTATE_H__
#define BFSSTATE_H__

#include <vector>
#include <queue>
#include <set>
#include <map>
#include <string>
#include "recycling/recyclegate.h"

#define NOLEVEL -1

using namespace std;

class bfsState
{
public:
	vector<int> toScheduleInputs[2];
	vector<int> toDraw;
	set<int> scheduledInputs;
	vector<int> currentIdProWire;
	int nrLines;

	map<int, int> operationIdToCircuitPinIndex;

	void resetLevels();
	void saveMaxLevel(long newLevel);

	bool isSomethingToDo();
	void init(vector<int>& inputs);

	/*
	 * The lines to be checked for computing the next minLevel
	 */
	set<int> minLevelLinesToCheck;
	void initLinesToCheck();
	void removeFromLinesToCheck(int line);
	void getMinLevelFromLinesToCheck(vector<recyclegate>& circuit, long& minLevel, int& minLevelPos);
	void resetToDrawAndToSchedule();

public:
	long getRequiredMaximumInputLevel();
	void setRequiredMaximumInputLevel(long value, string who);

	long getMaximumInputLevel();
	void setMaximumInputLevel(long value);

	long getMinimumLevel();
	void setMinimumLevel(long value);

	bool isCircuitFinished();
	void setCircuitFinished(bool value);

private:
	/*
	 * The maximum level of the inputs to be scheduled
	 */
	long maximumInputLevel;

	/*
	 * The maximum level required after scheduling was performed.
	 * The boxes may "push" in time the level of the inputs/elements
	 */
	long requiredMaximumInputLevel;

	/*
	 * The minimum level of the inputs to be scheduled
	 * required, for the moment, only by blockwiseBfs
	 */
	long minimumLevel;

	bool circuitFinished;
};

#endif
