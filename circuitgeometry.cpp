#include <algorithm>
#include <cmath>
#include "circuitgeometry.h"
#include "cnotcounter.h"

long gridPosition::totalWires = 0;
long gridPosition::logicalQubitsPerLine = 5;

gridPosition::gridPosition()
{
	gridRow = 0;
	gridColumn = 0;

	isControl = false;
	isFakeTarget = false;
}

gridPosition::gridPosition(const gridPosition& c)
{
	gridRow = c.gridRow;
	gridColumn = c.gridColumn;

	isControl = c.isControl;
	isFakeTarget = c.isFakeTarget;
}

gridPosition& gridPosition::operator= (const gridPosition& c)
{
	gridRow = c.gridRow;
	gridColumn = c.gridColumn;

	isControl = c.isControl;
	isFakeTarget = c.isFakeTarget;

	return *this;
}

gridPosition circuitgeometry::getWirePositionInGrid(long wire)
{
	long totalNrRows = (long) ceil ((gridPosition::totalWires + 0.0) / gridPosition::logicalQubitsPerLine);

	gridPosition ret;

	/*
	 * 3 NOV
	 * the negative wires (e.g. -1) represent the place where the T gate is connected
	 */
	if(wire >= 0)
	{
		ret.gridRow = (wire + totalNrRows) % totalNrRows;
		ret.gridColumn = (wire + totalNrRows) / totalNrRows - 1;
		//make it with curves
		if(!ret.isEvenGridColumn())
		{
			ret.gridRow = (totalNrRows - 1) - ret.gridRow;
		}
		ret.gridRow *= -1;
	}
	else
	{
		ret.gridRow = 1;
		ret.gridColumn = -1 - wire;
	}

//	gridPosition ret;
//	ret.gridRow = -((wire + logicalQubitsPerLine) / logicalQubitsPerLine - 1);
//	ret.gridColumn = (wire + logicalQubitsPerLine) % logicalQubitsPerLine;
//
//	//make it with curves
//	// 1 2 3
//	// 6 5 4
//	// 7 8 9
//	if(!ret.isEvenGridRow())
//	{
//		ret.gridColumn = (logicalQubitsPerLine - 1) - ret.gridColumn;
//	}

	return ret;
}

bool gridPosition::isEvenGridRow()
{
	return abs(gridRow) % 2 != 1;
}

bool gridPosition::isEvenGridColumn()
{
//	if(gridRow == 1)
//		return false;

	return abs(gridColumn) % 2 != 1;
}

int circuitgeometry::getInputCoordAxis()
{
	//27.10.2017
//	int where = inputConnectionDown ? CIRCUITDEPTH : CIRCUITHEIGHT;
	int where = CIRCUITWIDTH;
	return where;
}

void circuitgeometry::liAddPinPair(int wire, int ioIndex, int type)
{
	pinpair pins;//8 ints with value -100

	pins[TYPE] = type;
	pins[INJNR] = ioIndex;

	pindetails detail1;
	detail1.coord = simplegeom.coords[lastIndex[wire][CURR1]];

	pindetails detail2;
	detail2.coord = simplegeom.coords[lastIndex[wire][CURR2]];

	pins.setPinDetail(0, detail1);
	pins.setPinDetail(1, detail2);

	allpins.addEntry(pins);
}

void circuitgeometry::liConnectPrevsWithCurrs(int wire)
{
	//add segments to the left
	for (int si = 0; si < 2; si++)
	{
		simplegeom.addSegment(lastIndex[wire][PREV1 + si], lastIndex[wire][CURR1 + si], DONOTADDEXISTINGSEGMENT);
	}
}

void circuitgeometry::liConnectIOPointToCurrs(int wire, int& ioIndex, bool connectionDown)
{
	//not used?
	if (connectionDown)
	{
		//add segment down
		simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2], DONOTADDEXISTINGSEGMENT);

		//create a point after the ioIndex
		convertcoordinate nioc(simplegeom.coords[ioIndex]);
		nioc[CIRCUITDEPTH] += DELTA / 2;
		int nindex = simplegeom.addCoordinate(nioc);

		//add segment to the right, between curr1 and ioIndex
		simplegeom.addSegment(lastIndex[wire][CURR1], ioIndex, DONOTADDEXISTINGSEGMENT);

		//add segment between ioIndex and point following it
		simplegeom.addSegment(ioIndex, nindex, DONOTADDEXISTINGSEGMENT);

		//update CURR1
		lastIndex[wire][CURR1] = nindex;
		//at this point CURR1 and CURR2 are not parallel anymore
		//CURR1 has an offset of 2 on the circuitdepth axis
	}
	else
	{
		//add segments between the CURR points and the ioIndex
		for (int si = 0; si < 2; si++)
		{
			simplegeom.addSegment(ioIndex, lastIndex[wire][CURR1 + si], DONOTADDEXISTINGSEGMENT);
		}
	}
}

int circuitgeometry::liInsertIOPointUsingCurr1(int wire, bool isInit)
{
	//the io will be with one layer up
	convertcoordinate ioc(simplegeom.coords.at(lastIndex[wire][CURR1]));
	//ioc.assign(coords.at(lastindex[CURR1]).begin(), coords.at(lastindex[CURR1]).end());

	//29.10.2017
//	int where = isInit ? getInputCoordAxis() : CIRCUITHEIGHT;
	int where = isInit ? getInputCoordAxis() : CIRCUITWIDTH;
	ioc[where] += DELTA/2;

	int ioIndex = simplegeom.addIOPoint(ioc, isInit);

	return ioIndex;
}

void circuitgeometry::liCopyCurrsOverPrevs(int wire)
{
	//move the curr indices to be prev indices
	for (int si = 0; si < 2; si++)
	{
		lastIndex[wire][PREV1 + si] = lastIndex[wire][CURR1 + si]; //will be used at the later point at indices 0,1
	}
}

int circuitgeometry::liAddIOPoint2(int wire)
{
    convertcoordinate ioc;
    for(int i=0; i<3; i++)
	{
		ioc[i] = (simplegeom.coords[lastIndex[wire][CURR1]][i] + simplegeom.coords[lastIndex[wire][CURR2]][i])/2;
		if(ioc[i] != simplegeom.coords[lastIndex[wire][CURR1]][i])
		{
			//daca se afla la mijlocul distantei intre CURR1 si CURR2
			//coordonatele defectelor sunt in unitati de celula
			//coordonatele io sunt in qubits
			//deci celulele primare au coordonate toate impare
			if(ioc[i] % 2 == 1)
			{
				//daca e coordonata de celula
				ioc[i]--;//par e coordonata de qubit
			}
		}
	}
    int ioIndex = simplegeom.addIOPoint(ioc, true);
    return ioIndex;
}

void circuitgeometry::addHadamard(int& wire, int level)
{
	liInitCurrDual(wire, level);

	liDualAddAxisOffset(CURR1, CIRCUITWIDTH, -DELTA);
	liDualAddAxisOffset(CURR2, CIRCUITWIDTH, -DELTA);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -DELTA);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2 * DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2 * DELTA);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, DELTA);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -2 * DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -2 * DELTA);
}


