#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <limits.h>
#include <map>
#include <string>
#include <queue>

#include "boxworld2.h"
#include "rtree/RStarTree.h"
#include "astar/astaralg.h"
#include "recycling/graphanalysis.h"

#define NORMALTYPE -1

void boxworld2::switchLayoutConfig(int configNumber)
{
	switch(configNumber)
	{
	case NORMALTYPE:
		currentConfig._pinScenario = PINSVERTICAL;
		currentConfig._heightStepSize = 0;
		break;
	case ATYPE:
		currentConfig._pinScenario = PINSHORIZLEFT;
		currentConfig._heightStepSize = DELTA;
		break;
	case YTYPE:
		currentConfig._pinScenario = PINSHORIZRIGHT;
		currentConfig._heightStepSize = DELTA;
		break;
	}
}

void boxworld2::init()
{
	switchLayoutConfig(NORMALTYPE);
}

void boxworld2::pinsVertical(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH];
	y = currBoxCoords[CIRCUITDEPTH] + currentConfig.boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT] + DELTA + pinNr * currentConfig.distBetweenBoxes[CIRCUITHEIGHT] + DELTA;
}

void boxworld2::pinsVerticalLeft(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH] + currentConfig.boxSize[boxType][CIRCUITWIDTH] - DELTA;
	y = currBoxCoords[CIRCUITDEPTH] + currentConfig.boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT] + (1 - pinNr) * currentConfig.distBetweenBoxes[CIRCUITHEIGHT] + DELTA;
}

void boxworld2::pinsHorizontalRight(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH] + /*DELTA + */pinNr * currentConfig.distBetweenBoxes[CIRCUITWIDTH];
	y = currBoxCoords[CIRCUITDEPTH] + currentConfig.boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT];// + DELTA/*put the contacts a bit higher*/;
}

void boxworld2::pinsHorizontalLeft(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH] + currentConfig.boxSize[boxType][CIRCUITWIDTH] - DELTA - (1-pinNr) * currentConfig.distBetweenBoxes[CIRCUITWIDTH];
	y = currBoxCoords[CIRCUITDEPTH] + currentConfig.boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT] + DELTA/*put the contacts a bit higher*/;
}

void boxworld2::saveBoxAndPinCoords(int nr, int boxType)
{
	/**
	 * Box Coords
	 */
	//coltul din stanga jos spate este al unei cutii
	boxcoord pc;
	pc[TYPE] = boxType;
	pc[INJNR] = nr;
	pc[OFFSETNONCOORD + CIRCUITWIDTH] = currBoxCoords[CIRCUITWIDTH];
	pc[OFFSETNONCOORD + CIRCUITDEPTH] = currBoxCoords[CIRCUITDEPTH];
	pc[OFFSETNONCOORD + CIRCUITHEIGHT] = currBoxCoords[CIRCUITHEIGHT];

	boxCoords.push_back(pc);

	/**
	 * Pin Coords
	 */
	pinpair bp;
	bp[TYPE] = boxType;

	for (int i = 0; i < 2; i++)
	{
		long x,y,z;
		switch (currentConfig._pinScenario) {
		case PINSVERTICAL:
			pinsVertical(i, boxType, x, y, z);
			break;
		case PINSVERTICALLEFT:
			pinsVerticalLeft(i, boxType, x, y, z);
			break;
		case PINSHORIZRIGHT:
			pinsHorizontalRight(i, boxType, x, y, z);
			break;
		case PINSHORIZLEFT:
			pinsHorizontalLeft(i, boxType, x, y, z);
			break;
		default:
			break;
		}

		/*wrong thing to do, the order of the pins is 1,0 instead of 0,1
		  this has historical reasons...*/
//		int pos = OFFSETNONCOORD + 3*(1-i);
		/*
		 * 3 NOV
		 */
		int pos = OFFSETNONCOORD + 3*i;
		bool dualPins = true;
		if(dualPins)
		{
			//the coordinates have to be even?
			bp[pos + CIRCUITWIDTH] = x + 3;
			bp[pos + CIRCUITDEPTH] = y - 3;
			bp[pos + CIRCUITHEIGHT] = z - 3;
		}
		else
		{
			bp[pos + CIRCUITWIDTH] = x;
			bp[pos + CIRCUITDEPTH] = y ;
			bp[pos + CIRCUITHEIGHT] = z;
		}

	}

	boxPins.push_back(bp);
}

twobbox boxworld2::computeBBox()
{
	twobbox ret;

	for(std::vector<boxcoord>::iterator it = boxCoords.begin(); it != boxCoords.end(); it++)
	{
		convertcoordinate c = it->getPinDetail(0).coord;
		ret.updateBBox(c, (*it)[TYPE], currentConfig.boxSize);
	}

	return ret;
}

