#include "stdlib.h"
#include "stdio.h"

#include "astar/astarpoint.h"

void Point::init()
{
	parent = NULL;
	closed = false;
	opened = false;

	f = g = h = 0;
}

Point::Point()
{
    init();
	setWalkAndPriority(WALKFREE, NOPRIORITY, " -> just constructed");
}

Point::Point(int x, int y, int z, int walk)
{
    init();
    setWalkAndPriority(walk, NOPRIORITY, " -> just constructed");

    this->coord[CIRCUITWIDTH] = x;
    this->coord[CIRCUITDEPTH] = y;
    this->coord[CIRCUITHEIGHT] = z;
}

Point* Point::getParent()
{
    return parent;
}

void Point::setParent(Point *p)
{
    parent = p;
}

long Point::getX()
{
    return this->coord[CIRCUITWIDTH];
}

long Point::getY()
{
    return this->coord[CIRCUITDEPTH];
}

long Point::getZ()
{
    return this->coord[CIRCUITHEIGHT];
}

int Point::getGScore(Point* p)
{
	long score = p->g + (abs(p->getX() - getX()) + abs(p->getY() - getY()) + abs(p->getZ() - getZ()));
			//p->g + ((getX() == p->getX() || getY() == p->getY() || getZ() == p->getZ()) ? 10 : 14);

	if(Point::useSecondHeuristic && abs(p->getZ() - getZ()) != 0)
	{
		score -= 5;
	}

	return score;
}

int Point::getHScore(Point* p)
{
	//Manhattan?
    return ((abs(p->getX() - getX()) + abs(p->getY() - getY()) + abs(p->getZ() - getZ())) * 20)/10;
}

int Point::getGScore()
{
    return g;
}

int Point::getHScore()
{
    return h;
}

int Point::getFScore()
{
    return f;
}

void Point::computeScores(Point* end)
{
    g = getGScore(parent);
    h = getHScore(end);
    f = g + h;
}

bool Point::hasParent()
{
    return parent != NULL;
}

bool Point::isFree()
{
	return walkable == WALKFREE;
}

int Point::getPriority()
{
	return blockPriority;
}

void Point::setWalkAndPriority(int walk, int priority, std::string journalMessage)
{
	this->walkable = walk;
	this->blockPriority = priority;

	if(blockJournal.size() == Point::heuristicParam->pointJournalLength)
	{
		blockJournal.erase(blockJournal.begin());
	}
	blockJournal.push_back(journalMessage);
}

void Point::printBlockJournal()
{
	for(std::list<std::string>::iterator it = blockJournal.begin();
			it != blockJournal.end(); it++)
	{
		printf("%s\n", it->c_str());
	}
}


bool Point::useSecondHeuristic = false;
heuristicparameters* Point::heuristicParam = NULL;
