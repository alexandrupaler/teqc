#include <climits>
#include <cmath>
#include <algorithm>

#include "gatenumbers.h"
#include "recycling/causalgraph.h"
#include "numberandcoordinate.h"

#include "utils/trim.h"

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
//	state.initLinesToCheck();
	/*
	 * Reset the inputLevel
	 */
	state.resetLevels();

	/*
	 * september 2017
	 * A step is finished when all the inputs (scheduled or not) at a certain level were processed
	 */
	bool finishedStep = (state.septBfs.size() == state.septLastIndex);

	int currentIndex = state.septLastIndex;
	while(!finishedStep)
	{
		recyclegate* minLevelOperationPtr = state.septBfs[currentIndex];
		long minLevel = minLevelOperationPtr->level;

		if(state.getMaximumInputLevel() != NOLEVEL && minLevel != state.getMaximumInputLevel())
		{
			/*
			 * an injection was found at the previous step
			 * and the current level is higher
			 */
			finishedStep = true;
		}

//		/*RANDOM RES ESTIMTOR
//		 * 3 NOV
//		 * take only one element for scheduling
//		 */
//		if(state.toDraw.size() > 0 && state.toScheduleInputs[0].size() != 0)
//		{
//			finishedStep = true;
//			state.toScheduleInputs[0].clear();
//		}
		if(state.toScheduleInputs[0].size() != 0 || state.toScheduleInputs[1].size() != 0)
		{
			finishedStep = true;
		}

		if(!finishedStep)
		{
			//advance the index
			currentIndex++;

//			bool isInjection = (minLevelOperationPtr->type == AA || minLevelOperationPtr->type == YY);
//			bool isInjection = (minLevelOperationPtr->type == 't' || minLevelOperationPtr->type == 'p');
			bool isInjection = (minLevelOperationPtr->type == 't');
			bool wasScheduled = (state.scheduledInputs.find(minLevelOperationPtr) != state.scheduledInputs.end());

			/*
			 * the first injection at a level is found
			 * store the current level
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

			if(!isInjection || wasScheduled)
			{
				//can be drawn
				state.toDraw.push_back(minLevelOperationPtr);
			}
			else
			{
				//cannot be drawn and needs scheduling
//				int boxType = (minLevelOperationPtr->type == AA ? ATYPE : YTYPE);
				int boxType = (minLevelOperationPtr->type == 't' ? ATYPE : YTYPE);
				state.toScheduleInputs[boxType].insert(minLevelOperationPtr);
			}

//			if(state.toScheduleInputs[0].size() == 0 && state.toScheduleInputs[1].size() == 0)
			{
				//if nothing was planned for scheduling, save the current index
				state.septLastIndex = currentIndex;
			}

			/*
			 * the index was advanced, but does it still fit the vector bounds?
			 */
			finishedStep = (state.septBfs.size() == currentIndex);
			if(state.septBfs.size() == state.septLastIndex)
			{
				state.setCircuitFinished(true);
			}
		}
	}



//	/*
//	 * Determines if all graph elements until the lowest possible unscheduled
//	 * input were collected in toDraw and toSchedule
//	 */
//	while(state.minLevelLinesToCheck.size() != 0)
//	{
//		//advance the minimum level
//		//get minimum level of current ids
//		long 	minLevel = LONG_MAX;
//		wireelement* minLevelPos = state.getMinLevelFromLinesToCheck(minLevel);
////		int 	minLevelOperationId = state.currentIdProWire[minLevelPos];
//		recyclegate* minLevelOperationPtr = state.currentGateProWire[minLevelPos];
//
//		/*
//		 * There is no minimum
//		 * bye bye - finished the entire circuit
//		 */
//		if(minLevel == LONG_MAX)
//		{
//			state.setCircuitFinished(true);
//			break;
//		}
//
//		bool isInjection = (minLevelOperationPtr->type == AA || minLevelOperationPtr->type == YY);
//		bool wasScheduled = (state.scheduledInputs.find(minLevelOperationPtr) != state.scheduledInputs.end());
//
//		/*
//		 * If this is the first scheduled/unscheduled input, save its level
//		 */
//		if(isInjection && state.getMaximumInputLevel() == NOLEVEL)
//		{
//			state.saveMaxLevel(minLevelOperationPtr->level);
//			printf("new inputLevel%ld\n", state.getMaximumInputLevel());
//		}
//
//		if(state.getMinimumLevel() == NOLEVEL)
//		{
//			state.setMinimumLevel(minLevel);
//		}
//
//		/*
//		 * The conditions to add in toDraw or toSchedule
//		 */
//		if(state.getMaximumInputLevel() == NOLEVEL || minLevel <= state.getMaximumInputLevel())
//		{
//			if(!isInjection || wasScheduled)
//			{
//				//can be drawn
//				state.toDraw.push_back(minLevelOperationPtr);
//
//				//advance on the wires having the operation with min level
//				for(std::set<wireelement*>::iterator it = state.minLevelLinesToCheck.begin();
//						it != state.minLevelLinesToCheck.end(); it++)
//				{
//					if(state.currentGateProWire[*it] == minLevelOperationPtr)
//					{
//						state.currentGateProWire[*it] = getSuccessorOnSameWire(state.currentGateProWire[*it], *it);
//					}
//				}
//			}
//			else
//			{
//				//cannot be drawn and needs scheduling
//				int boxType = (minLevelOperationPtr->type == AA ? ATYPE : YTYPE);
//				state.toScheduleInputs[boxType].push_back(minLevelOperationPtr);
//			}
//		}
//
//		/*
//		 * From here everything is on a higher level than the
//		 * inputs to be scheduled. Skip this wire in further searches.
//		 */
//		if(state.getMaximumInputLevel() > NOLEVEL && minLevel >= state.getMaximumInputLevel())
//		{
//			state.removeFromLinesToCheck(minLevelPos);
//		}
//	}

	if(state.isCircuitFinished())
	{
		state.setRequiredMaximumInputLevel(state.toDraw.front()->level, "stepwisebfs");
	}

	return state.isCircuitFinished();
}

