#ifndef INFILEREADER_H_
#define INFILEREADER_H_

#include <vector>
#include "fileformats/generaldefines.h"

class infilereader{
public:
	vector<qubitline> readInFile(FILE* file);
};


#endif /* INFILEREADER_H_ */
