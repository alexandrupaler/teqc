#ifndef IOFILEWRITER_H_
#define IOFILEWRITER_H_

#include <stdio.h>
#include <string>

#include "geometry.h"
#include "numberandcoordinate.h"

using namespace std;

class iofilewriter{
public:
	static string getIOFileName(const char* basisfilename);
	void writeIOFile(FILE* file, vector<inputpin>& allpins);
};


#endif /* IOFILEWRITER_H_ */
