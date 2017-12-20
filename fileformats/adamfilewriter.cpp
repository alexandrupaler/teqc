#include <stdio.h>
#include <vector>

#include "plumbingpieces.h"

#include "fileformats/adamfilewriter.h"

/**
 * Uses the ".adam" extension
 */
std::string adamfilewriter::getAdamFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".adam";
	return ret;
}

void adamfilewriter::writeAdamFile(FILE* f, std::vector<plumbingpiece>& pieces)
{
	fprintf(f, "var plumb={\n");
	fprintf(f, "\"version\": 1,\n");
	fprintf(f, "\"type\": \"PLUMB\",\n");
	fprintf(f, "\"plumbs\": [\n");

	for(size_t i=0; i<pieces.size(); i++)
	{
		std::string msg = pieces[i].toString();
		if(i>0)
			fprintf(f,",");
		fprintf(f, "%s\n", msg.c_str());
	}

	fprintf(f, "]}");
}