void circuitgeometry::addSGate(int& wire, int level)
{
	liInitCurrDual(wire, level);

	liDualAddAxisOffset(CURR1, CIRCUITWIDTH, -DELTA);
	liDualAddAxisOffset(CURR2, CIRCUITWIDTH, -DELTA);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -DELTA);
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2], DONOTADDEXISTINGSEGMENT);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2 * DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2 * DELTA);
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2], DONOTADDEXISTINGSEGMENT);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, DELTA);
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2], DONOTADDEXISTINGSEGMENT);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -2 * DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -2 * DELTA);
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2], DONOTADDEXISTINGSEGMENT);
}

void circuitgeometry::addVGate(int& wire, int level)
{
	liInitCurrDual(wire, level);

	liDualAddAxisOffset(CURR1, CIRCUITWIDTH, -DELTA);

	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2 * DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITDEPTH, DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, - 2 * DELTA);
//	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITDEPTH, -DELTA);
}

void circuitgeometry::makeGeometryFromCircuit(bfsState& state)
{
//	TODO: Is this constantly needed?
//	TODO: move to an initi method in circuitgeometry
	lastIndex.resize(state.getNrLines());
	for(size_t i=0; i<lastIndex.size(); i++)
	{
		lastIndex[i].resize(4);
	}

	lastDualIndices.resize(4);
	lastDualIndicesAxisSegment.resize(4, -1);

	size_t pos = 0;
	for(std::vector<recyclegate*>::iterator it = state.toDraw.begin();
			it != state.toDraw.end(); it++)
	{
		pos++;
		printf("draw %lu(ordnr_%ld id_%ld). %c [", pos, (*it)->orderNrInGateList, (*it)->getId(), (*it)->type);
//		if((*it)->type == 'c')
//		{
//			(*it)->print();
//		}
		for(size_t i = 0; i < (*it)->wirePointers.size(); i++)
		{
			printf(" %ld", (*it)->wirePointers[i]->number);
		}
		printf("] l%ld\n", (*it)->level);

		bool isCNOT = (*it)->type == 'c';
		bool isTGate = (*it)->type == 't';
		bool isHGate = (*it)->type == 'h';
		bool isSGate = (*it)->type == 'p';
		bool isVGate = (*it)->type == 'v';

		bool isInit = (*it)->isInitialisation();
		bool isMeasure = (*it)->isMeasurement();

		int level = (*it)->level;
		int wire = (*it)->wirePointers[0]->number;

		//initialise the CURR points
		//and update them according to their meaning
		if(isCNOT || isInit || isMeasure || isTGate || isHGate)
		{
			liInitCurrPrimal(wire, level);
		}

		/*
		 * Inputs are moved -25 to the left
		 */
//		if((*it)->isInput())
//		{
//			liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, -25 * DELTA);
//			liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, -25 * DELTA);
//		}

		if(isCNOT)//27.oct.2017
		{
			addCnotPrimal(wire);

			recyclegate* operationPtr = *it;
			std::vector<long> targets;
			for(std::vector<wireelement*>::iterator it = operationPtr->wirePointers.begin();
					it != operationPtr->wirePointers.end(); it++)
			{
				targets.push_back((*it)->number);
			}
			addCnotDual(targets, operationPtr->level, true);
		}

		/*
		 * Measurements
		 */
		if(isMeasure)
		{
			liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, DELTA);
			liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, DELTA);
			liConnectPrevsWithCurrs(wire);

			//add the io measurement point
			int ioIndex = liInsertIOPointUsingCurr1(wire, false);
			liConnectIOPointToCurrs(wire, ioIndex, false);
		}

		/*
		 * Initialisations
		 */
		if(isInit)
		{
//			simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2]);
//			liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, DELTA);
//			liAddAxisOffset(wire, CURR1, CIRCUITHEIGHT, DELTA);
//

			/**
			 * THIS NEEDS TO BE REPAIRED
			 */
//			int ioIndex = liAddIOPoint2(wire);
//
//			if((*it)->type == AA || (*it)->type == YY)
//			{
//				liAddPinPair(wire, ioIndex, (*it)->type == AA ? ATYPE : YTYPE);
//
//				state.operationIdToCircuitPinIndex[(*it)->getId()] = allpins.size() - 1;
//			}
//			else
//			{
//				//pentru a marca care sunt injectii, la inputuri leg de io, iar la injectii nu
//				simplegeom.addSegment(ioIndex, lastIndex[wire][CURR1]);
//				simplegeom.addSegment(ioIndex, lastIndex[wire][CURR2]);
//			}
//
//			liAddAxisOffset(wire, CURR1, CIRCUITHEIGHT, -DELTA);

			//add the io measurement point
			int ioIndex = liInsertIOPointUsingCurr1(wire, true);
			liConnectIOPointToCurrs(wire, ioIndex, false);
		}

		if(isTGate)
		{
			addCnotPrimal(wire);
//
//			//the A state pins
			gridPosition position = getWirePositionInGrid(-1);

			//modificat 29.10.2017 sa fie liniile paralele si nu una sub cealalalta
//			convertcoordinate c1;
//			c1[CIRCUITWIDTH] = position.gridColumn * 2 * DELTA + 1;
//			c1[CIRCUITDEPTH] = level * getDepthShift() + 1; //compacted to bridge?
//			c1[CIRCUITHEIGHT] = position.gridRow * DELTA + 1;

			/*NOW DUAL COORDS*/
			convertcoordinate c1;
			c1[CIRCUITWIDTH] = ((*it)->connChannel) * 2 * DELTA + getPrevHalfDelta(1);
			c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1); //compacted to bridge?
			c1[CIRCUITHEIGHT] = DELTA + getPrevHalfDelta(1);// + ((*it)->connChannel % 2 == 0  ? - DELTA : 0);


			convertcoordinate c2 (c1);
//			c2[CIRCUITWIDTH] += DELTA;
			//the pins are along the depth axis
			c2[CIRCUITDEPTH] += DELTA;

			//the io will be with one layer up
			convertcoordinate ioc(c1);
			int where = CIRCUITDEPTH;//isInit ? getInputCoordAxis() : CIRCUITWIDTH;
			ioc[where] += DELTA/2;

			int ioIndex1 = simplegeom.addIOPoint(ioc, isInit);

			pinpair pins;
			pins[TYPE] = ATYPE;
			pins[INJNR] = ioIndex1;
			pindetails detail1;
			detail1.coord = c1;
			pindetails detail2;
			detail2.coord = c2;

			pins.setPinDetail(0, detail1);
			pins.setPinDetail(1, detail2);

			allpins.addEntry(pins);

			state.operationIdToCircuitPinIndex[(*it)->getId()] = allpins.size() - 1;

			recyclegate* operationPtr = *it;
			std::vector<long> targets;
			targets.push_back(operationPtr->wirePointers[0]->number);
			/*
			 * 3NOV - neglect the dummy zero wire
			 *
			 * from the T gate which was added in circconvert.cpp
			 */
			targets.push_back(-(*it)->connChannel - 1);
			addCnotDual(targets, operationPtr->level, false);

			/*
			 * elimina segmentul care contine pinii drept capete
			 */
			int idx1 = simplegeom.addCoordinate(c1);
			int idx2 = simplegeom.addCoordinate(c2);
			simplegeom.removeSegment(idx1, idx2);
		}

		if(isHGate)
		{
			addHadamard(wire, level);
		}

		if(isSGate)
		{
			addSGate(wire, level);
		}

		if(isVGate)
		{
			addVGate(wire, level);
		}

		//move the curr indices to be prev indices
		if(isCNOT || isInit || isMeasure)
		{
			liCopyCurrsOverPrevs(wire);
		}
	}
}

