#ifndef BOXCOORDFILEWRITER_H_
#define BOXCOORDFILEWRITER_H_

#include <stdio.h>
#include <vector>
#include <string>
#include "boxworld2.h"

using namespace std;

/**
 * The class is used for writing files containing distillation box coordinates
 */
class boxcoordfilewriter{
public:
	/**
	 * Returns a file name with the corresponding extension for such files
	 * @param basisfilename the file name that will be extended by appending the extension
	 * @param numberofschedule box coordinates are the result of distillation schedulers, which can be numbered
	 * @return the file name
	 */
	static string getBoxCoordFileName(const char* basisfilename, int numberofschedule);

	/**
	 * Write the distillation box coordinates to a file
	 * @param file the file handler that is already opened with "w"
	 * @param boxSize a vector of three-dimensional box coordinates, for each box type there should be a dimension
	 * @param boxCoords a vector of boxcoord that represents the individual distillation boxes
	 * @param numberofschedule the number of the schedule, should correspond to the getBoxCoordFileName() method
	 */
	void writeBoxCoordFile(FILE* file, vector<vector<int> >& boxSize, vector<boxcoord>& boxCoords, int numberofschedule);
};


#endif /* BOXCOORDFILEWRITER_H_ */
