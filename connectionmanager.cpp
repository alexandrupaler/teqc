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
	//19.05.2017
//	size_t connNr = pool.reserveConnection(boxType);

	/*
	 * NEEDED TO SIM ASAP
	 * 21.02.2017
	 * considering that connection number zero is at width coordinate zero
	 */
	long cw = boxPins.getPinDetail(SOURCEPIN).coord[CIRCUITWIDTH];
	if(cw < 0)
	{
		cw = 0;
	}
	size_t approxConnectionNumber = cw / DELTA;
	size_t connNr = pool.reserveConnectionPreferred(boxType, approxConnectionNumber);

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

		printf("3.11.2017: assigned conn %d to %d\n", connNr, operationId);

		return true;
	}

	return false;
}

void connectionManager::removeConnections(int boxType, std::vector<int>& operationIds)
{
	for(size_t i=0; i<operationIds.size(); i++)
	{
		int operationId = operationIds[i];

		int connNr = assignedIdToConnection[operationId];
		assignedIdToConnection.erase(operationId);

		pool.preReleaseConnection(boxType, connNr);
	}
}

void connectionManager::releaseIfNotUsed(long inputTimeCoordinate, long connectionPinHeight, Pathfinder& pathfinder, bool checkInPoints)
{
	for(int boxType=0; boxType<2; boxType++)
	{
		std::set<size_t> toRelease;

		for(std::set<size_t>::iterator it = pool.toBeAvailable[boxType].begin();
				it != pool.toBeAvailable[boxType].end(); it++)
		{
			/*compute connection pin coordinates based on heuristic and depth*/
			int connNr = *it;
			pinpair connPins = getConnectionPinPair(connNr, inputTimeCoordinate, connectionPinHeight);

			printf("712: check conn %d @ coord %d\n", connNr, inputTimeCoordinate);

			bool isReleased = true;
			if(checkInPoints)
			{
				for(int i = 0; i < 2; i++)
				{
					Point* point = pathfinder.getOrCreatePoint(connPins.getPinDetail(i).coord[CIRCUITWIDTH],
							connPins.getPinDetail(i).coord[CIRCUITDEPTH],
							connPins.getPinDetail(i).coord[CIRCUITHEIGHT], false /*no boxes should exist in future*/);

					printf("712:         %s\n", connPins.getPinDetail(i).coord.toString(',').c_str());

					isReleased = isReleased && point->isFree();
				}
			}

			if(isReleased)
			{
				toRelease.insert(connNr);
			}
		}

		for(std::set<size_t>::iterator it = toRelease.begin();
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
	//19.05.2017
//	int blockHeight = 100;
//	int heightPositionParity = (connNr/blockHeight)%2;
//	int heightAdd =  heightPositionParity * blockHeight + (1 - heightPositionParity*2) * connNr%blockHeight;
	int heightAdd = 0;

	height = height + heightAdd;

//	pindetails pin1;
//	pin1.coord[CIRCUITWIDTH] = connNr * DELTA + 1;
//	pin1.coord[CIRCUITDEPTH] =  (time + 0) * DELTA  + 1;
//	pin1.coord[CIRCUITHEIGHT] = height * DELTA + 1;
//
//	pindetails pin2;
//	pin2.coord[CIRCUITWIDTH] = connNr * DELTA  + 1;
//	pin2.coord[CIRCUITDEPTH] =  (time + 1) * DELTA  + 1;
//	pin2.coord[CIRCUITHEIGHT] = height * DELTA + DELTA + 1;


/**
 * 3 NOV RESOURCE Estimator
 * Dual coords
 */
//	pindetails pin1;
//	pin1.coord[CIRCUITWIDTH] = 2*connNr * DELTA + 1;
//	pin1.coord[CIRCUITDEPTH] =  (time + 0) * DELTA  + 1;
//	pin1.coord[CIRCUITHEIGHT] = height * DELTA + 1;
//
//	pindetails pin2;
//	pin2.coord[CIRCUITWIDTH] = (2*connNr +  1)* DELTA  + 1;
//	pin2.coord[CIRCUITDEPTH] =  (time + 0) * DELTA  + 1;
//	pin2.coord[CIRCUITHEIGHT] = height * DELTA + 1;

	pindetails pin1;
	pin1.coord[CIRCUITWIDTH] = 2*connNr * DELTA + 4;
	pin1.coord[CIRCUITDEPTH] =  (time + 0) * DELTA  + 4;
	pin1.coord[CIRCUITHEIGHT] = height * DELTA + 4;

	pindetails pin2;
	pin2.coord[CIRCUITWIDTH] = (2*connNr +  1) * DELTA + 4;
	pin2.coord[CIRCUITDEPTH] =  (time + 0) * DELTA  + 4;
	pin2.coord[CIRCUITHEIGHT] = height * DELTA + 4;

	pinpair newPins;
	newPins.setPinDetail(0, pin1);
	newPins.setPinDetail(1, pin2);

	return newPins;
}

void connectionManager::updateConnections(int typeOfConnection, long timeWhenPoolEnds, long heightIncrement, std::vector<pinpair>& toconn)
{
	std::set<size_t> usedConnNrs;
	switch(typeOfConnection)
	{
	case EXTENDCONNECTION:
		usedConnNrs = pool.getAssigned();
		break;
	case BOXCONNECTION:
		usedConnNrs = pool.getReservedButNotAssigned();
		break;
	case CIRCUITCONNECTION:
		break;
	}

	long timeCoordinate = timeWhenPoolEnds;
//	long heightIncrement = heuristic->connectionHeight;

	for(std::set<size_t>::iterator it = usedConnNrs.begin();
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

		//TODO: a second parallel layer of connection pool
//		//22.05.2017
//		if(oldPins.getPinDetail(SOURCEPIN).coord[CIRCUITHEIGHT] < 0)
//		{
//			newPins.getPinDetail(DESTPIN).coord[CIRCUITHEIGHT] -= 60 * DELTA;
//			newPins.getPinDetail(SOURCEPIN).coord[CIRCUITHEIGHT] -= 60 * DELTA;
//		}

		/*
		 * form pairs
		 */
		std::vector<pinpair> pairs = formPairs(oldPins, newPins);

		if(typeOfConnection == BOXCONNECTION
				&& oldPins.getPinDetail(0).coord[CIRCUITWIDTH] < newPins.getPinDetail(0).coord[CIRCUITWIDTH])
		{
			/*
			 * 3 NOV
			 * the connection pool rail has a circuitwidth coordinate higher than the box
			 * and the connections will intersect
			 * swap the pins
			 */
			pindetails dettmp = pairs[0].getPinDetail(0);

			pairs[0].setPinDetail(0, pairs[1].getPinDetail(0));
			pairs[1].setPinDetail(0, dettmp);
		}

		//22.05.2017
		//do insertion sort?
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
		std::vector<pinpair>::iterator iti = toconn.begin();// + startInsPos;
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

		printf("update connection connect %d %s %s\n", connNr,
				pairs[0].getPinDetail(DESTPIN).coord.toString(',').c_str(),
				pairs[1].getPinDetail(DESTPIN).coord.toString(',').c_str());
	}
}
bool connectionManager::finaliseAssignedConnections(bfsState& state, std::vector<pinpair>& circPins,
		std::vector<pinpair>& constructedListOfConnections)
{
	std::vector<int> toRemOperationIds[2];

	for(std::map<int, int>::iterator it = state.operationIdToCircuitPinIndex.begin();
			it != state.operationIdToCircuitPinIndex.end(); it++)
	{
		int operationId = it->first;
		int circPinIndex = it->second;

		if(assignedIdToConnection.find(operationId) == assignedIdToConnection.end())
		{
			//for some buggy reason, an operation was not assigned a connection
//			toRemOperationIds.clear();

			printf("   fAILED %d\n", operationId);

			return false;
		}

		//get connection of operationid
		int connNr = assignedIdToConnection[operationId];

		printf("PAIR id%d circ%d conn%d\n", operationId, circPinIndex, connNr);

		pinpair dist = connectionPins[connNr];
		pinpair circ = circPins[circPinIndex];

		printf("712 circ coord %d\n", circ.getPinDetail(0).coord[CIRCUITDEPTH]);

		int boxType = circ.getType();

		std::vector<pinpair> pairs = formPairs(dist, circ);

		//01.11.2017
		pairs[0].allowConnectionThroughChannel = true;
		pairs[1].allowConnectionThroughChannel = true;

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

std::vector<pinpair> connectionManager::formPairs(pinpair& source, pinpair& destination)
{
	std::vector<pinpair> ret;

//	pinpair pair1;
//	pair1.setPinDetail(SOURCEPIN, source.getPinDetail(0));
//	pair1.setPinDetail(DESTPIN, destination.getPinDetail(0));
//
//	pinpair pair2;
//	pair2.setPinDetail(SOURCEPIN, source.getPinDetail(1));
//	pair2.setPinDetail(DESTPIN, destination.getPinDetail(1));

	/*
	 * 3 NOV RESOURCE ESTIM
	 */
	pinpair pair1;
	pair1.setPinDetail(SOURCEPIN, source.getPinDetail(1));
	pair1.setPinDetail(DESTPIN, destination.getPinDetail(1));

	pinpair pair2;
	pair2.setPinDetail(SOURCEPIN, source.getPinDetail(0));
	pair2.setPinDetail(DESTPIN, destination.getPinDetail(0));

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
