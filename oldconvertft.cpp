#include "gatenumbers.h"
#include "databasereader.h"
#include "decomposition.h"
#include "plumbingpieces.h"

#include "fileformats/infilereader.h"
#include "fileformats/geomfilewriter.h"
#include "fileformats/adamfilewriter.h"
#include "fileformats/iofilewriter.h"
#include "fileformats/boundingboxfilewriter.h"
#include "fileformats/chpfilewriter.h"
#include "fileformats/psfilewriter.h"
#include "fileformats/qcircfilewriter.h"

#include "fileformats/generaldefines.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <sys/time.h>

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

using namespace std;

databasereader dbReader;
cnotcounter cncounter;
circuitgeometry geom;
circuitmatrix circ;
plumbingpiecesgenerator ppgen;
heuristicparameters heuristicParam;

string writeGeometry(char** argv)
{
	/**
	 * The geometry
	 */
	geomfilewriter geomwr;
	string fname = geomfilewriter::getGeomFileName(argv[1]);
	FILE* geomfile = fopen(fname.c_str(), "w");
	geomwr.writeGeomFile(geomfile, geom.simplegeom.io, geom.simplegeom.segs, geom.simplegeom.coords);
	fclose(geomfile);

	/**
	 * The plumbing pieces of a geometry
	 */
	adamfilewriter adamwr;
	string fnamea = adamfilewriter::getAdamFileName(argv[1]);
	FILE* adamfile = fopen(fnamea.c_str(), "w");
	adamwr.writeAdamFile(adamfile, ppgen.pieces);
	fclose(adamfile);

	/**
	 * The I/O of a circuit. DEBUGGING PURPOSES ONLY
	 */
	iofilewriter iowr;
	string iofname = iofilewriter::getIOFileName(fname.c_str());
	FILE* iofile = fopen(iofname.c_str(), "w");
	iowr.writeIOFile(iofile, geom.allpins.inputList);
	fclose(iofile);

	/**
	 * The bounding box of a circuit. DEBUGGING PURPOSES ONLY
	 */
	boundingboxfilewriter bboxwr;
	string bboxname = boundingboxfilewriter::getBoundingBoxFileName(fname.c_str());
	FILE* bboxfile = fopen(bboxname.c_str(), "w");
	bboxwr.writeBoundingBoxFile(bboxfile, geom.simplegeom.boundingbox);
	fclose(bboxfile);

	return iofname;
}

void writeOtherFormats(const char* basisname)
{
	/*chpfilewriter chpwr;
	string chpfname = chpfilewriter::getCHPFileName(basisname);
	FILE* chpfile = fopen(chpfname.c_str(), "w");
	chpwr.writeCHPFile(chpfile, circ);
	fclose(chpfile);*/

	/*qcircfilewriter qcircwr;
	string fname = qcircfilewriter::getQCircFileName(basisname);
	FILE* file = fopen(fname.c_str(), "w");
	qcircwr.writeQCircFile(file, circ);
	fclose(file);*/

	psfilewriter pswr;
	string psfname = psfilewriter::getPostScriptFileName(basisname);
	FILE* psfile = fopen(psfname.c_str(), "w");
	pswr.writePostScriptFile(psfile, circ);
	fclose(psfile);
}

void placeGate(causalgraph& causal, int currId, circuitmatrix& circ, cnotcounter& ccnot)
{
	int column = causal.circuit[currId].level;
	int controlline = causal.circuit[currId].wires[0];
	int operation = causal.circuit[currId].type;

	if(causal.circuit[currId].wires.size() > 1)
	{
		int cnot = ccnot.getNextCnotNumber();

		int targetline = causal.circuit[currId].wires[1];
		circ[controlline][column] = cnot + 0;//+0 because control
		circ[targetline][column] = cnot + 1;//+1 because target
	}
	else if(causal.circuit[currId].wires.size() == 1)
	{
		circ[controlline][column] = operation;
	}
}

