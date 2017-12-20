#ifndef COSTMODEL__
#define COSTMODEL__

#include <map>

struct costs
{
	int gatecost;
	int wirecost;
};

class costmodel
{
public:
	std::map<char, costs> gateTypeCosts;
};

#endif
