#include "gatenumbers.h"

bool gatenumbers::isInitialisationNumber(int val)
{
	return initialisations.find(val) != initialisations.end();
}

bool gatenumbers::isMeasurementNumber(int val)
{
	return measurements.find(val) != measurements.end();
}

bool gatenumbers::isAncillaOutput(int val)
{
	return isMeasurementNumber(val) && val != OUTPUT;
}

bool gatenumbers::isAncillaInput(int val)
{
	return isInitialisationNumber(val) && val != INPUT;
}

gatenumbers& gatenumbers::getInstance()
{
	static gatenumbers instance;
	return instance;
}

gatenumbers::gatenumbers()
{
	/**/
	int myints1[]= {MX, MZ, MXZ, MZX, MA, MYZ, MY, OUTPUT};
	measurements = std::set<int>(myints1, myints1 + 8);

	/**/
	int myints2[]= {AA, YY, ZERO, PLUS, INPUT};
	initialisations = std::set<int>(myints2, myints2 + 5);
}

