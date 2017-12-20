#ifndef CUCCARO_H_
#define CUCCARO_H_

#include <string>
#include <vector>

class cuccaro{
private:
	std::vector<std::string> circuit;

public:
	std::string cnot(int c, int t);

	std::string tof(int c1, int c2, int t);

	std::string xnot(int t);

	int b(int i);

	int a(int i);

	std::vector<std::string> makeCircuit(int nrb, int doxnot);
};

#endif /* CUCCARO_H_ */
