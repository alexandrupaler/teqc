#ifndef FASTWIRES_H_
#define FASTWIRES_H_

#include <vector>
#include <string>
#include "wiredouble.h"

using namespace std;

class fastwires
{
public:
	vector<wireDouble*> allCircuitWires;

	fastwires(int nrWires);

	void insertWires();

	void split();

	void insertBefore(string gate, vector<wireDouble* >& before);

	void pushBack(string gate, vector<wireDouble* >& wires);
};

#endif /* FASTWIRES_H_ */
