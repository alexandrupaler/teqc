#include <algorithm>

#include "circuitgeometry.h"
#include "cnotcounter.h"

int circuitgeometry::getInputCoordAxis()
{
	int where = inputConnectionDown ? CIRCUITDEPTH : CIRCUITHEIGHT;
	return where;
}

void circuitgeometry::addInputPins(int ioIndex, circuitmatrix& paramCirc, int i, int j)
{
	inputpin pins;//8 ints with value -100
	int type = (paramCirc.at(i).at(j) == AA ? ATYPE : YTYPE);

	pins[TYPE] = type;
	pins[INJNR] = ioIndex;

	for (int si = 0; si < 2; si++)
	{
		coordinate pinc(simplegeom.coords[ioIndex]);

		int where = getInputCoordAxis();

		pinc[where] += (DELTA / 2) - DELTA * (si % 2);

		//pins.insert(pins.begin() + OFFSETNONCOORD, pinc.begin(), pinc.end()); //coord of pin
		//pins.insert(pins.end(), pinc.begin(), pinc.end()); //coord of pin
		pins.setCoordinate(si, pinc);
	}

	allpins.addEntry(pins);
}

void circuitgeometry::liConnectPrevsWithCurrs()
{
	//add segments to the left
	for (int si = 0; si < 2; si++)
	{
		simplegeom.addSegment(lastindex[PREV1 + si], lastindex[CURR1 + si]);
	}
}

void circuitgeometry::liConnectIOPointToCurrs(int& ioIndex, bool connectionDown)
{
	if (connectionDown)
	{
		//add segment down
		simplegeom.addSegment(lastindex[CURR1], lastindex[CURR2]);

		//create a point after the ioIndex
		coordinate nioc(simplegeom.coords[ioIndex]);
		nioc[CIRCUITDEPTH] += DELTA / 2;
		int nindex = simplegeom.addCoordinate(nioc);

		//add segment to the right, between curr1 and ioIndex
		simplegeom.addSegment(lastindex[CURR1], ioIndex);

		//add segment between ioIndex and point following it
		simplegeom.addSegment(ioIndex, nindex);

		//update CURR1
		lastindex[CURR1] = nindex;
		//at this point CURR1 and CURR2 are not parallel anymore
		//CURR1 has an offset of 2 on the circuitdepth axis
	}
	else
	{
		//add segments between the CURR points and the ioIndex
		for (int si = 0; si < 2; si++)
		{
			simplegeom.addSegment(ioIndex, lastindex[CURR1 + si]);
		}
	}
}

int circuitgeometry::liInsertIOPointUsingCurr1(bool isInit)
{
	//the io will be with one layer up
	coordinate ioc(simplegeom.coords.at(lastindex[CURR1]));
	//ioc.assign(coords.at(lastindex[CURR1]).begin(), coords.at(lastindex[CURR1]).end());

	int where = isInit ? getInputCoordAxis() : CIRCUITHEIGHT;
	ioc[where] += DELTA/2;

	int ioIndex = simplegeom.addIOPoint(ioc, isInit);

	return ioIndex;
}

void circuitgeometry::liCopyCurrsOverPrevs()
{
	//move the curr indices to be prev indices
	for (int si = 0; si < 2; si++)
	{
		lastindex[PREV1 + si] = lastindex[CURR1 + si]; //will be used at the later point at indices 0,1
	}
}

