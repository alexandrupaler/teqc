#include <sys/time.h>
#include <time.h>

#include "utils/profiling.h"

#include "gatenumbers.h"
#include "databasereader.h"
#include "decomposition.h"
#include "plumbingpieces.h"

#include "fileformats/infilereader.h"
#include "fileformats/geomfilewriter.h"
#include "fileformats/geomfilereader.h"
#include "fileformats/adamfilewriter.h"
#include "fileformats/iofilewriter.h"
#include "fileformats/boundingboxfilewriter.h"
#include "fileformats/chpfilewriter.h"
#include "fileformats/psfilewriter.h"
#include "fileformats/qcircfilewriter.h"
#include "fileformats/gmlfilewriter.h"
#include "fileformats/recycledcircuitwriter.h"
#include "fileformats/recycledcircuitreader.h"

#include "fileformats/generaldefines.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

#include "recycling/recycle.h"
#include "cnotcounter.h"
#include "circuitgeometry.h"
#include "circuitmatrix.h"

#include "computeadditional.h"

#include "heuristicparameters.h"

#include "scheduling/schedulercontrol.h"
#include "scheduling/seqschedcontrol.h"
#include "scheduling/asapschedcontrol.h"
#include "scheduling/alaptschedcontrol.h"

#include <climits>
#include <map>

#include "boxworld2.h"
#include "connectpins.h"
#include "connectionmanager.h"

#include "fileformats/iofilewriter.h"
#include "fileformats/boxcoordfilewriter.h"
#include "fileformats/toconnectfilewriter.h"
#include "fileformats/geomfilewriter.h"
#include "fileformats/infilereader.h"

#include "recycling/wireelement.h"
#include "recycling/costmodel.h"
#include "recycling/causalgraph.h"
#include "recycling/recyclegate.h"
#include "circconvert.h"
#include "gidney.h"

//databasereader dbReader;
//cnotcounter cncounter;
circuitgeometry geom;
plumbingpiecesgenerator ppgen;
heuristicparameters heuristicParam;

std::string writeGeometry(char** argv)
{
	/**
	 * The geometry
	 */
	geomfilewriter geomwr;
	std::string fname = geomfilewriter::getGeomFileName(argv[1]);
	FILE* geomfile = fopen(fname.c_str(), "w");
	geomwr.writeGeomFile(geomfile, geom.simplegeom.io, geom.simplegeom.segs, geom.simplegeom.coords);
	fclose(geomfile);

	/**
	 * The plumbing pieces of a geometry
	 */
	adamfilewriter adamwr;
	std::string fnamea = adamfilewriter::getAdamFileName(argv[1]);
	FILE* adamfile = fopen(fnamea.c_str(), "w");
	adamwr.writeAdamFile(adamfile, ppgen.pieces);
	fclose(adamfile);

	/**
	 * The I/O of a circuit. DEBUGGING PURPOSES ONLY
	 */
	iofilewriter iowr;
	std::string iofname = iofilewriter::getIOFileName(fname.c_str());
	FILE* iofile = fopen(iofname.c_str(), "w");
	iowr.writeIOFile(iofile, geom.allpins.inputList);
	fclose(iofile);

//	/**
//	 * The bounding box of a circuit. DEBUGGING PURPOSES ONLY
//	 */
//	boundingboxfilewriter bboxwr;
//	std::string bboxname = boundingboxfilewriter::getBoundingBoxFileName(fname.c_str());
//	FILE* bboxfile = fopen(bboxname.c_str(), "w");
//	bboxwr.writeBoundingBoxFile(bboxfile, geom.simplegeom.boundingbox);
//	fclose(bboxfile);

	return iofname;
}

void writeOtherFormats(const char* basisname, circuitmatrix& circ)
{
	/*chpfilewriter chpwr;
	std::string chpfname = chpfilewriter::getCHPFileName(basisname);
	FILE* chpfile = fopen(chpfname.c_str(), "w");
	chpwr.writeCHPFile(chpfile, circ);
	fclose(chpfile);*/

//	qcircfilewriter qcircwr;
//	std::string fname = qcircfilewriter::getQCircFileName(basisname);
//	FILE* file = fopen(fname.c_str(), "w");
//	qcircwr.writeQCircFile(file, circ);
//	fclose(file);

	/*PSFILEWRITER CAN HANDLE ONLY ICM*/
//	psfilewriter pswr;
//	std::string psfname = psfilewriter::getPostScriptFileName(basisname);
//	FILE* psfile = fopen(psfname.c_str(), "w");
//	pswr.writePostScriptFile(psfile, circ);
//	fclose(psfile);

//	gmlfilewriter gwr;
//	std::string fname22 = gmlfilewriter::getGMLFileName(basisname, 0);
//	FILE* filegwr = fopen(fname22.c_str(), "w");
//	gwr.writeGMLFile(filegwr, causalG);
//	fclose(filegwr);
//
//	fname22 = gmlfilewriter::getGMLFileName(basisname, 1);
//	filegwr = fopen(fname22.c_str(), "w");
//	gwr.writeGMLFile2(filegwr, causalG);
//	fclose(filegwr);
}

