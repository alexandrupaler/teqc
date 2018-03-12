#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "fileformats/generaldefines.h"
#include "connectpins.h"
#include "geometry.h"

#include "pins/pinconstants.h"

int connectpins::getCircuitPinIndex(convertcoordinate& coord)
{
	//assumes coordinates are only 0+2, 4+6, 8+10
	return (coord[CIRCUITHEIGHT]%4)/2;//functioneaza?
}

//returns 1 if Y: on the left side
//2 if initial distillation: normal
// 0 if A: on the right side
int connectpins::getAfterFail2(convertcoordinate& coordDist, convertcoordinate& coordCirc)
{
	//dist pin X - coord pin X
	int dif = coordDist[CIRCUITWIDTH] - coordCirc[CIRCUITWIDTH];
	int sign = ( dif==0 ? 0 : dif/abs(dif));

	if(sign == -1)
		return YBOXADD;
	else if(sign == 0)
		return NONFAILEDBOX;
	return ABOXADD;
}

pindetails connectpins::extractPoint(int part, pinpair& cl)
{
	pindetails ret = cl.getPinDetail(part);

	connectionsGeometry.addCoordinate(ret.coord);

	return ret;
}

int connectpins::offsetChangeIndexAndStore(convertcoordinate& p, int pos, int off)
{
	if(off == 0)
		return connectionsGeometry.coordMap[p.toString(',')];

	p[pos] += off;
	return connectionsGeometry.addCoordinate(p);
}

void connectpins::connectCanonicalOne(pinpair& coordline, std::vector<Point*>& path)
{
	convertcoordinate distCoord = extractPoint(SOURCEPIN, coordline).coord;//from the distillationbox
	convertcoordinate circCoord = extractPoint(DESTPIN, coordline).coord;//from the input

	Point* start = pathfinder.getOrCreatePoint(distCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH], distCoord[CIRCUITHEIGHT], false);
	path.push_back(start);

	Point* m1 = pathfinder.getOrCreatePoint(distCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], distCoord[CIRCUITHEIGHT], false);
	markSegmentInPathFinderUsed(start, m1, CIRCUITDEPTH);
	path.push_back(m1);

	Point* m2 = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], distCoord[CIRCUITHEIGHT], false);
	markSegmentInPathFinderUsed(m1, m2, CIRCUITWIDTH);
	path.push_back(m2);

	Point* end = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], circCoord[CIRCUITHEIGHT], false);
	markSegmentInPathFinderUsed(m2, end, CIRCUITHEIGHT);
	path.push_back(end);
}

void connectpins::connectCanonicalTwo(pinpair& coordline, std::vector<Point*>& path)
{
	convertcoordinate distCoord = extractPoint(SOURCEPIN, coordline).coord;//from the distillationbox
	convertcoordinate circCoord = extractPoint(DESTPIN, coordline).coord;//from the input

	Point* start = pathfinder.getOrCreatePoint(distCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH], distCoord[CIRCUITHEIGHT], false);
	path.push_back(start);

	Point* m1 = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH], distCoord[CIRCUITHEIGHT], false);
	markSegmentInPathFinderUsed(start, m1, CIRCUITWIDTH);
	path.push_back(m1);

	Point* m2 = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH], circCoord[CIRCUITHEIGHT], false);
	markSegmentInPathFinderUsed(m1, m2, CIRCUITHEIGHT);
	path.push_back(m2);

	Point* end = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], circCoord[CIRCUITHEIGHT], false);
	markSegmentInPathFinderUsed(m2, end, CIRCUITDEPTH);
	path.push_back(end);
}

void connectpins::interchangeStartEnd(Point*& start, Point*& end)
{
	Point* x = start;
	start = end;
	end = x;
}