void circuitgeometry::makeGeometryFromCircuit(circuitmatrix& paramCirc)
{
	//neccesary?
	bool circuitHasInjections = paramCirc.hasInjections();

	//here the primal space is constructed

	for(int i=0; i<paramCirc.size(); i++)
	{
		for(int j=0; j<paramCirc.at(i).size(); j++)
		{
			bool isInit = paramCirc.isInitialisation(i, j);
			bool isMeasure = paramCirc.isMeasurement(i, j);
			bool isControl = cnotcounter::isControl(paramCirc.at(i).at(j));

			//initialise the CURR points
			//and update them according to their meaning
			if(isControl || isInit || isMeasure)
			{
				liInitCurrPrimal(i, j);

				if(isMeasure)//21.sept.2015//practic da inapoi
				{
					simplegeom.coords[lastindex[CURR1]][CIRCUITDEPTH] -= DELTA;
					simplegeom.coords[lastindex[CURR2]][CIRCUITDEPTH] -= DELTA;
				}

				if(isInit)//01.oct.2015//practic da inainte
				{
					simplegeom.coords[lastindex[CURR1]][CIRCUITDEPTH] += DELTA;
					simplegeom.coords[lastindex[CURR2]][CIRCUITDEPTH] += DELTA;
				}

				if(circuitHasInjections && paramCirc.isInput(i, j))
				{
					simplegeom.coords[lastindex[CURR1]][CIRCUITDEPTH] -= 25 * DELTA;
					simplegeom.coords[lastindex[CURR2]][CIRCUITDEPTH] -= 25 * DELTA;
				}
			}

			if(isControl)
			{
				//add segments to the left
				liConnectPrevsWithCurrs();

				//add first segment down
				simplegeom.addSegment(lastindex[CURR1], lastindex[CURR2]);

				liAddAxisOffset(CURR1, CIRCUITDEPTH, DELTA);
				liAddAxisOffset(CURR2, CIRCUITDEPTH, DELTA);

				//add second segment down
				simplegeom.addSegment(lastindex[CURR1], lastindex[CURR2]);

//				//add segments to the left
//				for(int si=0; si<2; si++)
//				{
//					lastindex[PREV1 + si] = lastindex[CURR1 + si]; //will be used at the later point at indices 0,1
//				}
//
//				//necessary?
//				//if(!bridgecnotprimals)
//				{
//					addDepthOffsetLastIndices(PREV1, DELTA, lastindex);
//					addDepthOffsetLastIndices(PREV2, DELTA, lastindex);
//					addSegment(lastindex[PREV1], lastindex[PREV2]);
//				}
			}

			if(isInit)
			{
				int ioIndex = liInsertIOPointUsingCurr1(true);

				liConnectIOPointToCurrs(ioIndex, inputConnectionDown);

				//save the input pins to a list
				if(paramCirc.at(i).at(j) == AA || paramCirc.at(i).at(j) == YY)
				{
					addInputPins(ioIndex, paramCirc, i, j);
				}
			}

			if(isMeasure)
			{
				liConnectPrevsWithCurrs();

				//add the io measurement point
				int ioIndex = liInsertIOPointUsingCurr1(false);

				liConnectIOPointToCurrs(ioIndex, false);
			}

			//move the curr indices to be prev indices
			liCopyCurrsOverPrevs();
		}
	}

	//the dual space is constructed
	for(int i=0; i<paramCirc.size(); i++)
	{
		for(int j=0; j<paramCirc.at(i).size(); j++)
		{
			if(cnotcounter::isControl(paramCirc.at(i).at(j)))
			{
				//printf("%d --> %d\n", i, comps.at(comps.size()-1));
				addDual(paramCirc, i, j);
			}
		}
	}
}

void circuitgeometry::addDual(circuitmatrix& circ, int ctrli, int ctrlj)
{
//	int maxheight = circ.size() * DELTA;
//
//	vector<int> targets = circ.findTarget(ctrli, ctrlj);
//
//	int min = ctrli < targets[0] ? ctrli : targets[0];
//	int max = ctrli > targets[targets.size() - 1] ? ctrli : targets[targets.size() - 1];
//
//	//2 points for control: c1 the left, c2 the right
//	vector<int> c1(3, -100);//-100 represents nothing
//
//	c1[CIRCUITWIDTH] = DELTA*min - 1;//-1 because of dual space
//	c1[CIRCUITDEPTH] = ctrlj * getDepthShift() - 1;//compact to bridge ?//-1 because of dual space
//	c1[CIRCUITHEIGHT] = DUALUPCOORD;
//
//	vector<int> c2;
//	c2.assign(c1.begin(), c1.end());
//	c2[CIRCUITDEPTH] += 2*DELTA;//ce se intampla aici?
//
//	int firstDIndex = addCoordinate(c2);
//
//	int lastDIndex = addCoordinate(c1);

	int maxheight = circ.size() * DELTA;

	vector<int> targets = circ.findTarget(ctrli, ctrlj);

	int min = ctrli < targets[0] ? ctrli : targets[0];
	int max = ctrli > targets[targets.size() - 1] ? ctrli : targets[targets.size() - 1];

	liInitCurrDual(min, ctrlj, circ);

	//c1 este la lastindex[CURR1]
	//c2 este la lastindex[CURR2]

	//create the horizontal defect
	simplegeom.addSegment(lastindex[CURR1], lastindex[CURR2]);

	int prevTarget = min;

	bool firstTarget = true;

	//why was this?
	//c1[CIRCUITWIDTH] += DELTA;//why?

	//it is not guaranteed that the targets are ordered
	//iterate through all the possible target positions in the circuit
	for(int i=min; i<=max; i++)
	{
		vector<int>::iterator it = find (targets.begin(), targets.end(), i);
		//if a target was found
		if (it != targets.end())
		{
			//compute the distance between the current target and the previous one
			int dist = i - prevTarget - 1;

			if(firstTarget)
			{
				dist++;
				firstTarget = false;
			}

			//if(dist > 0)
			{
				//prelungeste segmentul doar daca nu e prima oara
				//printf("dist=%d\n", dist);
				//move the c1 point to
				//modifyCoordinateAndAddSegment(CIRCUITWIDTH, c1[CIRCUITWIDTH] + DELTA*dist, CURR1);
				//addDepthOffsetLastIndices(CURR1, DELTA * dist, lastindex);
				liAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA * dist);
			}

			liSetAxisValueAndAddSegment(CURR1, CIRCUITHEIGHT, DUALDOWNCOORD);//go down
			//modifyCoordinateAndAddSegment(CIRCUITWIDTH, c1[CIRCUITWIDTH] + DELTA, CURR1);//braid
			liAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, DELTA);//braid

			//go up if the last index is a target
			if(i == max)
				liSetAxisValueAndAddSegment(CURR1, CIRCUITHEIGHT, DUALUPCOORD);//go up

			prevTarget = i;
		}
		else
		{
			//go up only if the next index is not a target
			liSetAxisValueAndAddSegment(CURR1, CIRCUITHEIGHT, DUALUPCOORD);//go up
		}
	}

	if(ctrli == max)
	{
		//controlul nu apare in lista de targeturi, dar defectul trebuie extins sa cuprinda si bara verticala
		//modifyCoordinateAndAddSegment(cCIRCUITWIDTH, c1[CIRCUITWIDTH] + 2*(max-lasttarget), CURR1);
		liAddAxisOffsetAndAddSegment(CURR1, CIRCUITWIDTH, 2*(max - prevTarget));
	}

	//move to the right
	//modifyCoordinateAndAddSegment(c1, 1, c1[1] + (bridgecnotprimals ? 2 : 4), lastDIndex);
	//modifyCoordinateAndAddSegment(CIRCUITDEPTH, c1[CIRCUITDEPTH] + (2*DELTA), CURR1);
	liAddAxisOffsetAndAddSegment(CURR1, CIRCUITDEPTH, 2*DELTA);

	//connect upwards on the circuitwidth axis to the c2 point
	simplegeom.addSegment(lastindex[CURR1], lastindex[CURR2]);
}

