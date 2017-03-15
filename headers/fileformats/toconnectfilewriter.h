#ifndef TOCONNECTFILEWRITER_H_
#define TOCONNECTFILEWRITER_H_

#include <vector>
#include <string>
#include "faildistillations.h"

using namespace std;

class toconnectfilewriter
{
public:
	static string getToConnectFileName(const char* basisname);
	void writeToConnectFile(FILE* file, vector<pinpair>& pairs);
};


#endif /* TOCONNECTFILEWRITER_H_ */