circuitmatrix createCircuitMatrix(causalgraph& causal)
{
	vector<int> order = causal.equalizeConsideringCosts();

	//TODO
	//causal.equalizeInputLevels();

	//construct a vector of qubitlines
	int length = causal.getMaxLevel() + 1;

	//vector<vector<int> > inputs = extractSortedInputsList();
	vector<int> inputs = causal.getRoots();
	int nrLines = inputs.size();

	vector<qubitline> temp;
	for (int k = 0; k < nrLines; k++)
	{
		qubitline wireQubit(length, WIRE);
		temp.push_back(wireQubit);
	}

	//use the temp vector to initialise the circuitmatrix
	circuitmatrix circ(temp);

	//construct the circuitmatrix from the causalgraph
	//for the ICM representation it should contain only cnot gates
	cnotcounter ccounter;

	//vector<int> operations = causal.bfs();

	for(vector<int>::iterator it = order.begin(); it != order.end(); it++)
	{
		int currid = *it;
		placeGate(causal, currid, circ, ccounter);
	}

	return circ;
}

void writeBoxSchedulerAndClearCoordList(boxworld2& boxworld, const char* basisname, int scheduleNumber)
{
	boxcoordfilewriter bcwr;
	string fname = boxcoordfilewriter::getBoxCoordFileName(basisname, scheduleNumber);
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
	circconvert convert(argv[1]);
	convert.replaceNonICM();
	convert.replaceNonICM();
	convert.replaceICM();

	costmodel model[2];
	initCostModels(model);

	/**
	 * Perform recycling
	 */
	wirerecycle rec;
//	rec.recycle(convert, RECYCLENONE, model[0]);
	rec.recycle(convert, RECYCLEWIRESEQ, model[0]);

	//rec.recycle(convert, RECYCLENONE, model[0]);

	/**
	 * Generate a circuitmatrix from a causalgraph
	 */
	//circ  = createCircuitMatrix(rec.causal);
	//circ.printCirc();

	/**
	 * The circuitmatrix is used to generate the geometry
	 * or
	 * The causalgraph is used to generate the geometry
	 */
	geom.useBridge = true;
	//geom.makeGeometryFromCircuit(circ);
	rec.causal.equalizeConsideringCosts();
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

	boxworld.currentConfig._pinScenario = PINSHORIZRIGHT;
	boxworld.initGeomBoundingBox(-DELTA, (rec.causal.getRoots().size() + 1) * DELTA,
			INT_MIN, INT_MAX,
			-DELTA - 1, DELTA + 2);
	boxworld.initScheduleGreedy(aStateFail, yStateFail, tGateFail, pGateFail);

	/**
	 * Prepare the iteration
	 */
	bfsState iterationState;
	vector<int> rootInputs = rec.causal.getRoots();
	iterationState.init(rootInputs);

	rec.causal.stepwiseBfs(iterationState);

	/**
	 * Initialise connnectpins
	 */
	connectpins pinConnector;
	//set the pointer to the rtree of boxworld
	pinConnector.pathfinder.useBoxWorld(&boxworld);
	vector<pinpair> toconn;

	/**
	 * The connections pool
	 */
	connectionManager connManager(&heuristicParam);

	/*
	 * There is something to draw or to schedule
	 */
	int counter = 0;
//	long previousTimeCoordinate = -1;

	greedyschedulercontrol schedControl;
//	seqschedulercontrol schedControl;
//	asapschedcontrol schedControl;
//	alaptschedcontrol schedControl;

	while(iterationState.isSomethingToDo())
	{
		counter++;

		pinblockjournalentry currentJournalEntry;
		currentJournalEntry.synthesisStepNumber = counter;

		printf("************\n         STEP %d\n***********\n", counter);

		/*
		 * Schedule the required boxes
		 */
		bool scheduledNewBoxes = false;
		bool sufficientBoxes = true;

		/*
		 * Each round of scheduling requires recalibration
		 */
		boxworld.setCalibration(true);


		if(counter==1)
		{
			iterationState.setMinimumLevel(0);
		}
//		schedControl.updateControl(iterationState, heuristicParam, boxworld, connManager, ATYPE);
		schedControl.updateControl(iterationState, heuristicParam, boxworld.currentConfig);

//
		if(schedControl.shouldTriggerScheduling())
		{
			printf("THIS STEP IS SCHEDULING %ld %ld\n", schedControl.getAssumedInputTimeCoordinate()/*, schedControl.getSchedulingRoundLength()*/);

			scheduledNewBoxes = schedControl.shouldTriggerScheduling();
		}

//		schedControl.updateControl(iterationState, heuristicParam, boxworld, connManager);

//		schedControl.updateControl(iterationState, heuristicParam, boxworld);

//		schedControl.updateControl(iterationState, heuristicParam, boxworld, connManager);

		if(schedControl.shouldTriggerScheduling())
		{
			printf("THIS STEP IS SCHEDULING @ %ld\n", schedControl.getAssumedInputTimeCoordinate()/*, schedControl.getSchedulingRoundLength()*/);
		}


		bool limitReached = false;
		for(int boxType = 0; boxType < 2; boxType++)
		{
			/*
			 * Check if there are previously successful boxes
			 */
			//FOR OTHER SCHEDS?
//			int necessary = iterationState.toScheduleInputs[boxType].size();
//			int available = connManager.getUnusedReservedNr(boxType);

			//FOR SEQSCHED
			int necessary = 30;
			int available = 0;

			printf("Greedy Schedule @ level%ld type%c exist%d nec%d \n",
								iterationState.getRequiredMaximumInputLevel(),
								boxType == ATYPE ? 'A' : 'Y',
								/*available, necessary*/
								connManager.getUnusedReservedNr(boxType), iterationState.toScheduleInputs[boxType].size());

			//trigger
			if(schedControl.shouldTriggerScheduling(boxType))
			{
				scheduledNewBoxes = true;

				schedControl.cancelTrigger();

				queue<int> greedyBoxPinIds = boxworld.greedyScheduleBoxes(schedControl.getBoxStartTimeCoordinate(), boxType, available, necessary);

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
				}
			}

			/*
			 * Update sufficient
			 */
//			sufficientBoxes = sufficientBoxes && connManager.enoughReserved(boxType, necessary);
			//always true
			sufficientBoxes = true;
		}

		for(int boxType = 0; boxType < 2; boxType++)
		{
			/*
			 * Consume connections for the inputs that need scheduling
			 */
			for(size_t i = 0; i<iterationState.toScheduleInputs[boxType].size(); i++)
			{
				int opId = iterationState.toScheduleInputs[boxType][i];

				if(!connManager.assignOperationToConnection(opId, boxType))
				{
					printf("NO CONN AVAILABLE for type %d\n", boxType);
					limitReached = true;
					break;
				}
			}

			if(limitReached)
			{
				break;
			}
		}
		if(limitReached)
		{
			break;
		}

		/*
		 * Are there sufficient successful boxes?
		 */
		if(!sufficientBoxes)
		{
			printf("Not sufficient successful boxes. Another scheduling round is required.\n");
		}

		/*
		 * This is, for the moment, required only for the seqschedcontrol
		 */
//		if(scheduledNewBoxes)
//		{
////			rec.causal.updateLevelsAtValue(iterationState, iterationState.requiredMaximumInputLevel, achievedCost);
//			rec.causal.updateLevelsAtValue(iterationState, iterationState.getMaximumInputLevel(), schedControl.getAchievedCost());
//			rec.causal.equalizeConsideringCosts();
//
////			iterationState.requiredMaximumInputLevel = achievedCost;
//
//			printf("NEW COST %ld where prev%ld curr%ld box %ld\n",
//					schedControl.getAchievedCost(), iterationState.getMaximumInputLevel(),
//								iterationState.getRequiredMaximumInputLevel(),
//									boxworld.greedyLevel.maxDepth);
//		}

//		/*
//		 * Recalculate causal graph
//		 */
//		if(scheduledNewBoxes)
//		{
//			long achievedCost = heuristicParam.connectionBufferTimeLength/*HEURISTIC*/ + boxworld.getTimeWhenBoxesEnd();
//			if(iterationState.requiredMaximumInputLevel < achievedCost)
//			{
//				/*
//				 * Inputs (scheduled/unscheduled) are on the rightmost boundary of the circuit segment
//				 * that will be drawn. It is fine to move them to have the minimum achievedCost.
//				 */
//				rec.causal.updateLevelsAtValue(iterationState, iterationState.requiredMaximumInputLevel, achievedCost);
//				rec.causal.equalizeConsideringCosts();
//
//				iterationState.requiredMaximumInputLevel = achievedCost;
//
//				printf("NEW COST %ld where prev%ld curr%ld box %ld\n",
//									achievedCost, iterationState.maximumInputLevel,
//									iterationState.requiredMaximumInputLevel,
//									boxworld.greedyLevel.maxDepth);
//			}
//		}

		/*
		 * Resize connection pool width
		 * Width can increase or decrease
		 */
		if(scheduledNewBoxes)
		{
			int maxConn = connManager.getCurrentNumberOfConnections();
			int max = maxConn * DELTA;
			boxworld.setConnectionBoxWidth(max);

			printf("Resize connection box to width %d\n", max);
		}

		/*
		 * Form pairs of pool connections and circuit pins
		 */
		vector<pinpair> finaliseConn;
		if(sufficientBoxes)
		{
			/*
			 * Draw geometry until the inputs that were scheduled.
			 * The inputs are not drawn in this iteration, but in the future one
			 * The explanation is that after inputs are found (at level d1), the inputs could
			 * need to be moved at a later level (d2). However, there may be other unscheduled
			 * inputs at level d3 so that d1<d3<d2. Therefore, it seemed easier, to schedule as they come,
			 * and draw as they result after the scheduling
			 */
			geom.makeGeometryFromCircuit(rec.causal, iterationState);

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
				/*
				 * The first pin is connection. The second pin is circuit.
				 * Because circuit pins are drawn in a future step
				 */
				for(size_t i = 0; i < finaliseConn.size(); i++)
				{
					currentJournalEntry.blockPriority = i;

					currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
					finaliseConn[i].getPinDetail(SOURCEPIN).
							addPinBlock(0, CIRCUITDEPTH, heuristicParam.timeBeforePoolEnd, WALKBLOCKED_GUIDE, currentJournalEntry);

					currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
					finaliseConn[i].getPinDetail(DESTPIN).
							addPinBlock(0, CIRCUITHEIGHT, 10, WALKBLOCKED_OCCUPY, currentJournalEntry);
				}
			}
		}

		currentJournalEntry.operationType = "connect and extend";

		connManager.updateConnections(schedControl.getTimeWhenPoolEnds(), heuristicParam.connectionHeight, toconn);

