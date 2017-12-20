
#include <climits>
#include <cmath>
#include <algorithm>

#include "gatenumbers.h"
#include "recycling/causalgraph.h"
#include "numberandcoordinate.h"

std::vector<recyclegate*> causalgraph::getRoots()
{
	//asta merge pe gate-uri, dar ar fi mai interesant de mers pe fire
	std::vector<recyclegate*> ret;
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin(); it != tmpCircuit.end(); it++)
	{
		if((*it)->isFirstLevelInitialisation() && (*it)->pushedBy.size() == 0)
		{
			ret.push_back(*it);
		}
	}

	return ret;
}

//std::vector<int> causalgraph::getAncillaInitialisations()
//{
//	std::vector<int> ret;
//	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin(); it != tmpCircuit.end(); it++)
//	{
//		if((*it)->isFirstLevelInitialisation())
//		{
//			ret.push_back((*it)->id);
//		}
//	}
//
//	return ret;
//}

/**
 * Traverse the causal graph in an iterative manner
 * @param a traversal state
 * @return if the circuit is finished
 */
bool causalgraph::stepwiseBfs(bfsState& state)
{
	/*
	 * If all the graph nodes were processed
	 */
	//bool circuitFinished = false;
	state.setCircuitFinished(false);

	/*
	 * Reset toDraw and toSchedule
	 */
	state.resetToDrawAndToSchedule();

	/*
	 * Which Lines/Wires to Check
	 */
	state.initLinesToCheck();
	/*
	 * Reset the inputLevel
	 */
	state.resetLevels();

	/*
	 * Determines if all graph elements until the lowest possible unscheduled
	 * input were collected in toDraw and toSchedule
	 */
	while(state.minLevelLinesToCheck.size() != 0)
	{
		//advance the minimum level
		//get minimum level of current ids
		long 	minLevel = LONG_MAX;
		wireelement* minLevelPos = state.getMinLevelFromLinesToCheck(minLevel);
//		int 	minLevelOperationId = state.currentIdProWire[minLevelPos];
		recyclegate* minLevelOperationPtr = state.currentGateProWire[minLevelPos];

		/*
		 * There is no minimum
		 * bye bye - finished the entire circuit
		 */
		if(minLevel == LONG_MAX)
		{
			state.setCircuitFinished(true);
			break;
		}

		bool isInjection = (minLevelOperationPtr->type == AA || minLevelOperationPtr->type == YY);
		bool wasScheduled = (state.scheduledInputs.find(minLevelOperationPtr) != state.scheduledInputs.end());

		/*
		 * If this is the first scheduled/unscheduled input, save its level
		 */
		if(isInjection && state.getMaximumInputLevel() == NOLEVEL)
		{
			state.saveMaxLevel(minLevelOperationPtr->level);
			printf("new inputLevel%ld\n", state.getMaximumInputLevel());
		}

		if(state.getMinimumLevel() == NOLEVEL)
		{
			state.setMinimumLevel(minLevel);
		}

		/*
		 * The conditions to add in toDraw or toSchedule
		 */
		if(state.getMaximumInputLevel() == NOLEVEL || minLevel <= state.getMaximumInputLevel())
		{
			if(!isInjection || wasScheduled)
			{
				//can be drawn
				state.toDraw.push_back(minLevelOperationPtr);

				//advance on the wires having the operation with min level
				for(std::set<wireelement*>::iterator it = state.minLevelLinesToCheck.begin();
						it != state.minLevelLinesToCheck.end(); it++)
				{
					if(state.currentGateProWire[*it] == minLevelOperationPtr)
					{
						state.currentGateProWire[*it] = getSuccessorOnSameWire(state.currentGateProWire[*it], *it);
					}
				}
			}
			else
			{
				//cannot be drawn and needs scheduling
				int boxType = (minLevelOperationPtr->type == AA ? ATYPE : YTYPE);
				state.toScheduleInputs[boxType].push_back(minLevelOperationPtr);
			}
		}

		/*
		 * From here everything is on a higher level than the
		 * inputs to be scheduled. Skip this wire in further searches.
		 */
		if(state.getMaximumInputLevel() > NOLEVEL && minLevel >= state.getMaximumInputLevel())
		{
			state.removeFromLinesToCheck(minLevelPos);
		}
	}

	if(state.isCircuitFinished())
	{
		state.setRequiredMaximumInputLevel(state.toDraw.front()->level, "stepwisebfs");
	}

	return state.isCircuitFinished();
}

/*
 * Very similar to stepwiseBfs. Too similar.
 */
