#ifndef ADAMFILEWRITER_H_
#define ADAMFILEWRITER_H_

#include <string>
#include <vector>
#include "numberandcoordinate.h"
#include "plumbingpieces.h"

using namespace std;

/**
 * The class is used for writing files containing circuit geometries
 */
class adamfilewriter{
public:
	/**
	 * Returns a file name with the corresponding extension for such files
	 * @param basisfilename the file name that will be extended by appending the extension
	 * @return the file name
	 */
	static string getAdamFileName(const char* basisfilename);

	/**
	 * Write a circuit geometry to a file
	 * @param f the file handler already opened with "w"
	 * @param pieces a vector of plumbing pieces
	 */
	void writeAdamFile(FILE* f, vector<plumbingpiece>& pieces);
};


#endif /* ADAMFILEWRITER_H_ */
