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

#define CANONICAL1 0
#define ASTAR 1
#define CANONICAL2 2

#define ENFORCE_BLOCK true
#define REMOVE_BLOCK false

class connectpins
{
public:
	geometry connectionsGeometry;

	geometry debugBlockOccupyGeometry;
	geometry debugBlockGuideGeometry;

	Pathfinder pathfinder;

	heuristicparameters heuristicParam;
	void processFoundPath(pinpair& coordline, std::vector<Point*>& path);

public:
//	bool processPins(char* fname, int method);
	bool processPins(std::vector<pinpair>& pins, int method);
	int getCircuitPinIndex(convertcoordinate& coord);
	int getAfterFail2(convertcoordinate& coordDist,
			convertcoordinate& coordCirc);
	pindetails extractPoint(int part, pinpair& cl);
	int offsetChangeIndexAndStore(convertcoordinate& p, int pos, int off);
	void interchangeStartEnd(Point*& start, Point*& end);

	void connectCanonicalOne(pinpair& coordline, std::vector<Point*>& path);
	void connectCanonicalTwo(pinpair& coordline, std::vector<Point*>& path);
	bool connectWithAStar(pinpair& coordline, std::vector<Point*>& path);

	void blockPins(std::vector<pinpair>& pins);
	void unblockPins(std::vector<pinpair>& pins);
	void blockCoordinates(pindetails& detail);
	void unblockCoordinates(pindetails& detail);
	void unblockCoordinatesForced(pindetails& detail);
	void setWalkable(pindetails& detail, bool enforce, int whichBlockType);
	void filterAndAddToCorners(std::vector<convertcoordinate>& corners,
			Point* current);
	geometry* whereToDebug(int blockType);
	int debugFirstCoordinate(int blockType, convertcoordinate& coord);
	int debugSecondCoordinate(int blockType, int deb1, convertcoordinate& coord, bool enforce);

	/*
	 * 3 NOV
	 */
	void markSegmentInPathFinderUsed(Point* start, Point* stop, size_t axis);

};
#endif /* CONNECTPINS_H_ */