BoundingBox boxworld2::generateBounds(int width, int depth, int height, int boxTypeIndex)
{
	BoundingBox bb;

	bb.edges[0].first  = width - currentConfig.getBoxTotalDimension(boxTypeIndex, CIRCUITWIDTH);
	bb.edges[0].second = width;

	bb.edges[1].first  = depth;
	bb.edges[1].second = depth + currentConfig.getBoxTotalDimension(boxTypeIndex, CIRCUITDEPTH);

	bb.edges[2].first = height;
	bb.edges[2].second = height + currentConfig.getBoxTotalDimension(boxTypeIndex, CIRCUITHEIGHT);

	return bb;
}

bool boxworld2::overlapsConnectionChannel(BoundingBox bBox, int axis, int direction)
{
	bBox.edges[axis].first += direction;
	bBox.edges[axis].second += direction;

	return connectionsBox.overlaps(bBox)/* || connectionsBox.encloses(bBox)*/;
}

bool boxworld2::canSlide(BoundingBox bBox, int axis, int direction)
{
	bBox.edges[axis].first += direction;
	bBox.edges[axis].second += direction;

	//as long as the box overlaps other boxes, slide its coordinates upwards
	bool canSlide = rtree.Query(RTree::AcceptOverlapping(bBox), Visitor()).count == 0;
	bool overlapsChan = !overlapsConnectionChannel(bBox, axis, 0 /*direction*/);
//	printf("slide%d ch%d\n", canSlide, overlapsChan);
	canSlide = canSlide && overlapsChan;

	return canSlide;
}

void boxworld2::addBoxSlidingDepth(BoundingBox& bBox, int boxTypeIndex)
{
	//as long as the box does not overlap other boxes, slide its coordinates upwards
	while(rtree.Query(RTree::AcceptOverlapping(bBox), Visitor()).count == 0)
	{
		bBox.edges[CIRCUITDEPTH].first -= DELTA;
		bBox.edges[CIRCUITDEPTH].second -= DELTA;
	}

	bBox.edges[CIRCUITDEPTH].first += DELTA;
	bBox.edges[CIRCUITDEPTH].second += DELTA;

	rtree.Insert(rtree.GetSize(), bBox);

	currBoxCoords[CIRCUITWIDTH] = bBox.edges[0].first;
	currBoxCoords[CIRCUITDEPTH] = bBox.edges[1].first;
	currBoxCoords[CIRCUITHEIGHT] = bBox.edges[2].first;
}

void boxworld2::addBoxSlidingDepth2(BoundingBox& bBox, int boxTypeIndex)
{
	//as long as the box overlaps other boxes, slide its coordinates upwards
	while(rtree.Query(RTree::AcceptOverlapping(bBox), Visitor()).count == 0)
	{
		bBox.edges[1].first--;
		bBox.edges[1].second--;
	}
	rtree.Insert(rtree.GetSize(), bBox);

	currBoxCoords[CIRCUITWIDTH] = bBox.edges[0].first;
	currBoxCoords[CIRCUITDEPTH] = bBox.edges[1].first;
	currBoxCoords[CIRCUITHEIGHT] = bBox.edges[2].first;
}

void boxworld2::addBoxSlidingHeight(BoundingBox& bBox, int boxTypeIndex)
{
	//as long as the box overlaps other boxes, slide its coordinates upwards
	while(rtree.Query(RTree::AcceptOverlapping(bBox), Visitor()).count > 0)
	{
		bBox.edges[2].first++;
		bBox.edges[2].second++;
	}
	rtree.Insert(rtree.GetSize(), bBox);

	currBoxCoords[CIRCUITWIDTH] = bBox.edges[0].first;
	currBoxCoords[CIRCUITDEPTH] = bBox.edges[1].first;
	currBoxCoords[CIRCUITHEIGHT] = bBox.edges[2].first;
}

void boxworld2::computeAdditionalATypeScheduler(long boxStartTimeCoordinate, int difStates)
{
	int offset[3];
	offset[CIRCUITWIDTH] = -currentConfig.distBetweenBoxes[CIRCUITWIDTH] - currentConfig.boxSize[ATYPE][CIRCUITWIDTH] + 1;
	offset[CIRCUITDEPTH] = /*-2*/ boxStartTimeCoordinate * DELTA + DELTA + 1;//-6 * DELTA - currentConfig.boxSize[ATYPE][CIRCUITDEPTH] + 1;
	offset[CIRCUITHEIGHT] = 2 * DELTA + 1;

	placeAdditionalsAlongWidthAxis(difStates, ATYPE, -1, offset);
}

