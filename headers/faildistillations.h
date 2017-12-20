#ifndef FAILDISTILLATIONS_H_
#define FAILDISTILLATIONS_H_

#include "numberandcoordinate.h"
#include <vector>
#include <utility>

#define X 1
#define Y 2
#define Z 3

#define NOBOXESAVAIL -1


class faildistillations
{
public:
	faildistillations();

	void writeCircuitToDistillationConnections(char* fname);

	void selectDistillationForCircuitInput (const char* circIoFile,
			const char* allPinsFile, double afail, double yfail, std::vector<pinpair>& toconn);

	void selectDistillationForCircuitInput(numberandcoordinate& circIOs,
			numberandcoordinate& boxIOs, double afail, double yfail, std::vector<pinpair>& toconn);

	bool randomCheckThreshold(double& prob);

	int decideAndStore(double failprob, numberandcoordinate& circIOs,
			int IOIndex, numberandcoordinate& boxIOs, int& lastpos, std::vector<pinpair>& toconn);

	int decide(double prob, numberandcoordinate& boxIOs, int origposition, int lastPosition);

	std::vector<std::pair<int, int> > computePinIndexPairs(numberandcoordinate& boxIOs,
			int boxIndex);

	int storeConnectEntry(numberandcoordinate& circIOs, int IOIndex,
			numberandcoordinate& boxIOs, int boxIndex, std::vector<pinpair>& toconn);

public:
//	vector<int> failedDistillations;
//	numberandcoordinate circIOs;
//	numberandcoordinate boxIOs;
//	vector<pinpair> nonFailedPinPairCoords;
};


#endif /* FAILDISTILLATIONS_H_ */
