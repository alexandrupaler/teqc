#ifndef ASTARALG_H_
#define ASTARALG_H_

#include "astarpoint.h"
#include "boxworld2.h"

#include <vector>
#include <map>

enum PathfinderCode {PathFinderOK,
	PathFinderStartErr,
	PathFinderStopErr,
	PathFinderOtherErr,
	PathFinderEndSoonerThanStart};

class Pathfinder
{
public:
	Pathfinder();

	~Pathfinder();

	void clean();

	void cleanList(std::list<Point*>& clist);

	void useBoxWorld(boxworld2* world);

	PathfinderCode aStar(Point* start, Point* end, int axes[], unsigned int steps, std::vector<Point*>& path);

	Point* getOrCreatePoint(long x, long y, long z, bool checkAwayFromBox);

	bool pointExists(long x, long y, long z);

	bool isPointAwayFromBoxes(long x, long y, long z);

	bool checkCondition(Point* p1, Point* p2, int axes[]);

	void openPoint(Point* point);

	void closePoint(Point* point);

	//pointer to the RTree received from boxworld
	boxworld2* boxworld;

    // Define the open and the close list
    // The set of currently discovered nodes still to be evaluated.
	std::list<Point*> openList;

    //The set of nodes already evaluated
	std::list<Point*> closedList;

    //maps for memorising visited points
    std::map<long, std::map<long, std::map<long, Point*> > > visited;

};
#endif /* ASTARALG_H_ */
