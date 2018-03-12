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
	//constructor and operations using list
	std::list<recyclegate*> tmpCircuit;

	//used to store all the pointers which were operated in tmpCircuit. in destructor
	std::list<recyclegate*> memTmpCircuit;

	//pointer to the first wire of a circuit. wires are linked.
	//see getFirstWireElement()
//	wireelement* firstWire;

	//used to store all the pointers of the wires. in destructor.
	std::list<wireelement*> memWireElements;

	std::vector<int> lastSeen;
//	std::set<int> inAncillae;
//	std::set<int> outAncillae;

	costmodel costModel;

	int getNrQubits();
	long getMaxLevel();

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

    /*
     * 3 NOV
     */
    void setLevelIterative();

    causalgraph(costmodel& model);
    ~causalgraph();

	wireelement* getFirstWireElement();

//private:
	long getAndSetMaxPrevLevel(recyclegate* current, bool useGateCost, int operationDistance);

	/**
	 *
	 * @param tmpWireIndices
	 * @return index of new wire in tmpWires. Should be tmpWires.size() - 1;
	 */
	int constructWire2(std::vector<wireelement*>& tmpWireIndices);

	std::list<recyclegate*>::iterator configureInputOutput2(bool isInput, char ctype,
			std::list<recyclegate*>::iterator where);

	std::list<recyclegate*>::iterator constructGate2(std::string& line,
			std::vector<wireelement*>& tmpWires,
			std::list<recyclegate*>::iterator where);

	std::list<recyclegate*>::iterator constructRecycleGate2(std::vector<wireelement*>& tmpWires,
			std::vector<int>& indices,
			std::list<recyclegate*>::iterator where);

	static bool retCompareOnLevel (recyclegate* i, recyclegate* j);
};

#endif
