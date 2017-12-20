#ifndef BFSSTATE_H__
#define BFSSTATE_H__

#include <vector>
#include <queue>
#include <set>
#include <map>
#include <string>
#include "recycling/recyclegate.h"

#define NOLEVEL -1

class bfsState
{
public:
//	std::vector<int> toScheduleInputs[2];
//	std::vector<int> toDraw;
//	std::set<int> scheduledInputs;
//	std::vector<int> currentIdProWire;

	std::vector<recyclegate*> toScheduleInputs[2];
	std::vector<recyclegate*> toDraw;
	std::set<recyclegate*> scheduledInputs;
	std::map<wireelement*, recyclegate*> currentGateProWire;

	//	int nrLines;

	std::map<int, int> operationIdToCircuitPinIndex;

	void resetLevels();
	void saveMaxLevel(long newLevel);

	bool isSomethingToDo();
	void init(std::vector<recyclegate*>& inputs);

	/*
	 * The lines to be checked for computing the next minLevel
	 */
	std::set<wireelement*> minLevelLinesToCheck;
	void initLinesToCheck();
	void removeFromLinesToCheck(wireelement* line);
	wireelement* getMinLevelFromLinesToCheck(long& minLevel);
	void resetToDrawAndToSchedule();

public:
	long getRequiredMaximumInputLevel();
	void setRequiredMaximumInputLevel(long value, std::string who);

	long getMaximumInputLevel();
	void setMaximumInputLevel(long value);

	long getMinimumLevel();
	void setMinimumLevel(long value);

	bool isCircuitFinished();
	void setCircuitFinished(bool value);

	size_t getNrLines();

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
