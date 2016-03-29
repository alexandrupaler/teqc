#include <stdio.h>
#include <vector>

#include "fileformats/geomfilewriter.h"

/**
 * Uses the ".geom" extension
 */
string geomfilewriter::getGeomFileName(const char* basisfilename)
{
	string ret(basisfilename);
	ret += ".geom";
	return ret;
}

/**
 * The file format is as follows:
 * - first block:
 * 	-# first line: the number of input or output points
 * 	-# second line: the number of segments
 * 	-# third line: the number of coordinates
 * 	-# fourth line: a comma separated list of the indices representing inputs or output points
 * - second block:
 * 	-# each line contains a pair of comma separated segment index pairs
 * - third block:
 * 	-# each line contains a comma separated list containing "coordinateid, x, y, z"
 * - fourth block:
 *	-# the inputs and the outputs have names, but the writer uses their indices for this purpose
 *	each line is of the form "index,name"
 */
void geomfilewriter::writeGeomFile(FILE* f, vector<long>& io, vector<pair<long, long> >& segs, vector<coordinate>& coords)
{
	fprintf(f, "%d\n", (int)io.size());
	fprintf(f, "%d\n", (int)segs.size());
	fprintf(f, "%d\n", (int)coords.size());

	for(int i=0; i<io.size(); i++)
	{
		if(i!=0)
			fprintf(f, ",");
		fprintf(f, "%ld", io.at(i) + 1);
	}
	fprintf(f, "\n");

	for(int i=0; i<segs.size(); i++)
	{
		fprintf(f, "%ld,%ld\n", segs.at(i).first + 1, segs.at(i).second + 1);
	}

	for(int i=0; i<coords.size(); i++)
	{
		fprintf(f, "%d,%ld,%ld,%ld\n", i + 1, coords.at(i)[0], coords.at(i)[1], coords.at(i)[2]);
	}

	for(int i=0; i<io.size(); i++)
	{
		fprintf(f, "%ld,%ld\n", io.at(i) + 1, io.at(i) + 1);
	}
}
