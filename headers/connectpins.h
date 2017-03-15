#ifndef CONNECTPINS_H_
#define CONNECTPINS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "numberandcoordinate.h"
#include "geometry.h"
#include "faildistillations.h"
#include "rtree/RStarTree.h"
#include "astar/astaralg.h"
#include "heuristicparameters.h"

#define NONFAILEDBOX 2
#define ABOXADD 0
#define YBOXADD 1
#define INDEX 0

#define CANONICAL 0
#define ASTAR 1

#define ENFORCE_BLOCK true
#define REMOVE_BLOCK false

using namespace std;

class connectpins
{
public:
	geometry connectionsGeometry;

	geometry debugBlockOccupyGeometry;
	geometry debugBlockGuideGeometry;

	Pathfinder pathfinder;

	heuristicparameters heuristicParam;

public:
	bool processPins(char* fname, int method);

	bool processPins(vector<pinpair>& pins, int method);

	int getCircuitPinIndex(convertcoordinate& coord);

	int getAfterFail2(convertcoordinate& coordDist, convertcoordinate& coordCirc);

	pindetails extractPoint(int part, pinpair& cl);

	int offsetChangeIndexAndStore(convertcoordinate& p, int pos, int off);

	void connectCanonical(pinpair& coordline);

	bool connectWithAStar(pinpair& coordline);

	void blockPins(vector<pinpair>& pins);

	void unblockPins(vector<pinpair>& pins);

	void blockCoordinates(pindetails& detail);

	void unblockCoordinates(pindetails& detail);

	void unblockCoordinatesForced(pindetails& detail);

	void setWalkable(pindetails& detail, bool enforce, int whichBlockType/*, int walk, bool force*/);

	void filterAndAddToCorners(vector<convertcoordinate>& corners, Point* current);

	/*Debugging*/
	geometry* whereToDebug(int blockType);
	int debugFirstCoordinate(int blockType, convertcoordinate& coord);
	int debugSecondCoordinate(int blockType, int deb1, convertcoordinate& coord, bool enforce);
};
#endif /* CONNECTPINS_H_ */
