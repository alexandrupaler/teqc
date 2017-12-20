#ifndef CIRCCONVERT_H__
#define CIRCCONVERT_H__

#include <string>
#include <vector>
#include <list>

#include "recycling/recyclegate.h"
#include "recycling/causalgraph.h"

class circconvert{
public:
//	circconvert(char* fname);
//	circconvert(std::vector<std::string>& file);

	circconvert();

//	void makeGates(std::vector<std::string> file);

//	std::list<recyclegate> gates;
//
//	std::list<char> inputs;
//	std::list<char> outputs;

//	void updateWiresStartingFromGate(std::list<recyclegate>::iterator after, int minWireNumber, int incrValue);

//	void includeWires(int howMany, int where);
//	void includeWires2(int howMany, int where);

//	void replaceNonICM();
	void replaceNonICM2(causalgraph& causal);

//	void replaceICM();
	void replaceICM2(causalgraph& causal);


//	void configInputs(int qubit, std::string ins);
//	void configOutputs(int qubit, std::string outs);
//
//	void makeAncillaInput(int qubit);
//	void makeAncillaOutput(int qubit);
//	void makeComputeInput(int qubit);
//	void makeComputeOutput(int qubit);

//	void print();
};

#endif
