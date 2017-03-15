#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <vector>
#include <map>
#include "heuristicparameters.h"
#include "connectionspool.h"
#include "numberandcoordinate.h"
#include "bfsstate.h"
#include "astar/astaralg.h"

using namespace std;

struct connectionManager
{
	connectionsPool pool;

	vector<pinpair> connectionPins;

	map<int, int> assignedIdToConnection;

	heuristicparameters* heuristic;

	connectionManager(heuristicparameters* heur);

//	bool enoughReserved(int boxType, int necessary);

	int addConnection(int boxType, pinpair boxPins);

	bool assignOperationToConnection(int operationId, int boxType);

	void removeConnections(int boxType, vector<int>& operationId);

	void updateConnections(long timeWhenPoolEnds, long height, vector<pinpair>& toconn);

	void releaseIfNotUsed(long inputTimeCoordinate, long connectionPinHeight, Pathfinder& pathfinder);

	bool finaliseAssignedConnections(bfsState& state, vector<pinpair>& circPins, vector<pinpair>& toconn);

	vector<pinpair> formPairs(pinpair& source, pinpair& destination);

	int getUnusedReservedNr(int boxType);

	size_t getCurrentNumberOfConnections();

//	long getTimeWhenPoolEnds(bfsState& state);

	pinpair getConnectionPinPair(int connNr, long time, long height);
};


#endif /* CONNECTIONMANAGER_H_ */
