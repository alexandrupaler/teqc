#ifndef RECYCLEDCIRCUITWRITER_H_
#define RECYCLEDCIRCUITWRITER_H_

#include <recycling/causalgraph.h>
#include <recycling/recyclegate.h>
#include <string>

class recycledcircuitwriter
{
//private:
//	void writeGateText(FILE* f, recyclegate* current);

public:
	void writeRecycledCircuitFile(FILE* f, causalgraph& causal);
	static std::string getRecycledFileName(const char* basefilename);
};

#endif
