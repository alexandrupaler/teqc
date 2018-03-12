#include "fileformats/geomfilereader.h"

void geomfilereader::readGeomFile(FILE* f, std::vector<long>& io, std::vector<std::pair<long, long> >& segs, std::vector<convertcoordinate>& coords)
{
	io.clear();
	segs.clear();
	coords.clear();

	size_t ioSize = 0;
	fscanf(f, "%lu", &ioSize);

	size_t segsSize = 0;
	fscanf(f, "%lu", &segsSize);

	size_t coordsSize = 0;
	fscanf(f, "%lu", &coordsSize);

	io.resize(ioSize);
	segs.resize(segsSize);
	coords.resize(coordsSize);

	for(size_t i=0; i < ioSize; i++)
	{
		if(i != 0)
			fscanf(f, ",");
		fscanf(f, "%ld", &io[i]);
		io[i] -= 1;
	}
	fprintf(f, "\n");

	for(size_t i=0; i < segsSize; i++)
	{
		fscanf(f, "%ld,%ld\n", &segs.at(i).first, &segs.at(i).second);
		segs.at(i).first -= 1;
		segs.at(i).second -= 1;
	}

	for(size_t i=0; i < coordsSize; i++)
	{
		fscanf(f, "%*u,%ld,%ld,%ld\n", &coords[i][0], &coords[i][1], &coords[i][2]);
	}

	/*
	 * TODO: Do not care about injection names.
	 */
//	for(size_t i=0; i < ioSize; i++)
//	{
//		fprintf(f, "%ld,%ld\n", io.at(i) + 1, io.at(i) + 1);
//	}
}
