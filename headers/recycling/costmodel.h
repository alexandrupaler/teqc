#ifndef COSTMODEL__
#define COSTMODEL__

#include <map>

using namespace std;

struct costs
{
	int gatecost;
	int wirecost;
};

class costmodel
{
public:
	map<char, costs> gateTypeCosts;
};

#endif