bool causalgraph::retCompareOnLevel (recyclegate* i, recyclegate* j)
{
	bool ret = (i->level < j->level);
	if(i->level == j->level)
	{
		ret = (i->getId() < j->getId());
	}
	return ret;
}

/**
 * Very similar to stepwiseBfs. Too similar.
 * @return A sorted list of the gates based on their level
 */
std::vector<recyclegate*> causalgraph::bfs()
{
//	std::vector<recyclegate*> ret;

	//copy
	std::vector<recyclegate*> ret(tmpCircuit.begin(), tmpCircuit.end());

	std::sort(ret.begin(), ret.end(), causalgraph::retCompareOnLevel);

//	printf("ORDERED BFS: ");
//	for(std::vector<recyclegate*>::iterator it = ret.begin(); it != ret.end(); it++)
//	{
//		printf("%ld ", (*it)->getId());
//	}
//	printf("\n");

//
//	std::vector<recyclegate*> inputs = getRoots();
//
//	//this is a similar trick to lastSeen
//	std::map<wireelement*, recyclegate*> currentIdProWire;//(inputs.size(), NULL);
//
//	for(size_t i=0; i<inputs.size(); i++)
//	{
//		//take the id of the input operations
//		currentIdProWire[inputs[i]->wirePointers[0]] = inputs[i];
//	}
//
//	std::map<recyclegate*, int> visited;
//
//	while(true)
//	{
//		//advance the minimum level
//		//get minimum level of current ids
//		int min = INT_MAX;
//
//		wireelement* minpos = NULL;
//		for(size_t i=0; i<inputs.size(); i++)
//		{
//			wireelement* wireEl = inputs[i]->wirePointers[0];
//
//			if(currentIdProWire.find(wireEl) != currentIdProWire.end()
//					&& min > currentIdProWire[wireEl]->level)
//			{
//				min = currentIdProWire[wireEl]->level;
//				minpos = wireEl;
//			}
//		}
//
//		//there is no minimum - why? bye bye
//		if(min == INT_MAX)
//		{
//			break;
//		}
//
//		printf("minlevel %d %d\n", min, minpos);
//
//		//take the operation having the min level from minpos
//		recyclegate* oldid = currentIdProWire[minpos];
//
//		//put it in the return list
//		ret.push_back(oldid);
//
//		//advance on the wires having the operation with min level
//		for(size_t i=0; i<inputs.size(); i++)
//		{
//			wireelement* wireEl = inputs[i]->wirePointers[0];
//			if(currentIdProWire.find(wireEl) != currentIdProWire.end() /*exists*/
//					&& currentIdProWire[wireEl] == oldid)/*compare pointers*/
//			{
//				recyclegate* next = getSuccessorOnSameWire(currentIdProWire[wireEl], wireEl);
//
////				if(visited.find(next) != visited.end())
////				{
////					if(visited[next] == 2)
////						printf("mai fost odata %d\n", next);
////				}
////
////				visited[next]++;
//
//				if(next != NULL)
//				{
//					currentIdProWire[wireEl] = next;
//				}
//				else
//				{
//					currentIdProWire.erase(wireEl);
//				}
//
//			}
//		}
//	}
//

//	for(std::vector<recyclegate*>::iterator it = ret.begin(); it != ret.end(); it++)
//	{
//		printf("li %d \n", (*it)->level);
//	}

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

	std::vector<int> wiresIndices;
	/*
	 * Process initialisations
	 */
	for(size_t i = 0; i < ins.size(); i++)
	{
		if(ins[i] != '_')
		{
			wiresIndices.clear();
			wiresIndices.push_back(i);
//			std::list<recyclegate*>::iterator newIter =
//				constructInputOutput2(true, ins[i],
//						wires,
//						wiresIndices,
//						originalGate);

			std::list<recyclegate*>::iterator newIter =
					constructRecycleGate2(wires, wiresIndices, originalGate);
			configureInputOutput2(true, ins[i],	newIter);

			/*
			recyclegate* nInitialisation = new recyclegate();
			nInitialisation->updateTypeAndCost(true, ins[i], costModel);
			nInitialisation->updateCausalType();

			nInitialisation->wirePointers.push_back(wires[i]);//updated wire reference in new gate

			memTmpCircuit.push_back(nInitialisation);
			tmpCircuit.insert(originalGate, nInitialisation);
			*/

			(*newIter)->orderNrInGateList = originalNr;//is this ok?
			ret.push_back(*newIter);
		}
	}

	/*
	 * Process measurements: backwards
	 */
	for(size_t i = outs.size(); i > 0; i--)
	{
		if(outs[i-1] != '_')
		{
			wiresIndices.clear();
			wiresIndices.push_back(i - 1);

			std::list<recyclegate*>::iterator newIter =
						constructRecycleGate2(wires, wiresIndices, originalGate);

			configureInputOutput2(false, outs[i - 1], newIter);

			/*
			recyclegate* nMeasurement = new recyclegate();
			nMeasurement->updateTypeAndCost(false, outs[i - 1], costModel);
			nMeasurement->updateCausalType();

			nMeasurement->wirePointers.push_back(wires[i - 1]);//updated wire reference in new gate

			memTmpCircuit.push_back(nMeasurement);
			originalGate = tmpCircuit.insert(originalGate, nMeasurement);
			*/

			originalGate = newIter;

			(*newIter)->orderNrInGateList = originalNr;//is this ok?
			ret.push_back(*newIter);
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
		constructWire2(wires);

		/*
		wireelement* additionalWire = new wireelement();

		//memory management
		memWireElements.push_back(additionalWire);

		//add to list
		wires.push_back(additionalWire);

		//connect with current
		current->linkForward(additionalWire);

		current = additionalWire;
		*/
	}

	//update current to be the last inserted wire
	current = wires.back();

	current->linkForward(lastWire);
	wires.push_back(lastWire);

	return wires;
}

void causalgraph::insertGates2(std::vector<std::string>& gateList,
		std::vector<wireelement*>& wires,
		std::list<recyclegate*>::iterator& originalGate)
{
	for(size_t i = 0; i < gateList.size(); i++)
	{
		/*recyclegate* gate = new recyclegate(gateList[i], wires);
		gate->updateCausalType();

		tmpCircuit.insert(originalGate, gate);
		memTmpCircuit.push_back(gate);*/

		std::list<recyclegate*>::iterator newIterator =
				constructGate2(gateList[i], wires, originalGate);

		(*newIterator)->orderNrInGateList = (*originalGate)->orderNrInGateList;
	}
}

void causalgraph::setLevelIterative()
{
	long nrLevel = 0;
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin(); it != tmpCircuit.end(); it++)
	{
		if((*it)->type != 'i' && (*it)->type != 'o' )
			nrLevel += 2;

		if((*it)->type == 't' && nrLevel < 6)
		{
			nrLevel = 6;
		}

		(*it)->level = nrLevel;
	}
}