void placeGate(recyclegate* current, circuitmatrix& circ, cnotcounter& ccnot)
{
//	printf("place %p\n", current);

	int column = current->level;
	int controlline = current->wirePointers[0]->getUpdatedWire(current->orderNrInGateList)->number;
	int operation = current->type;

	if(current->wirePointers.size() > 1)
	{
		/*almost certain this is a cnot, because only this should be left*/
		int cnot = ccnot.getNextCnotNumber();

		int targetline = current->wirePointers[1]->getUpdatedWire(current->orderNrInGateList)->number;
		circ[controlline][column] = cnot + 0;//+0 because control
		circ[targetline][column] = cnot + 1;//+1 because target
	}
	else if(current->wirePointers.size() == 1)
	{
		circ[controlline][column] = operation;
	}
}

//circuitmatrix createCircuitMatrix(causalgraph& causal)
//{
////	std::vector<recyclegate*> order = causal.equalizeConsideringCosts();
//
//	std::vector<recyclegate*> order = causal.bfs();
//
//	//TODO
//	//causal.equalizeInputLevels();
//
//	//construct a vector of qubitlines
//	long length = causal.getMaxLevel() + 1;
//
//	//vector<vector<int> > inputs = extractSortedInputsList();
//	std::vector<recyclegate*> inputs = causal.getRoots();
//	int nrLines = inputs.size();
//
//	std::vector<qubitline> temp;
//	for (int k = 0; k < nrLines; k++)
//	{
//		qubitline wireQubit(length, WIRE);
//		temp.push_back(wireQubit);
//	}
//
//	//construct the circuitmatrix from the causalgraph
//	//for the ICM representation it should contain only cnot gates
//	cnotcounter ccounter;
//
//	//vector<int> operations = causal.bfs();
//
//	circuitmatrix circ(temp);
//
//	for(std::vector<recyclegate*>::iterator it = order.begin(); it != order.end(); it++)
//	{
//		placeGate(*it, circ, ccounter);
//	}
//
//	return circ;
//}

//void createCircuitText(causalgraph& causal)
//{
//	std::vector<recyclegate*> order = causal.bfs();
//
//	long length = causal.getMaxLevel() + 1;
//
//	std::vector<recyclegate*> inputs = causal.getRoots();
//	int nrLines = inputs.size();
//
//	for(std::vector<recyclegate*>::iterator it = order.begin(); it != order.end(); it++)
//	{
//		writeGateText(*it);
//	}
//}

void writeBoxSchedulerAndClearCoordList(boxworld2& boxworld, const char* basisname, int scheduleNumber)
{
	boxcoordfilewriter bcwr;
	std::string fname = boxcoordfilewriter::getBoxCoordFileName(basisname, scheduleNumber);
	FILE* fboxplan = fopen(fname.c_str(), "w");
	bcwr.writeBoxCoordFile(fboxplan, boxworld.currentConfig.boxSize, boxworld.boxCoords, scheduleNumber);
	fclose(fboxplan);

	//the box coords are cleared after writing them to a file
	//because boxes are separate from the pins
	//the boxes file is used for displaying it
	//the pin file is used for computing the connections

	boxworld.boxCoords.clear();
}

void initCostModels(costmodel* model)
{
	//ASAP
	model[0].gateTypeCosts['a'].gatecost = 1;
	model[0].gateTypeCosts['a'].wirecost = 0;
	model[0].gateTypeCosts['y'].gatecost = 1;
	model[0].gateTypeCosts['y'].wirecost = 0;
	model[0].gateTypeCosts['d'].gatecost = 1; //undeva tre sa fie 1, dar nu mai stiu de ce
	model[0].gateTypeCosts['d'].wirecost = 0;


	model[1].gateTypeCosts['a'].gatecost = 10 + 3; //1;
	model[1].gateTypeCosts['a'].wirecost = 10 + 3; //0;
	model[1].gateTypeCosts['y'].gatecost = 5 + 3; //1;
	model[1].gateTypeCosts['y'].wirecost = 5 + 3; //0;
	model[1].gateTypeCosts['d'].gatecost = 1; //undeva tre sa fie 1, dar nu mai stiu de ce
	model[1].gateTypeCosts['d'].wirecost = 0;
}

bool mySortConnectionsFunction (pinpair p1, pinpair p2)
{
	int orderOn1 = SOURCEPIN;
	int orderOn2 = DESTPIN;

	bool ret = (p1.getPinDetail(orderOn1).coord[CIRCUITWIDTH] < p2.getPinDetail(orderOn1).coord[CIRCUITWIDTH]);
	if(p1.getPinDetail(orderOn1).coord[CIRCUITWIDTH] == p2.getPinDetail(orderOn1).coord[CIRCUITWIDTH])
	{
		ret = p1.getPinDetail(orderOn2).coord[CIRCUITDEPTH] < p2.getPinDetail(orderOn2).coord[CIRCUITDEPTH];
	}

	return ret;
}

