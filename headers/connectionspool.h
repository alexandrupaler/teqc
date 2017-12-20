#ifndef CONNECTIONSPOOL_H_
#define CONNECTIONSPOOL_H_

#include <vector>
#include <set>

struct connectionsPool
{
	/*
	 * List of available connections
	 */
	std::vector<size_t> available;

	/*
	 * Lists of connections assigned to boxes (of both types),
	 * but not connected to the circuit
	 */
	std::vector<size_t> reservedButNotAssigned[2];

	/*
	 * Set of connections connected to the circuit
	 * The connections need to be computed by A*
	 */
	std::set<size_t> assigned[2];

	/*
	 * Set of computed connections, which could be available in future iterations
	 */
	std::set<size_t> toBeAvailable[2];

	/*
	 * Maximum number of connections of a certain type in the pool/rail
	 */
	size_t maxConnNr[2];

	/*
	 * Number of available connections for a specific box type
	 */
	size_t nrAvailable[2];

	connectionsPool();

	std::set<size_t> getAllUnavailable();

	void preReleaseConnection(int boxType, size_t connNr);

	void releaseConnection(int boxType, size_t connNr);

	size_t consumeConnection(int boxType);

	bool sufficientToConsume(int boxType);

	size_t reserveConnection(int boxType);

	bool sufficientAvailable(int boxType);

	bool increaseAvailable(int boxType, int max);

	size_t reserveConnectionPreferred(int boxType, size_t approxConnectionNumber);

};


#endif /* CONNECTIONSPOOL_H_ */