std::vector<recyclegate*> causalgraph::bfs()
{
	std::vector<recyclegate*> ret;

	std::vector<recyclegate*> inputs = getRoots();

	//this is a similar trick to lastSeen
	std::map<wireelement*, recyclegate*> currentIdProWire;//(inputs.size(), NULL);

	for(size_t i=0; i<inputs.size(); i++)
	{
		//take the id of the input operations
		currentIdProWire[inputs[i]->wirePointers[0]] = inputs[i];
	}

	while(true)
	{
		//advance the minimum level
		//get minimum level of current ids
		int min = INT_MAX;

		wireelement* minpos = NULL;
		for(size_t i=0; i<inputs.size(); i++)
		{
			wireelement* wireEl = inputs[i]->wirePointers[0];

			if(currentIdProWire.find(wireEl) != currentIdProWire.end()
					&& min > currentIdProWire[wireEl]->level)
			{
				min = currentIdProWire[wireEl]->level;
				minpos = wireEl;
			}
		}

		//there is no minimum - why? bye bye
		if(min == INT_MAX)
		{
			break;
		}

		//take the operation having the min level from minpos
		recyclegate* oldid = currentIdProWire[minpos];

		//put it in the return list
		ret.push_back(oldid);

		//advance on the wires having the operation with min level
		for(size_t i=0; i<inputs.size(); i++)
		{
			wireelement* wireEl = inputs[i]->wirePointers[0];
			if(currentIdProWire.find(wireEl) != currentIdProWire.end() /*exists*/
					&& currentIdProWire[wireEl] == oldid)/*compare pointers*/
			{
				recyclegate* next = getSuccessorOnSameWire(currentIdProWire[wireEl], wireEl);
				if(next != NULL)
				{
					currentIdProWire[wireEl] = next;
				}
				else
				{
					currentIdProWire.erase(wireEl);
				}

			}
		}
	}

	return ret;
}

void causalgraph::reconstructConsideringCosts(std::vector<recyclegate*>& order)
{
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
				it != tmpCircuit.end(); it++)
	{
		(*it)->computeWiresWithCosts();
	}

	computeLinks(order);
}

void causalgraph::reconstructWithoutConsideringCosts(std::vector<recyclegate*>& order)
{
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
				it != tmpCircuit.end(); it++)
	{
		(*it)->computeOnlyWires();
	}

	computeLinks(order);
}

void causalgraph::computeLinks(std::vector<recyclegate*> operationOrder)
{
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
					it != tmpCircuit.end(); it++)
	{
		(*it)->willPush.clear();
		(*it)->pushedBy.clear();
	}

	std::map<wireelement*, recyclegate*> lastSeen;
//    int lines = getRoots().size();
	std::vector<recyclegate*> roots = getRoots();

    for(size_t i = 0; i < roots.size(); i++)
    {
        lastSeen[roots[i]->wirePointers[0]] = NULL; //the ids of the inputs
    }

    for(std::vector<recyclegate*>::iterator itOperation = operationOrder.begin();
    		itOperation != operationOrder.end(); itOperation++)
    {
    	//ce cauta nrWire aici?
    	int nrWire = 0;
        for(std::vector<wireelement*>::iterator itWire = (*itOperation)->wiresToUseForLinks.begin();
        		itWire != (*itOperation)->wiresToUseForLinks.end(); itWire++)
        {
        	//if(*itWire < lines)
//        	{
        		if(lastSeen[*itWire] != NULL)
        		{
        			//the same wire for both gates
//        			connectNodes(*itWire, *itWire, lastSeen[*itWire], *itOperationId);
        			connectNodes(-1, nrWire, lastSeen[*itWire], *itOperation);
        		}
        		lastSeen[*itWire] = *itOperation;

        		nrWire++;
//        	}
        }
    }

}

