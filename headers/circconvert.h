#ifndef CIRCCONVERT_H__
#define CIRCCONVERT_H__

#include <string>
#include <vector>
#include <list>

#include "recycling/recyclegate.h"

using namespace std;

class circconvert{
public:
	circconvert(char* fname);
	circconvert(vector<string> file);

	static vector<string> readFile(char* fname);
	void makeGates(vector<string> file);

//	list<circgate> gates;

	list<recyclegate> gates;

	string inputs;
	string outputs;
	void updateWiresStartingFromGate(list<recyclegate>::iterator after, int minWireNumber, int incrValue);

	void includeWires(int howMany, int where);

	void replaceNonICM();
	void replaceICM();

	void configInputs(int qubit, string ins);
	void configOutputs(int qubit, string outs);

	void makeAncillaInput(int qubit);
	void makeAncillaOutput(int qubit);
	void makeComputeInput(int qubit);
	void makeComputeOutput(int qubit);

	void print();
};

#endif