int circuitgeometry::getDepthShift()
{
//	int shift = bridgecnotprimals ? DELTA : 2*DELTA;
//	shift +=  bridgecnotduals ? 0 : DELTA;

	int shift = 2*DELTA;
	shift +=  DELTA;

	return shift;
}

void circuitgeometry::liAddAxisOffset(int indexPos, int axis, int offset)
{
	coordinate c1(simplegeom.coords[lastindex[indexPos]]);
	c1[axis] += offset;

	lastindex[indexPos] = simplegeom.addCoordinate(c1);
}

void circuitgeometry::liAddAxisOffsetAndAddSegment(int indexPos, int axis, int offset)
{
	int previndex = lastindex[indexPos];
	liAddAxisOffset(indexPos, axis, offset);
	simplegeom.addSegment(previndex, lastindex[indexPos]);
}

void circuitgeometry::liSetAxisValueAndAddSegment(int indexPos, int axis, int value)
{
	coordinate c1(simplegeom.coords[lastindex[indexPos]]);
	c1[axis] = value;

	//vector<int> c;
	//c.insert(c.begin(), c1.begin(), c1.end());

	int currindex = simplegeom.addCoordinate(c1);

	simplegeom.addSegment(lastindex[indexPos], currindex);
	lastindex[indexPos] = currindex;
}

void circuitgeometry::liInitCurrPrimal(int i, int j)
{
	coordinate c1; //-100 represents nothing
	//c1[0] = maxheight - i*2;
	c1[CIRCUITWIDTH] = i * DELTA;
	//c1[1] = j*4;
	c1[CIRCUITDEPTH] = j * getDepthShift(); //compacted to bridge?
	c1[CIRCUITHEIGHT] = 0;

	coordinate c2 (c1);
	//c2.assign(c1.begin(), c1.end());
	c2[CIRCUITHEIGHT] += DELTA;

	//add the two points
	lastindex[CURR1] = simplegeom.addCoordinate(c1);
	lastindex[CURR2] = simplegeom.addCoordinate(c2);
}

void circuitgeometry::liInitCurrDual(int i, int j, circuitmatrix& circ)
{
	//2 points for control: c1 the left, c2 the right
	coordinate c1;//-100 represents nothing

	c1[CIRCUITWIDTH] = i * DELTA - 1;//-1 because of dual space
	c1[CIRCUITDEPTH] = j * getDepthShift() - 1;//compact to bridge ?//-1 because of dual space
	c1[CIRCUITHEIGHT] = DUALUPCOORD;

	coordinate c2 (c1);
	//c2.assign(c1.begin(), c1.end());
	c2[CIRCUITDEPTH] += 2 * DELTA;//ce se intampla aici?

	//int firstDIndex = addCoordinate(c2);
	lastindex[CURR2] = simplegeom.addCoordinate(c2);

	//int lastDIndex = addCoordinate(c1);
	lastindex[CURR1] = simplegeom.addCoordinate(c1);
}
