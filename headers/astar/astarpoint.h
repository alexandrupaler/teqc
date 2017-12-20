#ifndef ASTARPOINT_H_
#define ASTARPOINT_H_

#include <list>
#include <string>

#include "pins/pinconstants.h"
#include "convertcoordinate.h"
#include "heuristicparameters.h"

class Point
{
public:
	Point();
	Point(int x, int y, int z, int w);

	void init();

	Point* getParent();
	void setParent(Point *p);
	long getX();
	long getY();
	long getZ();

	int getGScore(Point* p);
	int getHScore(Point* p);
	int getGScore();
	int getHScore();
	int getFScore();
	void computeScores(Point* end);
	bool hasParent();
	bool isFree();

	/*
	 * The type of the block affecting this Point
	 */
	int walkable;

	/*
	 * The priority of the block affecting this Point
	 * Priority is correlated with walkable
	 */
	int blockPriority;
//	void removePriority();
//	void setPriority(int priority);
	int getPriority();
	void setWalkAndPriority(int walk, int priority, std::string journalMessage);
	void printBlockJournal();
	std::list<std::string> blockJournal;

	/*
	 * If the Point belongs to the closed list in A*
	 */
	bool closed;

	/*
	 * If the Point belongs to the opened list in A*
	 */
	bool opened;

	/*
	 * The 3D coordinate of the Point
	 */
	convertcoordinate coord;

	/*
	 * Strange place for this boolean parameter to exist
	 */
	static bool useSecondHeuristic;

	static heuristicparameters* heuristicParam;

private :
	unsigned int f;
	unsigned int g;
	unsigned int h;

	Point *parent;
};

#endif