void circuitgeometry::addCnotPrimal(int& wire)
{
	if (!useBridge)
	{
		liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, -DELTA);
		liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, -DELTA);
	}
	//add segments to the left
	liConnectPrevsWithCurrs(wire);
	//first down
	simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2], DONOTADDEXISTINGSEGMENT);
	if (!useBridge)
	{
		liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, DELTA);
		liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, DELTA);

		//second down - non-bridge?
		simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2], DONOTADDEXISTINGSEGMENT);
	}
}

std::vector<gridPosition> circuitgeometry::computePositionsForCnotDual(std::vector<long>& targets)
{
	std::vector<gridPosition> positionsToDraw;

	if(targets.size() == 2 && targets[1] < 0)
	{
		//place the first element
		gridPosition position0 = getWirePositionInGrid(targets[0]);
		position0.isControl = true;//the first one is control
		gridPosition position1 = getWirePositionInGrid(targets[1]);

		positionsToDraw.push_back(position0);
		positionsToDraw.push_back(position1);

		/**
		 * This is the situation for the T gate
		 */
		bool swap = false;
		//do not sort because this is a T gate?
		if(position1.gridColumn < position0.gridColumn)
		{
			swap = true;
		}
		else if(position1.gridColumn == position0.gridColumn)
		{
			//check column direction
			//target1 has always positive row and is higher than the control
			if(position1.isEvenGridColumn())
			{
				swap = true;
			}
		}

		if(swap)
		{
			positionsToDraw.push_back(positionsToDraw[0]);
			positionsToDraw.erase(positionsToDraw.begin());
		}
	}
	else
	{
		long controlWire = targets[0];
		std::sort(targets.begin(), targets.end());
		for(size_t i = 0; i < targets.size(); i++)
		{
			gridPosition p = getWirePositionInGrid(targets[i]);
			if(targets[i] == controlWire)
			{
				p.isControl = true;
			}
			positionsToDraw.push_back(p);
		}
	}

//	/*
//	 * search for min row
//	 */
//	long minRow = LONG_MAX;
//	for(size_t i = 0; i < positionsToDraw.size(); i++)
//	{
//		if(minRow > positionsToDraw[i].gridRow)
//		{
//			minRow = positionsToDraw[i].gridRow;
//		}
//	}
//
//	//make minRow minus one, to be sure
//	long gotoMinRow = minRow;
////	if(positionsToDraw.size() > 2)
//		gotoMinRow--;
//
//	if(minRow != positionsToDraw.front().gridRow || positionsToDraw.front().isEvenGridColumn())
//	{
//		//daca merge in jos
//		if(positionsToDraw.front().isEvenGridColumn())
//		{
//			//cel din dreapta cea mai de jos
//			gridPosition np;
//			np.isFakeTarget = true;
//			np.gridRow = gotoMinRow;
//			np.gridColumn = positionsToDraw.front().gridColumn - 1;
//			positionsToDraw.insert(positionsToDraw.begin(), np);
//		}
//		else
//		{
//			//cel de cel mai jos
//			gridPosition np;
//			np.isFakeTarget = true;
//			np.gridRow = gotoMinRow;
//			np.gridColumn = positionsToDraw.front().gridColumn;
//			positionsToDraw.insert(positionsToDraw.begin(), np);
//		}
//	}
//
//	if(minRow != positionsToDraw.back().gridRow || !positionsToDraw.back().isEvenGridColumn())
//	{
//		//daca merge in sus
//		if(!positionsToDraw.back().isEvenGridColumn())
//		{
//			gridPosition np;
//			np.isFakeTarget = true;
//			np.gridRow = gotoMinRow;
//			np.gridColumn = positionsToDraw.back().gridColumn + 1;
//			positionsToDraw.push_back(np);
//		}
//		else
//		{
//			gridPosition np;
//			np.isFakeTarget = true;
//			np.gridRow = gotoMinRow;
//			np.gridColumn = positionsToDraw.back().gridColumn;
//			positionsToDraw.push_back(np);
//		}
//	}

	return positionsToDraw;
}

#define NEWCNOT 0

#if NEWCNOT == 1
int circuitgeometry::drawCtrlTgt(gridPosition& position0,
		bool previousWasDirectlyOnThisColumn,
		bool moveDirectlyOnThisColumn,
		bool changeColumn)
{
	int widthOffset = 0;

	bool isControlWire = position0.isControl;
	int sign0 = position0.isEvenGridColumn() ? -1 : 1;

	if (isControlWire)
	{
		if(previousWasDirectlyOnThisColumn)
		{
			//pleaca din centru si nu de pe margine
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -sign0 * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITDEPTH, DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
			if(!moveDirectlyOnThisColumn && !changeColumn)
			{
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -DELTA);
			}
			else
			{
				widthOffset = DELTA;
			}
		}
		else
		{
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -sign0 * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITDEPTH, DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
			if(!moveDirectlyOnThisColumn && !changeColumn)
			{
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -DELTA);
			}
			else
			{
				widthOffset = DELTA;
			}
		}
	}
	else
	{
		if(position0.isFakeTarget)
		{
			//fake targets are either the first or the last
			if(previousWasDirectlyOnThisColumn)
			{
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -DELTA);

				//go in the direction of the column
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);

				if(moveDirectlyOnThisColumn)
				{
					liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA);
					widthOffset = DELTA;
				}
			}
			else
			{
				//do nothing, just go in the direction of the column
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
				if(moveDirectlyOnThisColumn)
				{
					liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA);
					widthOffset = DELTA;
				}
			}
		}
		else
		{
			if(!previousWasDirectlyOnThisColumn)
			{
				//it's on the rhs of primal defect pair
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA);
			}

			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
			if (changeColumn)
			{
				//schimba coloana
				//e deja cu un DELTA in fata, din cauza ca ramane
				widthOffset = DELTA;
			}
			if(!moveDirectlyOnThisColumn && !changeColumn)
			{
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -DELTA);
			}
		}
	}
	return widthOffset;
}

void circuitgeometry::checkAndMoveBackOnWidth(
		bool& moveBackToNormalWidthAfterAControl, bool& isControl, long movementFromLastControlPosition)
{
	if (moveBackToNormalWidthAfterAControl)
	{
		if (isControl && (movementFromLastControlPosition != 0))
		{
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITDEPTH, -DELTA);
			moveBackToNormalWidthAfterAControl = false;
		}
		if (!isControl)
		{
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITDEPTH, -DELTA);
			moveBackToNormalWidthAfterAControl = false;
		}
	}
}

/*
 * Works only for single target CNOT? Not general?
 */