void generateCircuitFromFile(causalgraph& causalG, std::vector<std::string>& cucCirc)
{
	causalG.constructFrom2(cucCirc);

	int nrGates0 = causalG.tmpCircuit.size();

	wireelement* current = causalG.getFirstWireElement();
	int nrWire = 0;
	while (current != NULL)
	{
		nrWire++;
		current = current->next;
	}

	int nrWires0 = nrWire;

	double cpu0 = profiling::util_get_cpu_time();

	circconvert convert;
	/*replaces Toffoli gates with Clifford+T decomposition*/
	printf("1\n");
	convert.replaceNonICM2(causalG);

	/*replaces Hadamard gates*/
	//26.10.2017 - comment H and ICM transforms
//	printf("2\n");
//	convert.replaceNonICM2(causalG);

//	printf("3\n");
//	/*required for threshold*/
//	causalG.numberGateList2();
//	convert.replaceICM2(causalG);

	printf("4\n");
	double cpu1 = profiling::util_get_cpu_time();

	for (std::list<recyclegate*>::iterator it = causalG.tmpCircuit.begin();
			it != causalG.tmpCircuit.end(); it++)
	{
		for (size_t i = 0; i < (*it)->wirePointers.size(); i++)
		{
			(*it)->wirePointers[i] = (*it)->wirePointers[i]->getUpdatedWire(
					(*it)->orderNrInGateList);
		}
	}

	for (std::list<recyclegate*>::iterator it = causalG.tmpCircuit.begin();
			it != causalG.tmpCircuit.end(); it++)
	{
		for (size_t i = 0; i < (*it)->wirePointers.size(); i++)
		{
			//11.05.2017
			(*it)->wirePointers[i]->threshold = LONG_MIN;
		}
	}

	int nrWires1 = causalG.numberWires2();
	std::vector<recyclegate*> order = causalG.numberGateList2();

	int nrGates1 = order.size();
	printf("5 order:%d\n", order.size());

	/*
	 * Prepare for recycling?
	 */
	causalG.reconstructWithoutConsideringCosts(order);

#if RELEVELCIRCUIT == 1
	causalG.computeLevels();
#endif

#define TRANSITION 0
#if TRANSITION == 1
	/*
	 * Recycle
	 */
	wirerecycle recycler;
	recycler.recycle(causalG, RECYCLEWIRESEQ);

	for (std::list<recyclegate*>::iterator it = causalG.tmpCircuit.begin();
			it != causalG.tmpCircuit.end(); it++)
	{
		for (size_t i = 0; i < (*it)->wirePointers.size(); i++)
		{
			(*it)->wirePointers[i] = (*it)->wirePointers[i]->getUpdatedWire(0);
		}
	}

	double cpu2 = profiling::util_get_cpu_time();
	printf("STATS: nrW0 %d, nrG0 %d, nrW1 %d, nrG1 %d, time0 %f, time1 %f\n", nrWires0, nrGates0, nrWires1, nrGates1, cpu1-cpu0, cpu2-cpu1);
#endif

	causalG.numberWires2();
	causalG.numberGateList2();

#if RELEVELCIRCUIT == 1
	printf("compute levels...\n");
	causalG.computeLevels();
#endif

#if RELEVELCIRCUIT == 0
	causalG.setLevelIterative();
#endif

//	printf("writing file... \n");
//	recycledcircuitwriter recwr;
//	std::string recname = recycledcircuitwriter::getRecycledFileName("cuc");
//	FILE* frec = fopen(recname.c_str(), "w");
//	recwr.writeRecycledCircuitFile(frec, causalG);
//	fclose(frec);
}

std::vector<std::string> generateRandomTGateCircuit(int nrqubits, int nrgates)
{
	std::vector<std::string> circ;

	std::string ins = "in ";
	std::string outs = "out ";
	for(int i=0; i<nrqubits; i++)
	{
		ins += "-";
		outs += "-";
	}
	circ.push_back(ins);
	circ.push_back(outs);

	for(int i=0; i<nrgates; i++)
	{
		std::stringstream ss;
		ss << (int)floor(drand48() * nrqubits);

		std::string gate = "t " + ss.str();
		circ.push_back(gate);
	}

	return circ;
}

#define RELEVELCIRCUIT 0

