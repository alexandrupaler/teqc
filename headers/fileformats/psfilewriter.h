#ifndef PSFILEWRITER_H_
#define PSFILEWRITER_H_

#include <stdio.h>
#include <string>
#include "circuitmatrix.h"

using namespace std;

class psfilewriter{
public:
	void writePostScriptFile(FILE* file, circuitmatrix& circ);
	static string getPostScriptFileName(const char* basisfilename);
protected:
	void appendFile(FILE* destFile, const char* name);
};

#endif /* PSFILEWRITER_H_ */
