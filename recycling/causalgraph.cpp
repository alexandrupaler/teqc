
#include <climits>
#include <cmath>
#include "gatenumbers.h"
#include "recycling/causalgraph.h"
#include "numberandcoordinate.h"

vector<int> causalgraph::getRoots()
{
	vector<int> ret;
	for(vector<recyclegate>::iterator it = circuit.begin(); it != circuit.end(); it++)
	{
		if(it->isFirstLevelInitialisation() && it->pushedBy.size() == 0)
		{
			ret.push_back(it->id);
		}
	}

	return ret;
}

vector<int> causalgraph::getAncillaInitialisations()
{
	vector<int> ret;
	for(vector<recyclegate>::iterator it = circuit.begin(); it != circuit.end(); it++)
	{
		if(it->isFirstLevelInitialisation())
		{
			ret.push_back(it->id);
		}
	}

	return ret;
}

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
		int 	minLevelPos;
		long 	minLevel;
		state.getMinLevelFromLinesToCheck(circuit, minLevel, minLevelPos);
		int 	minLevelOperationId = state.currentIdProWire[minLevelPos];

		/*
		 * There is no minimum
		 * bye bye - finished the entire circuit
		 */
		if(minLevel == LONG_MAX)
		{
			state.setCircuitFinished(true);
			break;
		}

		bool isInjection = (circuit[minLevelOperationId].type == AA || circuit[minLevelOperationId].type == YY);
		bool wasScheduled = (state.scheduledInputs.find(minLevelOperationId) != state.scheduledInputs.end());

		/*
		 * If this is the first scheduled/unscheduled input, save its level
		 */
		if(isInjection && state.getMaximumInputLevel() == NOLEVEL)
		{
			state.saveMaxLevel(circuit[minLevelOperationId].level);
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
				state.toDraw.push_back(minLevelOperationId);

				//advance on the wires having the operation with min level
				for(set<int>::iterator it = state.minLevelLinesToCheck.begin(); it != state.minLevelLinesToCheck.end(); it++)
				{
					if(state.currentIdProWire[*it] == minLevelOperationId)
					{
						state.currentIdProWire[*it] = getSuccessorOnSameWire(state.currentIdProWire[*it], *it);
					}
				}
			}
			else
			{
				//cannot be drawn and needs scheduling
				int boxType = (circuit[minLevelOperationId].type == AA ? ATYPE : YTYPE);
				state.toScheduleInputs[boxType].push_back(minLevelOperationId);
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
		state.setRequiredMaximumInputLevel(circuit[state.toDraw.front()].level, "stepwisebfs");
	}

	return state.isCircuitFinished();
}

vector<int> causalgraph::bfs()
{
	vector<int> ret;

	vector<int> inputs = getRoots();

	int nrLines = inputs.size();

	vector<int> currentIdProWire(nrLines, -1);

	for(int i=0; i<nrLines; i++)
	{
		//take the id of the input operations
		currentIdProWire[i] = inputs[i];
	}

	while(true)
	{
		//advance the minimum level
		//get minimum level of current ids
		int min = INT_MAX;
		int minpos = -1;
		for(int i=0; i<nrLines; i++)
		{
			if(currentIdProWire[i] != -1 && min > circuit[currentIdProWire[i]].level)
			{
				min = circuit[currentIdProWire[i]].level;
				minpos = i;
			}
		}

		//there is no minimum - why? bye bye
		if(min == INT_MAX)
			break;

		//take the operation having the min level from minpos
		int oldid = currentIdProWire[minpos];

		//put it in the return list
		ret.push_back(oldid);

		//advance on the wires having the operation with min level
		for(int i=0; i<nrLines; i++)
		{
			if(currentIdProWire[i] == oldid)
			{
				currentIdProWire[i] = getSuccessorOnSameWire(currentIdProWire[i], i);
			}
		}
	}

	return ret;
}

void causalgraph::reconstructConsideringCosts(vector<int> order)
{
	for(size_t i=0; i<circuit.size(); i++)
	{
		circuit[i].computeWiresWithCosts();
	}

	computeLinks(order);
}

void causalgraph::reconstructWithoutConsideringCosts(vector<int> order)
{
	for(size_t i=0; i<circuit.size(); i++)
	{
		circuit[i].computeOnlyWires();
	}

	computeLinks(order);
}