void boxworld2::computeAdditionalYTypeScheduler(long boxStartTimeCoordinate, int difStates)
{
	int offset[3];
	offset[CIRCUITWIDTH] = geomBoundingBox.edges[CIRCUITWIDTH].second
			+ currentConfig.distBetweenBoxes[CIRCUITWIDTH]
			+ currentConfig.boxSize[YTYPE][CIRCUITWIDTH]
			+ 1;
	offset[CIRCUITDEPTH] = boxStartTimeCoordinate*DELTA + 2* DELTA + 1;//-5 * DELTA - currentConfig.boxSize[YTYPE][CIRCUITDEPTH] + 1;
	offset[CIRCUITHEIGHT] = 2 * DELTA + 1;

	placeAdditionalsAlongWidthAxis(difStates, YTYPE, 1, offset);
}

std::queue<int> boxworld2::computeScheduleCanonical(long boxStartTimeCoordinate, causalgraph& causal /*vector<pinpair>& toconn*/)
{
	long offset[] = {0, -5 * DELTA, 8 * DELTA};

	int nrA = graphanalysis::getTotalAStates(causal);
	int nrY = graphanalysis::getTotalYStates(causal);

	computeadditional cp;
	int maxAStates = cp.findParam(nrA, greedyLevel.aStateFail, greedyLevel.tGateFail);
	int difAStates = maxAStates - nrA;

	int maxYStates = cp.findParam(nrY, greedyLevel.yStateFail, greedyLevel.pGateFail);
	int difYStates = maxYStates - nrY;

	std::vector<std::vector<int> > injectionTypeAndRow = graphanalysis::getTypesAndWiresOfInjections(causal);

	std::queue<int> successfulABoxIds;
	preSimulateFailuresInGreedy(successfulABoxIds, difAStates, ATYPE, greedyLevel);

	std::queue<int> successfulYBoxIds;
	preSimulateFailuresInGreedy(successfulYBoxIds, difYStates, YTYPE, greedyLevel);

	std::queue<int> greedyBoxPinIds;
	/*
	 * Place the schedules
	 */
	switchLayoutConfig(NORMALTYPE);
	for (std::vector<std::vector<int> >::iterator it = injectionTypeAndRow.begin(); it != injectionTypeAndRow.end(); it++)
	{
		int boxType = (*it)[0];
		int circuitRow = (*it)[1]; //row as coordinate of the injection point

		BoundingBox bBox = generateBounds(circuitRow * DELTA + offset[CIRCUITWIDTH] + 1,
				boxStartTimeCoordinate * DELTA - 0 * DELTA + 1,
				1 + offset[CIRCUITHEIGHT], boxType);

		/*
		 * Presimulate failures of boxes
		 */
		int nrsucc = preSimulateFailuresInGreedy(greedyBoxPinIds, 1, boxType, greedyLevel);
		if(nrsucc == 0)
		{
			if(boxType == ATYPE)
			{
				int val = successfulABoxIds.front();
				successfulABoxIds.pop();
				greedyBoxPinIds.push(val + injectionTypeAndRow.size());
			}
			else
			{
				int val = successfulYBoxIds.front();
				successfulYBoxIds.pop();
				greedyBoxPinIds.push(val + injectionTypeAndRow.size() + difAStates);
			}
		}

		addBoxSlidingHeight(bBox, boxType);

		saveBoxAndPinCoords(FAKEID, boxType);
	}

	computeAdditionalATypeScheduler(boxStartTimeCoordinate, difAStates);
	computeAdditionalYTypeScheduler(boxStartTimeCoordinate, difYStates);

	return greedyBoxPinIds;
}

void boxworld2::placeAdditionalsAlongWidthAxis(int total, int boxType, int direction, int offset[3])
{
	switchLayoutConfig(boxType);

	int widthIncrement = currentConfig.boxSize[boxType][CIRCUITWIDTH] + currentConfig.distBetweenBoxes[CIRCUITWIDTH];

	int maxBoxesPerLine = widthIncrement / abs(currentConfig._heightStepSize);

	// -1 left, decreasing coordinates
	// 1 right, increasing coordinates
	widthIncrement *= direction;

	int currIdx = 0;
	for(int i=0; i<total; i++)
	{
		if(currIdx >= maxBoxesPerLine)
		{
			currIdx = 0;
		}

		BoundingBox activeBox = generateBounds(offset[0], offset[1], offset[2], boxType);

		activeBox.edges[0].first += currIdx * widthIncrement;
		activeBox.edges[0].second += currIdx * widthIncrement;

		activeBox.edges[2].first += currIdx * currentConfig._heightStepSize;
		activeBox.edges[2].second += currIdx * currentConfig._heightStepSize;

		addBoxSlidingHeight(activeBox, boxType);

		saveBoxAndPinCoords(FAKEID, boxType);

		currIdx++;
	}
}

