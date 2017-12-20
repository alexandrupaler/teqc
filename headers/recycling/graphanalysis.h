#ifndef GRAPHANALYSIS_H_
#define GRAPHANALYSIS_H_

#include "recycling/causalgraph.h"

class graphanalysis
{
public:
	static int getTotalAStates(causalgraph& causal);
	static int getTotalYStates(causalgraph& causal);
	static std::vector< std::vector <int> > getTypesAndWiresOfInjections(causalgraph& causal);
};

#endif /* GRAPHANALYSIS_H_ */
