#include "recycling/graphanalysis.h"
#include "gatenumbers.h"
#include "numberandcoordinate.h"

int graphanalysis::getTotalAStates(causalgraph& causal)
{
	int ret = 0;
	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
				it != causal.tmpCircuit.end(); it++)
	{
		if((*it)->type == AA)
		{
			ret++;
		}
	}
	return ret;
}

int graphanalysis::getTotalYStates(causalgraph& causal)
{
	int ret = 0;
	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
				it != causal.tmpCircuit.end(); it++)
	{
		if((*it)->type == YY)
		{
			ret++;
		}
	}
	return ret;
}

std::vector< std::vector<int> > graphanalysis::getTypesAndWiresOfInjections(causalgraph& causal)
{
	std::vector< std::vector<int> > ret;
	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
				it != causal.tmpCircuit.end(); it++)
	{
		std::vector<int> entry;
		if((*it)->type == AA || (*it)->type == YY)
		{
			entry.push_back( (*it)->type == AA ? ATYPE : YTYPE);

			entry.push_back((*it)->wirePointers[0]->number);

			ret.push_back(entry);
		}
	}
	return ret;
}
