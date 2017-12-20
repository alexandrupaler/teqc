#ifndef CAUSAL_GRAPH_H__
#define CAUSAL_GRAPH_H__

#include <vector>
#include <queue>
#include <list>

#include "recycling/recyclegate.h"
#include "recycling/wireelement.h"
#include "recycling/wireorder.h"
#include "recycling/costmodel.h"
#include "recycling/bfsstate.h"

class causalgraph
{
public:
	//make pointers
	//the vector is constructed only after
	//graph operations finished
//	std::vector<recyclegate*> circuit;

	//constructor and operations using list
	std::list<recyclegate*> tmpCircuit;

	std::list<recyclegate*> memTmpCircuit;
	std::list<wireelement*> memWireElements;

	wireelement* firstWire;

////	wireorder* initiallyFirstWireOrder;
//	std::vector<wireorder*> memWireOrder;

	std::vector<int> lastSeen;
//	std::set<int> inAncillae;
//	std::set<int> outAncillae;

	costmodel costModel;

	int getNrQubits();
	long getMaxLevel();

//	void constructFrom(std::list<recyclegate>& gates, std::list<char>& inputs, std::list<char>& outputs, costmodel& model);
	void constructFrom2(std::vector<std::string>& circuit);

	void connectNodes(int prevWire, int currWire, recyclegate* previd, recyclegate* currid);
	void updateLabels();

	recyclegate* getSuccessorOnSameWire(recyclegate* current, wireelement* wire);

    void resetAllLevels();
    void resetLevelsStartingFrom(recyclegate* current);
	void computeLevels();
	void computeLevels(bool useGateCost, int operationDistance);
	void updateLevelsAtValue(bfsState& current, long oldCost, long newCost);

	void equalizeLevels();
	void equalizeOutputLevels();
	void equalizeInputLevels();
	std::vector<recyclegate*> equalizeConsideringCosts();

//	void reachOutputs(/*std::set<recyclegate*>& visited, */std::set<recyclegate*>& outputs, recyclegate* curr);

	//intr-un fel este identica cu functia din circconvert
//	void replaceQubitIndex(std::set<int>& visited, int curr, int oldvalue, int newvalue);

//	void findShortestPath(std::set<int>& visited, std::set<int>& outputs, std::vector<int> path, std::vector<int>& shortest, int stepback, int prev, int curr);

	int moveInputAfterOutput(std::vector<int> shortest, int inputId);

	std::vector<recyclegate*> bfs();
	bool stepwiseBfs(bfsState& state);

	void reconstructConsideringCosts(std::vector<recyclegate*>& order);
	void reconstructWithoutConsideringCosts(std::vector<recyclegate*>& order);

	std::vector<recyclegate*> getRoots();
//	std::vector<int> getAncillaInitialisations();
    void computeLinks(std::vector<recyclegate*> order);

    std::vector<wireelement*> splitWire2(wireelement* wire, std::list<recyclegate*>::iterator& originalGate);
    void joinWires2(wireelement* wireMeasurement, wireelement* wireInitialisation);
    std::vector<recyclegate*> insertMeasurementAndInitialisation2(std::vector<wireelement*>& wires,
    		std::string ins,
    		std::string outs,
    		std::list<recyclegate*>::iterator& originalGate);
    std::vector<wireelement*> insertWires2(std::vector<wireelement*>& wires, int nr);
    void insertGates2(std::vector<std::string>& gateList, std::vector<wireelement*>& wires, std::list<recyclegate*>::iterator& originalGate);
    std::vector<recyclegate*> numberGateList2();
    void printGateList2();
    size_t numberWires2();
//    void orderWires2();

    causalgraph(costmodel& model);
    ~causalgraph();

	wireelement* getFirstWireElement();

private:
	long getAndSetMaxPrevLevel(recyclegate* current, bool useGateCost, int operationDistance);
};

#endif
