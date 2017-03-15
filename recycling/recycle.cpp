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

#include "recycling/recycle.h"
#include "recycling/costmodel.h"
#include "recycling/recyclegate.h"
#include "recycling/causalgraph.h"

#include "cnotcounter.h"
#include "circuitmatrix.h"

#include "recycling/trim.h"
#include "cuccaro.h"
#include "circconvert.h"

using namespace std;

void wirerecycle::writeGMLFile(const char* fname)
{
	FILE* fg = fopen(fname, "w");
	fprintf(fg, "graph [\n");
	fprintf(fg, "directed 1\n");

	for(vector<recyclegate>::iterator it=causal.circuit.begin(); it!=causal.circuit.end(); it++)
	{
		fprintf(fg, "node [\n");
		fprintf(fg, " id %d\n label \"%s\" \n", it->id, it->label.c_str());
		fprintf(fg, "]\n");
	}

	for(vector<recyclegate>::iterator it=causal.circuit.begin(); it!=causal.circuit.end(); it++)
	{
		for(set<int>::iterator it2 = it->willPush.begin(); it2 != it->willPush.end(); it2++)
		{
			fprintf(fg, "edge [\n");
			fprintf(fg, " source %d\n target %d \n", it->id, *it2);
			fprintf(fg, "]\n");
		}
	}

	fprintf(fg, "]");
	fclose(fg);
}

void wirerecycle::initialiseSimQueue()
{
	allReachableAncillaOutputs.clear();
	perInputReachedOutputs.clear();

	allReachableAncillaOutputs.insert(causal.outAncillae.begin(), causal.outAncillae.end());

	for(int i=0; i<causal.getNrQubits(); i++)
	{
		if(causal.inAncillae.find(i) == causal.inAncillae.end())
			continue;

		set<int> vis;
		set<int> out;
		causal.reachOutputs(vis, out, i);

		perInputReachedOutputs[i] = out;
	}
}

void wirerecycle::updateSimQueue(int outputId, set<int>& receivedOutputs)
{
	//this is not an output any more
	allReachableAncillaOutputs.erase(outputId);

	for(map<int, set<int> >::iterator it = perInputReachedOutputs.begin(); it!= perInputReachedOutputs.end(); it++)
	{
		//if from this input the specified output was found
		if(it->second.find(outputId) != it->second.end())
		{
			//this is not an output any more
			it->second.erase(outputId);
			it->second.insert(receivedOutputs.begin(), receivedOutputs.end());
		}
	}
}

vector<vector<int> > wirerecycle::sortInputsForICM()
{
	vector<vector<int> > ret;
	for(int i=0; i<causal.getNrQubits(); i++)
	{
		if(causal.inAncillae.find(i) == causal.inAncillae.end())
			continue;

		if(perInputReachedOutputs.find(i) == perInputReachedOutputs.end())
			continue;

		vector<int> el(3);
		el[0] = i;
		el[1] = i;
		el[2] = i;
		ret.push_back(el);
	}

	return ret;
}

vector<vector<int> > wirerecycle::sortInputs()
{
//	//use priority queue?
	vector<vector<int> > ret;

	for(map<int, set<int> >::iterator it = perInputReachedOutputs.begin(); it!= perInputReachedOutputs.end(); it++)
	{
		vector<int> el(3);
		el[0] = it->first;
		el[1] = causal.circuit[it->first].level;
		el[2] = allReachableAncillaOutputs.size() - it->second.size();

		ret.push_back(el);
	}

	if(ret.size() == 0)
		return ret;

	//bubble sort - as putea inlocui cu o cautare de maximum doar?
	for(size_t i=0; i < ret.size() - 1; i++)
	{
		for(size_t j = i+1; j < ret.size(); j++)
		{
			if((ret[i][1] < ret[j][1]) || (ret[i][1] == ret[j][1] && causal.circuit[ret[i][0]].wires[0] < causal.circuit[ret[j][0]].wires[0]))
			{
				vector<int> x = ret[i];
				ret[i] = ret[j];
				ret[j] = x;
			}
		}
	}

	return ret;
}

