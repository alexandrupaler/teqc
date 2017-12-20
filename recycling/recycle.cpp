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
#include "recycling/recyclegate.h"
#include "recycling/causalgraph.h"

#include "cnotcounter.h"
#include "circuitmatrix.h"

#include "recycling/trim.h"
#include "cuccaro.h"
#include "circconvert.h"

#include "fileformats/gmlfilewriter.h"

void wirerecycle::initialiseSimQueue(causalgraph& causal)
{
	allReachableAncillaOutputs.clear();
	perInputReachedOutputs.clear();

	int nrAncIn = 0;
	int nrIn = 0;
	int nrOut = 0;
	int nrOut2 = 0;
	for(std::list<recyclegate*>::reverse_iterator it = causal.tmpCircuit.rbegin();
				it != causal.tmpCircuit.rend(); it++)
	{
		for(size_t i=0; i<(*it)->wirePointers.size() - 1; i++)
		{
			for(size_t j=i+1; i<(*it)->wirePointers.size(); i++)
			{
				wireelement* wire1 = (*it)->wirePointers[i];
				wireelement* wire2 = (*it)->wirePointers[j];

				wire1->bits = wire1->bits | wire2->bits;
				wire2->bits = wire1->bits;
			}
		}

		if((*it)->isAncillaMeasurement())
		{
			allReachableAncillaOutputs[(*it)->wirePointers[0]->number] = *it;
		}
		if((*it)->isAncillaInitialisation())
		{
			nrAncIn++;
		}
		if((*it)->isInput())
		{
			nrIn++;
		}
		if((*it)->isOutput())
		{
			nrOut++;
		}
		if((*it)->willPush.size() == 0 && !(*it)->isOutput() && !(*it)->isAncillaMeasurement())
		{
			nrOut2++;
			printf("HERE %d", (*it)->type);
		}
	}

	printf("ancmeas %d %d ancin %d nrin %d nrout %d\n", allReachableAncillaOutputs.size(), nrOut2, nrAncIn, nrIn, nrOut);

//	//test printing
//	wireelement* firstWire = causal.getFirstWireElement();
//	while(firstWire != NULL)
//	{
//		printf("%d %s\n", firstWire->number, firstWire->bits.to_string().c_str());
//		firstWire = firstWire->next;
//	}

//	std::vector<recyclegate*> order = causal.bfs();
//
//	int nrConnect = 0;
////	for(std::list<recyclegate*>::reverse_iterator it = causal.tmpCircuit.rbegin();
////				it != causal.tmpCircuit.rend(); it++)
////	{
//	for(std::vector<recyclegate*>::reverse_iterator it = order.rbegin();
//					it != order.rend(); it++)
//	{
//		printf("%d\n", (*it)->orderNrInGateList);
//
//		//functioneaza doar pentru doi. cnot.
//		for(size_t i=1; i<(*it)->wirePointers.size(); i++)
//		{
//			printf("    %lu\n", i);
//			wireorder* a = (*it)->wirePointers[i - 1]->start;
//			wireorder* b = (*it)->wirePointers[i - 1]->stop;
//
//			wireorder* c = (*it)->wirePointers[i]->start;
//			wireorder* d = (*it)->wirePointers[i]->stop;
//
////			bool b1 = (*a) < (*b);
////			bool b2 = (*c) < (*d);
////
//////			gmlfilewriter gmwr;
//////			char name[1024];
//////			sprintf(name, "order_%03d_%d_%d_%p_%p_%p_%p", nrConnect, b1, b2, a, b, c, d);
//////
//////			std::string fname = gmlfilewriter::getGMLFileName(name, nrConnect);
//////			FILE* f = fopen(fname.c_str(), "w");
//////			gmwr.writeGMLFileOrder(f, causal);
//////			fclose(f);
////
////			printf("%d, %d, %d\n", nrConnect, (*it)->wirePointers[0]->number, (*it)->wirePointers[1]->number);
//////			(*it)->print();
//
//			std::vector<wireorder*> ni = a->joinWithInterval(b, c, d);
//
//			(*it)->wirePointers[i - 1]->start = ni[0];
//			(*it)->wirePointers[i - 1]->stop = ni[1];
//
//			(*it)->wirePointers[i]->start = ni[0];
//			(*it)->wirePointers[i]->stop = ni[1];
//
//			nrConnect++;
//		}
//	}
//
//	allReachableAncillaOutputs.clear();
//	perInputReachedOutputs.clear();
//
//	std::map<wireelement*, std::set<recyclegate*> > perWireReachedOutputs;
//
//	long nr = 0;
//	for(std::list<recyclegate*>::reverse_iterator it = causal.tmpCircuit.rbegin();
//				it != causal.tmpCircuit.rend(); it++)
//	{
//		printf("%ld %p\n", nr++, *it);
//
//		if((*it)->isAncillaMeasurement())
//		{
//			perWireReachedOutputs[(*it)->wirePointers[0]].insert(*it);
//		}
//		else
//		{
//			std::set<recyclegate*> outputsUnion;
//
//			for(std::vector<wireelement*>::iterator itw = (*it)->wirePointers.begin();
//					itw != (*it)->wirePointers.end(); itw++)
//			{
//				outputsUnion.insert(perWireReachedOutputs[*itw].begin(), perWireReachedOutputs[*itw].end());
//			}
//
//			for(std::vector<wireelement*>::iterator itw = (*it)->wirePointers.begin();
//								itw != (*it)->wirePointers.end(); itw++)
//			{
//				perWireReachedOutputs[*itw].insert(outputsUnion.begin(), outputsUnion.end());
//			}
//		}
//	}
//
//	long nr = 0;
//	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
//				it != causal.tmpCircuit.end(); it++)
//	{
//		if((*it)->isAncillaMeasurement())
//		{
//			allReachableAncillaOutputs.insert(*it);
//		}
//		if((*it)->isAncillaInitialisation())
//		{
//			perInputReachedOutputs[*it];// =  perWireReachedOutputs[(*it)->wirePointers[0]];
//
//			std::list<recyclegate*> nextToLookAt;
//			nextToLookAt.push_back(*it);
//
//			while(nextToLookAt.size() > 0)
//			{
//				recyclegate* current = nextToLookAt.front();
//				while(current->isVisited)
//				{
//					nextToLookAt.pop_front();
//
//					current = nextToLookAt.front();
//				}
//				nextToLookAt.pop_front();
//
//				current->isVisited = true;
//
//				if(current->isAncillaMeasurement())
//				{
//					perInputReachedOutputs[*it].insert(current);
//				}
//
//				for(std::map<int, recyclegate*>::iterator wpit = current->willPush.begin();
//						wpit != current->willPush.end(); wpit++)
//				{
//					nextToLookAt.push_back(wpit->second);
//				}
//
////				printf("%ld %ld\n", nr++, nextToLookAt.size());
//			}
//		}
//	}
//
////	for(std::map<recyclegate*, std::set<recyclegate*> >::iterator it = perInputReachedOutputs.begin();
////			it != perInputReachedOutputs.end(); it++)
////	{
////		//causal.reachOutputs(perInputReachedOutputs[it->first], it->first);
////
//////		perInputReachedOutputs[*it] = out;
////	}
}

