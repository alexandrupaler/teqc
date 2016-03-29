#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <limits.h>
#include <map>
#include <string>

#include "boxworld2.h"

using namespace std;

void boxworld2::pinsVertical(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH];
	y = currBoxCoords[CIRCUITDEPTH] + boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT] + DELTA + pinNr * distBetweenBoxes[CIRCUITHEIGHT] + DELTA;
}

void boxworld2::pinsHorizontalRight(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH] + DELTA + pinNr * distBetweenBoxes[CIRCUITWIDTH];
	y = currBoxCoords[CIRCUITDEPTH] + boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT] + DELTA/*put the contacts a bit higher*/;
}

void boxworld2::pinsHorizontalLeft(int pinNr, int boxType, long& x, long&y, long& z)
{
	x = currBoxCoords[CIRCUITWIDTH] + boxSize[boxType][CIRCUITWIDTH] - DELTA - (1-pinNr) * distBetweenBoxes[CIRCUITWIDTH];
	y = currBoxCoords[CIRCUITDEPTH] + boxSize[boxType][CIRCUITDEPTH];
	z = currBoxCoords[CIRCUITHEIGHT] + DELTA/*put the contacts a bit higher*/;
}

void boxworld2::savePinCoord(int boxType)
{
	inputpin bp;
	bp[TYPE] = boxType;

	for (int i = 0; i < 2; i++)
	{
		long x,y,z;
		switch (_pinScenario) {
		case PINSVERTICAL:
			pinsVertical(i, boxType, x, y, z);
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
		bp[OFFSETNONCOORD + 3*(1-i) + CIRCUITWIDTH] = x;
		bp[OFFSETNONCOORD + 3*(1-i) + CIRCUITDEPTH] = y;
		bp[OFFSETNONCOORD + 3*(1-i) + CIRCUITHEIGHT] = z;
	}

	tempBoxPins.push_back(bp);
}

void boxworld2::saveBoxCoord(int nr, int boxType)
{
	//coltul din stanga jos spate este al unei cutii
	long x = currBoxCoords[CIRCUITWIDTH]; // - bsize[bindex][0]];
	long y = currBoxCoords[CIRCUITDEPTH]; // - bsize[bindex][1]];
	long z = currBoxCoords[CIRCUITHEIGHT];

	boxcoord pc;
	pc[TYPE] = boxType;
	pc[INJNR] = nr;
	pc[OFFSETNONCOORD + CIRCUITWIDTH] = x;
	pc[OFFSETNONCOORD + CIRCUITDEPTH] = y;
	pc[OFFSETNONCOORD + CIRCUITHEIGHT] = z;

	tempBoxCoords.push_back(pc);

//	boxcoord xx = tempBoxCoords[tempBoxCoords.size() - 1];
}

void boxworld2::pushTempInFinal(vector<int> offset)
{
	for(vector<boxcoord>::iterator it = tempBoxCoords.begin(); it != tempBoxCoords.end(); it++)
	{
		for(int i=0; i<3; i++)
		{
			(*it)[OFFSETNONCOORD + i] += offset[i];
		}
		boxCoords.push_back(*it);
	}

	for(vector<inputpin>::iterator it = tempBoxPins.begin(); it != tempBoxPins.end(); it++)
	{
		for(int nrpin = 0; nrpin<2; nrpin++)
		{
			for(int i=0; i<3; i++)
			{
				(*it)[OFFSETNONCOORD + nrpin*3 + i] += offset[i];
			}
		}
		boxPins.push_back(*it);
	}

	tempBoxCoords.clear();
	tempBoxPins.clear();
}

twobbox boxworld2::computeBBox()
{
	twobbox ret;

	for(vector<boxcoord>::iterator it = tempBoxCoords.begin(); it != tempBoxCoords.end(); it++)
	{
		coordinate c = it->getCoordinate(0);
		//printf("dd %ld : %s\n", (*it)[TYPE], c.toString(',').c_str());
		ret.updateBBox(c, (*it)[TYPE], boxSize);
	}

	return ret;
}

bool boxworld2::advance(schedulerspace& sp, int boxType, int positionWidthAxis)
{
	int width = boxSize[boxType][CIRCUITWIDTH];
	int height = boxSize[boxType][CIRCUITHEIGHT];

	int reqWidth = width + distBetweenBoxes[CIRCUITWIDTH];
	int reqHeight = height + distBetweenBoxes[CIRCUITHEIGHT];

	int positionHeightAxis = NOSPACE;
	bool foundcoordinates = sp.computeCoordinates(reqWidth, reqHeight, positionWidthAxis, positionHeightAxis);
	if(!foundcoordinates)
		return false;

	//actualizeaza focup
	currBoxCoords[CIRCUITWIDTH] = positionWidthAxis;
	currBoxCoords[CIRCUITDEPTH] = -boxSize[boxType][CIRCUITDEPTH];
	//the factor row/spatiu: row is a coordinate: it is a multiple of spatiu
	currBoxCoords[CIRCUITHEIGHT] = _mirrorAboutCircuitWidthAxis*(positionHeightAxis + _heightStepSize*(positionWidthAxis/reqWidth));//on special ocasions shift the boxes to enable routing

	return true;
}

void boxworld2::computeSchedule()
{
	schedulerspace sp(nandc, boxSize[ATYPE][CIRCUITHEIGHT]);

	for (vector<inputpin>::iterator it = nandc.inputList.begin(); it != nandc.inputList.end();)
	{
		int boxTypeIndex = (*it)[TYPE];
		int circuitRow = (*it)[ROWCOORD]; //row as coordinate of the injection point
		int injNumber = (*it)[INJNR];

		// the iterator is advanced using a strange method
		// historic reason
		// to change
		bool torem = advance(sp, boxTypeIndex, circuitRow);
		if (torem)
		{
			saveBoxCoord(injNumber, boxTypeIndex);
			savePinCoord(boxTypeIndex/*, (*it)*/);

			it = nandc.inputList.erase(it); //asta avanseaza iteratorul
		}
		else
			it++;
	}
}

int boxworld2::prepareAdditionalSingleTypeNandc(int nrInitialStates, double stateFail, double gateFail, int startCoord, int stopCoord, int type)
{
	computeadditional cp;
	int maxStates = cp.findParam(nrInitialStates, stateFail, gateFail);
	int difStates = maxStates - nrInitialStates;

	int circuitRowCoordIncrement = boxSize[type][CIRCUITWIDTH] + distBetweenBoxes[CIRCUITWIDTH];

	nandc.addSingleTypeEntries(difStates, type, startCoord, stopCoord, circuitRowCoordIncrement);

	return difStates;
}

int boxworld2::computeMaxNrBoxes(int type, int stepsize)
{
	if(stepsize == 0)
		return 0;

	int howmany = (boxSize[type][CIRCUITWIDTH] + distBetweenBoxes[CIRCUITWIDTH])/ abs(stepsize);

	return howmany;
}

boxworld2::boxworld2(char* iofile, char* bboxfile):boxSize(2)
{
	_pinScenario = PINSVERTICAL;
	_mirrorAboutCircuitWidthAxis = NOMIRROR;
	_heightStepSize = 0;

	int bsizetmp[] = {20, 20, 20, 10, 10, 10}; //deci 5,5,5 si 10,10,10
	boxSize[0] = vector<int>(bsizetmp + 0, bsizetmp + 3);
	boxSize[1] = vector<int>(bsizetmp + 3, bsizetmp + 6);

	int distatmp[] = { 2, 2, 2 }; //totul este in increment de 2, deoarece sunt coordonate de celule primare/duale
	distBetweenBoxes = vector<int>(distatmp + 0, distatmp + 4);

	/*
	 * read the input file to get the coordinates and the parameters of injection point coordinates
	 * at the same time get the minimum and maximum coordinate of the injections
	 * these will be used to create a second stack of additional distillation boxes such that
	 * the astatefail probability will lead to a specified tgatefailprobability
	 */
	FILE* iof = fopen(iofile, "r");
	iofilereader ior;
	nandc = ior.readIOFile(iof);
	fclose(iof);
}
