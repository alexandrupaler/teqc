#ifndef TOCONNECTFILEWRITER_H_
#define TOCONNECTFILEWRITER_H_

#include <vector>
#include <string>
#include "faildistillations.h"

class toconnectfilewriter
{
public:
	static std::string getToConnectFileName(const char* basisname);
	void writeToConnectFile(FILE* file, std::vector<pinpair>& pairs);
};


#endif /* TOCONNECTFILEWRITER_H_ */
