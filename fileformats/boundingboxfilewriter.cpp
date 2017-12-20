#include <stdio.h>
#include <string>
#include "numberandcoordinate.h"
#include "fileformats/boundingboxfilewriter.h"

/**
 * Uses the extension ".bounding"
 */
std::string boundingboxfilewriter::getBoundingBoxFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".bounding";
	return ret;
}

/**
 * Assumes that the minimal coordinates are 0,0,0 and only the maximum coordinates are required.
 * @param file
 * @param boundingbox
 */
void boundingboxfilewriter::writeBoundingBoxFile(FILE* file, convertcoordinate& boundingbox)
{
	fprintf(file, "%s\n", boundingbox.toString(',').c_str());
}
