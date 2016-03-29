#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "fileformats/iofilereader.h"
#include "faildistillations.h"

using namespace std;

faildistillations::faildistillations(char* circIoFile, char* boxPinsFile)
{
	FILE* file = fopen(circIoFile, "r");
	iofilereader ior;
	circIOs = ior.readIOFile(file);
	//nandc.read(circIoFile);//read the inputs
	fclose(file);

	file = fopen(boxPinsFile, "r");
	boxIOs = ior.readIOFile(file);
	fclose(file);
}

/* Probability of Failures: 0 - does not fail; 1 - always fail
 * returns true if diced prob over parameter
 * false if not
 */
double faildistillations::randomCheckThreshold(double& prob)
{
	double randomFail = drand48();
	//printf("p=%lf\n", randomFail);

	bool ret = (randomFail >= prob);
	return ret;
}

/*
 * the function returns
 * NOBOXESAVAIL == -1, if the list of ADDITIONAL distillation boxes was CONSUMED
 * value less than nandc.size() if an ORIGINAL distillation box was used
 * value greater than nandc.size() if an ADDITIONAL distillation box was used
 */
int faildistillations::decide(double prob, int origposition, int lastPosition)
{
	//what type does the original distillation box have?
	int type = boxIOs.getIOType(origposition);

	//try a normal box first, not an additional one
	bool randomIsOkBox = randomCheckThreshold(prob);
	if(randomIsOkBox)
	{
		//the initial box is not fucked
		return origposition;
	}

	//if there are no additionals, and the initial box did not work
	if(lastPosition == NOBOXESAVAIL)
		return lastPosition;

	//the initial box is fucked, check the additional ones
	//two pins belong to the same distillation box
	for(int i=lastPosition + 1/*+2*/; i<boxIOs.size();/* i+=2*/i++)
	{
		//is the current box of a type similar to the original?
		//this check is somehow redundant given the way laspos are computed
		if(boxIOs.getIOType(i) == type)
		{
			randomIsOkBox = randomCheckThreshold(prob);
			if(randomIsOkBox)
			{
				/*not failed*/
				return i;
			}
			else
			{
				failedDistillations.push_back(i);//save the failed distillation box indices into a list
			}
		}
	}

	return NOBOXESAVAIL;//could happen when no distillation box is available, because all the additional ones are fucked
}

/*
 * 	if Atype: pair pins are connected 0+1, 1+0
 * 	otherwise 0+0, 1+1
 * 	where first index is circuit io, and second is distillation box
 *
 * 	This method is used in order to generate connections which do not generate intersecting geometries (see connectpins)
 */

vector<pair<int, int> > faildistillations::computePinIndexPairs(int boxIndex)
{
	vector<pair<int, int> > ret;

	bool horizontalAPins = false;
	int firstPinOffset = 0;
	int secondPinOffset = 1;

	if(boxIOs.getIOType(boxIndex) == ATYPE)
	{
		//are horizontal pins? - from the additional boxes? - see boxworld2.cpp pinsHorizontal
		//horizontalAPins = (distpins[boxIndex + firstPinOffset][1 + 0] != distpins[boxIndex + secondPinOffset][1 + 0]);
		horizontalAPins = (boxIOs.inputList[boxIndex][ROWCOORD] != boxIOs.inputList[boxIndex][ROWCOORD + 3]);
	}

	for(int i=0; i<2; i++)
	{
		pair<int, int> p;
		int boxpin = ((boxIOs.getIOType(boxIndex) == ATYPE && horizontalAPins) ? 1 - i : i);

		p.first = i;
		p.second = boxpin;

		ret.push_back(p);
	}

	return ret;
}

int faildistillations::storeConnectEntry(int IOIndex, int boxIndex)
{
	int type = circIOs.getIOType(IOIndex);

	if(boxIndex == NOBOXESAVAIL)
	{
		return nonFailedPinPairCoords.size();//nothing changed, nothing added
	}

	vector<pair<int, int> > pinIndices = computePinIndexPairs(boxIndex);

	for (int i=0; i<2; i++)
	{
		pinpair pair;

		pair.setCoordinate(0, circIOs.inputList[IOIndex].getCoordinate(pinIndices[i].first));

		//if A pair pins are connected 0+1, 1+0
		//otherwise 0+0, 1+1
		//int boxpin = ((type == ATYPE && horizontalAPins) ? 1 - i : i);
		pair.setCoordinate(1, boxIOs.inputList[boxIndex].getCoordinate(pinIndices[i].second));

		nonFailedPinPairCoords.push_back(pair);
	}

	return nonFailedPinPairCoords.size();
}

int faildistillations::decideAndStore(double failprob, int IOIndex, int& lastpos)
{
	int currlastpos = NOBOXESAVAIL;

	currlastpos = decide(failprob, IOIndex, lastpos);

	if (currlastpos != NOBOXESAVAIL)
		storeConnectEntry(IOIndex, currlastpos);

	//if not the original distillation box, but an additional one
	//if (currlastpos != transformIOIndexToDistIndex(IOIndex))
	if (currlastpos != IOIndex)
		lastpos = currlastpos;

	return currlastpos;
}

/*
 * iterates through the inputs of the circuit and chooses a distillation box to attach
 * because the additional Y distillation boxes are on the lhs of the circuit, the iteration goes from left to right
 * because the additional A distillation boxes are on the rhs of the circuit, the iteration goes from right to left
 * once an input of corresponding type was found, the decideAndStore method is called
 */
void faildistillations::selectDistillationForCircuitInput(double afail, double yfail)
{
	//lastpos stores the index of the last used additional distillation box
	//-2 because in decide the loop begins with +2
	int lastAPos = (circIOs.size() - 1);// * 2;
	//foreach input to the circuit get the first correct distillation box
	for(int nrInput = 0; nrInput < circIOs.size(); nrInput++)
	{
		if(circIOs.getIOType(nrInput) != ATYPE)
			continue;

		decideAndStore(afail, nrInput, lastAPos);
	}

	int lastYPos = (circIOs.size() - 1);// * 2;
	//foreach input to the circuit get the first correct distillation box
	for(int nrInput = circIOs.size() - 1; nrInput >=0; nrInput--)
	{
		if(circIOs.getIOType(nrInput) != YTYPE)
			continue;

		decideAndStore(yfail, nrInput, lastYPos);
	}
}

void faildistillations::writeFailedDistillations()
{
	/*not implemented*/
}

faildistillations::faildistillations()
{

}