void wirerecycle::updateSimQueue(causalgraph& causal, recyclegate* outputId, recyclegate* inputId/*, std::set<recyclegate*>& receivedOutputs*/)
{
//	//this is not an output any more
//	allReachableAncillaOutputs.erase(outputId);

	//this ancilla is not reachable anymore
	//do not take the new pointer to the wire
	size_t outputWireNr = outputId->wirePointers[0]->number;
	allReachableAncillaOutputs.erase(outputWireNr);

	size_t inputWireNr = outputId->wirePointers[0]->number;

	/*
	 * All wires which touched the output wire are also touching the input wire
	 */
	wireelement* currentWire = causal.getFirstWireElement();
	while(currentWire != NULL)
	{
		if(currentWire->bits[outputWireNr] == 0)
		{
			//the current wire did not touch the output
			//but the output is recycled, so it cannot be used anymore
			currentWire->bits[outputWireNr] = 1;
		}
		else if(currentWire->bits[outputWireNr] == 1)
		{
//			if(currentWire->bits[inputWireNr] == 0)
			{
				//if the output was touched, then the inputs is touched too
				currentWire->bits[inputWireNr] = 1;

				//aici?
	//			currentWire->bits = currentWire->bits | outputId->wirePointers[0]->bits;
				currentWire->bits |= inputId->wirePointers[0]->bits;
			}
		}

		currentWire = currentWire->next;
	}

//
//	for(std::map<recyclegate*, std::set<recyclegate*> >::iterator it = perInputReachedOutputs.begin();
//			it!= perInputReachedOutputs.end(); it++)
//	{
//		//if from this input the specified output was found
//		if(it->second.find(outputId) != it->second.end())
//		{
//			//this is not an output any more
//			it->second.erase(outputId);
//			it->second.insert(receivedOutputs.begin(), receivedOutputs.end());
//		}
//	}
}

