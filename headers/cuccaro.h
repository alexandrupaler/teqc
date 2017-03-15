#ifndef CUCCARO_H_
#define CUCCARO_H_

#include <string>
#include <vector>


using namespace std;

class cuccaro{
private:
	vector<string> circuit;

public:
	string cnot(int c, int t);

	string tof(int c1, int c2, int t);

	string xnot(int t);

	int b(int i);

	int a(int i);

	vector<string> makeCircuit(int nrb, int doxnot);
};

#endif /* CUCCARO_H_ */
