#ifndef RECYCLEGATE_H__
#define RECYCLEGATE_H__

#include <stdio.h>
#include <vector>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include <algorithm>
//#include "circgate.h"
#include "costmodel.h"


#define RECNOTYPE -1
#define RECINIT 0
#define RECMEAS 1
#define RECINITCONN 2
#define RECMEASCONN 3
#define RECGATE 4
#define RECINPUT 5
#define RECOUTPUT 6

using namespace std;

//class recyclegate : public circgate{
class recyclegate
{
public:
	set<int> willPush;
    set<int> pushedBy;
	int id;
	int causalType;
	long level;
	string label;

	vector<int> wiresToUseForLinks;

	/*from circgate.h*/
	char type;
	vector<int> wires;
	int gateCost;
	int additionalWireCost;

	void replaceWires(map<int, int>& dict);
	void updateTypeAndCost(bool isInitialisation, char ctype, costmodel& model);
	/*end*/

	recyclegate();
	void initFrom(recyclegate& g);
	recyclegate(string& s);

	void generateLabel();
	void print();

	void addWillPush(int nr);

    void addPushedBy(int nr);

	void getMinMax(int& min, int& max);

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
};

#endif /* RECYCLEGATE_H_ */
