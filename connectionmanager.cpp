#include <stdlib.h>
#include "connectionmanager.h"
#include "fileformats/generaldefines.h"
#include "convertcoordinate.h"

/**
 * Constructor
 * @param param the heuristic settings
 */
connectionManager::connectionManager(heuristicparameters* heur)
{
	heuristic = heur;

	for(int i = 0; i < heuristic->connPoolStart; i++)
	{
		//pool.available.push_back(i);
		pool.increaseAvailable(0 /*ATYPE*/, heuristic->connPoolStart);
		pool.increaseAvailable(1 /*YTYPE*/, heuristic->connPoolStart);
	}
}

/**
 * Add a specific connection to the manager
 * @param boxType type of connection
 * @param boxPins a pair of pins representing the connection
 * @return the number of the connection
 */
int connectionManager::addConnection(/*is this still used?*/int boxType2, pinpair boxPins)
{
	int boxType = boxPins.getType();

	if(!pool.sufficientAvailable(boxType))
	{
		if(!pool.increaseAvailable(boxType, heuristic->maxConnections))
		{
			return -1;
		}
	}

	size_t connNr = pool.reserveConnection(boxType);

	/*
	 * NEEDED TO SIM ASAP
	 * 21.02.2017
	 * considering that connection number zero is at width coordinate zero
	 */
//	long cw = boxPins.getPinDetail(SOURCEPIN).coord[CIRCUITWIDTH];
//	if(cw < 0)
//		cw = 0;
//	size_t approxConnectionNumber = cw / DELTA;
//	size_t connNr = pool.reserveConnectionPreferred(boxType, approxConnectionNumber);

	/*
	 * Increase the length of the vector managing the connection pins
	 * if the connection number returned is higher than it size
	 */
	if(connectionPins.size() <= connNr)
	{
		connectionPins.resize(connNr + 1);
	}
	connectionPins[connNr] = boxPins;

	return connNr;
}

bool connectionManager::assignOperationToConnection(int operationId, int boxType)
{
	if(pool.sufficientToConsume(boxType))
	{
		int connNr = pool.consumeConnection(boxType);
		assignedIdToConnection[operationId] = connNr;

		return true;
	}

	return false;
}

void connectionManager::removeConnections(int boxType, vector<int>& operationIds)
{
	for(size_t i=0; i<operationIds.size(); i++)
	{
		int operationId = operationIds[i];

		int connNr = assignedIdToConnection[operationId];
		assignedIdToConnection.erase(operationId);

		pool.preReleaseConnection(boxType, connNr);
	}
}

void connectionManager::releaseIfNotUsed(long inputTimeCoordinate, long connectionPinHeight, Pathfinder& pathfinder)
{
	for(int boxType=0; boxType<2; boxType++)
	{
		set<size_t> toRelease;

		for(set<size_t>::iterator it = pool.toBeAvailable[boxType].begin();
				it != pool.toBeAvailable[boxType].end(); it++)
		{
			/*compute connection pin coordinates based on heuristic and depth*/
			int connNr = *it;
			pinpair connPins = getConnectionPinPair(connNr, inputTimeCoordinate, connectionPinHeight);

			bool isReleased = true;
			for(int i = 0; i < 2; i++)
			{
				Point* point = pathfinder.getOrCreatePoint(connPins.getPinDetail(i).coord[CIRCUITWIDTH],
						connPins.getPinDetail(i).coord[CIRCUITDEPTH],
						connPins.getPinDetail(i).coord[CIRCUITHEIGHT], false /*no boxes should exist in future*/);

				isReleased = isReleased && point->isFree();
			}

			if(isReleased)
			{
				toRelease.insert(connNr);
			}
		}

		for(set<size_t>::iterator it = toRelease.begin();
					it != toRelease.end(); it++)
		{
			int connNr = *it;
			pool.releaseConnection(boxType, connNr);
		}
	}
}

pinpair connectionManager::getConnectionPinPair(int connNr, long time, long height)
{
	/*
	 * TODO: A class for coordinate system conversions
	 */
	pindetails pin1;
	pin1.coord[CIRCUITWIDTH] = connNr * DELTA + 1;
	pin1.coord[CIRCUITDEPTH] =  (time + 0) * DELTA  + 1;
	pin1.coord[CIRCUITHEIGHT] = height * DELTA + 1;

	pindetails pin2;
	pin2.coord[CIRCUITWIDTH] = connNr * DELTA  + 1;
	pin2.coord[CIRCUITDEPTH] =  (time + 1) * DELTA  + 1;
	pin2.coord[CIRCUITHEIGHT] = height * DELTA + DELTA + 1;

	pinpair newPins;
	newPins.setPinDetail(0, pin1);
	newPins.setPinDetail(1, pin2);

	return newPins;
}