void connectpins::processFoundPath(pinpair& coordline, std::vector<Point*>& path)
{
	/*
	 * If a solution was found
	 */
	if (path.size() > 0)
	{
		/*
		 * Generate the corners of the path
		 */
		std::vector < convertcoordinate > corners;
		for (std::vector<Point*>::iterator it = path.begin(); it != path.end();
				it++)
		{
			filterAndAddToCorners(corners, *it);

			if ((*it)->walkable != WALKFREE
					&& (it != path.begin() && it != (path.end()--)))
			{
				printf("HOW DID THIS HAPPEN? PATH CONTAINS USED POINT!!!!!\n");
				(*it)->printBlockJournal();
			}

			/*
			 * the end of a path is not blocked.
			 * path is constructed backwards in pathfinder.
			 */
			if (it == path.begin())
			{
				//ai grija la cele conectate la circuit. acolo trebuie USED
				//(*it)->walkable = WALKBLOCKED_GUIDE;
				(*it)->setWalkAndPriority(WALKFREE, PREVIOUSPATH,
						" -> path ends here");

			}
			else
			{
				(*it)->setWalkAndPriority(WALKUSED, PREVIOUSPATH,
						" -> path element");
			}
		}

		int prevIdx = -1;
		/*
		 * Add  segments between the corners of the defect connection
		 */
		for (std::vector<convertcoordinate>::iterator it = corners.begin();
				it != corners.end(); it++)
		{
			int idx = connectionsGeometry.addCoordinate(*it);

			if (prevIdx != -1)
			{
				connectionsGeometry.addSegment(idx, prevIdx);
			}
			prevIdx = idx;
		}
	}
	else
	{
		convertcoordinate distCoord = coordline.getPinDetail(SOURCEPIN).coord;
		convertcoordinate circCoord = coordline.getPinDetail(DESTPIN).coord;

		/*used for debugging purposes*/
		int idx1 = connectionsGeometry.addCoordinate(circCoord);
		int idx2 = connectionsGeometry.addCoordinate(distCoord);
		connectionsGeometry.addSegment(idx1, idx2);
	}
}

bool connectpins::connectWithAStar(pinpair& coordline, std::vector<Point*>& path)
{
	pathfinder.allowConnectionThroughChannel = coordline.allowConnectionThroughChannel;

	convertcoordinate distCoord = coordline.getPinDetail(SOURCEPIN).coord;
	convertcoordinate circCoord = coordline.getPinDetail(DESTPIN).coord;
//
//	printf("======\nsrc:%s dst:%s mindist:%ld \n", distCoord.toString(',').c_str(),
//			circCoord.toString(',').c_str(),
//			coordline.minDistBetweenPins());

	int axes[] = {CIRCUITWIDTH, CIRCUITDEPTH, CIRCUITHEIGHT};
//	std::vector<Point*> path;
	PathfinderCode err = PathFinderOtherErr;/*init err*/

	//HEURISTIC: NUMBER OF STEPS TO TRY CONSTRUCT PATH *50
	heuristicParam.init();
	unsigned int steps = circCoord.manhattanDistance(distCoord) * heuristicParam.manhattanMultiplier;
	while(err != PathFinderOK)
	{
//		printf("    with %d steps ", steps);
		for(int nrtries = 0; nrtries < 2 && (err != PathFinderOK); nrtries++)
		{
			Point* start = pathfinder.getOrCreatePoint(distCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH], distCoord[CIRCUITHEIGHT], false);
			Point* end = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], circCoord[CIRCUITHEIGHT], false);

			Point* otherStart = NULL;
			Point* otherEnd = NULL;
			if(!pathfinder.isPointAwayFromBoxes(distCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH]/* + DELTA*/, distCoord[CIRCUITHEIGHT]))
			{
				/*if the starting point was correctly computed
				 * being not away from the boxes means that
				 * the point is box pin, therefore take the next point
				 * and reinsert the current start after a path was computed*/

				otherStart = pathfinder.getOrCreatePoint(distCoord[CIRCUITWIDTH], distCoord[CIRCUITDEPTH] + DELTA, distCoord[CIRCUITHEIGHT], true);
			}

			if(!pathfinder.isPointAwayFromBoxes(circCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], circCoord[CIRCUITHEIGHT]))
			{
				/*if the starting point was correctly computed
				 * being not away from the boxes means that
				 * the point is a geometry box pin, therefore take the next point
				 * and reinsert the current start after a path was computed*/

//				int randH = ((int)(drand48() * 100) * DELTA);
//				printf("randH%d\n", randH);

				int randH = DELTA;
				//the circCoord is in the channel of the geometry
				//setting true at getOrCreatePoint will block the point, although it should be free
				//TODO: set to false
				otherEnd = pathfinder.getOrCreatePoint(circCoord[CIRCUITWIDTH], circCoord[CIRCUITDEPTH], circCoord[CIRCUITHEIGHT] + randH, false);
			}

			//23.05.2017
			if(coordline.hasSourceAndDestinationReversed)
			{
				interchangeStartEnd(start, end);
				interchangeStartEnd(otherStart, otherEnd);
			}

			err = pathfinder.aStar(otherStart != NULL ? otherStart : start,
							otherEnd !=  NULL ? otherEnd : end,
							axes, steps, path, coordline.hasSourceAndDestinationReversed ? -1 : 1);

			if(otherStart != NULL && err == PathFinderOK)
			{
				path.push_back(start);
			}

			if(otherEnd != NULL && err == PathFinderOK)
			{
				path.insert(path.begin(), end);
			}

			switch(err)
			{
			case PathFinderOK:
//				printf("solved\n");
				break;
			case PathFinderOtherErr:
//		    	printf("not_solved\n");
				break;
			case PathFinderStartErr:
				printf("THE START IS BLOCKED (%d) by priority %d\n", start->walkable, start->getPriority());
				start->printBlockJournal();
				break;
			case PathFinderStopErr:
				printf("THE END IS BLOCKED (%d) by priority %d\n", end->walkable, end->getPriority());
				end->printBlockJournal();
				break;
			case PathFinderEndSoonerThanStart:
				printf("END SOONER THAN START \n");
				break;
			}