#define FORWARD 1
#define BACKWARD -1

BoundingBox boxworld2::initNewBox(schedulerLevelInfo& level, int boxType)
{
	/**
	 * in functie de level.lastMoveIndex ia unul din colturi
	 * si genereaza noua cutie
	 *  3____2
	 *  |    |
	 *  |____|
	 *  0    1
	 */
	long x, t/*time*/, z;

	t = level.depth;

	long dx = currentConfig.getBoxTotalDimension(boxType, CIRCUITWIDTH);
	long dt = currentConfig.getBoxTotalDimension(boxType, CIRCUITDEPTH);
	long dz = currentConfig.getBoxTotalDimension(boxType, CIRCUITHEIGHT);

	BoundingBox startBox;

	switch(level.lastMoveIndex)
	{
	case 0:
		x = level.lastBounding.edges[CIRCUITWIDTH].second;
		z = level.lastBounding.edges[CIRCUITHEIGHT].first;
		dx *= 1;
		dz *= 1;
		startBox.edges[CIRCUITWIDTH].first = x;
		startBox.edges[CIRCUITHEIGHT].first = z;
		startBox.edges[CIRCUITWIDTH].second = x + dx;
		startBox.edges[CIRCUITHEIGHT].second = z + dz;
		break;
	case 1:
		x = level.lastBounding.edges[CIRCUITWIDTH].first;
		z = level.lastBounding.edges[CIRCUITHEIGHT].first;
		dx *= 1;
		dz *= -1;
		startBox.edges[CIRCUITWIDTH].first = x;
		startBox.edges[CIRCUITHEIGHT].first = z + dz;
		startBox.edges[CIRCUITWIDTH].second = x + dx;
		startBox.edges[CIRCUITHEIGHT].second = z;
		break;
	case 2:
		x = level.lastBounding.edges[CIRCUITWIDTH].first;
		z = level.lastBounding.edges[CIRCUITHEIGHT].second;
		dx *= -1;
		dz *= -1;
		startBox.edges[CIRCUITWIDTH].first = x + dx;
		startBox.edges[CIRCUITHEIGHT].first = z + dz;
		startBox.edges[CIRCUITWIDTH].second = x;
		startBox.edges[CIRCUITHEIGHT].second = z;
		break;
	case 3:
		x = level.lastBounding.edges[CIRCUITWIDTH].second;
		z = level.lastBounding.edges[CIRCUITHEIGHT].second;
		dx *= -1;
		dz *= 1;
		startBox.edges[CIRCUITWIDTH].first = x + dx;
		startBox.edges[CIRCUITHEIGHT].first = z;
		startBox.edges[CIRCUITWIDTH].second = x;
		startBox.edges[CIRCUITHEIGHT].second = z + dz;
		break;
	}

	startBox.edges[CIRCUITDEPTH].first = t;
	startBox.edges[CIRCUITDEPTH].second = t + dt;

	return startBox;
}

int boxworld2::preSimulateFailuresInGreedy(std::queue<int>& boxPinIds, int totalToSim, int boxType, schedulerLevelInfo& greedyLevel)
{
	double prob = (boxType == ATYPE ? greedyLevel.aStateFail : greedyLevel.yStateFail);
	int nrsucc = 0;
	for(int i = 0; i < totalToSim; i ++)
	{
		bool success = fd.randomCheckThreshold(prob);
		if(success)
		{
			nrsucc++;
			boxPinIds.push(boxPins.size() + i);
		}
	}
	return nrsucc;
}

void boxworld2::initGeomBoundingBox(long wf, long ws, long df, long ds, long hf, long hs)
{
	geomBoundingBox.edges[CIRCUITWIDTH].first = wf;
	geomBoundingBox.edges[CIRCUITWIDTH].second = ws;
	geomBoundingBox.edges[CIRCUITDEPTH].first = df;
	geomBoundingBox.edges[CIRCUITDEPTH].second = ds;
	geomBoundingBox.edges[CIRCUITHEIGHT].first = hf;
	geomBoundingBox.edges[CIRCUITHEIGHT].second = hs;
}