void circuitgeometry::addCnotDual(std::vector<long>& targets, long level, bool constructFirstSegment)
{
	resetMinMaxCoords();

//	long controlWire = targets[0];

	std::vector<gridPosition> positionsToDraw = computePositionsForCnotDual(targets);

	gridPosition position0 = positionsToDraw[0];
	gridPosition position1 = positionsToDraw[1];
	int sign0 = position0.isEvenGridColumn() ? -1 : 1;
	int sign1 = position1.isEvenGridColumn() ? -1 : 1;

	/**
	 * THE LOOP
	 */
	bool previousWasDirectlyOnThisColumn = !position0.isFakeTarget;//true;

	//2 points for control: c1 the left, c2 the right
	convertcoordinate c1;
	//daca previousWasDirectlyOnThisColumn == true atunci minus 0, altfel minus 1
	c1[CIRCUITWIDTH] = (position0.gridColumn * 2 - (previousWasDirectlyOnThisColumn ? 0 : 1)) * DELTA + getPrevHalfDelta(1);
	c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
	c1[CIRCUITHEIGHT] = position0.gridRow * DELTA + getPrevHalfDelta(1);
	if(sign0 == 1)
	{
		c1[CIRCUITHEIGHT] -= DELTA;
	}

	convertcoordinate c2(c1);
//	c2[CIRCUITDEPTH] += getDepthShift();

//	lastDualIndices[CURR1] = simplegeom.addCoordinate(c1);
	lastDualIndices[CURR2] = simplegeom.addCoordinate(c2);
	int firstIndex = lastDualIndices[CURR2];
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);

//	if(targets[0] == controlWire)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
//	}

	gridPosition distance;
	distance.gridRow = position1.gridRow - position0.gridRow;
	distance.gridColumn = position1.gridColumn - position0.gridColumn;

	//for(int i = 0; i < targets.size(); i++)
	bool moveBackToNormalWidthAfterAControl = false;
	for(size_t i = 0; i < positionsToDraw.size(); i++)
	{
		//place the first element
		position0 = positionsToDraw[i];//getWirePositionInGrid(targets[i]);
		if (i == positionsToDraw.size() - 1)
		{
			position1 = position0;
//			position1 = positionsToDraw[0];
			//TODO: this does not look good when drawn - keeps the control movement on the right side
		}
		else
		{
			position1 = positionsToDraw[i + 1];//getWirePositionInGrid(targets[i + 1]);
		}

		sign0 = position0.isEvenGridColumn() ? -1 : 1;
		sign1 = position1.isEvenGridColumn() ? -1 : 1;

		distance.gridRow = position1.gridRow - position0.gridRow;
		distance.gridColumn = position1.gridColumn - position0.gridColumn;

		int signGridRow = 1;
		if(distance.gridRow != 0)
		{
			signGridRow = abs(distance.gridRow) / distance.gridRow;
		}

		int signGridColumn = 1;
		if(distance.gridColumn!= 0)
		{
			signGridColumn = abs(distance.gridColumn) / distance.gridColumn;
		}

		bool nextElementIsRightBelow = (signGridRow == sign0) && distance.gridColumn == 0 && abs(distance.gridRow) == 1;
		bool changeColumnImmediately = (signGridRow != sign0) || (distance.gridRow == 0);

		/*
		 * Aici se mai leaga inca de targets
		 */
		//isControl should be removed with position0.isControl ?
		bool isControl = position0.isControl;
		//if the movement back was not performed, maintain the flag
		moveBackToNormalWidthAfterAControl = moveBackToNormalWidthAfterAControl ? true : isControl;
		int widthOffset = drawCtrlTgt(position0, previousWasDirectlyOnThisColumn, nextElementIsRightBelow, changeColumnImmediately);
		previousWasDirectlyOnThisColumn = nextElementIsRightBelow;

		/*
		 * the control changes the depth of the line
		 * after the first non-zero width or height change go back with depth -DELTA
		 */

		//parcurge distanta pe coloana care se potriveste
		if(!changeColumnImmediately)
		{
			//advance on this column
			int offset3 = 1;
			if(distance.gridColumn != 0)
			{
				offset3 = 0;
			}

			long move1 = (distance.gridRow - sign0 * offset3) * DELTA;
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, move1);

			checkAndMoveBackOnWidth(moveBackToNormalWidthAfterAControl,
					isControl, move1);

//			//AICI ESTE AL DOILEA ELEMENT
			if(distance.gridColumn != 0)
			{
				//advance on next column
				long move2 = 2*DELTA - widthOffset;
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, move2);
				checkAndMoveBackOnWidth(moveBackToNormalWidthAfterAControl,
									isControl, move2);
			}
		}
		else
		{
			//advance on next column
			//first move to next column
			long move3 = 2*DELTA - widthOffset;
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, move3);
			checkAndMoveBackOnWidth(moveBackToNormalWidthAfterAControl,
								isControl, move3);


			//move into the direction
			//changed sign0, because of next column
			long move4 = (distance.gridRow - 0) * DELTA;
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, move4);
			checkAndMoveBackOnWidth(moveBackToNormalWidthAfterAControl,
								isControl, move4);

		}

		if(distance.gridColumn != 0)
		{
			if(sign0 == sign1)
			{
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -sign1 * DELTA);
			}

			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*(distance.gridColumn - 1) * DELTA);
		}
	}

//	close loop
//	if(sign1*simplegeom.coords[lastDualIndices[CURR2]][CIRCUITHEIGHT] <= sign1*heightToCompareAtEnd)
//	{
//		long offoff = sign1*heightToCompareAtEnd - sign1*simplegeom.coords[lastDualIndices[CURR2]][CIRCUITHEIGHT];
//		offoff = sign1*offoff + sign1 * DELTA;
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, offoff);
//	}
//
//	long offset = simplegeom.coords[lastDualIndices[CURR2]][CIRCUITWIDTH];
//	offset = offset - minCoords[CIRCUITWIDTH];
//	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -offset);
//
//	simplegeom.addSegment(firstIndex, lastDualIndices[CURR2]);
}
#endif

#if NEWCNOT == 0
convertcoordinate circuitgeometry::addLoopAround(int coordsIndexPos, int direction, convertcoordinate& startcoord)
{
	int idx = simplegeom.addCoordinate(startcoord);
	lastDualIndices[coordsIndexPos] = idx;

	liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITWIDTH, -DELTA);
	liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITHEIGHT, direction * DELTA);
	liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITWIDTH, DELTA);

	return simplegeom.coords[lastDualIndices[coordsIndexPos]];
}

convertcoordinate circuitgeometry::addGoThrough(int coordsIndexPos, int direction, convertcoordinate& startcoord)
{
	int idx = simplegeom.addCoordinate(startcoord);
	lastDualIndices[coordsIndexPos] = idx;

	liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITHEIGHT, direction * DELTA);

	return simplegeom.coords[lastDualIndices[coordsIndexPos]];
}

