#ifndef RECYCLEDCIRCUITREADER_H_
#define RECYCLEDCIRCUITREADER_H_

#include "recycling/causalgraph.h"

class recycledcircuitreader
{
	public:
		void readRecycledFile(const char* fname, causalgraph& graph);
};

#endif /* RECYCLEDCIRCUITREADER_H_ */