void boxworld2::initScheduleGreedy(double aStateFail, double yStateFail, double tGateFail, double pGateFail)
{
	greedyLevel.aStateFail = aStateFail;
	greedyLevel.yStateFail = yStateFail;
	greedyLevel.tGateFail = tGateFail;
	greedyLevel.pGateFail = pGateFail;

	//cutia geometriei
	rtree.Insert(rtree.GetSize(), geomBoundingBox);

	//add floor
	BoundingBox floor;
	floor.edges[0].first = INT_MIN;
	floor.edges[0].second = INT_MAX;

	/*TODO:HEURISTIC*/
	floor.edges[1].first = -550;
	floor.edges[1].second = -549;

	floor.edges[2].first = INT_MIN;
	floor.edges[2].second = INT_MAX;

	rtree.Insert(rtree.GetSize(), floor);

	greedyLevel.greedyAxis[0] = CIRCUITWIDTH;
	greedyLevel.greedyAxis[1] = CIRCUITHEIGHT;
	greedyLevel.greedyAxis[2] = CIRCUITWIDTH;
	greedyLevel.greedyAxis[3] = CIRCUITHEIGHT;
	greedyLevel.greedyDirection[0] = DELTA * FORWARD;
	greedyLevel.greedyDirection[1] = DELTA * BACKWARD;
	greedyLevel.greedyDirection[2] = DELTA * BACKWARD;
	greedyLevel.greedyDirection[3] = DELTA * FORWARD;

	//use index 0 for axis and direction
	greedyLevel.lastMoveIndex = 0;
	/*TODO: Heuristic*/
	greedyLevel.maxDepth = -600;
//	greedyLevel.calibration = true;
	setCalibration(true);
	greedyLevel.lastScheduledBoxType = -1;

	/*
	 * Connections Box is surrounding the geomBoundingBox.
	 * It is not added to the rtree, but used to place the boxes
	 */
	connectionsBox = geomBoundingBox;

	long geomWidth = geomBoundingBox.edges[CIRCUITWIDTH].second - geomBoundingBox.edges[CIRCUITWIDTH].first;
	long geomHeight = geomBoundingBox.edges[CIRCUITHEIGHT].second - geomBoundingBox.edges[CIRCUITHEIGHT].first;

//	setConnectionBoxWidth(geomWidth);
//	15.05.2017
	setConnectionBoxWidth(heuristicParam->connPoolStart * DELTA);

	/*TODO: Heuristic*/
	setConnectionBoxHeight(geomHeight + heuristicParam->connectionBoxHeight * DELTA);
}

bool boxworld2::setConnectionBoxWidth(int totalWidth)
{
	int geomWidth = geomBoundingBox.edges[CIRCUITWIDTH].second - geomBoundingBox.edges[CIRCUITWIDTH].first;

	int dif = totalWidth - geomWidth;

	connectionsBox.edges[CIRCUITWIDTH].second = geomBoundingBox.edges[CIRCUITWIDTH].second + dif;

	connectionsBox.edges[CIRCUITWIDTH].second +=  heuristicParam->connectionBoxWidthMargin * DELTA;

	return true;
}

bool boxworld2::setConnectionBoxHeight(int totalHeight)
{
	int geomHeight = geomBoundingBox.edges[CIRCUITHEIGHT].second - geomBoundingBox.edges[CIRCUITHEIGHT].first;

	int dif = totalHeight - geomHeight;

	int firstDif = DELTA;
	int secondDif = dif - firstDif;

	connectionsBox.edges[CIRCUITHEIGHT].first = geomBoundingBox.edges[CIRCUITHEIGHT].first - firstDif;
	connectionsBox.edges[CIRCUITHEIGHT].second = geomBoundingBox.edges[CIRCUITHEIGHT].second + secondDif;

	return true;
}

/*
 * Calibration is a process in which the first box that is placed around the circuit
 * is moved such that it does not overlap with the connections bounding box
 */
void boxworld2::setCalibration(bool value)
{
	greedyLevel.calibration = value;
}

