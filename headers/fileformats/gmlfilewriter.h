#ifndef GMLFILEWRITER_H_
#define GMLFILEWRITER_H_

#include "recycling/causalgraph.h"
#include "stdio.h"
#include <string>

class gmlfilewriter
{
public:
	static std::string getGMLFileName(const char* basisfilename, int nrconnect);
	void writeGMLFile(FILE* file, causalgraph& causal);
	void writeGMLFile2(FILE* file, causalgraph& causal);
	void writeGMLFileOrder(FILE* file, causalgraph& causal);
};


#endif /* GMLFILEWRITER_H_ */
