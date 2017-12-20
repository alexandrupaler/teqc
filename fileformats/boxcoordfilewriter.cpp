#include "fileformats/boxcoordfilewriter.h"
#include "gatenumbers.h"
#include "numberandcoordinate.h"
#include "geometry.h"

/**
 * Uses the extension ".box.[scheduleNumber]"
 */
std::string boxcoordfilewriter::getBoxCoordFileName(const char* basisfilename, int scheduleNumber)
{
	std::string ret(basisfilename);
	char str[1024];
	sprintf(str, "%d", scheduleNumber);
	ret += ".box." + std::string(str);
	return ret;
}

/**
 * The file consists actually of a JSON where an object contains two fields, types and coords,
 * corresponding to the \p boxSize and \p boxCoords
 */
void boxcoordfilewriter::writeBoxCoordFile(FILE* file, std::vector<std::vector<int> >& boxSize,
		std::vector<boxcoord>& boxCoords, int scheduleNumber)
{
	bool myisfirst = true;
	fprintf(file, "var boxes%d={\"types\":[", scheduleNumber);
	fprintf(file, "[%d,%d,%d],", boxSize[ATYPE][CIRCUITWIDTH], boxSize[ATYPE][CIRCUITDEPTH], boxSize[ATYPE][CIRCUITHEIGHT]);
	fprintf(file, "[%d,%d,%d]],", boxSize[YTYPE][CIRCUITWIDTH], boxSize[YTYPE][CIRCUITDEPTH], boxSize[YTYPE][CIRCUITHEIGHT]);
	fprintf(file, "\"coords\": [");

	for(std::vector<boxcoord>::iterator it = boxCoords.begin(); it != boxCoords.end(); it++)
	{
		if (!myisfirst)
			fprintf(file, ",");
		else
			myisfirst = false;

		fprintf(file, "[%ld, %ld, %ld, %ld, %ld]", (*it)[TYPE], (*it)[INJNR],
				(*it)[OFFSETNONCOORD + CIRCUITWIDTH],
				(*it)[OFFSETNONCOORD + CIRCUITDEPTH],
				(*it)[OFFSETNONCOORD + CIRCUITHEIGHT]);
	}

	fprintf(file, "]}");
}
