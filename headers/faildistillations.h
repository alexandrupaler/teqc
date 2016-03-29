#ifndef FAILDISTILLATIONS_H_
#define FAILDISTILLATIONS_H_

#include "numberandcoordinate.h"
#include <vector>
#include <utility>

typedef inputpin pinpair;

using namespace std;

#define X 1
#define Y 2
#define Z 3

#define NOBOXESAVAIL -1


class faildistillations
{
public:
	faildistillations();

	faildistillations(char* circIoFile, char* allPinsFile);

	void writeFailedDistillations();

	void writeCircuitToDistillationConnections(char* fname);

	void selectDistillationForCircuitInput(double afail, double yfail);

	double randomCheckThreshold(double& prob);

//	void readAllPinsCoordinates(char* filename);

	int decideAndStore(double failprob, int IOIndex, int& lastpos);

	int storeConnectEntry(int IOIndex, int posdist);

	int decide(double prob, int origposition, int lastPosition);

	vector<pair<int, int> > computePinIndexPairs(int posdist);

//	int transformIOIndexToDistIndex(int ioindex);

//	int getDistPinType(int distindex);

public:
	//vector<vector<int> > distpins;
	vector<int> failedDistillations;
	numberandcoordinate circIOs;
	numberandcoordinate boxIOs;
	vector<pinpair> nonFailedPinPairCoords;
};


#endif /* FAILDISTILLATIONS_H_ */