convertcoordinate circuitgeometry::moveToGridPosition(int coordsIndexPos, gridPosition& current, gridPosition& target, convertcoordinate& start)
{
	gridPosition distance;
	distance.gridRow = target.gridRow - current.gridRow;
	distance.gridColumn = target.gridColumn - current.gridColumn;

	int signGridRow = 1;
	if(distance.gridRow != 0)
	{
		signGridRow = abs(distance.gridRow) / distance.gridRow;
	}

	int signCurrent = current.isEvenGridColumn() ? -1 : 1;//0
	int signTarget = target.isEvenGridColumn() ? -1 : 1;//1

	int idx = simplegeom.addCoordinate(start);
	lastDualIndices[coordsIndexPos] = idx;

	//parcurge distanta pe coloana care se potriveste
	if(signGridRow == signCurrent)
	{
		int offset3 = 0;
		if(distance.gridColumn == 0)
		{
			offset3 = 1;
		}

		//advance on this column
		liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITHEIGHT, (distance.gridRow - signCurrent * offset3) * DELTA);

		//AICI ESTE AL DOILEA ELEMENT
		if(distance.gridColumn != 0)
		{
			//advance on next column
			liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITWIDTH, 2*DELTA);
		}
	}
	else
	{
		//advance on next column
		//first move to next column
		//TODO: THERE IS A BUG HERE....: face TARGET TOATA COLOANA. trebuie verificat din nou
		liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITWIDTH, 2*DELTA);
		//move into the direction
		//changed sign0, because of next column
		liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITHEIGHT, distance.gridRow * DELTA);
	}

	if(distance.gridColumn != 0)
	{
		if(signCurrent == signTarget)
		{
			liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITHEIGHT, -signTarget * DELTA);
		}

		liDualAddAxisOffsetAndAddSegment2(coordsIndexPos, CIRCUITWIDTH, 2*(distance.gridColumn - 1) * DELTA);
	}

	return simplegeom.coords[lastDualIndices[coordsIndexPos]];
}


void circuitgeometry::addCnotDual(std::vector<long>& targets, long level, bool constructFirstSegment)
{
	lastDualIndicesAxisSegment[CURR1] = CIRCUITNOAXIS;
	lastDualIndicesAxisSegment[CURR2] = CIRCUITNOAXIS;

	bool noLoop = false;
	std::vector<gridPosition> positionsToDraw;

	if (targets.size() == 2 && targets[1] < 0)
	{
		noLoop = true;
		positionsToDraw.push_back(getWirePositionInGrid(targets[0]));
		positionsToDraw.push_back(getWirePositionInGrid(targets[1]));
		positionsToDraw[0].isControl = true;
	}
	else
	{
		positionsToDraw = computePositionsForCnotDual(targets);
	}

	//2 points for control: c1 the left, c2 the right
	convertcoordinate c1;
	c1[CIRCUITWIDTH] = (positionsToDraw[0].gridColumn * 2) * DELTA + getPrevHalfDelta(1);
	c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
	c1[CIRCUITHEIGHT] = positionsToDraw[0].gridRow * DELTA + getPrevHalfDelta(1);

	int sign0 = positionsToDraw[0].isEvenGridColumn() ? -1 : 1;
	if(sign0 == 1)
	{
		c1[CIRCUITHEIGHT] -= DELTA;
	}

	convertcoordinate c2(c1);
	c2[CIRCUITDEPTH] += getDepthShift();

	/*
	 * Reinitialise the last coordinate
	 */
	lastTmpCoord.resize(4);
	lastTmpCoord[CURR1] = c1;
	lastTmpCoord[CURR2] = c2;

	int idx1 = simplegeom.addCoordinate(c1);
	int idx2 = simplegeom.addCoordinate(c2);
	simplegeom.addSegment(idx1, idx2);

	for(size_t i = 0; i < positionsToDraw.size(); i++)
	{
		sign0 = positionsToDraw[i].isEvenGridColumn() ? -1 : 1;

		c1 = addGoThrough(CURR1, sign0, c1);
		if(positionsToDraw[i].isControl)
		{
			c2 = addGoThrough(CURR2, sign0, c2);
		}
		else
		{
			c2 = addLoopAround(CURR2, sign0, c2);
		}

		if(noLoop)
		{
			//exit after first element because this is a T gate
			break;
		}

		if(i + 1 < positionsToDraw.size())
		{
			c1 = moveToGridPosition(CURR1, positionsToDraw[i], positionsToDraw[i+1], c1);
			c2 = moveToGridPosition(CURR2, positionsToDraw[i], positionsToDraw[i+1], c2);
		}
	}

	if(noLoop)
	{
		//happens for the T gate
		bool firstSegmentDownOnColumnEven = noLoop && positionsToDraw[0].gridColumn == positionsToDraw[1].gridColumn && positionsToDraw[0].isEvenGridColumn();
		if(firstSegmentDownOnColumnEven)
		{
			liDualAddAxisOffsetAndAddSegment2(CURR1, CIRCUITWIDTH, -DELTA);
			liDualAddAxisOffsetAndAddSegment2(CURR2, CIRCUITWIDTH, -DELTA);
		}

		gridPosition distance;
		distance.gridRow = positionsToDraw[1].gridRow - positionsToDraw[0].gridRow;
		distance.gridColumn = positionsToDraw[1].gridColumn - positionsToDraw[0].gridColumn;

		// for firstSegmentDownOnColumnEven == true the gridColumn distance is zero
		liDualAddAxisOffsetAndAddSegment2(CURR1, CIRCUITWIDTH, 2 * distance.gridColumn * DELTA);
		liDualAddAxisOffsetAndAddSegment2(CURR2, CIRCUITWIDTH, 2 * distance.gridColumn * DELTA);

		long offset = 0;
		if(positionsToDraw[0].isEvenGridColumn())
		{
			offset = DELTA;
		}

		liDualAddAxisOffsetAndAddSegment2(CURR1, CIRCUITHEIGHT, distance.gridRow * DELTA + offset);
		liDualAddAxisOffsetAndAddSegment2(CURR2, CIRCUITHEIGHT, distance.gridRow * DELTA + offset);

		if(firstSegmentDownOnColumnEven)
		{
			liDualAddAxisOffsetAndAddSegment2(CURR1, CIRCUITWIDTH, DELTA);
			liDualAddAxisOffsetAndAddSegment2(CURR2, CIRCUITWIDTH, DELTA);
		}
	}

//	int idx1 = simplegeom.addCoordinate(coordCurr1[0]);
//	for(size_t i=1; i<coordCurr1.size(); i++)
//	{
//		int idx2 = simplegeom.addCoordinate(coordCurr1[i]);
//
//		simplegeom.addSegment(idx1, idx2, DONOTADDEXISTINGSEGMENT);
//
//		idx1 = idx2;
//	}
//
//	idx1 = simplegeom.addCoordinate(coordCurr2[0]);
//	for(size_t i=1; i<coordCurr2.size(); i++)
//	{
//		int idx2 = simplegeom.addCoordinate(coordCurr2[i]);
//
//		simplegeom.addSegment(idx1, idx2, DONOTADDEXISTINGSEGMENT);
//
//		idx1 = idx2;
//	}

	liDualAddAxisOffsetAndAddSegment2(CURR1, -1, 0);
	liDualAddAxisOffsetAndAddSegment2(CURR2, -1, 0);

	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
}
#endif