std::vector<recyclegate*> causalgraph::numberGateList2()
{
	std::vector<recyclegate*> ret;
	long nrId = 0;
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin(); it != tmpCircuit.end(); it++)
	{
		(*it)->orderNrInGateList = nrId;
		ret.push_back(*it);

		nrId++;

		if((*it)->type == EMPTY)
		{
			printf("THERE IS A GATE with type EMPTY!!!!!!\n");
		}


	}

	return ret;
}

wireelement* causalgraph::getFirstWireElement()
{
	//get first wire
	recyclegate* firstgate = *tmpCircuit.begin();
	wireelement* first = firstgate->wirePointers[0]->getUpdatedWire(
			firstgate->orderNrInGateList);
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


/**
 * Creates a new wire and places it in the tmpWireIndices vector
 * @param tmpWireIndices list to update with pointers to wires
 * @return number of the wire
 */
int causalgraph::constructWire2(std::vector<wireelement*>& tmpWireIndices)
{
	/*construieste elementele de fir*/
	wireelement* wireEl = new wireelement();
	int nrWire = tmpWireIndices.size();

	tmpWireIndices.push_back(wireEl);
	if (nrWire > 0)
	{
		tmpWireIndices[nrWire - 1]->linkForward(tmpWireIndices[nrWire]);
	}
	memWireElements.push_back(wireEl);

	return nrWire + 1;
}

/**
 * Constructs a gate which operates on the wires indexed by indices. The wireelement
 * pointers are taken based on the indices from tmpWires. The gate is placed at the
 * position indicated by iterator where
 * @param tmpWires
 * @param indices
 * @param where
 * @return an iterator to the newly placed gate
 */
std::list<recyclegate*>::iterator causalgraph::constructRecycleGate2(std::vector<wireelement*>& tmpWires,
		std::vector<int>& indices,
		std::list<recyclegate*>::iterator where)
{
	recyclegate* recg = new recyclegate();

	for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); it++)
	{
		recg->wirePointers.push_back(tmpWires[*it]);
	}

	where = tmpCircuit.insert(where, recg);
	memTmpCircuit.push_back(recg);

	return where;
}

