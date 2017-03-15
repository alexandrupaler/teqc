#include <algorithm>

#include "circuitgeometry.h"
#include "cnotcounter.h"
#include <algorithm>

int circuitgeometry::getInputCoordAxis()
{
	int where = inputConnectionDown ? CIRCUITDEPTH : CIRCUITHEIGHT;
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
		simplegeom.addSegment(lastIndex[wire][PREV1 + si], lastIndex[wire][CURR1 + si]);
	}
}

void circuitgeometry::liConnectIOPointToCurrs(int wire, int& ioIndex, bool connectionDown)
{
	if (connectionDown)
	{
		//add segment down
		simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2]);

		//create a point after the ioIndex
		convertcoordinate nioc(simplegeom.coords[ioIndex]);
		nioc[CIRCUITDEPTH] += DELTA / 2;
		int nindex = simplegeom.addCoordinate(nioc);

		//add segment to the right, between curr1 and ioIndex
		simplegeom.addSegment(lastIndex[wire][CURR1], ioIndex);

		//add segment between ioIndex and point following it
		simplegeom.addSegment(ioIndex, nindex);

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
			simplegeom.addSegment(ioIndex, lastIndex[wire][CURR1 + si]);
		}
	}
}

int circuitgeometry::liInsertIOPointUsingCurr1(int wire, bool isInit)
{
	//the io will be with one layer up
	convertcoordinate ioc(simplegeom.coords.at(lastIndex[wire][CURR1]));
	//ioc.assign(coords.at(lastindex[CURR1]).begin(), coords.at(lastindex[CURR1]).end());

	int where = isInit ? getInputCoordAxis() : CIRCUITHEIGHT;
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

void circuitgeometry::makeGeometryFromCircuit(causalgraph& causal, bfsState& state)
{
//	TODO: Is this constantly needed?
	lastIndex.resize(state.nrLines);
	for(size_t i=0; i<lastIndex.size(); i++)
	{
		lastIndex[i].resize(4);
	}

	lastDualIndices.resize(4);

	for(vector<int>::iterator it = state.toDraw.begin(); it != state.toDraw.end(); it++)
	{
		bool isInit = causal.circuit[*it].isInitialisation();
		bool isMeasure = causal.circuit[*it].isMeasurement();
		bool isControl = causal.circuit[*it].isGate();//in mod normal doar CNOT poate fi din cauza de ICM. TODO: verifica

		int level = causal.circuit[*it].level;

		int wire = causal.circuit[*it].wires[0];

		//initialise the CURR points
		//and update them according to their meaning
		if(isControl || isInit || isMeasure)
		{
			liInitCurrPrimal(wire, level);

			if(isControl)//21.sept.2015//practic da inapoi
			{
				if(!useBridge)
				{
					liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, -DELTA);
					liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, -DELTA);
				}
				//add segments to the left
				liConnectPrevsWithCurrs(wire);

				//first down
				simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2]);

				if(!useBridge)
				{
					liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, DELTA);
					liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, DELTA);

					//second down - non-bridge?
					simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2]);
				}
			}

			if(isMeasure)
			{
				liConnectPrevsWithCurrs(wire);

				//add the io measurement point
				int ioIndex = liInsertIOPointUsingCurr1(wire, false);
				liConnectIOPointToCurrs(wire, ioIndex, false);
			}

			//if(circuitHasInjections && paramCirc.isInput(wire, level))
			if(causal.circuit[*it].isInput())
			{
				liAddAxisOffset(wire, CURR1, CIRCUITDEPTH, -25*DELTA);
				liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, -25*DELTA);
			}

			if(isInit)
			{
				simplegeom.addSegment(lastIndex[wire][CURR1], lastIndex[wire][CURR2]);
				liAddAxisOffset(wire, CURR2, CIRCUITDEPTH, DELTA);
				liAddAxisOffset(wire, CURR1, CIRCUITHEIGHT, DELTA);

				int ioIndex = liAddIOPoint2(wire);

				if(causal.circuit[*it].type == AA || causal.circuit[*it].type == YY)
				{
					liAddPinPair(wire, ioIndex, causal.circuit[*it].type == AA ? ATYPE : YTYPE);

					state.operationIdToCircuitPinIndex[*it] = allpins.size() - 1;
				}
				else
				{
					//pentru a marca care sunt injectii, la inputuri leg de io, iar la injectii nu
					simplegeom.addSegment(ioIndex, lastIndex[wire][CURR1]);
					simplegeom.addSegment(ioIndex, lastIndex[wire][CURR2]);
				}

				liAddAxisOffset(wire, CURR1, CIRCUITHEIGHT, -DELTA);
			}
		}

		//move the curr indices to be prev indices
		liCopyCurrsOverPrevs(wire);

		if(isControl)
		{
			addDual(causal, *it);
		}
	}
}

