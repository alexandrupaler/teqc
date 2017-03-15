#ifndef HEURISTICPARAMETERS_H_
#define HEURISTICPARAMETERS_H_

#include <stdlib.h>

struct heuristicparameters
{
public:
	/**
	 * Number of connections to be used by a fresh initialised connection pool
	 */
	int connPoolStart;

	/**
	 * Maximum number of connections in a pool
	 */
	int maxConnections;

	/**
	 * Number of time steps allowed for the connections to be made between boxes and connection pool
	 */
	int connectionBufferTimeLength;

	/**
	 * Number of deltas from circuit to where the connection pool is to be placed on the height axis
	 */
	int connectionHeight;

	/**
	 * Multiplication factor used to calculate the number of A* steps for
	 * reaching a solution using the Manhattan distance between the points to connect
	 */
	int manhattanMultiplier;

	/**
	 * How soon the pool connections should end before the first input to schedule
	 */
	int timeBeforePoolEnd;/*4*/

	/**
	 * The maximum length of the journal stored in an astarpoint
	 */
	size_t pointJournalLength;

	/**
	 * Connectionbox total height
	 */
	long connectionBoxHeight;

	/**
	 * Connectionbox total height
	 */
	long connectionBoxWidthMargin;

	void train();
	void init();
};


#endif /* HEURISTICPARAMETERS_H_ */