//TODO: this and constructGate2 are very similar
std::list<recyclegate*>::iterator causalgraph::configureInputOutput2(bool isInput, char ctype,
		std::list<recyclegate*>::iterator where)
{
	if(ctype != '*')
		(*where)->updateType(isInput, ctype);

	(*where)->updateCost(isInput, costModel);
	(*where)->updateCausalType();

	return where;
}

std::list<recyclegate*>::iterator causalgraph::constructGate2(std::string& line,
		std::vector<wireelement*>& tmpWires,
		std::list<recyclegate*>::iterator where)
{
	std::vector<int> indices;
	char type;

	recyclegate::fromString(line, type, indices);

	where = constructRecycleGate2(tmpWires,
			indices,
			where);

	(*where)->type = type;
	(*where)->updateCausalType();

	/**
	 * 3 NOV
	 */
//	if(type == 't')
//	{
//		wireelement* frontw = memWireElements.front();
//		if((*where)->wirePointers[0] != frontw)
//		{
//			(*where)->wirePointers.push_back(frontw);
//		}
//	}

//	/**
//	 * 3 NOV
//	 */
	if(type=='s')//there is no S gate
		(*where)->gateCost = 0; //is this a cnot?
	else
		(*where)->gateCost = 1;

	return where;
}

void causalgraph::constructFrom2(std::vector<std::string>& file)
{
	std::vector<wireelement*> tmpWires;

	std::istringstream inin(file[0]);
	std::string s;
	inin >> s;
	inin >> s;

	std::vector<int> wireIndices;

	for(size_t i = 0; i < s.size(); i++)
	{
		/*construieste elementele de fir*/
		int nrWire = constructWire2(tmpWires);
		wireIndices.clear();
		wireIndices.push_back(nrWire - 1);

		std::list<recyclegate*>::iterator newIter =
				constructRecycleGate2(tmpWires, wireIndices, tmpCircuit.end());

		configureInputOutput2(true, s[i], newIter);
		//set a type 27.10.2017
		(*newIter)->type = 'i'; //input
	}

	/*
	 * Update First Wire
	 */
	for(size_t i=2; i<file.size(); i++)
	{
		constructGate2(file[i], tmpWires, tmpCircuit.end());
	}

	std::istringstream outout(file[1]);
	outout >> s;
	outout >> s;

	for(size_t i = 0; i < s.size(); i++)
	{
		int nrWire = i;
		wireIndices.clear();
		wireIndices.push_back(nrWire);

		std::list<recyclegate*>::iterator newIter =
				constructRecycleGate2(tmpWires, wireIndices, tmpCircuit.end());

		configureInputOutput2(false, s[i], newIter);
		//set a type 27.10.2017
		(*newIter)->type = 'o';//input
	}
}