void causalgraph::computeLinks(vector<int> operationOrder)
{
	for(size_t i=0; i<circuit.size(); i++)
	{
		circuit[i].willPush.clear();
		circuit[i].pushedBy.clear();
	}

    vector<int> lastSeen;
    int lines = getRoots().size();

    for(int i = 0; i < lines; i++)
        lastSeen.push_back(-1); //the ids of the inputs

    for(vector<int>::iterator it = operationOrder.begin(); it != operationOrder.end(); it++)
    {
        for(vector<int>::iterator itw = circuit[*it].wiresToUseForLinks.begin();
        		itw != circuit[*it].wiresToUseForLinks.end();itw++)
        {
        	if(*itw < lines)
        	{
        		if(lastSeen[*itw] != -1)
        		{
        			connectNodes(lastSeen[*itw], *it);
        		}
        		lastSeen[*itw] = *it;
        	}
        }
    }

}

void causalgraph::constructFrom(circconvert& cc, costmodel& model)
{
	for(list<recyclegate>::iterator it=cc.gates.begin(); it!=cc.gates.end(); it++)
	{
		recyclegate gate;
		gate.initFrom(*it);
		//gate.print();
		circuit.push_back(gate);

		circuit.back().updateCausalType();
		circuit.back().gateCost = 1;//is this a cnot?
	}

	nrQubits = cc.inputs.size();

	//introduce input and output nodes
	int nrWire = 0;
	for(string::iterator it = cc.inputs.begin(); it != cc.inputs.end(); it++)
	{
		recyclegate input;
		input.updateTypeAndCost(true, *it, model);

		input.wires.push_back(nrWire);
		if(gatenumbers::getInstance().isAncillaInput(input.type))
		{
			inAncillae.insert(nrWire);
		}

		/*the first nodes are inputs*/
		circuit.insert(circuit.begin() + nrWire, input)->updateCausalType();

		nrWire++;
	}

	nrWire = 0;
	for(string::iterator it = cc.outputs.begin(); it != cc.outputs.end(); it++)
	{
		recyclegate output;
		output.updateTypeAndCost(false, *it, model);

		output.wires.push_back(nrWire);
		if(gatenumbers::getInstance().isAncillaOutput(output.type))
		{
			outAncillae.insert(cc.outputs.size() + cc.gates.size() + nrWire);
		}

		/*the last nodes are outputs*/
		circuit.insert(circuit.end(), output)->updateCausalType();

		nrWire++;
	}

	//set the id attribute of the gates
	vector<int> order;
	int nrId = 0;
	for(vector<recyclegate>::iterator it = circuit.begin(); it != circuit.end(); it++)
	{
		it->id = nrId;
		order.push_back(nrId);
		nrId++;
	}

    reconstructWithoutConsideringCosts(order);
}

int causalgraph::getTotalAStates()
{
	int ret = 0;
	for(size_t i=0; i<circuit.size(); i++)
	{
		if(circuit[i].type == AA)
		{
			ret++;
		}
	}
	return ret;
}

int causalgraph::getTotalYStates()
{
	int ret = 0;
	for(size_t i=0; i<circuit.size(); i++)
	{
		if(circuit[i].type == YY)
		{
			ret++;
		}
	}
	return ret;
}

vector< vector<int> > causalgraph::getTypesAndWiresOfInjections()
{
	vector< vector<int> > ret;
	for(size_t i=0; i<circuit.size(); i++)
	{
		vector<int> entry;
		if(circuit[i].type == AA || circuit[i].type == YY)
		{
			entry.push_back( circuit[i].type == AA ? ATYPE : YTYPE);

			entry.push_back(circuit[i].wires[0]);

			ret.push_back(entry);
		}
	}
	return ret;
}

int causalgraph::getSuccessorOnSameWire(int currid, int wire)
{
	int lmin = INT_MAX;
	int ret = -1;

	for(set<int>::iterator it = circuit[currid].willPush.begin(); it != circuit[currid].willPush.end(); it++)
	{
		for(vector<int>::iterator wit = circuit[*it].wiresToUseForLinks.begin(); wit != circuit[*it].wiresToUseForLinks.end(); wit++)
		{
			if(*wit == wire && lmin > circuit[*it].level)
			{
				lmin = circuit[*it].level;
				ret = *it;
			}
		}
	}

	//-1 should happen only for outputs
	return ret;
}

