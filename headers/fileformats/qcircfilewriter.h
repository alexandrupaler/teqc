#ifndef QCIRCFILEWRITER_H_
#define QCIRCFILEWRITER_H_

#include <string>
#include "circuitmatrix.h"

class qcircfilewriter{
public:
	static std::string getQCircFileName(const char* basisfilename);
	void writeQCircFile(FILE* file, circuitmatrix& circ);
};


#endif /* QCIRCFILEWRITER_H_ */
