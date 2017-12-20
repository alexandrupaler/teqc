#include <stdio.h>
#include <vector>

#include "gatenumbers.h"
#include "cnotcounter.h"
#include "databasereader.h"

#include "processraw.h"
#include "fileformats/rawfilereader.h"
#include "fileformats/infilewriter.h"

processraw::processraw(rawdata& rw)
{
	//update nrGates to reflect the decomposition of the Toffoli
	int localNrGates = rw.nrGates;
	int toffoGateLen = dbReader.decomp["toffoli"].gates[0].size();
	localNrGates = localNrGates + (toffoGateLen - 1)*rw.nrToffoli;

	circuit.reserve(rw.nrQubits);
	for(int i=0; i<rw.nrQubits; i++)
	{
		qubitline qubit(localNrGates + 2, WIRE);//a circuit consisting of a single line
		qubit.at(0) = INPUT; //this one is circuit input
		qubit.at(localNrGates + 1) = OUTPUT;
		circuit.push_back(qubit);
	}

	//start from the INPUTS; first circuit column is reserved for input values
	int currNrGate = 0;
	for(std::vector<fileline>::iterator it=rw.fileLines.begin(); it != rw.fileLines.end(); it++)
	{
		//first available column in the circuit is 1
		currNrGate++;

		char gate = (char) it->at(0);
		long qubit = it->at(1);

		std::vector<int> opnums = getOpNumber(gate);

		if(gate == CH_TOFFOLI)
		{
			//include Toffoli decomposition from database
			int added = placeNonICMDecomposition("toffoli", currNrGate, *it);
			currNrGate += added;
		}
		else if(gate == CH_CNOT)
		{
			placeCnot(currNrGate, opnums, *it);
		}
		else//other gates
		{
			circuit[qubit][currNrGate] = opnums[0];
		}
	}
}

std::vector<int> processraw::getOpNumber(char gate)
{
	int opnumber = WIRE;
	switch(gate)
	{
	case CH_HADAMARD:
		opnumber = HGATE;
		break;
	case CH_PHASE:
		opnumber = PGATE;
		break;
	case CH_RGATE:
		opnumber = RGATE;
		break;
	case CH_TGATE:
		opnumber = TGATE;
		break;
	case CH_CNOT:
		opnumber = cncounter.getNextCnotNumber();
		break;
	case CH_TOFFOLI:
		//the gate is for the moment hardcoded, and nothing to do in this case
		break;
	}

	std::vector<int> ret;
	ret.push_back(opnumber);

	if(gate == CH_CNOT)
		ret.push_back(opnumber + 1);

	return ret;
}

void processraw::placeCnot(int currNrGate, std::vector<int>& opnums, fileline& rawFileLine)
{
	//place the control
	circuit.at(rawFileLine[1]).at(currNrGate) = opnums[0];

	//place the targets
	std::vector<long> targets(rawFileLine.begin() + 2, rawFileLine.end());
	for(int i=0; i<targets.size(); i++)
	{
		circuit.at(targets.at(i)).at(currNrGate) = opnums[1];
	}
}

int processraw::placeNonICMDecomposition(const char* name, int currNrGate, fileline& rawFileLine)
{
	decomposition el = dbReader.decomp[name];
	int toffHeight = el.gates.size();
	int toffLength = el.gates.at(0).size();
	int toffCnots = el.getNumberOfCnots();

	std::vector<int> defaultOps(2, -1);
	std::vector<std::vector<int> > cnots(toffCnots, defaultOps);
	for(int i=0; i< toffCnots; i++)
	{
		cnots[i] = getOpNumber(CH_CNOT);
	}

	int pos[] = {rawFileLine[1]/*first control*/, rawFileLine[2]/*second control*/, rawFileLine[3] /*target*/};

	int currcnot = -1;
	for(int j=0; j<toffLength; j++)
	{
		int lastcnot = currcnot;
		for(int i=0; i<toffHeight; i++)
		{
			int opValue = el.gates[i][j];

			if((opValue == CTRL || opValue == TGT) && lastcnot == currcnot)
				currcnot++;

			if(opValue == CTRL)
				opValue = cnots.at(currcnot).at(0);
			if(opValue == TGT)
				opValue = cnots.at(currcnot).at(1);

			circuit.at(pos[i]).at(currNrGate + j) = opValue;
		}
	}

	return toffLength - 1;
}