//void causalgraph::constructFrom(std::list<recyclegate>& gates, std::list<char>& inputs, std::list<char>& outputs, costmodel& model)
//{
//	printf("gates...\n");
//	for(std::list<recyclegate>::iterator it = gates.begin();
//			it != gates.end(); it++)
//	{
//		recyclegate* gate = new recyclegate();
//		gate->initFrom(*it);
//		//gate.print();
//
//		circuit.push_back(gate);
//		tmpCircuit.push_back(gate);
//
//		circuit.back()->updateCausalType();
//		circuit.back()->gateCost = 1;//is this a cnot?
//	}
//
////	nrQubits = cc.inputs.size();
//
//	printf("inputs...\n");
//	//introduce input and output nodes
//	int nrWire = 0;
//	std::vector<recyclegate*> tmpInputs;
//	for(std::list<char>::iterator it = inputs.begin(); it != inputs.end(); it++)
//	{
//		recyclegate* input = new recyclegate();
//		input->updateTypeAndCost(true, *it, model);
//
//		input->wires.push_back(nrWire);
//		if(gatenumbers::getInstance().isAncillaInput(input->type))
//		{
//			inAncillae.insert(nrWire);
//		}
//
//		/*the first nodes are inputs*/
//		//was reallocating...slow...
//		//circuit.insert(circuit.begin() + nrWire, input)->updateCausalType();
//		input->updateCausalType();
//		tmpInputs.push_back(input);
//
//		nrWire++;
//	}
//	circuit.insert(circuit.begin(), tmpInputs.begin(), tmpInputs.end());
//	tmpCircuit.insert(tmpCircuit.begin(), tmpInputs.begin(), tmpInputs.end());
//
//	nrWire = 0;
//	printf("outputs...\n");
//	for(std::list<char>::iterator it = outputs.begin(); it != outputs.end(); it++)
//	{
//		recyclegate* output = new recyclegate();
//		output->updateTypeAndCost(false, *it, model);
//
//		output->wires.push_back(nrWire);
//		if(gatenumbers::getInstance().isAncillaOutput(output->type))
//		{
//			outAncillae.insert(outputs.size() + gates.size() + nrWire);
//		}
//
//		/*the last nodes are outputs*/
//		output->updateCausalType();
//		circuit.push_back(output);
//		tmpCircuit.push_back(output);
//		//circuit.insert(circuit.end(), output)->updateCausalType();
//
//		nrWire++;
//	}
//
//	//aici deja le da un ID...e bine? pentru noua versiune?
//	//set the id attribute of the gates
//	std::vector<int> order;
//	int nrId = 0;
//	for(std::vector<recyclegate*>::iterator it = circuit.begin(); it != circuit.end(); it++)
//	{
//		(*it)->id = nrId;
//		order.push_back(nrId);
//		nrId++;
//	}
//
//	printf("reconstructWithoutConsideringCosts\n");
//
//    reconstructWithoutConsideringCosts(order);
//}

std::vector<wireelement*> causalgraph::splitWire2(wireelement* wEOrig, std::list<recyclegate*>::iterator& originalGate)
{
	//set threshold in old wire
	wEOrig->threshold = (*originalGate)->orderNrInGateList;

//	printf("splits %d\n", (*originalGate)->orderNrInGateList);

	//cut the previous wireElement from the list
	wireelement* origPrev = wEOrig->prev;
	wireelement* origNext = wEOrig->next;
	if(origPrev != NULL)
	{
		origPrev->cutForward();
	}
	wEOrig->cutForward();

	//the old wire is pointing to the new wires
	wireelement* wE1 = new wireelement();
	wireelement* wE2 = new wireelement();
	wEOrig->ngh[0] = wE1;
	wEOrig->ngh[1] = wE2;

	//the new wires are connected between them
	wE1->linkForward(wE2);
	//and joined to the list
	if(origPrev != NULL)
	{
		origPrev->linkForward(wE1);
	}
	wE2->linkForward(origNext);

	//for memory management of wires
	memWireElements.push_back(wE1);
	memWireElements.push_back(wE2);

	std::vector<wireelement*> ret;
	ret.push_back(wE1);
	ret.push_back(wE2);

	return ret;
}

void causalgraph::joinWires2(wireelement* wireMeasurement, wireelement* wireInitialisation)
{
	//take the initialisation out
	wireelement* initNext = wireInitialisation->cutForward();
	wireelement* initPrev = wireInitialisation->cutBackward();

	//connect the prev to the next
	initPrev->linkForward(initNext);

	//join
	wireInitialisation->ngh[1] = wireMeasurement;
	wireInitialisation->threshold = -1;//the threshold is initialised to
}