int main(int argc, char** argv)
{
	FILE* badseedfile;
	unsigned int seed = 0;
	badseedfile = fopen("seeds.txt", "r");
	fscanf(badseedfile, "%u", &seed);
	fclose(badseedfile);

	if(seed == 0)
	{
		timeval tim;
		gettimeofday(&tim, NULL);
		seed = (unsigned int) (tim.tv_usec);
	}
	srand48(seed);

	printf("SIMULATION SEED %u\n", seed);

	/**
	 * Take a circuit and convert it to ICM
	 */
	costmodel model[2];
	initCostModels(model);

	causalgraph causalG(model[0]);

	bool generateCircuit = true;//false

	if(generateCircuit)
	{
//		cuccaro cuc;
//		int nrCucQb = 10; //1000;
//		std::vector<std::string> cucCirc = cuc.makeCircuit(nrCucQb, false);

//		gidney cuc;
//		int nrCucQb = 100; //1000;
//		std::vector<std::string> cucCirc = cuc.makeCircuit(nrCucQb);

//		std::vector<std::string> cucCirc = generateRandomTGateCircuit(10, 100);
//
//		std::vector<std::string> cucCirc;
//		cucCirc.push_back("in --------------");//--0
//		cucCirc.push_back("out --------------");//--z
////		cucCirc.push_back("c 0 3 4");
////		cucCirc.push_back("K 3 4 7");
////		cucCirc.push_back("c 0 7");
////		cucCirc.push_back("c 0 3 4");
//		cucCirc.push_back("K 3 4 7");
//		cucCirc.push_back("U 3 4 7");

		//used for the adder
//		generateCircuitFromFile(causalG, cucCirc);

		infilereader inread;
		std::vector<std::string> simple = inread.readInFile(argv[1]);
		generateCircuitFromFile(causalG, simple);

	}
	else
	{
		printf("read file...start");

		recycledcircuitreader recread;
		std::string fname = recycledcircuitwriter::getRecycledFileName("cuc");
		recread.readRecycledFile(fname.c_str(), causalG);

		printf("...stop\n");

		std::vector<recyclegate*> order = causalG.numberGateList2();
		causalG.reconstructWithoutConsideringCosts(order);
		causalG.computeLevels();

		causalG.numberWires2();
	}

	printf("CONVERT END\n");

	/**
	 * The causalgraph is used to generate the geometry
	 */
	geom.useBridge = true;

#if RELEVELCIRCUIT == 1
	causalG.computeLevels();
	causalG.equalizeConsideringCosts();
#endif

	//rec.causal.computeLevels();

	/**
	 * Heuristics
	 */
	heuristicParam.init();
	Point::heuristicParam = &heuristicParam;

	/**
	 * Boxworld
	 */
	boxworld2 boxworld;
	boxworld.heuristicParam = &heuristicParam;

	double aStateFail = atof(argv[3]);
	double tGateFail = atof(argv[4]);

	double yStateFail = atof(argv[5]);
	double pGateFail = atof(argv[6]);

	int schedulingType = atoi(argv[7]);

//	boxworld.currentConfig._pinScenario = PINSHORIZRIGHT;
	boxworld.currentConfig._pinScenario = PINSVERTICALLEFT;
	boxworld.initGeomBoundingBox(-2 * DELTA, (causalG.getRoots().size() + 1) * 2 * DELTA,
			INT_MIN, INT_MAX,
			-DELTA - 1, DELTA + 1);
	boxworld.initScheduleGreedy(aStateFail, yStateFail, tGateFail, pGateFail);

	/**
	 * Prepare the iteration
	 */
	bfsState iterationState;
	iterationState.septCurrentlyMaxLevel = -24;
//	std::vector<recyclegate*> rootInputs = causalG.getRoots();
//	iterationState.init(rootInputs);
	std::vector<recyclegate*> bfsOrder = causalG.bfs();
	int nrLines = causalG.getNrQubits();
	/*
	 * 3NOV - adapt the number of wires and total on the grid line
	 */
	gridPosition::logicalQubitsPerLine = boxworld.currentConfig.getBoxTotalDimension(ATYPE, CIRCUITWIDTH) / (2*DELTA);
	gridPosition::totalWires = nrLines;	//for makeGeometry
	iterationState.septInit(bfsOrder, nrLines);

	causalG.stepwiseBfs(iterationState);

	/**
	 * Initialise connnectpins
	 */
	connectpins pinConnector;
	//set the pointer to the rtree of boxworld
	pinConnector.pathfinder.useBoxWorld(&boxworld);
	std::vector<pinpair> toConnBoxes;
	std::vector<pinpair> toConnExtensions;


	/**
	 * The connections pool
	 */
	connectionManager connManager(&heuristicParam);

	/*
	 * There is something to draw or to schedule
	 */
	int counter = 0;

	//greedy e cel care calculeaza cate trebuie in functie de probabilitati
//	greedyschedulercontrol schedControl;
	//seq e cel care planifica in secventa numar egal
	seqschedulercontrol schedControl;
//	asapschedcontrol schedControl;
//	alaptschedcontrol schedControl;
#define WITHOUTSCHEDULING 0

	while(iterationState.isSomethingToDo())
	{
#if WITHOUTSCHEDULING == 1

		iterationState.toDraw.insert(iterationState.toDraw.end(), iterationState.toScheduleInputs[0].begin(), iterationState.toScheduleInputs[0].end());
		geom.makeGeometryFromCircuit(iterationState);
		causalG.stepwiseBfs(iterationState);
#else
		counter++;

		pinblockjournalentry currentJournalEntry;
		currentJournalEntry.synthesisStepNumber = counter;

		printf("************\n         STEP %d\n***********\n", counter);

		/*
		 * Schedule the required boxes
		 */
		bool scheduledNewBoxes = false;
		bool sufficientBoxes = true;

		if(counter == 1)
		{
			iterationState.setMinimumLevel(0);
		}

		sufficientBoxes = false;

		while(!sufficientBoxes)
		{

			/*
			 * Each round of scheduling requires recalibration
			 * of where the boxes are placed
			 */
			boxworld.setCalibration(true);

			//greedy
	//		schedControl.updateControl(iterationState, heuristicParam, boxworld.currentConfig);
			//seq
			schedControl.updateControl(iterationState, heuristicParam, boxworld, connManager);
	//		schedControl.updateControl(iterationState, heuristicParam, boxworld);//for asap and alap?

			if(schedControl.shouldTriggerScheduling())
			{
//				printf("THIS STEP IS SCHEDULING @%ld:", schedControl.getAssumedInputTimeCoordinate());
//				for(std::set<recyclegate*>::iterator it = iterationState.toScheduleInputs[0].begin();
//						it != iterationState.toScheduleInputs[0].end(); it++)
//				{
//					printf(" %d ", (*it)->getId());
//				}
//				printf("\n");

				iterationState.septCurrentlyMaxLevel = schedControl.getAchievedCost();
			}
			scheduledNewBoxes = schedControl.shouldTriggerScheduling();
			sufficientBoxes = !scheduledNewBoxes;

			if(scheduledNewBoxes ||
					(iterationState.getMaximumInputLevel() < iterationState.septCurrentlyMaxLevel))
			{
				/*
				 * This is, for the moment, required only for the seqschedcontrol
				 */
#if RELEVELCIRCUIT == 1
				causalG.updateLevelsAtValue(iterationState, iterationState.getMaximumInputLevel(), iterationState.septCurrentlyMaxLevel);
				causalG.equalizeConsideringCosts();
#endif

#if RELEVELCIRCUIT == 0
//				if(iterationState.toScheduleInputs[0].size() > 0)
//				{
//					//special case: determine if it could have been an r gate
//					if((*iterationState.toScheduleInputs[0].begin())->pushedBy[0]->type == 'i')
//					{
//						printf("draw assume this was a an r gate\n");
//
//						long costToAdd = -2;
//						(*iterationState.toScheduleInputs[0].begin())->level = (counter) * 6;
//						//long costToAdd = iterationState.septCurrentlyMaxLevel - iterationState.getMaximumInputLevel();
//						//if(costToAdd > 0)
//						{
//							printf("draw cost to add %d starting from ordnr%d\n", costToAdd, iterationState.septLastIndex - 1);
//							for(size_t i = iterationState.septLastIndex/* - 1*/; i < iterationState.septBfs.size(); i++)
//							{
//								iterationState.septBfs[i]->level += costToAdd;
//							}
//						}
//					}
//				}
#endif
				printf("NEW COST %ld where prev%ld curr%ld box %ld\n",
						iterationState.septCurrentlyMaxLevel, iterationState.getMaximumInputLevel(),
									iterationState.getRequiredMaximumInputLevel(),
										boxworld.greedyLevel.maxDepth);




#if RELEVELCIRCUIT == 1
				/*
				 * Recalculate the bfs, because the levels were changed
				 */
				std::vector<recyclegate*> bfsOrder = causalG.bfs();
				iterationState.septBfs = bfsOrder;
#endif

				/*
				 * aici nu e frumos: ultima linie arata ca cea din updateControl
				 */
				/**
				 * 3 NOV
				 * this changes the maximumInputLevel
				 */
				if(iterationState.getMaximumInputLevel() < iterationState.septCurrentlyMaxLevel)
				{
					iterationState.saveMaxLevel(iterationState.septCurrentlyMaxLevel);
					schedControl.timeWhenPoolEnds = iterationState.septCurrentlyMaxLevel - heuristicParam.timeBeforePoolEnd;
				}
			}

			/* Check which connections are free
			 * Compute where the pins would be on the time axis using achievedCost
			 */
			connManager.releaseIfNotUsed(/*iterationState.requiredMaximumInputLevel*/
					//connManager.getTimeWhenPoolEnds(iterationState),
					schedControl.getTimeWhenPoolEnds(),
					heuristicParam.connectionHeight,
					pinConnector.pathfinder,
					true /*3 NOV d*/);

			for(int boxType = 0; boxType <= ATYPE; boxType++)
			{
				/*
				 * Check if there are previously successful boxes
				 */
				int necessary = iterationState.toScheduleInputs[boxType].size() - iterationState.septNrAssignedInputs;
				int available = connManager.getUnusedReservedNr(boxType);

				printf("Greedy Schedule @ level%ld type%c exist%d nec%lu \n",
//									iterationState.getRequiredMaximumInputLevel(),
									schedControl.getBoxStartTimeCoordinate(),
									boxType == ATYPE ? 'A' : 'Y',
									/*available, necessary*/
									connManager.getUnusedReservedNr(boxType), iterationState.toScheduleInputs[boxType].size());

				if(schedControl.shouldTriggerScheduling(boxType))
				{
					schedControl.cancelTrigger();

					std::queue<int> greedyBoxPinIds = boxworld.greedyScheduleBoxes(schedControl.getBoxStartTimeCoordinate(), boxType, available, necessary);
	//				queue<int> greedyBoxPinIds = boxworld.computeScheduleCanonical(schedControl.getBoxStartTimeCoordinate(), rec.causal);//(schedControl.getBoxStartTimeCoordinate(), boxType, available, necessary);
	//				queue<int> greedyBoxPinIds = boxworld.computeScheduleALAPT(schedControl.getBoxStartTimeCoordinate(boxType), boxType, available, necessary);

					/*
					 * Reserve connections for the successful boxes
					 */
					while(greedyBoxPinIds.size() > 0)
					{
						int boxPinId = greedyBoxPinIds.front();

						/*inside, skip if too many*/
						/*in cazul asap .. addconnection nu merge*/

						int stopStop = connManager.addConnection(/*is this parameter still used?*/boxType, boxworld.boxPins[boxPinId]);
						if(stopStop == -1)
						{
							printf("1. limit reached\n");
						}
						greedyBoxPinIds.pop();

						/*
						 * 3 NOV - functioneaza doar pentru ca e o singura legatura
						 * in updateConnections ia toata lista la rand si atunci ar putea
						 * iese mai multe legaturi identice
						 */
						connManager.updateConnections(BOXCONNECTION, schedControl.getTimeWhenPoolEnds(), heuristicParam.connectionHeight, toConnBoxes);
					}
				}
			}

			/*
			 * Consume connections for the inputs that need scheduling
			 */
			for(int boxType = 0; boxType <= ATYPE; boxType++)
			{
				for(std::set<recyclegate*>::iterator it = iterationState.toScheduleInputs[boxType].begin();
						it != iterationState.toScheduleInputs[boxType].end(); it++)
				{
					int opId = (*it)->getId();

					if(connManager.assignedIdToConnection.find(opId) == connManager.assignedIdToConnection.end())
					{
						if(!connManager.assignOperationToConnection(opId, boxType))
						{
							printf("NO CONN AVAILABLE for type %d\n", boxType);
	//						break;
						}
						else
						{
							iterationState.septNrAssignedInputs++;

							/*
							 * 3 NOV
							 */
							(*it)->connChannel = connManager.assignedIdToConnection[opId];
						}
					}
				}

				printf("remaining TOSCHEDULE %d\n", iterationState.toScheduleInputs[boxType].size() - iterationState.septNrAssignedInputs);

				sufficientBoxes = (iterationState.toScheduleInputs[boxType].size() - iterationState.septNrAssignedInputs) == 0;
			}
		}

		/**
		 * 3 NOV
		 */
		for(std::set<recyclegate*>::iterator it = iterationState.toScheduleInputs[0].begin();
				it != iterationState.toScheduleInputs[0].end(); it++)
		{
			iterationState.toDraw.push_back(*it);
		}

		/*
		 * Are there sufficient successful boxes?
		 */
		if(!sufficientBoxes)
		{
			printf("Not sufficient successful boxes. Another scheduling round is required.\n");
		}

		/*
		 * Draw geometry until the inputs that were scheduled.
		 * The inputs are not drawn in this iteration, but in the future one
		 * The explanation is that after inputs are found (at level d1), the inputs could
		 * need to be moved at a later level (d2). However, there may be other unscheduled
		 * inputs at level d3 so that d1<d3<d2. Therefore, it seemed easier, to schedule as they come,
		 * and draw as they result after the scheduling
		 */
		geom.makeGeometryFromCircuit(iterationState);

		currentJournalEntry.operationType = "connect and extend";
//		connManager.updateConnections(BOXCONNECTION, schedControl.getTimeWhenPoolEnds(), heuristicParam.connectionHeight, toConnBoxes);
		/*Sort the connections*/
//		std::sort(toConnBoxes.begin(), toConnBoxes.end(), mySortConnectionsFunction);
//		for(size_t i = 0; i < toConnBoxes.size(); i++)
//		{
//			//TODO: this seems not to work. keep false
//			toConnBoxes[i].hasSourceAndDestinationReversed = false;
//			/*
//			 * The first connection has lowest block priority
//			 */
//			currentJournalEntry.blockPriority = 50 + i;
//
//			if(!toConnBoxes[i].isColinear())
//			{
//				//points are not guaranteed colinear if a box is to be connected
//				//to the connections rail
//				currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
//
//				toConnBoxes[i].getPinDetail(SOURCEPIN).
//						addPinBlock(0, CIRCUITDEPTH, /*heuristicParam.connectionBufferTimeLength -*/ 10/*2,3*/, WALKBLOCKED_OCCUPY, currentJournalEntry);//dubios
//
//				toConnBoxes[i].getPinDetail(DESTPIN).
//						addPinBlock(0, CIRCUITHEIGHT, 15/*-10*//*-4*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ghid
//
//				//15.05.2017
//				toConnBoxes[i].getPinDetail(DESTPIN).
//						addPinBlock(0, CIRCUITDEPTH, -6/*-4*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ghid
//			}
//			else
//			{
//				currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
//				toConnBoxes[i].getPinDetail(SOURCEPIN).
//						addPinBlock(0, CIRCUITDEPTH, 100 /*HEURISTIC block forever*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ocupat viitor
//			}
//
//			currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
//			toConnBoxes[i].getPinDetail(DESTPIN).
//					addPinBlock(0, CIRCUITDEPTH, 100 /*block forever*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ocupat viitor
//		}


		connManager.updateConnections(EXTENDCONNECTION, schedControl.getTimeWhenPoolEnds(), heuristicParam.connectionHeight, toConnExtensions);
//		/*Sort the connections*/
//		std::sort(toConnExtensions.begin(), toConnExtensions.end(), mySortConnectionsFunction);
//		for(size_t i = 0; i < toConnExtensions.size(); i++)
//		{
//			//TODO: this seems not to work. keep false
//			toConnExtensions[i].hasSourceAndDestinationReversed = false;
//			/*
//			 * The first connection has lowest block priority
//			 */
//			currentJournalEntry.blockPriority = 50 + i;
//
//			if(!toConnExtensions[i].isColinear())
//			{
//				//points are not guaranteed colinear if a box is to be connected
//				//to the connections rail
//				currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
//
//				toConnExtensions[i].getPinDetail(SOURCEPIN).
//						addPinBlock(0, CIRCUITDEPTH, /*heuristicParam.connectionBufferTimeLength -*/ 10/*2,3*/, WALKBLOCKED_OCCUPY, currentJournalEntry);//dubios
//
//				currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
//				toConnExtensions[i].getPinDetail(DESTPIN).
//						addPinBlock(0, CIRCUITHEIGHT, 15/*-10*//*-4*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ghid
//				toConnExtensions[i].getPinDetail(DESTPIN).
//						addPinBlock(0, CIRCUITDEPTH, 100 /*HEURISTIC block forever*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ocupat viitor
//			}
//			else
//			{
//				currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
//				toConnExtensions[i].getPinDetail(SOURCEPIN).
//						addPinBlock(0, CIRCUITDEPTH, 100 /*HEURISTIC block forever*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ocupat viitor
//			}
//		}


		/*
		 * Form pairs of pool connections and circuit pins
		 */
		std::vector<pinpair> finaliseConn;
		currentJournalEntry.operationType = "finalise";
		bool executedOK = connManager.finaliseAssignedConnections(iterationState, geom.allpins.inputList, finaliseConn);
		if(!executedOK)
		{
			/*
			 * A pair was not possible
			 */
			printf("A PAIR FAILED\n");
			break;
		}
		else
		{
//			/*
//			 * The first pin is connection. The second pin is circuit.
//			 * Because circuit pins are drawn in a future step
//			 */
//			for(size_t i = 0; i < finaliseConn.size(); i++)
//			{
//				currentJournalEntry.blockPriority = 300 + i;
//
//				//03.11.2017
//				currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
//				finaliseConn[i].getPinDetail(SOURCEPIN).
//						addPinBlock(0, CIRCUITDEPTH, heuristicParam.timeBeforePoolEnd, WALKBLOCKED_OCCUPY, currentJournalEntry);
//
//				currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
//				finaliseConn[i].getPinDetail(SOURCEPIN).
//						addPinBlock(0, CIRCUITHEIGHT, -3, WALKBLOCKED_OCCUPY, currentJournalEntry);
//
//				currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
//				finaliseConn[i].getPinDetail(DESTPIN).
//						addPinBlock(0, CIRCUITHEIGHT, 20/*heuristicParam.connectionHeight / 2*/, WALKBLOCKED_OCCUPY, currentJournalEntry);
//			}
		}

		printf("## Finalise %lu FromBoxes %lu Extend %lu\n", finaliseConn.size(), toConnBoxes.size(), toConnExtensions.size());

		/*
		 * Compute
		 */
		pinConnector.blockPins(toConnBoxes);
		pinConnector.blockPins(toConnExtensions);
		pinConnector.blockPins(finaliseConn);

		printf("Bring to Pool...\n");

		if(!pinConnector.processPins(toConnBoxes, CANONICAL1))
		{
			/*
			 * A connection was not possible
			 */
			printf("TO POOL: A CONNECTION FAILED\a\n");
			break;
		}

		/*
		 * Extend the pool connections
		 */
		printf("Extend in Pool...\n");
		if(!pinConnector.processPins(toConnExtensions, CANONICAL2))
		{
			/*
			 * A connection was not possible
			 */
			printf("EXTEND: A CONNECTION FAILED\a\n");
			break;
		}

		for(std::vector<pinpair>::iterator it = toConnExtensions.begin(); it != toConnExtensions.end(); it++)
		{
			pinConnector.setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);
			pinConnector.setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);

			it->getPinDetail(SOURCEPIN).removeBlocks();//blocks.clear();
			it->getPinDetail(DESTPIN).removeBlocks();//blocks.clear();
		}


		printf("Get from pool...\n");
		if(!pinConnector.processPins(finaliseConn, CANONICAL1))
		{
			/*
			 * A connection was not possible
			 */
			printf("FROM POOL: A CONNECTION FAILED\a\n");
			break;
		}
		else
		{
//			/*
//			 * 3NOV - add segment connecting the last connections
//			 * should be only two connections in this resource estimator
//			 */
//			if(finaliseConn.size() > 0)
//			{
//				int idx1 = pinConnector.connectionsGeometry.addCoordinate(finaliseConn[0].getPinDetail(DESTPIN).coord);
//				int idx2 = pinConnector.connectionsGeometry.addCoordinate(finaliseConn[1].getPinDetail(DESTPIN).coord);
//				pinConnector.connectionsGeometry.addSegment(idx1, idx2);
//			}
		}

		/**
		 * Unblock the unblockable
		 */
		for(std::vector<pinpair>::iterator it = toConnBoxes.begin(); it != toConnBoxes.end(); it++)
		{
			pinConnector.setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);
			pinConnector.setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);

			it->getPinDetail(SOURCEPIN).removeBlocks();//blocks.clear();
			it->getPinDetail(DESTPIN).removeBlocks();//blocks.clear();
		}
		for(std::vector<pinpair>::iterator it = finaliseConn.begin(); it != finaliseConn.end(); it++)
		{
			pinConnector.setWalkable(it->getPinDetail(SOURCEPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);
			pinConnector.setWalkable(it->getPinDetail(DESTPIN), REMOVE_BLOCK, WALKBLOCKED_GUIDE);

			it->getPinDetail(SOURCEPIN).removeBlocks();//blocks.clear();
			it->getPinDetail(DESTPIN).removeBlocks();//blocks.clear();
		}

		/*Unblock all connection pool pins of guides*/
		pinConnector.unblockPins(connManager.connectionPins);
		for(size_t i = 0; i < connManager.connectionPins.size(); i++)
		{
			connManager.connectionPins[i].getPinDetail(0).removeBlocks();
			connManager.connectionPins[i].getPinDetail(1).removeBlocks();
		}

		printf("712: %d %d %d\n", schedControl.getTimeWhenPoolEnds(), schedControl.getBoxStartTimeCoordinate(), iterationState.getRequiredMaximumInputLevel());

		//previous versions of the software will not work
		//because of toconnectfilewriter
		finaliseConn.clear();
		toConnBoxes.clear();
		toConnExtensions.clear();

		causalG.stepwiseBfs(iterationState);

//		/*
//		 * Check which connections are free
//		 * Compute where the pins would be on the time axis using achievedCost
//		 */
//		connManager.releaseIfNotUsed(/*iterationState.requiredMaximumInputLevel*/
//				//connManager.getTimeWhenPoolEnds(iterationState),
//				schedControl.getTimeWhenPoolEnds(),
//				heuristicParam.connectionHeight,
//				pinConnector.pathfinder,
//				true /*3 NOV do not check the points computed by the path finding. previous version had TRUE.*/);

		printf("------\n");

		if(counter == 10)
		{
//			cp.geom.segs.clear();
//			break;
		}

		/*
		 * Resize connection pool width
		 * Width can increase or decrease
		 */
		//if(scheduledNewBoxes)
		{
			int maxConn = connManager.getCurrentNumberOfConnections();
			int max = maxConn * DELTA;
			boxworld.setConnectionBoxWidth(max);

			printf("Resize connection box to width %d\n", max);
		}

		/*
		 * Reset debug geometries
		 */
		pinConnector.debugBlockOccupyGeometry.reset();
		pinConnector.debugBlockGuideGeometry.reset();
#endif
	}

	/*
	 * Load the geometry of the A state distillation circuit
	 */