#if NEWCNOT == 2
/*
 * Works only for single target CNOT. Not general.
 */
void circuitgeometry::addCnotDual(std::vector<long>& targets, long level, bool constructFirstSegment)
{
	bool noLoop = false;
	std::vector<gridPosition> positionsToDraw;

	if (targets.size() == 2 && targets[1] < 0)
	{
		noLoop = true;
		positionsToDraw.push_back(getWirePositionInGrid(targets[0]));
		positionsToDraw.push_back(getWirePositionInGrid(targets[1]));
		positionsToDraw[0].isControl = true;
	}
	else
	{
		positionsToDraw = computePositionsForCnotDual(targets);
	}

	gridPosition position0 = positionsToDraw[0];
	gridPosition position1 = positionsToDraw[1];

	int sign0 = position0.isEvenGridColumn() ? -1 : 1;
	int sign1 = position1.isEvenGridColumn() ? -1 : 1;

	//2 points for control: c1 the left, c2 the right
	convertcoordinate c1;
	c1[CIRCUITWIDTH] = (position0.gridColumn * 2) * DELTA + getPrevHalfDelta(1);
	c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
	c1[CIRCUITHEIGHT] = position0.gridRow * DELTA + getPrevHalfDelta(1);
	if(sign0 == 1)
	{
		c1[CIRCUITHEIGHT] -= DELTA;
	}

	convertcoordinate c2(c1);
	c2[CIRCUITDEPTH] += getDepthShift();

	lastDualIndices[CURR1] = simplegeom.addCoordinate(c1);
	lastDualIndices[CURR2] = simplegeom.addCoordinate(c2);
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);

	if(!position0.isControl)
	{
		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
	}
	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, sign0 * DELTA);
	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
	if(!position0.isControl)
	{
		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
	}

	if(noLoop)
	{
		//happens for the T gate

		bool firstSegmentDownOnColumnEven = noLoop && position0.gridColumn == position1.gridColumn && position0.isEvenGridColumn();
		if(firstSegmentDownOnColumnEven)
		{
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, -DELTA);
		}

		gridPosition distance;
		distance.gridRow = position1.gridRow - position0.gridRow;
		distance.gridColumn = position1.gridColumn - position0.gridColumn;

		// for firstSegmentDownOnColumnEven == true the gridColumn distance is zero
		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2 * distance.gridColumn * DELTA);
		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2 * distance.gridColumn * DELTA);

		long offset = 0;
		if(position0.isEvenGridColumn())
			offset = DELTA;

		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, distance.gridRow * DELTA + offset);
		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, distance.gridRow * DELTA + offset);

		if(firstSegmentDownOnColumnEven)
		{
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA);
		}
	}

	/**
	 * THE LOOP
	 */
	for(size_t i = 1; i < positionsToDraw.size() && !noLoop; i++)
	{
		//place the first element
		position0 = positionsToDraw[i - 1];//getWirePositionInGrid(targets[i - 1]);
		position1 = positionsToDraw[i];//getWirePositionInGrid(targets[i]);

		gridPosition nextPosition = position1;
		if( i + 1 < positionsToDraw.size())
			nextPosition = positionsToDraw[i + 1];


		sign0 = position0.isEvenGridColumn() ? -1 : 1;
		sign1 = position1.isEvenGridColumn() ? -1 : 1;

		gridPosition distance;
		distance.gridRow = position1.gridRow - position0.gridRow;
		distance.gridColumn = position1.gridColumn - position0.gridColumn;
		int signGridRow = 1;
		if(distance.gridRow != 0)
		{
			signGridRow = abs(distance.gridRow) / distance.gridRow;
		}

		int offset1 = 0;
		int offset2 = 0;
		if(!position1.isControl)
		{
			if(distance.gridColumn == 1)
			{
				offset1 = 1;
			}
			else
			{
				offset2 = 1;
			}
		}

		//parcurge distanta pe coloana care se potriveste
		if(signGridRow == sign0)
		{
			int offset3 = 0;
			if(distance.gridColumn == 0)
			{
				offset3 = 1;
			}

			//advance on this column
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distance.gridRow - sign0 * offset3) * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distance.gridRow - sign0 * offset3) * DELTA);

			//AICI ESTE AL DOILEA ELEMENT
			if(distance.gridColumn != 0)
			{
				//advance on next column
				liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*DELTA  - offset1 * DELTA);
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*DELTA);
			}
			else
			{
				if(!position1.isControl)
				{
					liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
				}
			}
		}
		else
		{
			//advance on next column
			//first move to next column
			//TODO: THERE IS A BUG HERE....: face TARGET TOATA COLOANA. trebuie verificat din nou
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*DELTA - offset1 * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*DELTA);
			//move into the direction
			//changed sign0, because of next column
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distance.gridRow - 0) * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distance.gridRow - 0) * DELTA);
		}

		if(distance.gridColumn != 0)
		{
			if(sign0 == sign1)
			{
				liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -sign1 * DELTA);
				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -sign1 * DELTA);
			}

			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*(distance.gridColumn - 1) * DELTA - offset2 * DELTA);
			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*(distance.gridColumn - 1) * DELTA);
		}

		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, sign1 * DELTA);
		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign1 * DELTA);
		if(!position1.isControl /*&& !(nextPosition.gridRow + 1 == position1.gridRow && nextPosition.gridColumn == position1.gridColumn)*/)
		{
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
		}
	}

	//close loop
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
}
#endif