std::vector<recyclegate*> causalgraph::insertMeasurementAndInitialisation2(std::vector<wireelement*>& wires,
		std::string ins,
		std::string outs,
		std::list<recyclegate*>::iterator& originalGate)
{
	std::vector<recyclegate*> ret;

	/*
	 * Delete the original gate
	 */
	long originalNr = (*originalGate)->orderNrInGateList;
	originalGate = tmpCircuit.erase(originalGate);

	/*
	 * Process initialisations
	 */
	for(size_t i=0; i<ins.size(); i++)
	{
		if(ins[i] != '_')
		{
			recyclegate* nInitialisation = new recyclegate();
			nInitialisation->updateTypeAndCost(true, ins[i], costModel);
			nInitialisation->updateCausalType();
			nInitialisation->wirePointers.push_back(wires[i]);//updated wire reference in new gate
			nInitialisation->orderNrInGateList = originalNr;//is this ok?

			memTmpCircuit.push_back(nInitialisation);

			tmpCircuit.insert(originalGate, nInitialisation);

			ret.push_back(nInitialisation);
		}
	}

	/*
	 * Process measurements: backwards
	 */
	for(size_t i=outs.size(); i > 0; i--)
	{
		if(outs[i-1] != '_')
		{
			recyclegate* nMeasurement = new recyclegate();
			nMeasurement->updateTypeAndCost(false, outs[i - 1], costModel);
			nMeasurement->updateCausalType();
			nMeasurement->wirePointers.push_back(wires[i - 1]);//updated wire reference in new gate
			nMeasurement->orderNrInGateList = originalNr;//is this ok?

			memTmpCircuit.push_back(nMeasurement);

			originalGate = tmpCircuit.insert(originalGate, nMeasurement);

			ret.push_back(nMeasurement);
		}
	}


//	//measurement
//	recyclegate* nMeasurement = new recyclegate();
//	nMeasurement->id = (*originalGate)->id;
//	nMeasurement->afterSplit = (*originalGate)->afterSplit;
//	nMeasurement->afterSplit.push_back(0);
//	nMeasurement->wirePointers.push_back(wires[0]);//updated wire reference in new gate
//
//	//initialisation
//	recyclegate* nInitialisation = new recyclegate();
//	nInitialisation->id = (*originalGate)->id;
//	nInitialisation->afterSplit = (*originalGate)->afterSplit;
//	nInitialisation->afterSplit.push_back(1);
//	nInitialisation->wirePointers.push_back(wires[1]);//updated wire reference in new gate
//
//	//for memory management of gates
//	memTmpCircuit.push_back(nMeasurement);
//	memTmpCircuit.push_back(nInitialisation);
//
//	tmpCircuit.insert(originalGate, nInitialisation);
//	tmpCircuit.insert(originalGate, nMeasurement);
//	//The order will be nInitialisation, nMeasurement, originalGate
//
//	//remove the old gate from the gate list
//	originalGate = tmpCircuit.erase(originalGate);
//	originalGate--;//points to nMeasurement now
//
//	vector<recyclegate*> ret;
//	ret.push_back(nMeasurement);
//	ret.push_back(nInitialisation);

	return ret;
}

std::vector<wireelement*> causalgraph::insertWires2(std::vector<wireelement*>& wires, int nr)
{
	if(nr == 0)
	{
		return wires;
	}

	wireelement* current = wires[0];
	wireelement* lastWire = wires[1];
	wires.pop_back();//removes lastWire

	//cut
	current->cutForward();

	for(int i=0; i<nr; i++)
	{
		wireelement* additionalWire = new wireelement();

		//memory management
		memWireElements.push_back(additionalWire);

		//add to list
		wires.push_back(additionalWire);

		//connect with current
		current->linkForward(additionalWire);

		current = additionalWire;
	}

	current->linkForward(lastWire);
	wires.push_back(lastWire);

	return wires;
}

void causalgraph::insertGates2(std::vector<std::string>& gateList,
		std::vector<wireelement*>& wires,
		std::list<recyclegate*>::iterator& originalGate)
{
	for(size_t i=0; i<gateList.size(); i++)
	{
		recyclegate* gate = new recyclegate(gateList[i], wires);
		gate->updateCausalType();
		tmpCircuit.insert(originalGate, gate);

		memTmpCircuit.push_back(gate);

		gate->orderNrInGateList = (*originalGate)->orderNrInGateList;
	}
}

std::vector<recyclegate*> causalgraph::numberGateList2()
{
	std::vector<recyclegate*> ret;
	int nrId = 0;
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin(); it != tmpCircuit.end(); it++)
	{
		(*it)->orderNrInGateList = nrId;
		ret.push_back(*it);
		nrId++;


		if((*it)->type == EMPTY)
		{
			printf("THERE IS A GATE with type EMPTY!!!!!!\n");
		}

//		circuit.push_back(*it);
	}

	return ret;
}

wireelement* causalgraph::getFirstWireElement()
{
	//get first wire
	wireelement* first = (*tmpCircuit.begin())->wirePointers[0]->getUpdatedWire(
			(*tmpCircuit.begin())->orderNrInGateList);
	while (first->prev != NULL)
	{
		first = first->prev;
	}
	return first;
}

