#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "connectpins.h"
#include "fileformats/geomfilewriter.h"


using namespace std;

int main(int argc, char** argv)
{
	connectpins cp;

	cp.processPins(argv[1]);

	geomfilewriter geomwr;
	string fname = geomfilewriter::getGeomFileName(argv[1]);
	FILE* file = fopen(fname.c_str(), "w");
	geomwr.writeGeomFile(file, cp.geom.io, cp.geom.segs, cp.geom.coords);
	fclose(file);

	//cp.writeGeometryFile(argv[1]);

	return 1;
}