void connectionManager::updateConnections(long timeWhenPoolEnds, long heightIncrement, vector<pinpair>& toconn)
{
	set<size_t> usedConnNrs = pool.getAllUnavailable();

	long timeCoordinate = timeWhenPoolEnds;
//	long heightIncrement = heuristic->connectionHeight;

	for(set<size_t>::iterator it = usedConnNrs.begin();
			it != usedConnNrs.end(); it++)
	{
		size_t connNr = *it;

		/*
		 * The new coordinates of the connection.
		 */
		pinpair newPins = getConnectionPinPair(connNr, timeCoordinate, heightIncrement);

		/*
		 * Form two pairs between the new pins and the old pins
		 * Save the pairs in the toConn list
		 */
		pinpair oldPins = connectionPins[connNr];

		/*
		 * form pairs
		 */
		vector<pinpair> pairs = formPairs(oldPins, newPins);

		/*
		 * insertion sort depending on distance
		 * use distance between a single pin pair
		 * the other distance should be similar
		 */
		int distIns = pairs[0].minDistBetweenPins();
		/* distIns is allowed to be zero
		 * will be filtered when the connections are computed
		 * in pinConnector (connectpins)
		 */
		vector<pinpair>::iterator iti = toconn.begin();// + startInsPos;
		for( ; iti != toconn.end(); iti++)
		{
			int distCurr = iti->minDistBetweenPins();

			if(distIns < distCurr)
			{
//					printf("%d < %d\n", distIns, distCurr);
				toconn.insert(iti, pairs.begin(), pairs.end());
				break;
			}
		}
		if(iti == toconn.end())
		{
			toconn.insert(iti, pairs.begin(), pairs.end());
		}

		/*
		 * Update the pins of the connection
		 */
		connectionPins[connNr] = newPins;

//		printf("ua connect @ time:%d %d %s %s\n", time, connNr,
//				pairs[0].getPinDetail(DESTPIN).coord.toString(',').c_str(),
//				pairs[1].getPinDetail(DESTPIN).coord.toString(',').c_str());
	}
}
bool connectionManager::finaliseAssignedConnections(bfsState& state, vector<pinpair>& circPins, vector<pinpair>& constructedListOfConnections)
{
	vector<int> toRemOperationIds[2];

	for(map<int,int>::iterator it = state.operationIdToCircuitPinIndex.begin();
			it != state.operationIdToCircuitPinIndex.end(); it++)
	{
		int operationId = it->first;
		int circPinIndex = it->second;

		if(assignedIdToConnection.find(operationId) == assignedIdToConnection.end())
		{
			//for some buggy reason, an operation was not assigned a connection
//			toRemOperationIds.clear();
			return false;
		}

		//get connection of operationid
		int connNr = assignedIdToConnection[operationId];

		printf("PAIR id%d circ%d conn%d\n", operationId, circPinIndex, connNr);

		pinpair dist = connectionPins[connNr];
		pinpair circ = circPins[circPinIndex];

		int boxType = circ.getType();

		vector<pinpair> pairs = formPairs(dist, circ);

		constructedListOfConnections.insert(constructedListOfConnections.end(), pairs.begin(), pairs.end());

		/*
		 * Free the connection
		 */
		//removeConnection(operationId);
		toRemOperationIds[boxType].push_back(operationId);
	}

	//for these operation ids, the connections are finished
	state.operationIdToCircuitPinIndex.clear();

	removeConnections(0, toRemOperationIds[0]);
	toRemOperationIds[0].clear();

	removeConnections(1, toRemOperationIds[1]);
	toRemOperationIds[1].clear();

	return true;
}

vector<pinpair> connectionManager::formPairs(pinpair& source, pinpair& destination)
{
	vector<pinpair> ret;

	pinpair pair1;
	pair1.setPinDetail(SOURCEPIN, source.getPinDetail(0));
	pair1.setPinDetail(DESTPIN, destination.getPinDetail(0));

	pinpair pair2;
	pair2.setPinDetail(SOURCEPIN, source.getPinDetail(1));
	pair2.setPinDetail(DESTPIN, destination.getPinDetail(1));

	ret.push_back(pair1);
	ret.push_back(pair2);

	return ret;
}

int connectionManager::getUnusedReservedNr(int boxType)
{
	return pool.reservedButNotAssigned[boxType].size();
}

size_t connectionManager::getCurrentNumberOfConnections()
{
	/*
	 * Will constantly increase. Never decrease. See how to use with pool together
	 */
	return connectionPins.size();
}

//long connectionManager::getTimeWhenPoolEnds(bfsState& state)
//{
//	long poolend = (state.getRequiredMaximumInputLevel() - heuristic->timeBeforePoolEnd);
////	printf("pool end: %ld\n", poolend);
//
//	return poolend;
//}