size_t causalgraph::numberWires2()
{
	//get first wire

	wireelement* current = getFirstWireElement();

	size_t nrWire = 0;
	while(current != NULL)
	{
		current->number = nrWire;
		nrWire++;

		//advance first;
		current = current->next;
	}

	current = getFirstWireElement();
	while(current != NULL)
	{
		current->bits.reset();
		current->bits[current->number] = 1;

		//advance first;
		current = current->next;
	}

	printf("wires %lu\n", nrWire);

	return nrWire;
}

//void causalgraph::orderWires2()
//{
//	wireelement* current = getFirstWireElement();
//
//	wireorder* prevOrder = NULL;
//
//	while(current != NULL)
//	{
//		wireorder* order = new wireorder();
//
//		//set orders to the wire
//		current->start = order;
//		current->stop = order;
//
//		if(prevOrder!= NULL)
//		{
//			prevOrder->next = order;
//			order->prev = prevOrder;
//		}
//		prevOrder = order;
//
//		memWireOrder.push_back(order);
//
//		current = current->next;
//	}
//
//	initiallyFirstWireOrder = getFirstWireElement()->start;
//}

void causalgraph::printGateList2()
{
	printf("--------\n");
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin(); it != tmpCircuit.end(); it++)
	{
		printf("%c (%d) [%d] ", (*it)->type, (*it)->causalType, (*it)->orderNrInGateList);
		for(size_t i=0; i<(*it)->wirePointers.size(); i++)
		{
			printf("%lu ", (*it)->wirePointers[i]);
		}
		printf("\n");
	}
}


void causalgraph::constructFrom2(std::vector<std::string>& file)
{
	std::vector<wireelement*> tmpWireIndices;

	int nrWire = 0;

	std::istringstream inin(file[0]);
	std::string s;
	inin >> s;
	inin >> s;

	for(size_t i=0; i<s.size(); i++)
	{
		recyclegate* input = new recyclegate();
		input->updateTypeAndCost(true, s[i], costModel);
		input->updateCausalType();

		/*construieste elementele de fir*/
		wireelement* wireEl = new wireelement();
		tmpWireIndices.push_back(wireEl);
		if(nrWire > 0)
		{
			//make the links
//			tmpWireIndices[nrWire - 1]->next = tmpWireIndices[nrWire];
//			tmpWireIndices[nrWire]->prev = tmpWireIndices[nrWire - 1];
			tmpWireIndices[nrWire - 1]->linkForward(tmpWireIndices[nrWire]);
		}
		memWireElements.push_back(wireEl);

//		input->wires.push_back(nrWire);
		input->wirePointers.push_back(tmpWireIndices[nrWire]);

//		if(gatenumbers::getInstance().isAncillaInput(input->type))
//		{
//			inAncillae.insert(nrWire);
//		}

		tmpCircuit.push_back(input);
		memTmpCircuit.push_back(input);

		nrWire++;
	}

	/*
	 * Update First Wire
	 */
	firstWire = tmpWireIndices[0];

	for(size_t i=2; i<file.size(); i++)
	{
		recyclegate* gate = new recyclegate(file[i], tmpWireIndices);
		gate->updateCausalType();
		gate->gateCost = 1;//is this a cnot?

		tmpCircuit.push_back(gate);
		memTmpCircuit.push_back(gate);
	}

	nrWire = 0;
	std::istringstream outout(file[1]);
	outout >> s;
	outout >> s;
	for(size_t i=0; i<s.size(); i++)
	{
		recyclegate* output = new recyclegate();
		output->updateTypeAndCost(false, s[i], costModel);
		/*the last nodes are outputs*/
		output->updateCausalType();

//		output->wires.push_back(nrWire);
		output->wirePointers.push_back(tmpWireIndices[nrWire]);

//		if(gatenumbers::getInstance().isAncillaOutput(output->type))
//		{
////			outAncillae.insert(cc.outputs.size() + cc.gates.size() + nrWire);
//			outAncillae.insert(circuit.size());
//		}

		tmpCircuit.push_back(output);
		memTmpCircuit.push_back(output);

		nrWire++;
	}
}

recyclegate* causalgraph::getSuccessorOnSameWire(recyclegate* current, wireelement* wire)
{
//	int lmin = INT_MAX;
	recyclegate* ret = NULL;
//
//	for(std::map<int, recyclegate*>::iterator it = current->willPush.begin();
//			it != current->willPush.end(); it++)
//	{
//		recyclegate* operationId = it->second;
//
//		for(std::vector<wireelement*>::iterator wit = operationId->wiresToUseForLinks.begin();
//				wit != operationId->wiresToUseForLinks.end(); wit++)
//		{
//			if(*wit == wire /*pointer compare*/ && lmin > operationId->level)
//			{
//				lmin = operationId->level;
//				ret = operationId;
//			}
//		}
//	}

	//this is how it should maybe work. for the moment keep the map complication
//	return circuit[currid]->willPush[wire];
	std::vector<wireelement*>::iterator it =
			std::find(current->wirePointers.begin(), current->wirePointers.end(), wire);
	if(it != current->wirePointers.end())
	{
		int pos = std::distance(current->wirePointers.begin(), it);
		ret = current->willPush[pos];
	}

	//NULL should happen only for outputs
	return ret;
}