long causalgraph::getAndSetMaxPrevLevel(int& currid, bool useGateCost, int operationDistance)
{
	//take the maximum level from the neighbouring nodes that are "pushing"
	long maxPrevLevel = LONG_MIN;

	for (set<int>::iterator it = circuit[currid].pushedBy.begin(); it != circuit[currid].pushedBy.end(); it++)
	{
		int gateTimeLength = 0;
		if(useGateCost == true)
			gateTimeLength = circuit[*it].gateCost;

		//daca vecinul are nivelul -1. fix pula
		if(circuit[*it].level == -1)
		{
			int id = *it;
			int max = getAndSetMaxPrevLevel(id, useGateCost, operationDistance);

			//un mini setter ...
			circuit[*it].level = max + gateTimeLength + operationDistance;

			if(circuit[*it].causalType == RECGATE)
			{
				//if one of the precedent nodes is gate, then time + 1, because for cnots the length is dual
				int noDistBetweenGates = operationDistance + gateTimeLength + 1;

				//Imi este foarte neclar ce face bucata asta. A ramas aici de candva?
				for(set<int>::iterator pit = circuit[*it].pushedBy.begin();
						pit != circuit[*it].pushedBy.end();
						pit++)
				{
					if(circuit[*pit].causalType == RECGATE)
					{
						int timedif = circuit[*it].level - circuit[*pit].level;
						if(timedif < noDistBetweenGates)
						{
							noDistBetweenGates = timedif;
						}
					}
				}
				circuit[*it].level += (operationDistance + gateTimeLength + 1) - noDistBetweenGates;
			}
		}

		if (maxPrevLevel < circuit[*it].level)
		{
			maxPrevLevel = circuit[*it].level;
		}
	}

	return maxPrevLevel;
}

void causalgraph::connectNodes(int previd, int currid)
{
	circuit[previd].addWillPush(currid);
	circuit[currid].addPushedBy(previd);
}

void causalgraph::updateLabels()
{
	for (vector<recyclegate>::iterator it = circuit.begin(); it != circuit.end(); it++)
	{
		it->generateLabel();
	}
}

void causalgraph::equalizeLevels()
{
	for(vector<recyclegate>::reverse_iterator it = circuit.rbegin(); it != circuit.rend(); it++)
	{
		//vezi care este nivelul minim urmator
		int min = INT_MAX;
		for(set<int>::iterator it2 = it->willPush.begin(); it2 != it->willPush.end(); it2++)
		{
			if(min > circuit[*it2].level)
			{
				min = circuit[*it2].level;
			}
		}

		if(min != INT_MAX)
		{
			int dist = min - (it->level + 1);

			it->level += dist;
		}
	}
}

void causalgraph::replaceQubitIndex(set<int>& visited, int curr, int oldvalue, int newvalue)
{
	if(visited.find(curr) != visited.end())
		return;

	visited.insert(curr);
	std::vector<int>::iterator it;
	it = find (circuit[curr].wires.begin(), circuit[curr].wires.end(), oldvalue);
	if(it != circuit[curr].wires.end())
	{
		*it = newvalue;
	}


	for(set<int>::iterator it=circuit[curr].willPush.begin(); it!=circuit[curr].willPush.end(); it++)
	{
		replaceQubitIndex(visited, *it, oldvalue, newvalue);
	}
}

void causalgraph::findShortestPath(set<int>& visited, set<int>& outputs, vector<int> path, vector<int>& shortest, int stepback, int prev, int curr)
{
	if(visited.find(curr) != visited.end())
	{
		return;
	}

	if(visited.empty())
	{
		set<int> vis2;
		//set<int> outputs;
		reachOutputs(vis2, outputs, curr);
		//pentru a evita cicluri cu outputuri
		visited.insert(outputs.begin(), outputs.end());
	}

	visited.insert(curr);

	//do not consider nodes where output and input were joined
	if(circuit[curr].isConnection())
		//path.insert(curr);
		path.push_back(curr);

	//check solution
	//mai bine ar fi pe linia cea mai apropiata
	//e inutila conditia cu stepback?
	//if(stepback>0 &&  gatenumbers::getInstance()->isAncillaOutput(circuit[curr].type) /*circuit[curr].type == -2*/ && outputs.find(curr) == outputs.end())//todo
	if(stepback>0 && circuit[curr].isAncillaMeasurement() && outputs.find(curr) == outputs.end())//todo
	{
		/*METODA CU STEPBACK*/
		if (shortest.size() == 0 || (path.size() < shortest.size()-1 || stepback< -shortest.back() ))
		{
			shortest.clear();
			shortest.insert(shortest.begin(), path.begin(), path.end());

			//shortest.push_back(-distzero);

			/*METODA CU STEPBACK*/
			//last element is number of stepbacks - not an id
			shortest.push_back(-stepback);
			//shortest.insert(curr);

			/*METODA CU DISTWIRE*/
			//shortest.push_back(-distwire);

			//printf("# %d %d\n", path.size(), distwire);
		}
	}

	//BACKWARD
	for(set<int>::iterator it=circuit[curr].pushedBy.begin(); it!=circuit[curr].pushedBy.end(); it++)
	{
		if(*it != prev)
		{
			//fa pas inapoi
			//findShortestPath(visited, shortest, true, curr, *it);
			int counts = 1;
			//if(circuit[*it].type == -3 || circuit[*it].type == -4)
			//if(gatenumbers::getInstance()->isConnectionElement(circuit[*it].type))
			if(circuit[*it].isConnection())
				counts = 0;
			findShortestPath(visited, outputs, path, shortest, stepback + counts, curr, *it);
		}
	}

	//FORWARD
	for(set<int>::iterator it=circuit[curr].willPush.begin(); it!=circuit[curr].willPush.end(); it++)
	{
		//fa pas inapoi
		findShortestPath(visited, outputs, path, shortest, stepback, curr, *it);
	}
}

