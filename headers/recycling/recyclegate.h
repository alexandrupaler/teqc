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
	std::vector<recyclegate*> willPush;
	std::vector<recyclegate*> pushedBy;

	long orderNrInGateList;
	int causalType;
	long level;
	long connChannel;

	std::vector<wireelement*> wiresToUseForLinks;

	//22.03.2017
	std::vector<wireelement*> wirePointers;
	bool isVisited;

	/*from circgate.h*/
	char type;

	int gateCost;
	int additionalWireCost;

	void updateType(bool isInitialisation, char ctype);
	void updateCost(bool isInitialisation, costmodel& model);

	/*end*/

	recyclegate();
	~recyclegate();
//	void initFrom(recyclegate& g);
	recyclegate(std::string& s, std::vector<wireelement*>& wirePointersVector);

	/*
	 * The methods are historic. Used for debugging gml output
	 */
	std::string generateLabel();
	void print();

	std::string toString();
	static void fromString(std::string& line, char& type, std::vector<int>& indices);

	void addWillPush(recyclegate* gatePtr);
    void addPushedBy(recyclegate* gatePtr);

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
