#ifndef INFILEWRITER_H_
#define INFILEWRITER_H_

#include <string>
#include <vector>

#include "fileformats/generaldefines.h"

/**
 * NOT UPDATED TO INFILEREADER
 */
class infilewriter{
public:
	void writeInFile(FILE* file, std::vector<qubitline>& circuit);
	static std::string getInFileName(const char* basefilename);
};

#endif /* INFILEWRITER_H_ */