///**
// * DOES NOTHING
// * @param causal
// * @return
// */
//std::vector<std::vector<int> > wirerecycle::sortInputsForICM(causalgraph& causal)
//{
//	std::vector<std::vector<int> > ret;
//	//aici trebuie sa merg pe perInputReachedOutputs
//	//dar chiar e nevoie de functia asta?
//	for(int i=0; i<causal.getNrQubits(); i++)
//	{
//		if(causal.inAncillae.find(i) == causal.inAncillae.end())
//			continue;
//
//		if(perInputReachedOutputs.find(i) == perInputReachedOutputs.end())
//			continue;
//
//		std::vector<int> el(3);
//		el[0] = i;
//		el[1] = i;
//		el[2] = i;
//		ret.push_back(el);
//	}
//
//	return ret;
//}

//vector<vector<int> > wirerecycle::sortInputs()
//{
////	//use priority queue?
//	vector<vector<int> > ret;
//
//	for(map<int, set<int> >::iterator it = perInputReachedOutputs.begin(); it!= perInputReachedOutputs.end(); it++)
//	{
//		vector<int> el(3);
//		el[0] = it->first;
//		el[1] = causal.circuit[it->first].level;
//		el[2] = allReachableAncillaOutputs.size() - it->second.size();
//
//		ret.push_back(el);
//	}
//
//	if(ret.size() == 0)
//		return ret;
//
//	//bubble sort - as putea inlocui cu o cautare de maximum doar?
//	for(size_t i=0; i < ret.size() - 1; i++)
//	{
//		for(size_t j = i+1; j < ret.size(); j++)
//		{
//			if((ret[i][1] < ret[j][1]) || (ret[i][1] == ret[j][1] && causal.circuit[ret[i][0]].wires[0] < causal.circuit[ret[j][0]].wires[0]))
//			{
//				vector<int> x = ret[i];
//				ret[i] = ret[j];
//				ret[j] = x;
//			}
//		}
//	}
//
//	return ret;
//}

recyclegate* wirerecycle::chooseClosestAncillaOutputWire(recyclegate* inputId, bool preferLowNumberedWires)
{
	recyclegate* ret = NULL;

	wireelement* inputWireEl = inputId->wirePointers[0]->getUpdatedWire(inputId->orderNrInGateList);
	size_t inputWire = inputWireEl->number;
	int dif = INT_MAX;
//
//	for(std::set<recyclegate*>::iterator it = allReachableAncillaOutputs.begin();
//			it != allReachableAncillaOutputs.end(); it++)
//	{
////		if(perInputReachedOutputs[inputId].find(*it) == perInputReachedOutputs[inputId].end())
//		if((*it)->wirePointers[0]->bits[inputWire] == 1)
//		{
//			//not in reachable
//			size_t wire = (*it)->wirePointers[0]->number;
//			int ndif = abs(inputWire - wire);
//
//			//prefer only wires which are higher than the inputwire
//			if(preferLowNumberedWires && wire > inputWire)
//			{
//				continue;
//			}
//
//			if(ndif < dif)
//			{
//				dif = ndif;
//				ret = *it;
//			}
//		}
//	}

	for(size_t wire = inputWire; wire > 0; wire--)
	{
		//if the input does not reach output wire
		if(inputWireEl->bits[wire - 1] == 0)
		{
			std::map<size_t, recyclegate*>::iterator outputIt = allReachableAncillaOutputs.find(wire - 1);
			if( outputIt != allReachableAncillaOutputs.end())
			{
				return outputIt->second;
			}
		}
	}
//
	return NULL;
}