void boxworld2::placeBoxesInGreedy(long reqLevelDepth /*circuitInputTimeDepth*/, int& boxType, int nrBoxesToAdd)
{
#ifdef LOGPLACING
	printf("==BEGIN PLacing\n");
#endif

	if(greedyLevel.lastScheduledBoxType == -1)
	{
		greedyLevel.lastScheduledBoxType = boxType;
	}

	reqLevelDepth = reqLevelDepth * DELTA + 1;//transform the coordinate to a proper space-time coordinate

	if(greedyLevel.calibration)
	{
		//merg cu un nivel mai sus
		greedyLevel.depth = reqLevelDepth;

		greedyLevel.lastMoveIndex = 0;
		greedyLevel.lastBounding = generateBounds(1, 0 /*fake value, will be changed below*/, 1, boxType);

		greedyLevel.lastBounding.edges[CIRCUITDEPTH].first = reqLevelDepth;
		greedyLevel.lastBounding.edges[CIRCUITDEPTH].second = reqLevelDepth + currentConfig.boxSize[boxType][CIRCUITDEPTH];
	}

	/**
	 * Now add the boxes
	 */
	for (int i = 0; i < nrBoxesToAdd; i++)
	{
		/*
		 * Prima oara intreaba-te in ce directie poate merge
		 * daca nu poate merge in directia normala atunci ia-o pe cea anteriora
		 * mergi cu ea pana nu se opinteste in nimic
		 * apoi intreaba: pot lua curba?
		 */

		//muta incat sa nu depaseasca timecoordinate
		int ccwAxis = greedyLevel.getAxis(0);
		int ccwDir = greedyLevel.getDirection(0);

		/*
		 * Verific directia normala.
		 * In order to check that there is space, one has to advance
		 * the current bounding box. Advancing it should be performed with the
		 * dimensions of the previous type, otherwise small box types will be enclosed
		 * by the current bounding box.
		 */

		/**
		 * Check the normal direction, called ccwAxis
		 */
		if (!canSlide(greedyLevel.lastBounding, ccwAxis,
				(ccwDir / DELTA) * currentConfig.getBoxTotalDimension(greedyLevel.lastScheduledBoxType, ccwAxis)))
		{
			//contract was possible, assume that next direction can be used
			#ifdef LOGPLACING
			printf(" directia normala nu e ok\n");
			#endif
			//update ccwAxis and ccwDir, because lastMoveIndex was changed
			greedyLevel.lastMoveIndex = greedyLevel.getMoveIndex(BACKWARD);
			ccwAxis = greedyLevel.getAxis(0);
			ccwDir = greedyLevel.getDirection(0);
		}

		/**
		 * Could it not slide because it would hit the connection channel?
		 * If so, change the direction to backwards
		 */
		if(!greedyLevel.calibration && overlapsConnectionChannel(greedyLevel.lastBounding, ccwAxis,
				(ccwDir / DELTA) * currentConfig.getBoxTotalDimension(greedyLevel.lastScheduledBoxType, ccwAxis)))
		{
			//update ccwAxis and ccwDir, because lastMoveIndex was changed
			greedyLevel.lastMoveIndex = greedyLevel.getMoveIndex(BACKWARD);
			ccwAxis = greedyLevel.getAxis(0);
			ccwDir = greedyLevel.getDirection(0);
		}

		/*
		 * Pozitioneaza cutia, initializand-o.
		 */
		BoundingBox startBox = initNewBox(greedyLevel, boxType);
		#ifdef LOGPLACING
		printf("%d axis%d dir%d\n", i, ccwAxis, ccwDir);
		#endif
		/*
		 * Si acuma se poate ca o cutie sa nu dea bine
		 * Misc-o pana se aseaza. Dar cu directia care e normala acuma
		 */
		/*
		 * The box was constructed such that it is right next to the previous one
		 * It can happen that it does not fit, then it is moved
		 * Moving the box until it fits somewhere
		 */
		while (rtree.Query(RTree::AcceptOverlapping(startBox), Visitor()).count > 0
				|| connectionsBox.overlaps(startBox)/* || connectionsBox.encloses(startBox)*/)
		{
			//cannot move in the assumed direction ->expand means it used the previous direction to escape
			//				overlappedAndWasSlided(startBox, expandCcwAxis, expandCcwDirection);
			#ifdef LOGPLACING
			printf("move ");
			#endif

			startBox.edges[ccwAxis].first += ccwDir;
			startBox.edges[ccwAxis].second += ccwDir;
		}

		int contractCcwAxis = greedyLevel.getAxis(FORWARD);
		int contractCcwDirection = greedyLevel.getDirection(FORWARD);

		#ifdef LOGPLACING
		printf("test slide on axis %d \n", contractCcwAxis);
		#endif

		long boxDimension = currentConfig.getBoxTotalDimension(boxType, contractCcwAxis);
		long highestPossibleSlideDimension = 0;
		for(long dimIncrease = DELTA; dimIncrease <= boxDimension; dimIncrease += DELTA )
		{
			if (canSlide(startBox, contractCcwAxis,
					(contractCcwDirection / DELTA) * dimIncrease))
			{
				highestPossibleSlideDimension = dimIncrease;
				#ifdef LOGPLACING
				printf(" ---> %d increase %ld %ld\n", i, dimIncrease, boxDimension);
				#endif
			}
		}

		if(highestPossibleSlideDimension != 0)
		{
			if(highestPossibleSlideDimension == boxDimension)
			{
				//contract was possible, assume that next direction can be used
				greedyLevel.lastMoveIndex = greedyLevel.getMoveIndex(FORWARD);
			}
			else
			{
				/*17.02.2017 - contract*/
				startBox.edges[contractCcwAxis].first += (contractCcwDirection / DELTA) * highestPossibleSlideDimension;
				startBox.edges[contractCcwAxis].second += (contractCcwDirection / DELTA) * highestPossibleSlideDimension;
			}

			if(greedyLevel.calibration)
			{
				setCalibration(false);
			}
			#ifdef LOGPLACING
			printf(" ok\n");
			#endif
		}

		//verifica sa nu se duca la infinit in jos - pune o cutie care sa fie podeaua?
		rtree.Insert(rtree.GetSize(), startBox);

		currBoxCoords[CIRCUITWIDTH] = startBox.edges[0].first;
		currBoxCoords[CIRCUITDEPTH] = startBox.edges[1].first;
		currBoxCoords[CIRCUITHEIGHT] = startBox.edges[2].first;

		saveBoxAndPinCoords(FAKEID, boxType); //is id really required in this version?

		/*
		 * Save the current information regarding the reached level
		 */
		greedyLevel.lastBounding = startBox;
		greedyLevel.lastScheduledBoxType = boxType;

		/*
		 * Update maxdepth
		 */
		greedyLevel.maxDepth =
				greedyLevel.maxDepth < startBox.edges[1].second ?
						startBox.edges[1].second : greedyLevel.maxDepth;
	}

#ifdef LOGPLACING
	printf("==END Placing\n");
#endif
}

