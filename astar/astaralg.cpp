#include "astar/astaralg.h"
#include "fileformats/generaldefines.h"
#include "pins/pinconstants.h"

Pathfinder::Pathfinder()
{
	this->boxworld = NULL;
}

void Pathfinder::useBoxWorld(boxworld2* box)
{
	this->boxworld = box;
}

void Pathfinder::clean()
{
	openList.clear();
	closedList.clear();

	//iterate over all values in grid and delete pointers
	for (std::map<long, std::map<long, std::map<long, Point*> > >::iterator i = visited.begin(); i != visited.end() ; i++ )
	{
		for (std::map<long, std::map<long, Point*> >::iterator j = i->second.begin(); j != i->second.end() ; j++ )
		{
			for (std::map<long, Point*>::iterator k = j->second.begin(); k != j->second.end() ; k++ )
			{
				delete k->second;
			}
		}
	}

	visited.clear();
}

Pathfinder::~Pathfinder()
{
	clean();
}

bool Pathfinder::checkCondition(Point* p1, Point* p2, int axes[])
{
	bool ret = true;

	for(int i=0; i<3; i++)
	{
		if(axes[i] != -1)
		{
			ret = ret && (p1->coord[axes[i]] == p2->coord[axes[i]]);
		}
	}

	return ret;
}

PathfinderCode Pathfinder::aStar(Point* start, Point* end, int axes[], unsigned int steps, std::vector<Point*>& path)
{
    Point* current;
    Point* child;

    /*
     * Reset the open and closed lists
     */
    cleanList(openList);
    cleanList(closedList);

    std::list<Point*>::iterator i;

    if(end->coord[CIRCUITDEPTH] < start->coord[CIRCUITDEPTH])
	{
		return PathFinderEndSoonerThanStart;
	}

    // Add the start point to the openList
	// Initially, only the start node is known.
	if (start->isFree())
	{
		openPoint(start);
	}
	else
	{
		return PathFinderStartErr;
	}

	if (!end->isFree())
	{
		return PathFinderStopErr;
	}

    //parameter n is used to limit the number of steps
    unsigned int n = 0;
    while (n == 0 /*current does not exist at this point of the execution*/
    		|| (!checkCondition(current, end, axes) && n < steps/*max steps?*/))
    {
        // Look for the smallest F value in the openList and make it the current point
    	// this should be a priority queue?
        for (i = openList.begin(); i != openList.end(); ++ i)
        {
            if (i == openList.begin() || (*i)->getFScore() <= current->getFScore())
            {
                current = (*i);
            }
        }

        closePoint(current);

        // Get all current's adjacent walkable points
        for(int i = 0; i < 6; i++)
        {
			long z = ((i >= 0 && i < 2) ? 1 - 2*i : 0);
			long y = ((i >= 2 && i < 4) ? 1 /*1 - 2*(i-2)*/: 0); //time only positive
			long x = ((i >= 4 && i < 6) ? 1 - 2*(i-4) : 0);

			//do not try the same neighbour twice
			if(i == 2)
				i++;

			x = x * DELTA;
			y = y * DELTA;
			z = z * DELTA;

			// Get this point
			child = getOrCreatePoint(current->getX() + x, current->getY() + y, current->getZ() + z, true);

//			printf("w%d\n", child->walkable);
			// If it's closed or not walkable then pass

			if (child->closed)
			{
				continue;
			}

			if(!child->isFree())
			{
				closePoint(current);
				continue;
			}

			// If it's already in the openList
			if (child->opened)
			{
				// If it has a worse g score than the one that pass through the current point
				// then its path is improved when its parent is the current point
				if (child->getGScore() > child->getGScore(current))
				{
					// Change its parent and g score
					child->setParent(current);
					child->computeScores(end);
				}
			}
			else
			{
				// Add it to the openList with current point as parent
				openPoint(child);

				// Compute it's g, h and f score
				child->setParent(current);
				child->computeScores(end);
			}
        }

        n ++;
    }

    if(current == end /*same pointer, some coord?*/)
    {
	    // Resolve the path starting from the end point
		while (current->hasParent() && current != start)
		{
			path.push_back(current);
			current = current->getParent();
		}
		path.push_back(start);
    }
    else
    {
    	return PathFinderOtherErr;
    }

    return PathFinderOK;
}

void Pathfinder::cleanList(std::list<Point*>& clist)
{
	std::list<Point*>::iterator i;

	for (i = clist.begin(); i != clist.end(); ++ i)
	{
		(*i)->init();
	}
	clist.clear();
}

Point* Pathfinder::getOrCreatePoint(long x, long y, long z, bool checkAwayFromBox)
{
	Point* p;

	if(pointExists(x, y, z))
	{
		//retrieve
		p = visited[x][y][z];

		if(checkAwayFromBox)
		{
			//check if away from boxes
			bool w = isPointAwayFromBoxes(x, y, z);
			if(!w)
			{
				p->setWalkAndPriority(WALKBLOCKED_OCCUPY,
						NOPRIORITY, " -> a box occupied this point");
			}
		}
	}
	else
	{
		bool w = checkAwayFromBox ? isPointAwayFromBoxes(x, y, z) : true;
		p = new Point(x, y, z, w ? WALKFREE : WALKBLOCKED_OCCUPY);
		//store
		visited[x][y][z] = p;
	}

	return p;
}

bool Pathfinder::pointExists(long x, long y, long z)
{
	if(visited.count(x) > 0 && visited[x].count(y) > 0 && visited[x][y].count(z) > 0)
	{
		return true;
	}
	return false;
}

void Pathfinder::openPoint(Point* point)
{
	point->opened = true;
	openList.push_back(point);
}

void Pathfinder::closePoint(Point* point)
{
	point->opened = false;
	point->closed = true;

	openList.remove(point);
	closedList.push_back(point);
}

bool Pathfinder::isPointAwayFromBoxes(long x, long y, long z)
{
	bool isAway = true;

	int dist = 0;//1;//DELTA/2;

	if(boxworld != NULL)
	{
		BoundingBox bBox;
		bBox.edges[CIRCUITWIDTH].first = x - dist;
		bBox.edges[CIRCUITDEPTH].first = y - dist;
		bBox.edges[CIRCUITHEIGHT].first = z - dist;

		//care este dimensiunea unei cutii?
		bBox.edges[CIRCUITWIDTH].second = x + dist;
		bBox.edges[CIRCUITDEPTH].second = y + dist;
		bBox.edges[CIRCUITHEIGHT].second = z + dist;

		Visitor x;
		x = boxworld->rtree.Query(RTree::AcceptOverlapping(bBox), x);
//		x = boxworld->rtree.Query(RTree::AcceptEnclosing(bBox), x);
		bool doesNotTouch = (x.count == 0);

		isAway = isAway && doesNotTouch;
	}

	return isAway;
}