int wirerecycle::chooseClosestAncillaOutputWire(int inputId, bool preferLowNumberedWires)
{
	int ret = -1;

	int inputWire = causal.circuit[inputId].wires[0];
	int dif = INT_MAX;

	for(set<int>::iterator it = allReachableAncillaOutputs.begin(); it != allReachableAncillaOutputs.end(); it++)
	{
		if(perInputReachedOutputs[inputId].find(*it) == perInputReachedOutputs[inputId].end())
		{
			//not in reachable
			int wire = causal.circuit[*it].wires[0];
			int ndif = abs(inputWire - wire);

			//prefer only wires which are higher than the inputwire
			if(preferLowNumberedWires && wire > inputWire)
				continue;

			if(ndif < dif)
			{
				dif = ndif;
				ret = *it;
			}
		}
	}

	return ret;
}

void wirerecycle::outputGraph(int& nrConnect)
{
	causal.computeLevels();
	causal.updateLabels();
	char fname[1024];
	sprintf(fname, "circuit%03d.raw.gml", nrConnect);
	writeGMLFile(fname);
}

int wirerecycle::recycleUsingWireSequence()
{
	initialiseSimQueue();

	vector<vector<int> > sortedin = sortInputsForICM();

	int nrConnect = 0;
	while(sortedin.size() > 0)
	{
		int inputId = sortedin[0][0];

		if(sortedin[0][2] == 0)
		{
			perInputReachedOutputs.erase(inputId);
			sortedin = sortInputsForICM();
			continue;
		}

		int outputId = chooseClosestAncillaOutputWire(inputId, PREFERLOWNUMBEREDWIRES);
		if(outputId != -1)
		{
			nrConnect++;
			updateSimQueue(outputId, perInputReachedOutputs[inputId]);

			causal.connectNodes(outputId, inputId);

			causal.circuit[outputId].causalType = RECMEASCONN;//temp value, todo
			causal.circuit[inputId].causalType = RECINITCONN;

			set<int> vis;
			causal.replaceQubitIndex(vis, outputId, causal.circuit[inputId].wires[0], causal.circuit[outputId].wires[0]);
		}

		perInputReachedOutputs.erase(inputId);

		sortedin = sortInputsForICM();
	}

	return nrConnect;
}

bool mycompare (vector<int> vi, vector<int>vj) { return vi[1] < vj[1]; }

vector<vector<int> > wirerecycle::extractSortedInputsList()
{
	vector < vector<int> > remainingIns;
	for (int i = 0; i < causal.nrQubits; i++)
	{
		//if (causal.circuit[i].type == -1 || causal.circuit[i].type == INPUT)
		//if(gatenumbers::getInstance()->isInitialisationNumberButNotConnection(causal.circuit[i].type))
		if(causal.circuit[i].isFirstLevelInitialisation())
		{
			vector<int> el(2);
			el[0] = i;//id
			el[1] = causal.circuit[i].wires[0];//wire
			remainingIns.push_back(el);
		}
	}

	std::sort (remainingIns.begin(), remainingIns.end(), mycompare);

	return remainingIns;
}

void wirerecycle::renameWires(vector<vector<int> >& remainingIns)
{
	//receives a sorted list, so renaming should not be problematic
	//rename wires
	int newNumber = -1;
	for(vector<vector<int> >::iterator k = remainingIns.begin(); k!=remainingIns.end(); k++)
	{
		set<int> v;//empty set of visited nodes
		newNumber++;
		causal.replaceQubitIndex(v, k->at(0), k->at(1)/*old value*/, newNumber/*new value*/);
		//printf("repl %d %d\n", k->at(1), newNumber);
	}
}

void wirerecycle::recycle(circconvert& convert, int recycleMethod, costmodel& model)
{
	causal.constructFrom(convert, model);

	int nrConnect = 0;
	if(recycleMethod == RECYCLEWIRESEQ)
	{
		nrConnect = recycleUsingWireSequence();
	}
	else if(recycleMethod == RECYCLENONE)
	{
		nrConnect = 0;
	}

	if(recycleMethod != RECYCLENONE)
	{
		vector<vector<int> > remainingIns = extractSortedInputsList();
		renameWires (remainingIns);
	}
}