//	geomfilereader geomreader;
//	FILE* geomfile = fopen("adist.raw.in.geom", "r");
//	geometry distillationAState;
//	geomreader.readGeomFile(geomfile, distillationAState.io, distillationAState.segs, distillationAState.coords);
//	fclose(geomfile);
//	for(std::vector<boxcoord>::iterator it = boxworld.boxCoords.begin(); it != boxworld.boxCoords.end(); it++)
//	{
//		convertcoordinate offset = (*it).getPinDetail(0).coord;
//
//		long subs = 3;
//		offset[CIRCUITWIDTH] -= subs;
//		offset[CIRCUITDEPTH] -= subs;
//		offset[CIRCUITHEIGHT] -= -subs;
//
//		geom.simplegeom.appendGeometry(distillationAState, offset);
//	}


	/**
	 * Generate plumbing pieces
	 */
	ppgen.generateFromGeometry(geom.simplegeom);
	ppgen.generateFromGeometry(pinConnector.connectionsGeometry);
	ppgen.removeWrongCorners();

	printf("%s, %d\n", argv[1], ppgen.pieces.size());

	/**
	 * Write the generated geometry to a file
	 */
	std::string iofname = writeGeometry(argv);

	/**
	 * DEBUGGING: Write the file for pin connections
	 */
	toconnectfilewriter tcwr;
	std::string fnameConn = toconnectfilewriter::getToConnectFileName(iofname.c_str());
	FILE* fileConn = fopen(fnameConn.c_str(), "w");
	tcwr.writeToConnectFile(fileConn, toConnBoxes);
	fclose(fileConn);

	/**
	 * Write the scheduled boxes
	 */
	writeBoxSchedulerAndClearCoordList(boxworld, iofname.c_str(), 0);

	/**
	 * Write the connections between the geometry and the boxes
	 */
	geomfilewriter geomwr;
	std::string fname = geomfilewriter::getGeomFileName(fnameConn.c_str());
	FILE* file = fopen(fname.c_str(), "w");
	geomwr.writeGeomFile(file, pinConnector.connectionsGeometry.io, pinConnector.connectionsGeometry.segs, pinConnector.connectionsGeometry.coords);
	fclose(file);

	/**
	 * Write the occupy blocks debug
	 */
	geomfilewriter geomwr2;
	std::string fname2 = geomfilewriter::getGeomFileName(fnameConn.c_str());
	fname2 += "debug1";
	FILE* file2 = fopen(fname2.c_str(), "w");
	geomwr2.writeGeomFile(file2, pinConnector.debugBlockOccupyGeometry.io, pinConnector.debugBlockOccupyGeometry.segs, pinConnector.debugBlockOccupyGeometry.coords);
	fclose(file2);

	/**
	 * Write the guide blocks debug
	 */
	geomfilewriter geomwr3;
	std::string fname3 = geomfilewriter::getGeomFileName(fnameConn.c_str());
	fname3 += "debug2";
	FILE* file3 = fopen(fname3.c_str(), "w");
	geomwr3.writeGeomFile(file3, pinConnector.debugBlockGuideGeometry.io, pinConnector.debugBlockGuideGeometry.segs, pinConnector.debugBlockGuideGeometry.coords);
	fclose(file3);

	//writeOtherFormats(argv[1]);

	return 0;
}