//			steps *= 100;
			Point::useSecondHeuristic = true;
		}
		Point::useSecondHeuristic = false;

		//steps *= 100;
		break;
	}

	return (path.size() > 0);
}

void connectpins::filterAndAddToCorners(std::vector<convertcoordinate>& corners, Point* current)
{
	//take the point instead
	if (corners.size() >= 2)
	{
		/*if the last two corners are colinear*/
		if(corners[corners.size() - 2].isColinear(current->coord))
		{
			corners.pop_back();
		}
	}

	corners.push_back(current->coord);
}

//bool connectpins::processPins(char* fname, int method)
//{
//	FILE* fp = fopen(fname, "r");
//
//	while(!feof(fp))
//	{
//		pinpair cl;
//		for(int i=0; i<6; i++)
//		{
//			long nr = -10000;
//
//			fscanf(fp, "%ld", &nr);
//			cl[OFFSETNONCOORD + i] = nr;
//		}
//
//		if(!feof(fp))
//		{
//			switch (method) {
//				case CANONICAL:
//					connectCanonical(cl);
//					break;
//				case ASTAR:
//					connectWithAStar(cl);
//					break;
//			}
//		}
//	}
//
//	fclose(fp);
//
//	return true;
//}

void connectpins::setWalkable(pindetails& detail, bool enforce, int whichBlockType)
{
	for(std::vector<pinblocker>::iterator it = detail.blocks.begin();
			it != detail.blocks.end(); it++)
	{
		/*
		 * Only blocks of type whichBlockType should be applied
		 */
		if(it->blockType != whichBlockType)
		{
			continue;
		}

		/*
		 * The block's priority
		 */
//		int currentBlockPriority = it->priority;
		int currentBlockPriority = it->journal.blockPriority;

		/*
		 * Take the initial coordinate and the distance
		 */
		convertcoordinate c = convertcoordinate(detail.coord);
		c[it->axis] += it->offset * DELTA;

		int distance = it->distance;
		int sign = 1;
		if(distance < 0)
		{
			sign = -1;
			distance = -distance;
		}

		//debug
		int deb1 = debugFirstCoordinate(it->blockType, c);

		/*
		 * Along the distance apply the block for each Point
		 */
		for(int dist = 0; dist < distance; dist++)
		{
			Point* po = pathfinder.getOrCreatePoint(c[CIRCUITWIDTH], c[CIRCUITDEPTH], c[CIRCUITHEIGHT], false /*no boxes should exist in the future, but geometry exists*/);

			if(po->walkable != WALKUSED)
			{
				if(enforce == ENFORCE_BLOCK)
				{
					/*guides are stronger than occupies*/
					if(po->walkable == WALKBLOCKED_OCCUPY
							&& it->blockType == WALKBLOCKED_GUIDE)
					{
						po->setWalkAndPriority(it->blockType, currentBlockPriority, it->journal.toString());
//						po->walkable = it->blockType;
//						po->setPriority(currentBlockPriority);
					}
					/*if free, then block*/
					if(po->walkable == WALKFREE)
					{
						po->setWalkAndPriority(it->blockType, currentBlockPriority, it->journal.toString());

//						po->walkable = it->blockType;
//						po->setPriority(currentBlockPriority);
					}
					/*higher priority kills lower priority*/
					if(po->walkable == it->blockType
							&& po->getPriority() < currentBlockPriority)
					{
						po->setWalkAndPriority(it->blockType, currentBlockPriority, it->journal.toString());

//						po->setPriority(currentBlockPriority);
					}
				}
				else if(enforce == REMOVE_BLOCK)
				{
					/*if blocked, then free*/
					if(po->walkable == it->blockType
							&& po->getPriority() == currentBlockPriority)
					{
						it->journal.blockType = WALKFREE;
						po->setWalkAndPriority(WALKFREE, currentBlockPriority, it->journal.toString());

//						po->walkable = WALKFREE;
//						po->removePriority();
					}
				}
			}

			c[it->axis] += sign * DELTA;
		}

		/*Debugging*/
		deb1 = debugSecondCoordinate(it->blockType, deb1, c, enforce);
	}
}

geometry* connectpins::whereToDebug(int blockType)
{
	if(blockType == WALKBLOCKED_OCCUPY)
	{
		return &debugBlockOccupyGeometry;
	}
	return &debugBlockGuideGeometry;
}

