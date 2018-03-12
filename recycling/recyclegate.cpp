#include <climits>

#include "gatenumbers.h"
#include "recycling/recyclegate.h"
#include "utils/trim.h"
#include "recycling/costmodel.h"

void recyclegate::updateGateCostToAchieveLevel(long newCost)
{
	long costToAdd = newCost - level;
	if(costToAdd < 0)
	{
		printf("NEGATIVE UPDATE COST\n");
	}
	else
	{
		printf("u %d from %d to %ld\n", orderNrInGateList, gateCost, costToAdd);
		gateCost += costToAdd;//TODO += or = ?
	}
}

void recyclegate::updateCausalType()
{
	if (gatenumbers::getInstance().isAncillaInput(type))
	{
		causalType = RECINIT;
	}
	else if (gatenumbers::getInstance().isInitialisationNumber(type))
	{
		causalType = RECINPUT;
	}
	else if (gatenumbers::getInstance().isAncillaOutput(type))
	{
		causalType = RECMEAS;
	}
	else if (gatenumbers::getInstance().isMeasurementNumber(type))
	{
		causalType = RECOUTPUT;
	}
	else
	{
		causalType = RECGATE;
	}
}

void recyclegate::init()
{
	level = 0;
	orderNrInGateList = -1;
	causalType = RECNOTYPE;
	type = EMPTY;

	static unsigned long current = 0;
	current++;
	id = current;

	isVisited = false;

	gateCost = 0;
	additionalWireCost = 0;

	/*
	 * 3 NOV
	 */
	connChannel = 0;
}

unsigned long recyclegate::getId()
{
	return id;
}

recyclegate::recyclegate()
{
	init();
}

recyclegate::~recyclegate()
{
	wirePointers.clear();
	wiresToUseForLinks.clear();

	willPush.clear();
	pushedBy.clear();
}

/*from circgate.cpp*/
recyclegate::recyclegate(std::string& s, std::vector<wireelement*>& wirePointersVector)
{
	init();

	std::istringstream in(trim(s));
	std::string typ;
	in >>  typ;

	//set type
	type = typ[0];//a single char..not good?

	//append wire pointers
	while(!in.eof())
	{
		int nr;
		in >> nr;
		wirePointers.push_back(wirePointersVector[nr]);
	}

	//is performed in init()
//	//costs
//	gateCost = 0;
//	additionalWireCost = 0;
}

std::string recyclegate::generateLabel()
{
	std::string name = " ";
	if(causalType == RECINIT || causalType == RECINITCONN || causalType == RECINPUT)
	{
		name = "input" + //toString(it->lines[0]);
				intToString(wirePointers[0]->number);
	}
	else if(causalType == RECMEAS || causalType == RECMEASCONN || causalType == RECOUTPUT)
	{
		name = "output" + intToString(wirePointers[0]->number);
	}
	else
	{
		name = intToString(orderNrInGateList + 1);
	}

	std::string label(name + " [" + intToString(level) + "]");

	return label;
}

void recyclegate::print()
{
	printf("label:%s level:%ld g/w cost:%d/%d id:%d type:%d causalType:%d on lines:",
			generateLabel().c_str(), level, gateCost, additionalWireCost, orderNrInGateList, type, causalType);
	for(std::vector<wireelement*>::iterator it = wirePointers.begin(); it != wirePointers.end(); it++)
	{
		printf("%lu ", (*it)->number);
	}
	printf("\npushes: ");
	for(std::vector<recyclegate*>::iterator it = willPush.begin(); it != willPush.end(); it++)
	{
		printf("%d ", (*it)->orderNrInGateList);
	}
	printf("\npushed by: ");
	for(std::vector<recyclegate*>::iterator it = pushedBy.begin(); it != pushedBy.end(); it++)
	{
		printf("%d ", (*it)->orderNrInGateList);
	}
	printf("\n-------\n");
}

void recyclegate::addWillPush(recyclegate* gatePtr)
{
	if (std::find(willPush.begin(), willPush.end(), gatePtr) == willPush.end())
	{
		willPush.push_back(gatePtr);
	}
}

void recyclegate::addPushedBy(recyclegate* gatePtr)
{
	pushedBy.push_back(gatePtr);
}

bool recyclegate::isAncillaInitialisation()
{
	return causalType == RECINIT;
}

