#ifndef CHPFILEWRITER_H_
#define CHPFILEWRITER_H_

#include <string>
#include <vector>
#include "circuitmatrix.h"

using namespace std;

class chpfilewriter{
public:
	static string getCHPFileName(const char* basisfilename);
	void writeCHPFile(FILE* file, circuitmatrix& circ);
};


#endif /* CHPFILEWRITER_H_ */