void boxworld2::scheduleSingleBox(long boxStartTimeCoordinate, int boxType)
{
	/*
	 * Addition of boxes
	 */

	//placeBoxesInGreedy(boxStartTimeCoordinate, boxType, nrBoxesToAdd);
	/*
	 * 3 NOV - Dual coordinates of the pins
	 */
	currBoxCoords[CIRCUITWIDTH] = 1 - DELTA;
	currBoxCoords[CIRCUITDEPTH] = boxStartTimeCoordinate * DELTA + 1;
	currBoxCoords[CIRCUITHEIGHT] = 2 * DELTA + 1;

//	currBoxCoords[CIRCUITWIDTH] = 4 - DELTA;
//	currBoxCoords[CIRCUITDEPTH] = boxStartTimeCoordinate * DELTA - 4;
//	currBoxCoords[CIRCUITHEIGHT] = 2 * DELTA + 4;

	//getPrevHalfDelta(1) == 4 in versiunea curenta

	saveBoxAndPinCoords(FAKEID, boxType);
	/*
	 * Update maxdepth
	 */
	long newDepth = currentConfig.getBoxTotalDimension(boxType, CIRCUITDEPTH)
			+ currBoxCoords[CIRCUITDEPTH];
	greedyLevel.maxDepth =
			greedyLevel.maxDepth < newDepth ? newDepth : greedyLevel.maxDepth;

	BoundingBox startBox;
	startBox.edges[0].first = currBoxCoords[CIRCUITWIDTH];
	startBox.edges[0].second = currBoxCoords[CIRCUITWIDTH] + currentConfig.getBoxTotalDimension(boxType, CIRCUITWIDTH);
	startBox.edges[1].first = currBoxCoords[CIRCUITDEPTH];
	startBox.edges[1].second = currBoxCoords[CIRCUITDEPTH] + currentConfig.getBoxTotalDimension(boxType, CIRCUITDEPTH);
	startBox.edges[2].first = currBoxCoords[CIRCUITHEIGHT];
	startBox.edges[2].second = currBoxCoords[CIRCUITHEIGHT] + currentConfig.getBoxTotalDimension(boxType, CIRCUITHEIGHT);
	rtree.Insert(rtree.GetSize(), startBox);
}

std::queue<int> boxworld2::greedyScheduleBoxes(long boxStartTimeCoordinate, int boxType, int available, int necessary)
{
	std::queue<int> greedyBoxPinIds;

//	computeadditional ca;
//	int total = ca.findParam(necessary - available,
//			boxType == ATYPE ? greedyLevel.aStateFail : greedyLevel.yStateFail,
//			boxType == ATYPE ? greedyLevel.tGateFail : greedyLevel.pGateFail);
//
//	int nrBoxesToAdd = total;

	/*
	 * ADD ONE BOX AT A TIME
	 */
	int nrBoxesToAdd = 1;

	printf(" add %d boxes\n", nrBoxesToAdd);

	/*
	* Pre-Simulation:
	* Which of the boxes that will be added should be considered successful?
	*/
	preSimulateFailuresInGreedy(greedyBoxPinIds, nrBoxesToAdd, boxType, greedyLevel);

	/*
	 * Addition of boxes
	 */
	//placeBoxesInGreedy(boxStartTimeCoordinate, boxType, nrBoxesToAdd);
	scheduleSingleBox(boxStartTimeCoordinate, boxType);

	return greedyBoxPinIds;
}