bool recyclegate::isInput()
{
	return causalType == RECINPUT;
}

bool recyclegate::isFirstLevelInitialisation()
{
	return isAncillaInitialisation() || isInput();
}

bool recyclegate::isInitialisation()
{
	return isFirstLevelInitialisation() || causalType == RECINITCONN;
}

bool recyclegate::isAncillaMeasurement()
{
	return causalType == RECMEAS;
}

bool recyclegate::isOutput()
{
	return causalType == RECOUTPUT;
}

bool recyclegate::isLastLevelMeasurement()
{
	return isAncillaMeasurement() || isOutput();
}

bool recyclegate::isMeasurement()
{
	return isLastLevelMeasurement() || causalType == RECMEASCONN;
}

bool recyclegate::isConnection()
{
	return causalType == RECINITCONN || causalType == RECMEASCONN;
}

bool recyclegate::isGate()
{
	return causalType == RECGATE;
}

void recyclegate::computeWiresWithCosts()
{
	wiresToUseForLinks.clear();

	wireelement* minWire = NULL;
	wireelement* maxWire = NULL;
	long minNr = LONG_MAX;
	long maxNr = LONG_MIN;

	for(size_t i = 0; i< wirePointers.size(); i++)
	{
		if(minNr > wirePointers[i]->number)
		{
			minNr = wirePointers[i]->number;
			minWire = wirePointers[i];
		}
		if(maxNr < wirePointers[i]->number)
		{
			maxNr = wirePointers[i]->number;
			maxWire = wirePointers[i];
		}
	}

	while(minWire != maxWire->next)
	{
		wiresToUseForLinks.push_back(minWire);
		minWire = minWire->next;
	}
}

void recyclegate::computeOnlyWires()
{
	wiresToUseForLinks.clear();
	wiresToUseForLinks = wirePointers;
}


void recyclegate::updateCost(bool isInitialisation, costmodel& model)
{
	//update cost
	//Default values from the model?
	gateCost = model.gateTypeCosts['d'].gatecost;
	additionalWireCost = model.gateTypeCosts['d'].wirecost;

	if(isInitialisation && type == AA)
	{
		gateCost = model.gateTypeCosts['a'].gatecost;
		additionalWireCost = model.gateTypeCosts['a'].wirecost;
	}
	else if(isInitialisation && type == YY)
	{
		gateCost = model.gateTypeCosts['y'].gatecost;
		additionalWireCost = model.gateTypeCosts['y'].wirecost;
	}
}


void recyclegate::updateType(bool isInitialisation, char ctype)
{
	//update type
	if(isInitialisation)
	{
		if(ctype != '-')
		{
			if(ctype == 'a')
			{
				type = AA;//TODO:temp
			}
			else if(ctype == 'y')
			{
				type = YY;//todo:temp
			}
			else if(ctype == '0')
				type = ZERO;//todo:temp
			else if(ctype == '+')
				type = PLUS;//todo:temp
		}
		else
			type = INPUT;
	}
	else
	{
		if(ctype != '-')
		{
			if(ctype == 'a')
				type = MA;//todo:temp
			else if(ctype == 'y')
				type = MY;//todo:temp
			else if(ctype == 'z')
				type = MZ;//todo:temp
			else if(ctype == 'x')
				type = MX;//todo:temp
			else if(ctype == '#')
				type = MXZ;//todo:temp
			else if(ctype == '*')
				type = MZX;//todo:temp
		}
		else
			type = OUTPUT;
	}
}

std::string recyclegate::toString()
{
	std::ostringstream temp;

	int operation = type;
	temp << operation;

	for(int i = 0; i < wirePointers.size(); i++)
	{
		int wireIndex = wirePointers[i]->getUpdatedWire(orderNrInGateList)->number;
		temp << " " << wireIndex;
	}

	return temp.str();
}

void recyclegate::fromString(std::string& line, char& type, std::vector<int>& indices)
{
	std::istringstream in(trim(line));

	//the first element is the type
	std::string s;
	in >>  s;

	std::istringstream iss( s );
	int value = -300;
	//try to read it as an integer
	iss >> value;
	if(!iss)
	{
		//it did not work. it is a string. take the first char
		type = s[0];
	}
	else
	{
		//it worked
		type = value;
	}

	//append wire pointers
	int nr;
	while(in >> nr)
	{
		indices.push_back(nr);
	}
}
