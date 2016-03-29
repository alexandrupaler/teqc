#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <limits.h>
#include <map>

#include "boxworld2.h"
#include "fileformats/iofilewriter.h"
#include "fileformats/boxcoordfilewriter.h"


void writeBoxSchedulerAndClearCoordList(boxworld2& boxworld, const char* basisname, int scheduleNumber)
{
	boxcoordfilewriter bcwr;
	string fname = boxcoordfilewriter::getBoxCoordFileName(basisname, scheduleNumber);
	FILE* fboxplan = fopen(fname.c_str(), "w");

	bcwr.writeBoxCoordFile(fboxplan, boxworld.boxSize, boxworld.boxCoords, scheduleNumber);
	fclose(fboxplan);

	//the box coords are cleared after writing them to a file
	//because boxes are separate from the pins
	//the boxes file is used for displying it
	//the pin file is used for computing the connections

	boxworld.boxCoords.clear();
}

twobbox computeNormalBoxesScheduler(char* basisname, boxworld2& boxworld)
{
	boxworld.computeSchedule();
	twobbox bboxN = boxworld.computeBBox();

	int offsettmp[] = {0, -10, 4};
	vector<int> offset = vector<int>(offsettmp + 0, offsettmp + 4);
	boxworld.pushTempInFinal(offset);

	writeBoxSchedulerAndClearCoordList(boxworld, basisname, 0);

	return bboxN;
}

void computeAdditionalATypeScheduler(char* basisname, boxworld2& boxworld, double aStateFail, double tGateFail, twobbox bboxN)
{
	/*compute the scheduler for the additional A states*/
	boxworld._pinScenario = PINSHORIZLEFT;
	boxworld._heightStepSize = -DELTA;
	boxworld._mirrorAboutCircuitWidthAxis = NOMIRROR;

	int howmany = boxworld.computeMaxNrBoxes(ATYPE, boxworld._heightStepSize);
	//int howmany = (boxworld.bsize[0][CIRCUITWIDTH] + boxworld.dista[CIRCUITWIDTH])/ abs(boxworld._heightStepSize);

	int lengthAScheduler = howmany * boxworld.boxSize[ATYPE][CIRCUITWIDTH];
	boxworld.prepareAdditionalSingleTypeNandc(boxworld.nandc.nrAStates, aStateFail, tGateFail, lengthAScheduler, 0, 0);
	boxworld.computeSchedule();

	twobbox bboxA = boxworld.computeBBox();

	vector<int> offset = vector<int>(3);
	offset[CIRCUITWIDTH] = -bboxA.maxWidth + bboxN.minWidth - boxworld.distBetweenBoxes[CIRCUITWIDTH];
	offset[CIRCUITHEIGHT] = -bboxA.minHeight + 4/*hardcoded*/;
	offset[CIRCUITDEPTH] = -10;//hardcoded?

	boxworld.pushTempInFinal(offset);

	writeBoxSchedulerAndClearCoordList(boxworld, basisname, 1);
}

void computeAdditionalYTypeScheduler(char* basisname, boxworld2& boxworld, double yStateFail, double pGateFail, twobbox bboxN)
{
	/*compute the scheduler for the additional Y states*/
	boxworld._pinScenario = PINSHORIZRIGHT;
	boxworld._heightStepSize = DELTA;
	boxworld._mirrorAboutCircuitWidthAxis = NOMIRROR;

	int howmany = boxworld.computeMaxNrBoxes(YTYPE, boxworld._heightStepSize);
	//howmany = (boxworld.bsize[1][CIRCUITWIDTH] + boxworld.dista[CIRCUITWIDTH])/ abs(boxworld._heightStepSize);
	int lengthYScheduler = howmany * boxworld.boxSize[YTYPE][CIRCUITWIDTH];
	int totalAdditional = boxworld.prepareAdditionalSingleTypeNandc(boxworld.nandc.nrYStates, yStateFail, pGateFail, 0, lengthYScheduler, 1);

	//move at the beginning of the circuit
	boxworld.computeSchedule();

	twobbox bboxY = boxworld.computeBBox();

	vector<int> offset = vector<int>(3);
	offset[CIRCUITWIDTH] = bboxN.maxWidth - (bboxY.minWidth/*due to construction not zero*/) + boxworld.distBetweenBoxes[CIRCUITWIDTH];
	offset[CIRCUITHEIGHT] = -bboxY.minHeight + 4/*hardcoded*/;
	offset[CIRCUITDEPTH] = -10;//hardcoded?

	boxworld.pushTempInFinal(offset);

	writeBoxSchedulerAndClearCoordList(boxworld, basisname, 2);
}

int main(int arg, char** argv)
{
	srand48(time(NULL));

	double aStateFail = atof(argv[3]);
	double tGateFail = atof(argv[4]);

	double yStateFail = atof(argv[3]);
	double pGateFail = atof(argv[4]);

	boxworld2 boxworld(argv[1], argv[2]);

	twobbox b2 = computeNormalBoxesScheduler(argv[1], boxworld);

	computeAdditionalATypeScheduler(argv[1], boxworld, aStateFail, tGateFail, b2);

	computeAdditionalYTypeScheduler(argv[1], boxworld, yStateFail, pGateFail, b2);

	iofilewriter iowr;
	string fname = iowr.getIOFileName(argv[1]);
	FILE* file = fopen(fname.c_str(), "w");
	iowr.writeIOFile(file, boxworld.boxPins);
	fclose(file);

	return 1;
}
