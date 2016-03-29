#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "numberandcoordinate.h"

#include "faildistillations.h"
#include "fileformats/toconnectfilewriter.h"

int main(int argc, char** argv)
{
	//circuit.raw.in.geom.io circuit.raw.in.geom.io.allpins 0.4 0.4
	srand48(time(NULL));

	faildistillations fd(argv[1], argv[2]);

	fd.selectDistillationForCircuitInput(atof(argv[3]), atof(argv[4]));

	toconnectfilewriter tcwr;
	string fname = toconnectfilewriter::getToConnectFileName(argv[1]);
	FILE* file = fopen(fname.c_str(), "w");
	tcwr.writeToConnectFileName(file, fd.nonFailedPinPairCoords);
	fclose(file);

//	char fname[255];
//	sprintf(fname, "%s.pins", argv[1]);//results in e.g. circuit.raw.in.geom.io.pins
//	fd.writeCircuitToDistillationConnections(fname);
//
//	fd.writeFailedDistillations();

	return 0;
}
