#ifndef GATENUMBERS_H_
#define GATENUMBERS_H_

#define AA -1
#define YY -2
#define ZERO -3
#define PLUS -4
#define WIRE 0
#define CTRL 1
#define TGT 2
#define PGATE 3
#define HGATE 4
#define TGATE 5
#define RGATE 10 //; - The x rotation pi/2
#define MZ 6
#define MX 7
#define MXZ 8
#define MZX 9
#define MA 13
#define MYZ 14
#define MY 15
#define INPUT -100 //nothing?, mai mult input
#define OUTPUT -101
#define EMPTY -99 //no wire, no input, nothing...
//#define CONNIN -97
//#define CONNOUT -98

#include <set>

using namespace std;

class gatenumbers{

	/**
	 * The set of values that are interpreted as qubit measurements.
	 */
	set<int> measurements;

	/**
	 * The set of a values that are interpreted as qubit initialisations.
	 */
	set<int> initialisations;

private:
	gatenumbers();

public:
	static gatenumbers& getInstance();
	bool isInitialisationNumber(int val);
	bool isMeasurementNumber(int val);
	bool isInitialisationNumberButNotConnection(int val);
	bool isMeasurementNumberButNotConnection(int val);
	bool isAncillaOutput(int val);
	bool isAncillaInput(int val);
	bool isConnectionElement(int val);
};

#endif
