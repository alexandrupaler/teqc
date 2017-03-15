#ifndef RECYCLE_H_
#define RECYCLE_H_


#include <stdio.h>
#include <vector>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <algorithm>
#include <climits>

#include "cnotcounter.h"
#include "gatenumbers.h"
#include "circuitmatrix.h"

#include "trim.h"
#include "recyclegate.h"

#include "cuccaro.h"
#include "circconvert.h"
#include "causalgraph.h"

#include "costmodel.h"

#define PREFERLOWNUMBEREDWIRES true
#define PREFERANYWIRE false

#define RECYCLEWIRESEQ 0
#define RECYCLESEARCH 1
#define RECYCLENONE 2

using namespace std;

class wirerecycle{

public:
	causalgraph causal;

	set<int> allReachableAncillaOutputs;

	map<int, set<int> > perInputReachedOutputs;

	void writeGMLFile(const char* fname);

	void initialiseSimQueue();

	void updateSimQueue(int outputId, set<int>& receivedOutputs);

	vector<vector<int> > sortInputsForICM();

	vector<vector<int> > sortInputs();

	int chooseClosestAncillaOutputWire(int inputId, bool preferLowNumberedWires);

	void outputGraph(int& nrConnect);

	int recycleUsingWireSequence();

	vector<vector<int> > extractSortedInputsList();

	void renameWires(vector<vector<int> >& remainingIns);

	void recycle(circconvert& convert, int recycleMethod, costmodel& model);
};


#endif /* RECYCLE_H_ */