//		printf("  Extend %lu conn: assume@%ld -> PoolEnd@%ld, where minL@%ld, distRound@%ld, req@%ld, tbe@%d\n", toconn.size(),
//				schedControl.getAssumedInputTimeCoordinate(), schedControl.getTimeWhenPoolEnds(),
//				iterationState.getMinimumLevel(), schedControl.getSchedulingRoundLength(),
//				iterationState.getRequiredMaximumInputLevel(), heuristicParam.timeBeforePoolEnd);

		long maxHeightInDeltas = -1;
		for(size_t i = 0; i < toconn.size(); i++)
		{
			long height = toconn[i].getPinDetail(SOURCEPIN).coord[CIRCUITHEIGHT] / DELTA;
			if(maxHeightInDeltas < height)
			{
				maxHeightInDeltas = height;
			}
		}
		maxHeightInDeltas += 3;/*to be sure*/

		for(size_t i = 0; i < toconn.size(); i++)
		{
			/*
			 * The first connection has lowest block priority
			 */
			currentJournalEntry.blockPriority = i;

			if(!toconn[i].isColinear())
			{
				//points are not guaranteed colinear if a box is to be connected
				//to the connections rail
				currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
				toconn[i].getPinDetail(SOURCEPIN).
						addPinBlock(0, CIRCUITDEPTH, heuristicParam.connectionBufferTimeLength - 2/*3*/, WALKBLOCKED_OCCUPY, currentJournalEntry);//dubios
				toconn[i].getPinDetail(DESTPIN).
						addPinBlock(0, CIRCUITHEIGHT, -4, WALKBLOCKED_OCCUPY, currentJournalEntry);//ghid
			}
			else
			{
				currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
				toconn[i].getPinDetail(SOURCEPIN).
						addPinBlock(0, CIRCUITDEPTH, 100 /*HEURISTIC block forever*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ocupat viitor
			}

			currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
			toconn[i].getPinDetail(DESTPIN).
					addPinBlock(0, CIRCUITDEPTH, 100 /*block forever*/, WALKBLOCKED_GUIDE, currentJournalEntry);//ocupat viitor

			currentJournalEntry.blockType = WALKBLOCKED_OCCUPY;
			toconn[i].getPinDetail(DESTPIN).
					addPinBlock(0, CIRCUITHEIGHT, maxHeightInDeltas/*toconn.size() + heuristicParam.connectionBoxHeight*/, WALKBLOCKED_OCCUPY, currentJournalEntry);
		}

		printf("## Finalise %lu Extend %lu\n", finaliseConn.size(), toconn.size());

		pinConnector.blockPins(toconn);

		if(sufficientBoxes)
		{
			pinConnector.blockPins(finaliseConn);
		}

		/*
		 * Extend the pool connections
		 */
		printf("Bring to Pool...\n");
		if(!pinConnector.processPins(toconn, ASTAR))
		{
			/*
			 * A connection was not possible
			 */
			printf("TO POOL: A CONNECTION FAILED\a\n");
			break;
		}

		/*
		 * Connections from pool to the circuit
		 * These are performed only when sufficient boxes were scheduled
		 */
		if(sufficientBoxes)
		{
			/*
			 * Block all connection pool pins with guides
			 */
			currentJournalEntry.operationType = "block_rail_before_finalise";
//			printf("block_rail_before_finalise: ");
			for(size_t connNr = 0; connNr < connManager.getCurrentNumberOfConnections(); connNr++)
			{
				//TODO: use pinpair<pindetails*>
				/*
				 * The connections in preRelease are the ones to be finalised
				 * so do not block them
				*/
				bool willBeAvailable = false;
				for(int boxType = 0; boxType < 2; boxType++)
				{
					bool exists = (connManager.pool.toBeAvailable[boxType].find(connNr) != connManager.pool.toBeAvailable[boxType].end());

					willBeAvailable = willBeAvailable || exists;
				}

				if(!willBeAvailable)
				{
//						printf("b%lu, ", connNr);
						/*aici nu are de-a face cu source si destination, ci cu cei doi pini ai pool-ului*/
						currentJournalEntry.blockType = WALKBLOCKED_GUIDE;
						currentJournalEntry.blockPriority = 999;
						currentJournalEntry.poolConnectionNumber = connNr;

						connManager.connectionPins[connNr].getPinDetail(DESTPIN).addPinBlock(0, CIRCUITDEPTH, 100, WALKBLOCKED_GUIDE, currentJournalEntry);
						connManager.connectionPins[connNr].getPinDetail(SOURCEPIN).addPinBlock(0, CIRCUITDEPTH, 100, WALKBLOCKED_GUIDE, currentJournalEntry);
				}
				else
				{
//					printf("n%lu, ", connNr);
				}
			}
			printf("\n");
			pinConnector.blockPins(connManager.connectionPins);

			/*
			 * Compute
			 */
			printf("Get from pool...\n");
//			Point::useSecondHeuristic = true;
			if(!pinConnector.processPins(finaliseConn, ASTAR))
			{
				/*
				 * A connection was not possible
				 */
				printf("FROM POOL: A CONNECTION FAILED\a\n");
				break;
			}
//			Point::useSecondHeuristic = false;

			/*Unblock all connection pool pins of guides*/
			pinConnector.unblockPins(connManager.connectionPins);
			for(size_t i = 0; i < connManager.connectionPins.size(); i++)
			{
				connManager.connectionPins[i].getPinDetail(0).removeBlocks();
				connManager.connectionPins[i].getPinDetail(1).removeBlocks();
			}
		}

		/*
		 * Check which connections are free
		 * Compute where the pins would be on the time axis using achievedCost
		 */
		connManager.releaseIfNotUsed(/*iterationState.requiredMaximumInputLevel*/
				//connManager.getTimeWhenPoolEnds(iterationState),
				schedControl.getTimeWhenPoolEnds(),
				heuristicParam.connectionHeight,
				pinConnector.pathfinder);

		//previous versions of the software will not work
		//because of toconnectfilewriter
		finaliseConn.clear();
		toconn.clear();

		if(sufficientBoxes)
		{
			/*
			 * Move to next level
			 */
			rec.causal.stepwiseBfs(iterationState);
		}
		else
		{
			/*
			 * Advance the requiredLevel and let a new round of scheduling
			 * be performed in the next iteration
			 * correct?
			 */
//			long newReqLevel = iterationState.getRequiredMaximumInputLevel() + schedControl.getSchedulingRoundLength();
//			iterationState.setRequiredMaximumInputLevel(newReqLevel, "notsufficientboxes");
		}

		printf("------\n");

		if(counter == 2)
		{
//			cp.geom.segs.clear();
//			break;
		}
		if(counter == 1)
		{
//			cp.geom.segs.clear();
			break;
		}

		/*
		 * Reset debug geometries
		 */
		pinConnector.debugBlockOccupyGeometry.reset();
		pinConnector.debugBlockGuideGeometry.reset();
	}

	/**
	 * Generate plumbing pieces
	 */
	//ppgen.generateFromGeometry(geom.simplegeom);
	//printf("%s, %d\n", argv[1], ppgen.pieces.size());

	/**
	 * Write the generated geometry to a file
	 */
	string iofname = writeGeometry(argv);

	/**
	 * DEBUGGING: Write the file for pin connections
	 */
	toconnectfilewriter tcwr;
	string fnameConn = toconnectfilewriter::getToConnectFileName(iofname.c_str());
	FILE* fileConn = fopen(fnameConn.c_str(), "w");
	tcwr.writeToConnectFile(fileConn, toconn);
	fclose(fileConn);

	/**
	 * Write the scheduled boxes
	 */
	writeBoxSchedulerAndClearCoordList(boxworld, iofname.c_str(), 0);

	/**
	 * Write the connections between the geometry and the boxes
	 */
	geomfilewriter geomwr;
	string fname = geomfilewriter::getGeomFileName(fnameConn.c_str());
	FILE* file = fopen(fname.c_str(), "w");
	geomwr.writeGeomFile(file, pinConnector.connectionsGeometry.io, pinConnector.connectionsGeometry.segs, pinConnector.connectionsGeometry.coords);
	fclose(file);

	/**
	 * Write the occupy blocks debug
	 */
	geomfilewriter geomwr2;
	string fname2 = geomfilewriter::getGeomFileName(fnameConn.c_str());
	fname2 += "debug1";
	FILE* file2 = fopen(fname2.c_str(), "w");
	geomwr2.writeGeomFile(file2, pinConnector.debugBlockOccupyGeometry.io, pinConnector.debugBlockOccupyGeometry.segs, pinConnector.debugBlockOccupyGeometry.coords);
	fclose(file2);

	/**
	 * Write the guide blocks debug
	 */
	geomfilewriter geomwr3;
	string fname3 = geomfilewriter::getGeomFileName(fnameConn.c_str());
	fname3 += "debug2";
	FILE* file3 = fopen(fname3.c_str(), "w");
	geomwr3.writeGeomFile(file3, pinConnector.debugBlockGuideGeometry.io, pinConnector.debugBlockGuideGeometry.segs, pinConnector.debugBlockGuideGeometry.coords);
	fclose(file3);

	//writeOtherFormats(argv[1]);

	return 0;
}


