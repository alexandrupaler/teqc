#include "astar/astaralg.h"
#include "fileformats/generaldefines.h"
#include "pins/pinconstants.h"

#include "utils/profiling.h"

Pathfinder::Pathfinder()
{
	this->boxworld = NULL;
	allowConnectionThroughChannel = false;
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

//Point* Pathfinder::computeNextChild(int* direction, Point* current)
//{
//
//}

PathfinderCode Pathfinder::aStar(Point* start, Point* end, int axes[], unsigned int steps, std::vector<Point*>& path, int timeDirection)
{
    Point* current;
    Point* child;

    /*
     * Reset the open and closed lists
     */
    cleanList(openList);
    cleanList(closedList);

//    profiling::util_get_cpu_time_begin();

    std::list<Point*>::iterator i;

//    if(end->coord[CIRCUITDEPTH] < start->coord[CIRCUITDEPTH])
//	{
//		return PathFinderEndSoonerThanStart;
//	}

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
        /*for (i = openList.begin(); i != openList.end(); ++ i)
        {
            if (i == openList.begin() || (*i)->getFScore() <= current->getFScore())
            {
                current = (*i);
            }
        }*/

    	if(openList.size() == 0)
		{
			break;
		}

    	current = *(openList.begin());

        closePoint(current);

        // Get all current's adjacent walkable points
        for(int i = 0; i < 6; i++)
        {
			long z1 = ((i >= 0 && i < 2) ? 1 - 2*i : 0);
			long y1 = ((i >= 2 && i < 4) ? timeDirection /*1 - 2*(i-2)*/: 0); //time only positive
			long x1 = ((i >= 4 && i < 6) ? 1 - 2*(i-4) : 0);

			//do not try the same neighbour twice
			if(i == 2)
				i++;

			long x = x1 * DELTA;
			long y = y1 * DELTA;
			long z = z1 * DELTA;

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

			/*
			 * JPS type?
			 */
/*
			bool firstChild = true;
			Point* prevChild = NULL;
			int nrJump = 1;
			while(!child->closed && child->isFree()
					&& child->getY() <= end->getY()
					&& (child->getX() >= current->getX() - steps && child->getX() <= current->getX() + steps)
					&& (child->getZ() >= current->getZ() - steps && child->getZ() <= current->getZ() + steps))
			{
				firstChild = false;

				nrJump++;

				x = nrJump * x1 * DELTA;
				y = nrJump * y1 * DELTA;
				z = nrJump * z1 * DELTA;

				prevChild = child;
				child = getOrCreatePoint(current->getX() + x, current->getY() + y, current->getZ() + z, true);

//				printf("%s\n", child->coord.toString(':').c_str());
			}

			if(firstChild)
			{
				//there is nothing here, the direct child is blocked
				if(!child->isFree())
				{
					closePoint(current);//???
				}
				continue;
			}
			else
			{
				//it has hit a block, or is on the margin
				//step back, and add child
				child = prevChild;
			}
			*/

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

					//update the openedList
					openList.erase(child->openedIt);
					openPoint(child);
				}
			}
			else
			{
				// Compute it's g, h and f score
				child->setParent(current);
				child->computeScores(end);

				// Add it to the openList with current point as parent
				// add after having computed the scores
				openPoint(child);
			}
        }

        n ++;
    }

//    printf("search took %f\n", profiling::util_get_cpu_time_end());

    if(checkCondition(current, end, axes) /*same pointer, some coord?*/)
    {
//	    profiling::util_get_cpu_time_begin();
    	// Resolve the path starting from the end point
		while (current->hasParent() && current != start)
		{
			path.push_back(current);
			current = current->getParent();
		}
		path.push_back(start);

//		printf("preparation took %f\n", profiling::util_get_cpu_time_end());
    }
    else
    {
    	return PathFinderOtherErr;
    }

    return PathFinderOK;
}

void Pathfinder::cleanList(std::list<Point*>& clist)
{
//	printf("start clean with %u els takes ", clist.size());
//	profiling::util_get_cpu_time_begin();

	std::list<Point*>::iterator i;

	for (i = clist.begin(); i != clist.end(); ++ i)
	{
		(*i)->init();
	}
	clist.clear();

//	printf("%f \n", profiling::util_get_cpu_time_end());
}

void Pathfinder::cleanList(std::multiset<Point*, pointComp>& clist)
{
//	printf("start clean with %u els takes ", clist.size());
//	profiling::util_get_cpu_time_begin();

	std::multiset<Point*>::iterator i;

	for (i = clist.begin(); i != clist.end(); ++ i)
	{
		(*i)->init();
	}
	clist.clear();

//	printf("%f \n", profiling::util_get_cpu_time_end());
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

	point->openedIt = openList.insert(point);
//	openList.push_back(point);
}

void Pathfinder::closePoint(Point* point)
{
	if(point->opened)
	{
		point->opened = false;
		openList.erase(point->openedIt);
	}

	point->closed = true;
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
		bool doesNotTouch = (x.count == 0);

		bool channelOverlap = false;
		if(!allowConnectionThroughChannel)
			channelOverlap = boxworld->connectionsBox.overlaps(bBox);

		bool geomOverlap = boxworld->geomBoundingBox.overlaps(bBox);

		isAway = isAway && doesNotTouch && ! channelOverlap && !geomOverlap;
	}

	return isAway;
}
