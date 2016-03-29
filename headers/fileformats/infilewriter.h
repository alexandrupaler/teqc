#ifndef INFILEWRITER_H_
#define INFILEWRITER_H_

#include <string>
#include <vector>

#include "fileformats/generaldefines.h"

using namespace std;

class infilewriter{
public:
	void writeInFile(FILE* file, vector<qubitline>& circuit);
	static string getInFileName(const char* basefilename);
};

#endif /* INFILEWRITER_H_ */