///*
// * Works only for single target CNOT. Not general.
// */
//void circuitgeometry::addCnotDual(std::vector<long>& targets, long level)
//{
//	long controlWire = targets[0];
//	std::sort(targets.begin(), targets.end());
//
//	//place the first element
//	gridPosition position0 = getWirePositionInGrid(targets[0]);
//	gridPosition position1 = getWirePositionInGrid(targets[1]);
//
//	gridPosition distance;
//	distance.gridRow = position1.gridRow - position0.gridRow;
//	distance.gridColumn = position1.gridColumn - position0.gridColumn;
//
//	int sign0 = position0.isEvenGridColumn() ? -1 : 1;
//	int sign1 = position1.isEvenGridColumn() ? -1 : 1;
//
//	//2 points for control: c1 the left, c2 the right
//	convertcoordinate c1;
//	c1[CIRCUITWIDTH] = (position0.gridColumn * 2) * DELTA + getPrevHalfDelta(1);
//	c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
//	c1[CIRCUITHEIGHT] = position0.gridRow * DELTA + getPrevHalfDelta(1);
//	if(sign0 == 1)
//	{
//		c1[CIRCUITHEIGHT] -= DELTA;
//	}
//
//	convertcoordinate c2(c1);
//	c2[CIRCUITDEPTH] += getDepthShift();
//
//	lastDualIndices[CURR1] = simplegeom.addCoordinate(c1);
//	lastDualIndices[CURR2] = simplegeom.addCoordinate(c2);
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
//
//	if(targets[0] != controlWire)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
//	}
//	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, sign0 * DELTA);
//	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign0 * DELTA);
//	if(targets[0] != controlWire)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
//	}
//
//	int signGridRow = 1;
//	if(distance.gridRow != 0)
//	{
//		signGridRow = abs(distance.gridRow) / distance.gridRow;
//	}
//
//	int offset1 = 0;
//	int offset2 = 0;
//	if(targets[1] != controlWire)
//	{
//		if(distance.gridColumn == 1)
//		{
//			offset1 = 1;
//		}
//		else
//		{
//			offset2 = 1;
//		}
//	}
//
//	//parcurge distanta pe coloana care se potriveste
//	if(signGridRow == sign0)
//	{
//		int offset3 = 0;
//		if(distance.gridColumn == 0)
//		{
//			offset3 = 1;
//		}
//
//		//advance on this column
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distance.gridRow - sign0 * offset3) * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distance.gridRow - sign0 * offset3) * DELTA);
//
//		//AICI ESTE AL DOILEA ELEMENT
//		if(distance.gridColumn != 0)
//		{
//			//advance on next column
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*DELTA  - offset1 * DELTA);
//			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*DELTA);
//		}
//		else
//		{
//			if(targets[1] != controlWire)
//			{
//				liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
//			}
//		}
//	}
//	else
//	{
//		//advance on next column
//		//first move to next column
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*DELTA - offset1 * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*DELTA);
//		//move into the direction
//		//changed sign0, because of next column
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distance.gridRow - 0) * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distance.gridRow - 0) * DELTA);
//	}
//
//	if(distance.gridColumn != 0)
//	{
//		if(sign0 == sign1)
//		{
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -sign1 * DELTA);
//			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -sign1 * DELTA);
//		}
//
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*(distance.gridColumn - 1) * DELTA - offset2 * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, 2*(distance.gridColumn - 1) * DELTA);
//	}
//
//	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, sign1 * DELTA);
//	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, sign1 * DELTA);
//	if(targets[1] != controlWire)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
//	}
//
//	//close loop
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
//}

/*
 * Works only for single target CNOT. Not general.
 */
//void circuitgeometry::addCnotDual(std::vector<long>& targets, long level)
//{
//	long controlWire = targets[0];
//	std::sort(targets.begin(), targets.end());
//
//	//place the first element
//	gridPosition position0 = getWirePositionInGrid(targets[0]);
//	gridPosition position1 = getWirePositionInGrid(targets[1]);
//
//	gridPosition distance;
//	distance.gridRow = position1.gridRow - position0.gridRow;
//	distance.gridColumn = position1.gridColumn - position0.gridColumn;
//
//	//2 points for control: c1 the left, c2 the right
//	convertcoordinate c1;
//	c1[CIRCUITWIDTH] = (position0.gridColumn * 2) * DELTA + getPrevHalfDelta(1);
//	c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
//	c1[CIRCUITHEIGHT] = position0.gridRow * DELTA + getPrevHalfDelta(1);
//
//	convertcoordinate c2(c1);
//	c2[CIRCUITDEPTH] += getDepthShift();
//
//	lastDualIndices[CURR1] = simplegeom.addCoordinate(c1);
//	lastDualIndices[CURR2] = simplegeom.addCoordinate(c2);
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
//
//	if(targets[0] != controlWire)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
//	}
//	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);
//	liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -DELTA);
//	if(targets[0] != controlWire && distance.gridRow != 0)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
//	}
//
//	if(distance.gridColumn == 0)
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distance.gridRow + 1) * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distance.gridRow + 1) * DELTA);
//
//		if(targets[1] != controlWire)
//		{
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, -DELTA);
//		}
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -DELTA);
//		if(targets[1] != controlWire)
//		{
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);
//		}
//
//		simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
//	}
//	else
//	{
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distance.gridRow) * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distance.gridRow) * DELTA);
//
//		//2 points for control: c1 the left, c2 the right
//		convertcoordinate c3;
//		c3[CIRCUITWIDTH] = (position1.gridColumn * 2) * DELTA + getPrevHalfDelta(1);
//		c3[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
//		c3[CIRCUITHEIGHT] = position1.gridRow * DELTA + getPrevHalfDelta(1);
//
//		convertcoordinate c4(c3);
//		c4[CIRCUITDEPTH] += getDepthShift();
//
//		lastDualIndices[PREV1] = simplegeom.addCoordinate(c3);
//		lastDualIndices[PREV2] = simplegeom.addCoordinate(c4);
//		simplegeom.addSegment(lastDualIndices[PREV1], lastDualIndices[PREV2]);
//
//		if(targets[1] != controlWire)
//		{
//			liDualAddAxisOffsetAndAddSegment(PREV1, CIRCUITWIDTH, -DELTA);
//		}
//		liDualAddAxisOffsetAndAddSegment(PREV1, CIRCUITHEIGHT, -DELTA);
//		liDualAddAxisOffsetAndAddSegment(PREV2, CIRCUITHEIGHT, -DELTA);
//
//		simplegeom.addSegment(lastDualIndices[PREV1], lastDualIndices[CURR1]);
//		simplegeom.addSegment(lastDualIndices[PREV2], lastDualIndices[CURR2]);
//	}
//}

///*
// * Works only for single target CNOT. Not general.
// */
//void circuitgeometry::addCnotDual(std::vector<long>& targets, long level)
//{
//	long controlWire = targets[0];
//	std::sort(targets.begin(), targets.end());
//
//	liInitCurrDual(targets[0], level);
//
//	//create the horizontal defect
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
////
//	bool defectsAreUp[2] = {true, true};
//	for(size_t wireIndex = 0; wireIndex < targets.size(); wireIndex++)
//	{
//		gridPosition distx;
//		gridPosition currWire = getWirePositionInGrid(targets[wireIndex]);
//
//		int sign = 1;
//		if(wireIndex > 0)
//		{
//			gridPosition prevWire = getWirePositionInGrid(targets[wireIndex - 1]);
//
//			//-1 -> on the same row, but different column
//			distx.gridRow = currWire.gridRow - prevWire.gridRow;
//			//-1 -> on different row, but same column
//			distx.gridColumn = currWire.gridColumn - prevWire.gridColumn - 1;
//
//			if(distx.gridColumn <= -1)
//			{
//				distx.gridColumn += 1;
//				sign = -1;
//			}
//
//			//trial and error programming at its best
//			int minus = 0;
//			// a schimbat linia si sunt pe control
//			// inainte CURR2 era sus, dar trebuie sa fie jos
//			// nu pot sa il dau jos, pentru ca ar desena o linie
//			// atunci fortez sa para jos
////			if(targets[wireIndex] == controlWire && distx.gridRow != 0)
////			{
////				minus = 1;
////				defectsAreUp[1] = false;
////			}
//
//			if(distx.gridRow != 0 && defectsAreUp[1] && targets[wireIndex - 1] == controlWire)
//			{
//				//a schimbat randul
//				minus = -1;
//			}
//
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, (distx.gridRow - 0) * DELTA);
//			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, (distx.gridRow - minus) * DELTA);
//
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA * (distx.gridColumn) * 2);//extend
//			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, DELTA * (distx.gridColumn) * 2);//extend
//		}
//
//		if(defectsAreUp[0])
//		{
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);
//			defectsAreUp[0] = false;
//		}
//		if(targets[wireIndex] == controlWire && defectsAreUp[1])
//		{
//			liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, -DELTA);
//			defectsAreUp[1] = false;
//		}
//
//		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, sign * DELTA);
//		liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITWIDTH, sign * DELTA);
//
//		bool nextWireIsUsed = false;
//		gridPosition nextWire = currWire;
//		if(wireIndex + 1 < targets.size())
//		{
//			nextWire = getWirePositionInGrid(targets[wireIndex + 1]);
//			//on the same column
//			nextWireIsUsed = currWire.gridColumn == nextWire.gridColumn;
//			//on the same row but incremental columns
//			bool anotherCondition = (currWire.gridColumn  + 1 == nextWire.gridColumn) && (currWire.gridRow == nextWire.gridRow);
//			nextWireIsUsed = nextWireIsUsed || anotherCondition;
//		}
//
//		if(!nextWireIsUsed && !defectsAreUp[0])
//		{
//			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, DELTA);
//			defectsAreUp[0] = true;
//		}
//		if(targets[wireIndex] == controlWire && !defectsAreUp[1])
//		{
//			if(nextWire.gridRow == currWire.gridRow)
//				liDualAddAxisOffsetAndAddSegment(CURR2, CIRCUITHEIGHT, DELTA);
//			defectsAreUp[1] = true;
//		}
//	}
//	//move to the right - non-bridge?
//	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITDEPTH, getDepthShift());
//
//	//connect upwards on the circuitwidth axis to the c2 point
//	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
//}