int causalgraph::moveInputAfterOutput(vector<int> shortest, int inputId)
{
	int outputId = -1;

	//o cautare iditoata - ar trebui sa fie ultimul element
	for (vector<int>::iterator it = shortest.begin(); it != shortest.end(); it++)
	{
		if (circuit[*it].isAncillaMeasurement())
		{
			outputId = *it;

			circuit[outputId].causalType = RECMEASCONN;//temp value, todo
			circuit[inputId].causalType = RECINITCONN;

			connectNodes(outputId, inputId);

			set<int> vis;

			break;
		}
	}

	return outputId;
}

void causalgraph::computeLevels()
{
	resetAllLevels();
	computeLevels(false, 1);
}

void causalgraph::resetAllLevels()
{
    for(vector<recyclegate>::iterator it = circuit.begin(); it != circuit.end(); it++){
        //reset level
        if(it->pushedBy.size() == 0){
            it->level = it->gateCost;
        }else{
            //reset
            it->level = -1;
        }
    }
}

void causalgraph::equalizeOutputLevels()
{
	long maxlevel = getMaxLevel();
	for(vector<recyclegate>::iterator it = circuit.begin();it != circuit.end();it++)
	{
		if(it->willPush.size() == 0)
		{
			it->level = maxlevel;
		}
	}
}

void causalgraph::equalizeInputLevels()
{
	for(vector<recyclegate>::iterator it = circuit.begin();it != circuit.end();it++)
	{
		if(it->pushedBy.size() == 0)
		{
			it->level = 0;
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
		for(vector<int>::iterator it = current.toScheduleInputs[i].begin(); it!=current.toScheduleInputs[i].end(); it++)
		{
			if(circuit[*it].level == oldCost)
			{
				circuit[*it].updateGateCostToAchieveLevel(newCost);
			}
		}
	}

	for(vector<int>::iterator it = current.toDraw.begin(); it!=current.toDraw.end(); it++)
	{
		if(circuit[*it].level == oldCost)
		{
			circuit[*it].updateGateCostToAchieveLevel(newCost);
		}
	}
}

vector<int> causalgraph::equalizeConsideringCosts()
{
	computeLevels();

	vector<int> order;

	for(size_t i=0; i<circuit.size(); i++)
		circuit[i].computeOnlyWires();

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

void causalgraph::resetLevelsStartingFrom(int currId)
{
	if(circuit[currId].level != -1)
	{
		for(set<int>::iterator it = circuit[currId].willPush.begin(); it != circuit[currId].willPush.end(); it++)
		{
			resetLevelsStartingFrom(*it);
		}
		circuit[currId].level = -1;
	}
}


void causalgraph::computeLevels(bool useGateCost, int operationDistance)
{
    //take the outputs
    for(vector<recyclegate>::iterator it = circuit.begin(); it != circuit.end(); it++)
    {
        //reset level
        if(it->willPush.size() == 0)
        {
			long max = getAndSetMaxPrevLevel(it->id, useGateCost, operationDistance);
			it->level = max + operationDistance;
		}
	}
}

void causalgraph::reachOutputs(set<int>& visited, set<int>& outputs, int curr)
{
	if(visited.find(curr) != visited.end())
		return;

	visited.insert(curr);

	if(circuit[curr].isAncillaMeasurement())
	{
		outputs.insert(curr);
	}

	for(set<int>::iterator it = circuit[curr].willPush.begin(); it != circuit[curr].willPush.end(); it++)
	{
		reachOutputs(visited, outputs, *it);
	}
}

long causalgraph::getMaxLevel()
{
	int max = LONG_MIN;
	for(size_t i=0; i<circuit.size(); i++)
		if(max < circuit[i].level)
			max = circuit[i].level;
	return max;
}

int causalgraph::getNrQubits()
{
	return nrQubits;
}

