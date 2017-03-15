#ifndef BOUNDINGBOXFILEWRITER_H_
#define BOUNDINGBOXFILEWRITER_H_

#include <string>
#include <vector>

using namespace std;

/**
 * The class is used for writing files containing distillation box coordinates
 */
class boundingboxfilewriter{
public:
	/**
	 * Returns a file name with the corresponding extension for such files
	 * @param basisfilename the file name that will be extended by appending the extension
	 * @return the file name
	 */
	static string getBoundingBoxFileName(const char* basisfilename);

	/**
	 * Write a bounding box to the file
	 * @param file the file handler already opened with "w"
	 * @param boundingbox a coordinate that stores the maximum coordinates of the circuit geometry
	 */
	void writeBoundingBoxFile(FILE* file, convertcoordinate& boundingbox);
};

#endif /* BOUNDINGBOXFILEWRITER_H_ */