int circuitgeometry::getDepthShift()
{
	int shift = useBridge ? DELTA : 2*DELTA;

	return shift;
}

void circuitgeometry::resetMinMaxCoords()
{
	minCoords[CIRCUITWIDTH] = minCoords[CIRCUITDEPTH] = minCoords[CIRCUITHEIGHT] = LONG_MAX;
	maxCoords[CIRCUITWIDTH] = maxCoords[CIRCUITDEPTH] = maxCoords[CIRCUITHEIGHT] = LONG_MIN;
}

void circuitgeometry::liAddAxisOffset(int wire, int indexPos, int axis, int offset)
{
	convertcoordinate c1(simplegeom.coords[lastIndex[wire][indexPos]]);
	c1[axis] += offset;

	lastIndex[wire][indexPos] = simplegeom.addCoordinate(c1);
}

void circuitgeometry::liDualAddAxisOffset(int indexPos, int axis, int offset)
{
	convertcoordinate c1 = simplegeom.coords[lastDualIndices[indexPos]];

	if(c1[axis] < minCoords[axis])
	{
		minCoords[axis] = c1[axis];
	}

	if(c1[axis] > maxCoords[axis])
	{
		maxCoords[axis] = c1[axis];
	}

	c1[axis] += offset;

	if(c1[axis] < minCoords[axis])
	{
		minCoords[axis] = c1[axis];
	}

	if(c1[axis] > maxCoords[axis])
	{
		maxCoords[axis] = c1[axis];
	}

	lastDualIndices[indexPos] = simplegeom.addCoordinate(c1);
}

void circuitgeometry::liAddAxisOffsetAndAddSegment(int wire, int indexPos, int axis, int offset)
{
	int previndex = lastIndex[wire][indexPos];
	liAddAxisOffset(wire, indexPos, axis, offset);
	simplegeom.addSegment(previndex, lastIndex[wire][indexPos], DONOTADDEXISTINGSEGMENT);
}

void circuitgeometry::liDualAddAxisOffsetAndAddSegment(int indexPos, int axis, int offset)
{
	long previndex = lastDualIndices[indexPos];
	liDualAddAxisOffset(indexPos, axis, offset);
	simplegeom.addSegment(previndex, lastDualIndices[indexPos], DONOTADDEXISTINGSEGMENT);
}

void circuitgeometry::liDualAddAxisOffsetAndAddSegment2(int indexPos, int axis, int offset)
{
	// in cazul in care ultimul segment vine extins sau scurtat
	// fac operatiunea asta. practic este pentru a

	bool addedSegment = true;
	if((offset != 0 || axis == CIRCUITNOAXIS) && lastDualIndicesAxisSegment[indexPos] != axis)
	{
		long previndex = lastDualIndices[indexPos];

		int index = simplegeom.addCoordinate(lastTmpCoord[indexPos]);
		lastDualIndices[indexPos] = index;

		//lastTmpCoord[indexPos] = simplegeom.coords[index];

		addedSegment = simplegeom.addSegment(previndex, lastDualIndices[indexPos]);
	}

	lastTmpCoord[indexPos][axis] += offset;

	if(offset != 0)
		lastDualIndicesAxisSegment[indexPos] = axis;
}

void circuitgeometry::liSetAxisValueAndAddSegment(int wire, int indexPos, int axis, int value)
{
	convertcoordinate c1(simplegeom.coords[lastIndex[wire][indexPos]]);
	c1[axis] = value;

	//vector<int> c;
	//c.insert(c.begin(), c1.begin(), c1.end());

	int currindex = simplegeom.addCoordinate(c1);

	simplegeom.addSegment(lastIndex[wire][indexPos], currindex, DONOTADDEXISTINGSEGMENT);
	lastIndex[wire][indexPos] = currindex;
}

void circuitgeometry::liInitCurrPrimal(int wire, int level)
{
	gridPosition position = getWirePositionInGrid(wire);

	//modificat 29.10.2017 sa fie liniile paralele si nu una sub cealalalta
	convertcoordinate c1;
	c1[CIRCUITWIDTH] = position.gridColumn * 2 * DELTA + 1;
	c1[CIRCUITDEPTH] = level * getDepthShift() + 1; //compacted to bridge?
	c1[CIRCUITHEIGHT] = position.gridRow * DELTA + 1;

	convertcoordinate c2 (c1);
	c2[CIRCUITWIDTH] += DELTA;

	//add the two points
	lastIndex[wire][CURR1] = simplegeom.addCoordinate(c1);
	lastIndex[wire][CURR2] = simplegeom.addCoordinate(c2);
}

void circuitgeometry::liInitCurrDual(long wire, long level)
{
	gridPosition position = getWirePositionInGrid(wire);

	//2 points for control: c1 the left, c2 the right
	convertcoordinate c1;//-100 represents nothing
	c1[CIRCUITWIDTH] = position.gridColumn * 2 * DELTA + getPrevHalfDelta(1);
	c1[CIRCUITDEPTH] = level * getDepthShift() - getPrevHalfDelta(-1);
	c1[CIRCUITHEIGHT] = position.gridRow * DELTA + getPrevHalfDelta(1);

	convertcoordinate c2(c1);
	c2[CIRCUITDEPTH] += getDepthShift();

	lastDualIndices[CURR1] = simplegeom.addCoordinate(c1);
	lastDualIndices[CURR2] = simplegeom.addCoordinate(c2);
}

int circuitgeometry::getPrevHalfDelta(int direction)
{
	int ret = DELTA / 2;
	if(ret%2 == 1)
	{
		ret += direction;
	}
	return ret;
}