//recyclegate* causalgraph::getSuccessorOnSameWire(recyclegate* current, wireelement* wire)
//{
////	int lmin = INT_MAX;
//	recyclegate* ret = NULL;
////
////	for(std::map<int, recyclegate*>::iterator it = current->willPush.begin();
////			it != current->willPush.end(); it++)
////	{
////		recyclegate* operationId = it->second;
////
////		for(std::vector<wireelement*>::iterator wit = operationId->wiresToUseForLinks.begin();
////				wit != operationId->wiresToUseForLinks.end(); wit++)
////		{
////			if(*wit == wire /*pointer compare*/ && lmin > operationId->level)
////			{
////				lmin = operationId->level;
////				ret = operationId;
////			}
////		}
////	}
//
//	//this is how it should maybe work. for the moment keep the map complication
////	return circuit[currid]->willPush[wire];
//	std::vector<wireelement*>::iterator it =
//			std::find(current->wirePointers.begin(), current->wirePointers.end(), wire);
//	if(it != current->wirePointers.end())
//	{
//		int pos = std::distance(current->wirePointers.begin(), it);
//		ret = current->willPush[pos];
//	}
//
//	//NULL should happen only for outputs
//	return ret;
//}

long causalgraph::getAndSetMaxPrevLevel(recyclegate* current, bool useGateCost, int timeDistanceBetweenOperations)
{
	//take the maximum level from the neighbouring nodes that are "pushing"
	long maxPrevLevel = LONG_MIN;

	for (std::vector<recyclegate*>::iterator it = current->pushedBy.begin();
					it != current->pushedBy.end(); it++)
	{
		recyclegate* operationPtr = *it;

		int operationTimeCost = 0;
		if(useGateCost == true)
		{
			operationTimeCost = operationPtr->gateCost;
		}

		//daca vecinul are nivelul -1. fix pula
		if(operationPtr->level == -1)
		{
			long maxLevelUntilNow = getAndSetMaxPrevLevel(operationPtr, useGateCost, timeDistanceBetweenOperations);

			//un mini setter ...
			long increaseLevelValue = operationTimeCost + timeDistanceBetweenOperations;
			operationPtr->level = maxLevelUntilNow + increaseLevelValue;

			if(operationPtr->causalType == RECGATE)
			{
				//if one of the precedent nodes is gate, then time + 1, because for cnots the length is dual
				long enforcedTimeDifference = increaseLevelValue + 1;
				long minTimeDifference = enforcedTimeDifference;

				//Imi este foarte neclar ce face bucata asta. A ramas aici de candva?
				for(std::vector<recyclegate*>::iterator pit = operationPtr->pushedBy.begin();
						pit != operationPtr->pushedBy.end();
						pit++)
				{
					recyclegate* pitPtr = *pit;
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
	prevGate->addWillPush(currGate);
	currGate->addPushedBy(prevGate);
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
		for(std::vector<recyclegate*>::iterator it2 = (*it)->willPush.begin();
				it2 != (*it)->willPush.end(); it2++)
		{
			if(min > (*it2)->level)
			{
				min = (*it2)->level;
			}
		}

		if(min != INT_MAX)
		{
			int dist = min - ((*it)->level + 1);

			(*it)->level += dist;
		}
	}
}

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
	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
					it != tmpCircuit.end(); it++)
	{
		if((*it)->level == oldCost)
		{
			(*it)->updateGateCostToAchieveLevel(newCost);
		}
	}
//	for(int i=0; i<2; i++)
//	{
//		for(std::set<recyclegate*>::iterator it = current.toScheduleInputs[i].begin();
//				it!=current.toScheduleInputs[i].end(); it++)
//		{
//			if((*it)->level == oldCost)
//			{
//				(*it)->updateGateCostToAchieveLevel(newCost);
//			}
//		}
//	}
//
//	for(std::vector<recyclegate*>::iterator it = current.toDraw.begin(); it!=current.toDraw.end(); it++)
//	{
//		if((*it)->level == oldCost)
//		{
//			(*it)->updateGateCostToAchieveLevel(newCost);
//		}
//	}
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

//	/**
//	 * 3 NOV - daca urmatoarea poarta pe fire este cnot, atunci fa gatecost 0
//	 * daca nu, atunci fa 1
//	 */
//	for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
//					it != tmpCircuit.end(); it++)
//	{
//		bool nextIsCnot = true;
//		for(std::vector<wireelement*>::iterator itw = (*it)->wiresToUseForLinks.begin();
//				itw != (*it)->wiresToUseForLinks.end(); itw++)
//		{
//			(*it)->willPush[]
//		}
//	}

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
		for(std::vector<recyclegate*>::iterator it = current->willPush.begin();
				it !=current->willPush.end(); it++)
		{
			resetLevelsStartingFrom(*it);
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

//    //take the outputs
//    for(std::list<recyclegate*>::iterator it = tmpCircuit.begin();
//    		it != tmpCircuit.end(); it++)
//    {
//    	printf("%d %d lev\n", (*it)->level, (*it)->type);
//    }
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
	printf("Causal Graph Destrucctor\n");
	tmpCircuit.clear();

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
}

