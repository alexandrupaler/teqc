#ifndef GEOMFILEREADER_H_
#define GEOMFILEREADER_H_

#include <vector>
#include <utility>
#include <stdio.h>

#include "convertcoordinate.h"

class geomfilereader{
public:
	/**
	 * Read a circuit geometry to a file
	 * @param f the file handler already opened with "w"
	 * @param io a vector of coordinate indices referring to vertex that represent circuit inputs or outputs
	 * @param segs a vector of index pairs referring to existing coordinates from the geometry
	 * @param coords the vector of geometry vertices
	 */
	void readGeomFile(FILE* f, std::vector<long>& io, std::vector<std::pair<long, long> >& segs, std::vector<convertcoordinate>& coords);
};


#endif /* GEOMFILEREADER_H_ */
