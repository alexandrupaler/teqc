#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <vector>
#include <map>
#include "heuristicparameters.h"
#include "connectionspool.h"
#include "numberandcoordinate.h"
#include "recycling/bfsstate.h"
#include "astar/astaralg.h"

struct connectionManager
{
	connectionsPool pool;

	std::vector<pinpair> connectionPins;

	std::map<int, int> assignedIdToConnection;

	heuristicparameters* heuristic;

	connectionManager(heuristicparameters* heur);

//	bool enoughReserved(int boxType, int necessary);

	int addConnection(int boxType, pinpair boxPins);

	bool assignOperationToConnection(int operationId, int boxType);

	void removeConnections(int boxType, std::vector<int>& operationId);

	void updateConnections(long timeWhenPoolEnds, long height, std::vector<pinpair>& toconn);

	void releaseIfNotUsed(long inputTimeCoordinate, long connectionPinHeight, Pathfinder& pathfinder);

	bool finaliseAssignedConnections(bfsState& state, std::vector<pinpair>& circPins, std::vector<pinpair>& toconn);

	std::vector<pinpair> formPairs(pinpair& source, pinpair& destination);

	int getUnusedReservedNr(int boxType);

	size_t getCurrentNumberOfConnections();

//	long getTimeWhenPoolEnds(bfsState& state);

	pinpair getConnectionPinPair(int connNr, long time, long height);
};


#endif /* CONNECTIONMANAGER_H_ */
