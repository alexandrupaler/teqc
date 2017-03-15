#include "gatenumbers.h"

//gatenumbers* gatenumbers::instance = 0;

bool gatenumbers::isInitialisationNumber(int val)
{
	//return instance->initialisations.find(val) != instance->initialisations.end();
	return getInstance().initialisations.find(val) != getInstance().initialisations.end();
}

//bool gatenumbers::isInitialisationNumberButNotConnection(int val)
//{
//	return isInitialisationNumber(val) && (val != CONNIN);
//}
//
//bool gatenumbers::isMeasurementNumberButNotConnection(int val)
//{
//	return isMeasurementNumber(val) && (val != CONNOUT);
//}

bool gatenumbers::isMeasurementNumber(int val)
{
	return getInstance().measurements.find(val) != getInstance().measurements.end();
}

bool gatenumbers::isAncillaOutput(int val)
{
	return isMeasurementNumber(val) && val != OUTPUT;
}

bool gatenumbers::isAncillaInput(int val)
{
	return isInitialisationNumber(val) && val != INPUT;
}

//bool gatenumbers::isConnectionElement(int val)
//{
//	return val==CONNIN || val==CONNOUT;
//}

gatenumbers& gatenumbers::getInstance()
{
//	if(gatenumbers::instance == 0)
//	{
//		gatenumbers::instance = new gatenumbers();
//	}
//
//	return gatenumbers::instance;

	static gatenumbers instance;
	return instance;
}

//void gatenumbers::clean()
//{
//	if(gatenumbers::instance != 0)
//	{
////		gatenumbers::instance->measurements.clear();
////		gatenumbers::instance->initialisations.clear();
//		delete gatenumbers::instance;
//	}
//}

gatenumbers::gatenumbers()
{
	/**/
	int myints1[]= {MX, MZ, MXZ, MZX, MA, MYZ, MY, OUTPUT};
	measurements = set<int>(myints1, myints1 + 8);

	/**/
	int myints2[]= {AA, YY, ZERO, PLUS, INPUT};
	initialisations = set<int>(myints2, myints2 + 5);
}

//gatenumbers::~gatenumbers()
//{
//	//gatenumbers::clean();
//}

