#include <sys/time.h>
#include <time.h>

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
#include "fileformats/gmlfilewriter.h"

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

databasereader dbReader;
cnotcounter cncounter;
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

	/**
	 * The bounding box of a circuit. DEBUGGING PURPOSES ONLY
	 */
	boundingboxfilewriter bboxwr;
	std::string bboxname = boundingboxfilewriter::getBoundingBoxFileName(fname.c_str());
	FILE* bboxfile = fopen(bboxname.c_str(), "w");
	bboxwr.writeBoundingBoxFile(bboxfile, geom.simplegeom.boundingbox);
	fclose(bboxfile);

	return iofname;
}

void placeGate(recyclegate* current, circuitmatrix& circ, cnotcounter& ccnot)
{
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

circuitmatrix createCircuitMatrix(causalgraph& causal)
{
//	std::vector<recyclegate*> order = causal.equalizeConsideringCosts();

	std::vector<recyclegate*> order = causal.bfs();

	//TODO
	//causal.equalizeInputLevels();

	//construct a vector of qubitlines
	long length = causal.getMaxLevel() + 1;

	//vector<vector<int> > inputs = extractSortedInputsList();
	std::vector<recyclegate*> inputs = causal.getRoots();
	int nrLines = inputs.size();

	std::vector<qubitline> temp;
	for (int k = 0; k < nrLines; k++)
	{
		qubitline wireQubit(length, WIRE);
		temp.push_back(wireQubit);
	}

	//construct the circuitmatrix from the causalgraph
	//for the ICM representation it should contain only cnot gates
	cnotcounter ccounter;

	//vector<int> operations = causal.bfs();

	circuitmatrix circ(temp);

	for(std::vector<recyclegate*>::iterator it = order.begin(); it != order.end(); it++)
	{
		placeGate(*it, circ, ccounter);
	}

	return circ;
}

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

double get_cpu_time()
{
    return (double)clock() / CLOCKS_PER_SEC;
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
	costmodel model[2];
	initCostModels(model);

	cuccaro cuc;

//	circconvert convert(argv[1]);
	for(int nrCucQb = 100; nrCucQb <= 1000; nrCucQb += 100)
	{

		std::vector<std::string> cucCirc = cuc.makeCircuit(nrCucQb, false);

		causalgraph causalG(model[0]);

		infilereader inread;
	//	std::vector<std::string> simple = inread.readInFile(argv[1]);
	//	causalG.constructFrom2(simple);
		causalG.constructFrom2(cucCirc);

		int nrGates0 = causalG.tmpCircuit.size();
		wireelement* current = causalG.getFirstWireElement();
		int nrWire = 0;
		while(current != NULL)
		{
			nrWire++;
			current = current->next;
		}
		int nrWires0 = nrWire;


		double cpu0 = get_cpu_time();

		circconvert convert;
		printf("1\n");
		convert.replaceNonICM2(causalG);
		printf("2\n");
		convert.replaceNonICM2(causalG);
		printf("3\n");

		/*required for threshold*/
		causalG.numberGateList2();

		convert.replaceICM2(causalG);
		printf("4\n");

		double cpu1 = get_cpu_time();

		for(std::list<recyclegate*>::iterator it = causalG.tmpCircuit.begin(); it != causalG.tmpCircuit.end(); it++)
		{
			for(size_t i=0; i<(*it)->wirePointers.size(); i++)
			{
				(*it)->wirePointers[i] = (*it)->wirePointers[i]->getUpdatedWire((*it)->orderNrInGateList);
			}
		}

		int nrWires1 = causalG.numberWires2();
		std::vector<recyclegate*>order = causalG.numberGateList2();
		int nrGates1 = order.size();
		printf("5 order:%d\n", order.size());

		causalG.reconstructWithoutConsideringCosts(order);
		causalG.computeLevels();

		/**
		 * Perform recycling
		 */
		wirerecycle recycler;
		recycler.recycle(causalG, RECYCLEWIRESEQ);

		for(std::list<recyclegate*>::iterator it = causalG.tmpCircuit.begin(); it != causalG.tmpCircuit.end(); it++)
		{
			for(size_t i=0; i<(*it)->wirePointers.size(); i++)
			{
				(*it)->wirePointers[i] = (*it)->wirePointers[i]->getUpdatedWire(0);
			}
		}

		double cpu2 = get_cpu_time();

		printf("%d, %d, %d, %d, %d, %f,%f\n", nrCucQb, nrWires0, nrGates0, nrWires1, nrGates1, cpu1-cpu0, cpu2-cpu1);

		causalG.numberWires2();
		causalG.numberGateList2();

		causalG.computeLevels();
		//
		//	circuitmatrix circ = createCircuitMatrix(causalG);
		//	circ.printCirc();
	}

	printf("CONVERT END\n");

	return 0;
}

