#include <sstream>
#include <stdlib.h>

#include "convertcoordinate.h"

std::string convertcoordinate::toString(char separator)
{
	std::stringstream ss;
	ss << values[0] << separator << values[1] << separator << values[2];

	return ss.str();
}

bool convertcoordinate::isPrimalCorner(int val)
{
	int odd = 0;
	for(int i=0; i<3; i++)
		odd += ((values[0] - 1) % val);

	return (odd == 0);//TODO
}

bool convertcoordinate::isDualCorner(int val)
{
	return !isPrimalCorner(val);
}

bool convertcoordinate::isPrimalCenter(int val)
{
	//tre sa clarific odata ce pula mea e DELTA
	//si care este influenta lui asupra coordonatelor primare si duale

	int odd = 0;
	for(int i=0; i<3; i++)
		odd += (values[0] % val);

	return (odd == 3);//TODO
}

bool convertcoordinate::isDualCenter(int val)
{
	return !isPrimalCenter(val);
}

long convertcoordinate::manhattanDistance(convertcoordinate& other)
{
	long dist = 0;

	for(int i=0; i<3; i++)
		dist += abs(values[i] - other.values[i]);

	return dist;
}

bool convertcoordinate::isColinear(convertcoordinate& other)
{
	int eq = 0;

	for(int i=0; i<3; i++)
	{
		if(other.values[i] == values[i])
			eq++;
	}

	return (eq == 2);
}

void convertcoordinate::reset()
{
	values[0] = INT_MIN;
	values[1] = INT_MIN;
	values[2] = INT_MIN;
}
