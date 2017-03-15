#ifndef GEOMFILEWRITER_H_
#define GEOMFILEWRITER_H_

#include <string>
#include "numberandcoordinate.h"

using namespace std;

/**
 * The class is used for writing files containing circuit geometries
 */
class geomfilewriter{
public:
	/**
	 * Returns a file name with the corresponding extension for such files
	 * @param basisfilename the file name that will be extended by appending the extension
	 * @return the file name
	 */
	static string getGeomFileName(const char* basisfilename);

	/**
	 * Write a circuit geometry to a file
	 * @param f the file handler already opened with "w"
	 * @param io a vector of coordinate indices referring to vertex that represent circuit inputs or outputs
	 * @param segs a vector of index pairs referring to existing coordinates from the geometry
	 * @param coords the vector of geometry vertices
	 */
	void writeGeomFile(FILE* f, vector<long>& io, vector<pair<long, long> >& segs, vector<convertcoordinate>& coords);
};


#endif /* GEOMFILEWRITER_H_ */
