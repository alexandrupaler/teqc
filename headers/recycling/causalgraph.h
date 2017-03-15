#ifndef CAUSAL_GRAPH_H__
#define CAUSAL_GRAPH_H__

#include <vector>
#include <queue>
#include "circconvert.h"
#include "costmodel.h"
#include "bfsstate.h"

using namespace std;

class causalgraph
{
public:
	vector<recyclegate> circuit;
	vector<int> lastSeen;
	set<int> inAncillae;
	set<int> outAncillae;
	int nrQubits;

	int getNrQubits();
	long getMaxLevel();

	int getTotalAStates();
	int getTotalYStates();
	vector< vector <int> > getTypesAndWiresOfInjections();

	void constructFrom(circconvert& cc, costmodel& model);
	void connectNodes(int previd, int currid);
	void updateLabels();

	int getSuccessorOnSameWire(int currid, int wire);

    void resetAllLevels();
    void resetLevelsStartingFrom(int currId);
	void computeLevels();
	void computeLevels(bool useGateCost, int operationDistance);
	void updateLevelsAtValue(bfsState& current, long oldCost, long newCost);

	void equalizeLevels();
	void equalizeOutputLevels();
	void equalizeInputLevels();
	vector<int> equalizeConsideringCosts();

	void reachOutputs(set<int>& visited, set<int>& outputs, int curr);

	//intr-un fel este identica cu functia din circconvert
	void replaceQubitIndex(set<int>& visited, int curr, int oldvalue, int newvalue);

	void findShortestPath(set<int>& visited, set<int>& outputs, vector<int> path, vector<int>& shortest, int stepback, int prev, int curr);

	int moveInputAfterOutput(vector<int> shortest, int inputId);

	vector<int> bfs();
	bool blockwiseBfs(bfsState& state, long blockTimeLength);
	bool stepwiseBfs(bfsState& state);

	void reconstructConsideringCosts(vector<int> order);
	void reconstructWithoutConsideringCosts(vector<int> order);

	vector<int> getRoots();
	vector<int> getAncillaInitialisations();
    void computeLinks(vector<int> order);

private:
	long getAndSetMaxPrevLevel(int& currid, bool useGateCost, int operationDistance);
};

#endif
