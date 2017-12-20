#ifndef CHPFILEWRITER_H_
#define CHPFILEWRITER_H_

#include <string>
#include <vector>
#include "circuitmatrix.h"

class chpfilewriter{
public:
	static std::string getCHPFileName(const char* basisfilename);
	void writeCHPFile(FILE* file, circuitmatrix& circ);
};


#endif /* CHPFILEWRITER_H_ */
