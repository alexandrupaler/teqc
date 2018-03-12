#include <sstream>
#include <stdlib.h>

#include "convertcoordinate.h"

std::string convertcoordinate::toString(char separator)
{
	std::stringstream ss;
	ss << values[0];
	ss << separator;
	ss << values[1];
	ss << separator;
	ss << values[2];

	return ss.str();
}

convertcoordinate::convertcoordinate()
{
	values.resize(3, LONG_MIN);
};

convertcoordinate::convertcoordinate(const convertcoordinate& other)
{
	values.resize(3, LONG_MIN);
	values[0] = other.values[0];
	values[1] = other.values[1];
	values[2] = other.values[2];
};

long& convertcoordinate::operator[](int i){return values[i];};
long& convertcoordinate::at(int i){return values[i];};

convertcoordinate& convertcoordinate::operator=(const convertcoordinate& other)
{
	values.resize(3, LONG_MIN);
	values[0] = other.values[0];
	values[1] = other.values[1];
	values[2] = other.values[2];

	return *this;
};

bool convertcoordinate::operator==(const convertcoordinate& other)
{
	bool equal = true;
	equal = equal && (values[0] == other.values[0]);
	equal = equal && (values[1] == other.values[1]);
	equal = equal && (values[2] == other.values[2]);

	return equal;
};


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
	values[0] = LONG_MIN;
	values[1] = LONG_MIN;
	values[2] = LONG_MIN;
}