int wirerecycle::recycleUsingWireSequence(causalgraph& causal)
{
	initialiseSimQueue(causal);
	printf("initialsed sim\n");

//	std::vector<std::vector<int> > sortedin = sortInputsForICM(causal);

	int nrConnect = 0;
//	while(sortedin.size() > 0)
	//while(perInputReachedOutputs.size() > 0)
	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
				it != causal.tmpCircuit.end(); it++)
	{
//		int inputId = sortedin[0][0];
//		recyclegate* initGate = perInputReachedOutputs.begin()->first;
		recyclegate* initGate = *it;

		if(!initGate->isAncillaInitialisation())
		{
			continue;
		}

//		printf("%d %p\n", perInputReachedOutputs.size(), initGate);

//		if the number of remaining outputs is zero
//		if(sortedin[0][2] == 0)
//		{
//			perInputReachedOutputs.erase(inputId);
//			sortedin = sortInputsForICM(causal);
//			continue;
//		}

//		int outputId = chooseClosestAncillaOutputWire(causal, inputId, PREFERLOWNUMBEREDWIRES);
		recyclegate* measGate = chooseClosestAncillaOutputWire(initGate, PREFERLOWNUMBEREDWIRES);
		if(measGate != NULL)
		{
			nrConnect++;
			updateSimQueue(causal, measGate, initGate/*perInputReachedOutputs[initGate]*/);

//			causal.connectNodes(outputId, inputId);
			//VERY VERY BAD
//			causal.connectNodes(causal.circuit[outputId]->wires[0], causal.circuit[inputId]->wires[0],
//								outputId, inputId);
			initGate->wirePointers[0] =  initGate->wirePointers[0]->getUpdatedWire(0);
			measGate->wirePointers[0] =  measGate->wirePointers[0]->getUpdatedWire(0);

//			printf("join %d %d\n", measGate->wirePointers[0]->number, initGate->wirePointers[0]->number);

			causal.joinWires2(measGate->wirePointers[0], initGate->wirePointers[0]);
			initGate->wirePointers[0] =  initGate->wirePointers[0]->getUpdatedWire(0);

			causal.connectNodes(0, 0, measGate, initGate);

			measGate->causalType = RECMEASCONN;//temp value, todo
			initGate->causalType = RECINITCONN;

//			std::set<int> vis;
//			causal.replaceQubitIndex(vis, outputId, causal.circuit[inputId]->wires[0], causal.circuit[outputId]->wires[0]);

//			printf("rec nr %d\n", nrConnect);
		}

//		perInputReachedOutputs.erase(initGate);

//		sortedin = sortInputsForICM(causal);
	}

	printf("nr connect %d\n", nrConnect);

	return nrConnect;
}

//bool mycompare (std::vector<int> vi, std::vector<int>vj) { return vi[1] < vj[1]; }
//
//std::vector<std::vector<int> > wirerecycle::extractSortedInputsList(causalgraph& causal)
//{
//	std::vector < std::vector<int> > remainingIns;
//	for (int i = 0; i < causal.circuit.size(); i++)
//	{
//		//if (causal.circuit[i].type == -1 || causal.circuit[i].type == INPUT)
//		//if(gatenumbers::getInstance()->isInitialisationNumberButNotConnection(causal.circuit[i].type))
//		if(causal.circuit[i]->isFirstLevelInitialisation())
//		{
//			std::vector<int> el(2);
//			el[0] = i;//id
//			el[1] = causal.circuit[i]->wires[0];//wire
//			remainingIns.push_back(el);
//		}
//	}
//
//	std::sort (remainingIns.begin(), remainingIns.end(), mycompare);
//
//	return remainingIns;
//}

//void wirerecycle::renameWires(causalgraph& causal, std::vector<std::vector<int> >& remainingIns)
//{
//	//receives a sorted list, so renaming should not be problematic
//	//rename wires
//	int newNumber = -1;
//	for(std::vector<std::vector<int> >::iterator k = remainingIns.begin(); k!=remainingIns.end(); k++)
//	{
//		std::set<int> v;//empty set of visited nodes
//		newNumber++;
//		causal.replaceQubitIndex(v, k->at(0), k->at(1)/*old value*/, newNumber/*new value*/);
//		//printf("repl %d %d\n", k->at(1), newNumber);
//	}
//}

//void wirerecycle::recycle(circconvert& convert, int recycleMethod, costmodel& model)
void wirerecycle::recycle(causalgraph& causal, int recycleMethod)
{
//	causal.constructFrom(convert, model);

	int nrConnect = 0;
	if(recycleMethod == RECYCLEWIRESEQ)
	{
		nrConnect = recycleUsingWireSequence(causal);
	}
	else if(recycleMethod == RECYCLENONE)
	{
		nrConnect = 0;
	}

//	if(recycleMethod != RECYCLENONE)
//	{
//		std::vector<std::vector<int> > remainingIns = extractSortedInputsList(causal);
//		renameWires (causal, remainingIns);
//	}
}
