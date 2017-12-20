#ifndef IOFILEWRITER_H_
#define IOFILEWRITER_H_

#include <stdio.h>
#include <string>

#include "geometry.h"
#include "numberandcoordinate.h"

class iofilewriter{
public:
	static std::string getIOFileName(const char* basisfilename);
	void writeIOFile(FILE* file,std:: vector<pinpair>& allpins);
};


#endif /* IOFILEWRITER_H_ */