/**
 * JIT paper configuration
 */
//	//numara cate A si cate Y
//	int nrA = 0;
//	int nrY = 0;
//	for(int i=0; i<convert.inputs.size(); i++)
//	{
//		if(convert.inputs[i] == 'a')
//			nrA++;
//		if(convert.inputs[i] == 'y')
//			nrY++;
//	}
//	//ASAP
//	model[0].gateTypeCosts['a'].gatecost = 1;
//	model[0].gateTypeCosts['a'].wirecost = 0;
//	model[0].gateTypeCosts['y'].gatecost = 1;
//	model[0].gateTypeCosts['y'].wirecost = 0;
//	model[0].gateTypeCosts['d'].gatecost = 1;//undeva tre sa fie 1, dar nu mai stiu de ce
//	model[0].gateTypeCosts['d'].wirecost = 0;
//
//
//	//ALAPT
//	model[1].gateTypeCosts['a'].gatecost = 7 + 2;//10 + 2;
//	model[1].gateTypeCosts['a'].wirecost = (15 + 2) * 5;//10 + 2;
//	model[1].gateTypeCosts['y'].gatecost = 6 + 2;//5 + 2;
//	model[1].gateTypeCosts['y'].wirecost = (7 + 2) * 5; //5 + 2;
//	model[1].gateTypeCosts['d'].gatecost = 1;//undeva tre sa fie 1, dar nu mai stiu de ce
//	model[1].gateTypeCosts['d'].wirecost = 0;
//
//	//ALAPR
//	model[2].gateTypeCosts['a'].gatecost = (7 + 2) * 5;
//	model[2].gateTypeCosts['a'].wirecost = 15 + 2;
//	model[2].gateTypeCosts['y'].gatecost = (6 + 2) * 5;
//	model[2].gateTypeCosts['y'].wirecost = (7 + 2);
//	model[2].gateTypeCosts['d'].gatecost = 1;//undeva tre sa fie 1, dar nu mai stiu de ce
//	model[2].gateTypeCosts['d'].wirecost = 0;

//	printf("%s, %d, %d", argv[1], nrA, nrY);
//	for(int i=0; i<3; i++)
//	{
//		wirerecycle rec;
//		//circ = rec.recycle(convert, RECYCLEWIRESEQ, model);
//		rec.recycle(convert, RECYCLENONE, model[i]);
//
//		vector<int> order = rec.causal.equalizeConsideringCosts();
//		int mdepth = rec.causal.getMaxLevel() + 1;
//		int height = rec.causal.getRoots().size() + + model[i].gateTypeCosts['a'].wirecost;
//
//		int bb = mdepth * height;
//
//		if(i==0)
//		{
//			computeadditional cp;
//			int additionalA = cp.findParam(nrA, 0.2, 0.001);
//			int additionalY =cp.findParam(nrY, 0.2, 0.001);
//
//			int totalA = additionalA * (15 + 2);
//			int totalY = additionalY * (7 + 2);
//			int total = totalA + totalY;
//
//			bb = total * mdepth;
//
//			height = total;
//		}
//
//		printf(", %d, %d, %d", mdepth, height, bb);
//	}
//	printf("\n");
	//circuitmatrix