long causalgraph::getAndSetMaxPrevLevel(recyclegate* current, bool useGateCost, int timeDistanceBetweenOperations)
{
	//take the maximum level from the neighbouring nodes that are "pushing"
	long maxPrevLevel = LONG_MIN;

//	for (set<int>::iterator it = circuit[currid]->pushedBy.begin();
//			it != circuit[currid]->pushedBy.end(); it++)

	for (std::map<int, recyclegate*>::iterator it = current->pushedBy.begin();
				it != current->pushedBy.end(); it++)
	{
		recyclegate* operationPtr = it->second;

		int operationTimeCost = 0;
		if(useGateCost == true)
		{
			operationTimeCost = operationPtr->gateCost;
		}

		//daca vecinul are nivelul -1. fix pula
		if(operationPtr->level == -1)
		{
			int maxLevelUntilNow = getAndSetMaxPrevLevel(operationPtr, useGateCost, timeDistanceBetweenOperations);

			//un mini setter ...
			long increaseLevelValue = operationTimeCost + timeDistanceBetweenOperations;
			operationPtr->level = maxLevelUntilNow + increaseLevelValue;

			if(operationPtr->causalType == RECGATE)
			{
				//if one of the precedent nodes is gate, then time + 1, because for cnots the length is dual
				long enforcedTimeDifference = increaseLevelValue + 1;
				long minTimeDifference = enforcedTimeDifference;

				//Imi este foarte neclar ce face bucata asta. A ramas aici de candva?
				for(std::map<int, recyclegate*>::iterator pit = operationPtr->pushedBy.begin();
						pit != operationPtr->pushedBy.end();
						pit++)
				{
					recyclegate* pitPtr = pit->second;
					if(pitPtr->causalType == RECGATE)
					{
						long timeDif = operationPtr->level - pitPtr->level;
						if(timeDif < minTimeDifference)
						{
							minTimeDifference = timeDif;
						}
					}
				}
				long missingTimeDifference = enforcedTimeDifference - minTimeDifference;
				operationPtr->level += missingTimeDifference;
			}
		}

		if (maxPrevLevel < operationPtr->level)
		{
			maxPrevLevel = operationPtr->level;
		}
	}

	return maxPrevLevel;
}

void causalgraph::connectNodes(int prevWire, int currWire, recyclegate* prevGate, recyclegate* currGate)
{
//	prevGate->addWillPush(prevWire, currGate);

//	printf("connect %p %p\n", prevGate, currGate);

	int pos = -1;
	std::vector<wireelement*>::iterator foundIt;
	foundIt = std::find(prevGate->wirePointers.begin(), prevGate->wirePointers.end(),
			currGate->wirePointers[currWire]);

	if(foundIt != prevGate->wirePointers.end())
	{
		pos = std::distance(prevGate->wirePointers.begin(), foundIt);
	}
	else
	{
		printf("connectnodes: not found!\n");
	}
	prevGate->addWillPush(pos, currGate);

	currGate->addPushedBy(currWire, prevGate);
}

void causalgraph::updateLabels()
{
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
				it != tmpCircuit.end(); it++)
	{
		(*it)->generateLabel();
	}
}

void causalgraph::equalizeLevels()
{
	for(std::list<recyclegate*>::reverse_iterator it = tmpCircuit.rbegin();
			it != tmpCircuit.rend(); it++)
	{
		//vezi care este nivelul minim urmator
		int min = INT_MAX;
		for(std::map<int, recyclegate*>::iterator it2 = (*it)->willPush.begin();
				it2 != (*it)->willPush.end(); it2++)
		{
			if(min > it2->second->level)
			{
				min = it2->second->level;
			}
		}

		if(min != INT_MAX)
		{
			int dist = min - ((*it)->level + 1);

			(*it)->level += dist;
		}
	}
}

