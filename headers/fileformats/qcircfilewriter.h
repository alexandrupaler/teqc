#ifndef QCIRCFILEWRITER_H_
#define QCIRCFILEWRITER_H_

#include <string>
#include "circuitmatrix.h"

using namespace std;

class qcircfilewriter{
public:
	static string getQCircFileName(const char* basisfilename);
	void writeQCircFile(FILE* file, circuitmatrix& circ);
};


#endif /* QCIRCFILEWRITER_H_ */