void circuitgeometry::addDual(causalgraph& causal, int opid)
{
	vector<int> targets = causal.circuit[opid].wires;
	int level = causal.circuit[opid].level;
	int wire = targets[0];

	std::sort(targets.begin(), targets.end());

	liInitCurrDual(targets[0], level);

	//create the horizontal defect
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);

	for(size_t otherWire = 0; otherWire < targets.size(); otherWire++)
	{
		int dist = 0;
		if(otherWire>0)
		{
			dist = targets[otherWire] - targets[otherWire-1] - 1;
		}
		liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA * dist);//extend
		//if its not the control line
		if(targets[otherWire] != wire)
		{
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, -DELTA);//go down
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);//braid
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITHEIGHT, DELTA);//go up
		}
		else
		{
			liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);//extend
		}
	}

	//move to the right - non-bridge?
	liDualAddAxisOffsetAndAddSegment(CURR1, CIRCUITDEPTH, getDepthShift()/*2*DELTA*/);

	//connect upwards on the circuitwidth axis to the c2 point
	simplegeom.addSegment(lastDualIndices[CURR1], lastDualIndices[CURR2]);
}

int circuitgeometry::getDepthShift()
{
	int shift = useBridge ? DELTA : 2*DELTA;

	return shift;
}

void circuitgeometry::liAddAxisOffset(int wire, int indexPos, int axis, int offset)
{
	convertcoordinate c1(simplegeom.coords[lastIndex[wire][indexPos]]);
	c1[axis] += offset;

	lastIndex[wire][indexPos] = simplegeom.addCoordinate(c1);
}

void circuitgeometry::liDualAddAxisOffset(int indexPos, int axis, int offset)
{
	convertcoordinate c1(simplegeom.coords[lastDualIndices[indexPos]]);
	c1[axis] += offset;

	lastDualIndices[indexPos] = simplegeom.addCoordinate(c1);
}

void circuitgeometry::liAddAxisOffsetAndAddSegment(int wire, int indexPos, int axis, int offset)
{
	int previndex = lastIndex[wire][indexPos];
	liAddAxisOffset(wire, indexPos, axis, offset);
	simplegeom.addSegment(previndex, lastIndex[wire][indexPos]);
}

void circuitgeometry::liDualAddAxisOffsetAndAddSegment(int indexPos, int axis, int offset)
{
	int previndex = lastDualIndices[indexPos];
	liDualAddAxisOffset(indexPos, axis, offset);
	simplegeom.addSegment(previndex, lastDualIndices[indexPos]);
}

void circuitgeometry::liSetAxisValueAndAddSegment(int wire, int indexPos, int axis, int value)
{
	convertcoordinate c1(simplegeom.coords[lastIndex[wire][indexPos]]);
	c1[axis] = value;

	//vector<int> c;
	//c.insert(c.begin(), c1.begin(), c1.end());

	int currindex = simplegeom.addCoordinate(c1);

	simplegeom.addSegment(lastIndex[wire][indexPos], currindex);
	lastIndex[wire][indexPos] = currindex;
}

void circuitgeometry::liInitCurrPrimal(int wire, int level)
{
	convertcoordinate c1; //-100 represents nothing
	//c1[0] = maxheight - i*2;
	c1[CIRCUITWIDTH] = wire * DELTA + 1;
	//c1[1] = j*4;
	c1[CIRCUITDEPTH] = level * getDepthShift() + 1; //compacted to bridge?
	c1[CIRCUITHEIGHT] = 1;

	convertcoordinate c2 (c1);
	//c2.assign(c1.begin(), c1.end());
	c2[CIRCUITHEIGHT] += DELTA;

	//add the two points
	lastIndex[wire][CURR1] = simplegeom.addCoordinate(c1);
	lastIndex[wire][CURR2] = simplegeom.addCoordinate(c2);
}

int circuitgeometry::getPrevHalfDelta(int direction)
{
	int ret = DELTA / 2;
	if(ret%2 == 1)
		ret += direction;
	return ret;
}

void circuitgeometry::liInitCurrDual(int wire, int level)
{
	//DELTA este distanta, dar eu pentru plumbing pieces am nevoie de ceva care este factor de 5d/4

	//2 points for control: c1 the left, c2 the right
	convertcoordinate c1;//-100 represents nothing

	c1[CIRCUITWIDTH] = wire * DELTA - getPrevHalfDelta(-1);
	c1[CIRCUITDEPTH] = (level - 1) * getDepthShift() + getPrevHalfDelta(+1); //compacted to bridge?
	c1[CIRCUITHEIGHT] = getPrevHalfDelta(-1);

	convertcoordinate c2 (c1);
	//c2.assign(c1.begin(), c1.end());
	//c2[CIRCUITDEPTH] += 2 * DELTA;//ce se intampla aici?

	//non-bridge?
	c2[CIRCUITDEPTH] += getDepthShift();/*otherwise it would be before the primal defect*/;//ce se intampla aici?

	//int firstDIndex = addCoordinate(c2);
	lastDualIndices[CURR2] = simplegeom.addCoordinate(c2);

	//int lastDIndex = addCoordinate(c1);
	lastDualIndices[CURR1] = simplegeom.addCoordinate(c1);
}