//void causalgraph::replaceQubitIndex(std::set<int>& visited, int curr, int oldvalue, int newvalue)
//{
//	if(visited.find(curr) != visited.end())
//		return;
//
//	visited.insert(curr);
//	std::vector<int>::iterator it;
//	it = std::find (circuit[curr]->wires.begin(), circuit[curr]->wires.end(), oldvalue);
//	if(it != circuit[curr]->wires.end())
//	{
//		*it = newvalue;
//	}
//
//
//	for(std::map<int, int>::iterator it=circuit[curr]->willPush.begin(); it!=circuit[curr]->willPush.end(); it++)
//	{
//		replaceQubitIndex(visited, it->second, oldvalue, newvalue);
//	}
//}

//void causalgraph::findShortestPath(set<int>& visited, set<int>& outputs,
//		vector<int> path, vector<int>& shortest,
//		int stepback, int prev, int curr)
//{
//	if(visited.find(curr) != visited.end())
//	{
//		return;
//	}
//
//	if(visited.empty())
//	{
//		set<int> vis2;
//		//set<int> outputs;
//		reachOutputs(vis2, outputs, curr);
//		//pentru a evita cicluri cu outputuri
//		visited.insert(outputs.begin(), outputs.end());
//	}
//
//	visited.insert(curr);
//
//	//do not consider nodes where output and input were joined
//	if(circuit[curr]->isConnection())
//		//path.insert(curr);
//		path.push_back(curr);
//
//	//check solution
//	//mai bine ar fi pe linia cea mai apropiata
//	//e inutila conditia cu stepback?
//	//if(stepback>0 &&  gatenumbers::getInstance()->isAncillaOutput(circuit[curr].type) /*circuit[curr].type == -2*/ && outputs.find(curr) == outputs.end())//todo
//	if(stepback>0 && circuit[curr]->isAncillaMeasurement() && outputs.find(curr) == outputs.end())//todo
//	{
//		/*METODA CU STEPBACK*/
//		if (shortest.size() == 0 || (path.size() < shortest.size()-1 || stepback< -shortest.back() ))
//		{
//			shortest.clear();
//			shortest.insert(shortest.begin(), path.begin(), path.end());
//
//			//shortest.push_back(-distzero);
//
//			/*METODA CU STEPBACK*/
//			//last element is number of stepbacks - not an id
//			shortest.push_back(-stepback);
//			//shortest.insert(curr);
//
//			/*METODA CU DISTWIRE*/
//			//shortest.push_back(-distwire);
//
//			//printf("# %d %d\n", path.size(), distwire);
//		}
//	}
//
//	//BACKWARD
//	for(map<int, int>::iterator it=circuit[curr]->pushedBy.begin();
//			it!=circuit[curr]->pushedBy.end(); it++)
//	{
//		if(it->second != prev)
//		{
//			//fa pas inapoi
//			//findShortestPath(visited, shortest, true, curr, *it);
//			int counts = 1;
//			//if(circuit[*it].type == -3 || circuit[*it].type == -4)
//			//if(gatenumbers::getInstance()->isConnectionElement(circuit[*it].type))
//			if(circuit[it->second]->isConnection())
//				counts = 0;
//			findShortestPath(visited, outputs, path, shortest, stepback + counts, curr, it->second);
//		}
//	}
//
//	//FORWARD
//	for(map<int, int>::iterator it=circuit[curr]->willPush.begin();
//			it!=circuit[curr]->willPush.end(); it++)
//	{
//		//fa pas inapoi
//		findShortestPath(visited, outputs, path, shortest, stepback, curr, it->second);
//	}
//}

//int causalgraph::moveInputAfterOutput(vector<int> shortest, int inputId)
//{
//	int outputId = -1;
//
//	//o cautare iditoata - ar trebui sa fie ultimul element
//	for (vector<int>::iterator it = shortest.begin(); it != shortest.end(); it++)
//	{
//		if (circuit[*it]->isAncillaMeasurement())
//		{
//			outputId = *it;
//
//			circuit[outputId]->causalType = RECMEASCONN;//temp value, todo
//			circuit[inputId]->causalType = RECINITCONN;
//
//			connectNodes(circuit[outputId]->wires[0], circuit[inputId]->wires[0],
//					outputId, inputId);
//
//			break;
//		}
//	}
//
//	return outputId;
//}

void causalgraph::computeLevels()
{
	resetAllLevels();
	computeLevels(false, 1);
}

void causalgraph::resetAllLevels()
{
    for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
    		it != tmpCircuit.end(); it++)
    {
        //reset level
        if((*it)->pushedBy.size() == 0)
        {
        	//initialisations
            (*it)->level = (*it)->gateCost;
        }else
        {
            //reset, all others
            (*it)->level = -1;
        }
    }
}

