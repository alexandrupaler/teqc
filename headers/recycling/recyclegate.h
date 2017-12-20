#ifndef RECYCLEGATE_H__
#define RECYCLEGATE_H__

#include <stdio.h>
#include <vector>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include <algorithm>
#include "costmodel.h"

#include "recycling/wireelement.h"


#define RECNOTYPE -1
#define RECINIT 0
#define RECMEAS 1
#define RECINITCONN 2
#define RECMEASCONN 3
#define RECGATE 4
#define RECINPUT 5
#define RECOUTPUT 6

class recyclegate
{
public:
//	map<WIRE ID REFERRING TO THIS GATE, OP ID OF OTHER GATE>
//	std::map<int, int> willPush;
//	std::map<int, int> pushedBy;

	std::map<int, recyclegate*> willPush;
	std::map<int, recyclegate*> pushedBy;

	long orderNrInGateList;
	int causalType;
	long level;

	std::vector<wireelement*> wiresToUseForLinks;

	//22.03.2017
	std::vector<wireelement*> wirePointers;
//	std::vector<int> wires;
//	std::vector<int> afterSplit;
	bool isVisited;

	/*from circgate.h*/
	char type;

	int gateCost;
	int additionalWireCost;

//	void replaceWires(std::map<int, int>& dict);
	void updateTypeAndCost(bool isInitialisation, char ctype, costmodel& model);
	/*end*/

	recyclegate();
//	void initFrom(recyclegate& g);
	recyclegate(std::string& s, std::vector<wireelement*>& wirePointersVector);

	std::string generateLabel();
	void print();

	void addWillPush(int wire, recyclegate* gatePtr);
    void addPushedBy(int wire, recyclegate* gatePtr);

//	void getMinMax(int& min, int& max);

	void updateCausalType();

	bool isAncillaInitialisation();
	bool isInput();
	bool isFirstLevelInitialisation();
	bool isAncillaMeasurement();
	bool isOutput();
	bool isLastLevelMeasurement();
	bool isConnection();
	bool isGate();
	bool isInitialisation();
	bool isMeasurement();

	void computeWiresWithCosts();
	void computeOnlyWires();

	void updateGateCostToAchieveLevel(long level);

	unsigned long getId();

private:
	void init();

	/*const*/ unsigned long id;
};

#endif /* RECYCLEGATE_H_ */
