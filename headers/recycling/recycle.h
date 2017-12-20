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

class wirerecycle
{
protected:
	std::map<size_t, recyclegate*> allReachableAncillaOutputs;

	std::map<recyclegate*, std::set<recyclegate*> > perInputReachedOutputs;

	void initialiseSimQueue(causalgraph& causal);

	void updateSimQueue(causalgraph& causal, recyclegate* outputId, recyclegate* inputId /*std::set<recyclegate*>& receivedOutputs*/);

//	std::vector<std::vector<int> > sortInputsForICM(causalgraph& causal);

	recyclegate* chooseClosestAncillaOutputWire(recyclegate* inputId, bool preferLowNumberedWires);

	int recycleUsingWireSequence(causalgraph& causal);

//	std::vector<std::vector<int> > extractSortedInputsList(causalgraph& causal);

//	void renameWires(causalgraph& causal, std::vector<std::vector<int> >& remainingIns);

public:
	void recycle(causalgraph& causal, int recycleMethod);
};


#endif /* RECYCLE_H_ */