std::queue<int> boxworld2::computeScheduleALAPT(long boxStartTimeCoordinate, int boxType, int available, int necessary)
{
	std::queue<int> ret;

	boxConfiguration initialBoxes = currentConfig;

	boxConfiguration conf[2];
	conf[ATYPE]._pinScenario = PINSHORIZLEFT;
	conf[ATYPE]._heightStepSize = DELTA;

	conf[YTYPE]._pinScenario = PINSHORIZRIGHT;
	conf[YTYPE]._heightStepSize = DELTA;

	double stateFail[2];
	stateFail[ATYPE] = greedyLevel.aStateFail;
	stateFail[YTYPE] = greedyLevel.yStateFail;

	double gateFail[2];
	gateFail[ATYPE] = greedyLevel.tGateFail;
	gateFail[YTYPE] = greedyLevel.pGateFail;

	int difstates = 0;
	computeadditional cp;
	difstates = cp.findParam(necessary, stateFail[boxType], gateFail[boxType]) - available;

	/*
	 * Simulate failures of boxes
	 */
	preSimulateFailuresInGreedy(ret, difstates + available, boxType, greedyLevel);

	int offset[3];
	if(boxType == ATYPE)
	{
		offset[CIRCUITWIDTH] = -conf[boxType].distBetweenBoxes[CIRCUITWIDTH] - conf[boxType].boxSize[ATYPE][CIRCUITWIDTH] + 1;
		offset[CIRCUITDEPTH] = boxStartTimeCoordinate * DELTA + 1;
		offset[CIRCUITHEIGHT] = 2 * DELTA + 1;
	}
	else
	{
		offset[CIRCUITWIDTH] = geomBoundingBox.edges[CIRCUITWIDTH].second
				+ conf[boxType].distBetweenBoxes[CIRCUITWIDTH]
				+ conf[boxType].boxSize[boxType][CIRCUITWIDTH]
				+ 1;
		offset[CIRCUITDEPTH] = boxStartTimeCoordinate * DELTA + 1;
		offset[CIRCUITHEIGHT] = 2 * DELTA + 1;
	}

	int direction = (boxType == ATYPE ? - 1 : 1);

	placeAdditionalsAlongWidthAxis(difstates, boxType, direction, offset);

	for(int i=0; i<necessary; i++)
	{
		//add the normal box
		BoundingBox bBox = generateBounds(/* TODO: circuitRow*/1,
				boxStartTimeCoordinate * DELTA + 1,
				heuristicParam->connectionHeight * DELTA + /*circuit + rail*/2 * DELTA + 1,
				boxType);

		switchLayoutConfig(NORMALTYPE);
		addBoxSlidingHeight(bBox, boxType);
		saveBoxAndPinCoords(FAKEID, boxType);
	}

	return ret;
}

boxworld2::boxworld2()
{
    init();
}

boxworld2::boxworld2(char* iofile, char* boundingfile)
{
	/*
	 * These are not used any more
	 * nandc is computed on the fly in makegeometry
	 * the boundingbox is approximated by knowing how many qubits the circuit has
	 */


    FILE* iof = fopen(iofile, "r");
	iofilereader ior;
	nandc = ior.readIOFile(iof);
	fclose(iof);

	//read bounding box - is the generated box correct?
	FILE* iob = fopen(boundingfile, "r");
	long x, y, z;
	fscanf(iob, "%ld, %ld, %ld", &x, &y, &z);
	fclose(iob);

	geomBoundingBox.edges[0].first = 0;
	geomBoundingBox.edges[1].first = 0;
	geomBoundingBox.edges[2].first = 0;
	geomBoundingBox.edges[0].second = x;
	geomBoundingBox.edges[1].second = y;
	geomBoundingBox.edges[2].second = z;

	init();
}

long boxworld2::getTimeWhenBoxesEnd()
{
	return greedyLevel.maxDepth / DELTA;
}


int schedulerLevelInfo::getMoveIndex(int offset)
{
	return (4 + lastMoveIndex + offset) % 4;
}

int schedulerLevelInfo::getDirection(int offset)
{
	return greedyDirection[getMoveIndex(offset)];
}

int schedulerLevelInfo::getAxis(int offset)
{
	return  greedyAxis[getMoveIndex(offset)];
}

int boxConfiguration::getBoxTotalDimension(int boxType, int axis)
{
	return boxSize[boxType][axis] + distBetweenBoxes[axis];
}

boxConfiguration::boxConfiguration()
{
	_pinScenario = PINSVERTICAL;
	_heightStepSize = 0;

	boxSize.resize(2);
	int bsizetmp[] = {15 * DELTA, 5 * DELTA, 10 * DELTA, 30, 30, 30};
	boxSize[0] = std::vector<int>(bsizetmp + 0, bsizetmp + 3);
	boxSize[1] = std::vector<int>(bsizetmp + 3, bsizetmp + 6);

	int distatmp[] = { DELTA, DELTA, DELTA };
	distBetweenBoxes = std::vector<int>(distatmp + 0, distatmp + 4);
}