int connectpins::debugFirstCoordinate(int blockType, convertcoordinate& coord)
{
	geometry* debugGeom = whereToDebug(blockType);
	return debugGeom->addCoordinate(coord);
}

int connectpins::debugSecondCoordinate(int blockType, int deb1, convertcoordinate& coord, bool enforce)
{
	geometry* debugGeom = whereToDebug(blockType);
	int deb2 = debugGeom->addCoordinate(coord);
	if(enforce == ENFORCE_BLOCK)
	{
		if(!debugGeom->addSegment(deb1, deb2))
		{
//			printf("debug seg. not added ");
		}
	}
	if(enforce == REMOVE_BLOCK)
	{
		if(!debugGeom->removeSegment(deb1, deb2))
		{
//			printf("debug seg. not removed ");
		}
	}

	return deb2;
}

/**
 * The coordinates of the pins are not walkable in astar
 * @param pins is the list of pins
 */
void connectpins::blockPins(std::vector<pinpair>& pins)
{
	for(std::vector<pinpair>::iterator it = pins.begin();
						it!=pins.end(); it++)
	{
		setWalkable(it->getPinDetail(SOURCEPIN), ENFORCE_BLOCK, WALKBLOCKED_OCCUPY);
		setWalkable(it->getPinDetail(SOURCEPIN), ENFORCE_BLOCK, WALKBLOCKED_GUIDE);

		setWalkable(it->getPinDetail(DESTPIN), ENFORCE_BLOCK, WALKBLOCKED_OCCUPY);
		setWalkable(it->getPinDetail(DESTPIN), ENFORCE_BLOCK, WALKBLOCKED_GUIDE);
	}
}

void connectpins::unblockPins(std::vector<pinpair>& pins)
{
	for(std::vector<pinpair>::iterator it = pins.begin();
						it!=pins.end(); it++)
	{
		setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_OCCUPY);
		setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);

		setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_OCCUPY);
		setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);
	}
}

void connectpins::markSegmentInPathFinderUsed(Point* start, Point* stop, size_t axis)
{
	long coord[3] = {0, 0, 0};
	coord[CIRCUITWIDTH] = start->coord[CIRCUITWIDTH];
	coord[CIRCUITDEPTH] = start->coord[CIRCUITDEPTH];
	coord[CIRCUITHEIGHT] = start->coord[CIRCUITHEIGHT];

	while(coord[axis] <= stop->coord[axis])
	{
		coord[axis] += DELTA;
		Point* x = pathfinder.getOrCreatePoint(coord[CIRCUITWIDTH], coord[CIRCUITDEPTH], coord[CIRCUITHEIGHT], false);
		x->setWalkAndPriority(WALKUSED, NOPRIORITY, "");
	}
}


bool connectpins::processPins(std::vector<pinpair>& pins, int method)
{
	int count = 0;
	for(std::vector<pinpair>::iterator it = pins.begin();
			it != pins.end(); it++)
	{
//		if(count % 2 == 1)
//			continue;

		count++;
		printf("nr %d/%d ", count, pins.size());

		std::vector<Point*> path;
		path.clear();

		printf("======\nsrc:%s dst:%s mindist:%ld \n",
				(*it).getPinDetail(SOURCEPIN).coord.toString(',').c_str(),
				(*it).getPinDetail(DESTPIN).coord.toString(',').c_str(),
				(*it).minDistBetweenPins());

		switch (method) {
			case CANONICAL1:
				connectCanonicalOne(*it, path);
				break;
			case CANONICAL2:
				connectCanonicalTwo(*it, path);
				break;
			case ASTAR:
				setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_OCCUPY);
				setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_OCCUPY);

				setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);
				setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);

				int distIns = it->minDistBetweenPins();
				if(distIns == 0)
				{
					printf ("skip. zero length\n");
				}
				else
				{
					if(!connectWithAStar(*it, path))
					{
						return false;
					}
				}

				setWalkable(it->getPinDetail(SOURCEPIN), ENFORCE_BLOCK, WALKBLOCKED_GUIDE);
				setWalkable(it->getPinDetail(DESTPIN), ENFORCE_BLOCK, WALKBLOCKED_GUIDE);

				break;
		}

		processFoundPath(*it, path);

//		if(count == 150)
//			break;
	}

//	/**
//	 * Unblock the unblockable
//	 */
	for(std::vector<pinpair>::iterator it = pins.begin(); it != pins.end(); it++)
	{
//		setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);
//		setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);

//		it->getPinDetail(SOURCEPIN).removeBlocks();//blocks.clear();
//		it->getPinDetail(DESTPIN).removeBlocks();//blocks.clear();
	}

	return true;
}