void causalgraph::equalizeOutputLevels()
{
	long maxlevel = getMaxLevel();
	 for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
	    		it != tmpCircuit.end(); it++)
	 {
		if((*it)->willPush.size() == 0)
		{
			(*it)->level = maxlevel;
		}
	}
}

void causalgraph::equalizeInputLevels()
{
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
				it != tmpCircuit.end(); it++)
	{
		if((*it)->pushedBy.size() == 0)
		{
			(*it)->level = 0;
		}
	}
}

void causalgraph::updateLevelsAtValue(bfsState& current, long oldCost, long newCost)
{
	/*
	 * The method assumes that the inputs have different levels.
	 * This is not the case, in the way stepwiseBfs works
	 */
	/**
	 * Trebuie sa actualizez costurile doar inputurilor care nu sunt
	 * urmasi pe o anumita creanga a grafului
	 */
	for(int i=0; i<2; i++)
	{
		for(std::vector<recyclegate*>::iterator it = current.toScheduleInputs[i].begin(); it!=current.toScheduleInputs[i].end(); it++)
		{
			if((*it)->level == oldCost)
			{
				(*it)->updateGateCostToAchieveLevel(newCost);
			}
		}
	}

	for(std::vector<recyclegate*>::iterator it = current.toDraw.begin(); it!=current.toDraw.end(); it++)
	{
		if((*it)->level == oldCost)
		{
			(*it)->updateGateCostToAchieveLevel(newCost);
		}
	}
}

std::vector<recyclegate*> causalgraph::equalizeConsideringCosts()
{
	computeLevels();

	std::vector<recyclegate*> order;

	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
				it != tmpCircuit.end(); it++)
	{
		(*it)->computeOnlyWires();
	}

	order = bfs();

	reconstructConsideringCosts(order);

	resetAllLevels();
	computeLevels(true, 0);

//	causal.updateLabels();
//	writeGMLFile("y.gml");

	reconstructWithoutConsideringCosts(order);

	equalizeOutputLevels();

	return order;
}

/**
 * Is this used?
 * @param currId
 */
void causalgraph::resetLevelsStartingFrom(recyclegate* current)
{
	if(current->level != -1)
	{
		for(std::map<int, recyclegate*>::iterator it = current->willPush.begin();
				it !=current->willPush.end(); it++)
		{
			resetLevelsStartingFrom(it->second);
		}
		current->level = -1;
	}
}


void causalgraph::computeLevels(bool useGateCost, int operationDistance)
{
    //take the outputs
    for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
    		it != tmpCircuit.end(); it++)
    {
        //reset level
        if((*it)->willPush.size() == 0)
        {
        	//these are measurements
			long max = getAndSetMaxPrevLevel(*it, useGateCost, operationDistance);
			(*it)->level = max + operationDistance;
		}
	}
}

///**
// * Recursive search for output ancillae starting from a particular gate
// * @param visited
// * @param outputs
// * @param curr
// */
//void causalgraph::reachOutputs(/*std::set<recyclegate*>& visited, */std::set<recyclegate*>& outputs, recyclegate* curr)
//{
//	if(visited.find(curr) != visited.end())
//		return;
//
//	visited.insert(curr);
//
//	if(curr->isAncillaMeasurement())
//	{
//		outputs.insert(curr);
//	}
//
//	for(std::map<int, recyclegate*>::iterator it = curr->willPush.begin();
//			it != curr->willPush.end(); it++)
//	{
//		reachOutputs(visited, outputs, it->second);
//	}
//}

long causalgraph::getMaxLevel()
{
	long max = LONG_MIN;
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
			it != tmpCircuit.end(); it++)
	{
		if(max < (*it)->level)
		{
			max = (*it)->level;
		}
	}
	return max;
}

int causalgraph::getNrQubits()
{
	//return nrQubits;
	//is this the number of roots?
	return getRoots().size();
}


causalgraph::causalgraph(costmodel& model)
{
	costModel = model;
}

causalgraph::~causalgraph()
{
	for(std::list<recyclegate*>::iterator it = memTmpCircuit.begin();
				it != memTmpCircuit.end(); it++)
	{
		delete *it;
	}

	for(std::list<wireelement*>::iterator it = memWireElements.begin();
			it != memWireElements.end(); it++)
	{
		delete *it;
	}

//	while(initiallyFirstWireOrder->prev != NULL)
//	{
//		initiallyFirstWireOrder = initiallyFirstWireOrder->prev;
//	}
//
//	while(initiallyFirstWireOrder->next != NULL)
//	{
//		initiallyFirstWireOrder = initiallyFirstWireOrder->next;
//
//		delete initiallyFirstWireOrder->prev;
//	}
//	delete initiallyFirstWireOrder;
}

