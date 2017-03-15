#include <climits>

#include "gatenumbers.h"
#include "recycling/recyclegate.h"
#include "recycling/trim.h"
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
		printf("u %d from %d to %ld\n", id, gateCost, costToAdd);
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

void recyclegate::initFrom(recyclegate& g)
{
	//copy attributes
	type = g.type;
	wires = g.wires;

	computeOnlyWires();
}

recyclegate::recyclegate()
{
	level = 0;
	label = " ";
	id = -1;
	causalType = RECNOTYPE;

	gateCost = 0;
	additionalWireCost = 0;
}

void recyclegate::generateLabel()
{
	string name = " ";
	if(causalType == RECINIT || causalType == RECINITCONN || causalType == RECINPUT)
	{
		name = "input" + //toString(it->lines[0]);
				toString(wires[0]);
	}
	else if(causalType == RECMEAS || causalType == RECMEASCONN || causalType == RECOUTPUT)
	{
		name = "output" + toString(wires[0]);
	}
	else
	{
		name = toString(id + 1);
	}

	label = name + " [" + toString(level) + "]";
}

void recyclegate::print()
{
	printf("label:%s level:%ld g/w cost:%d/%d id:%d type:%d causalType:%d on lines:",
			label.c_str(), level, gateCost, additionalWireCost, id, type, causalType);
	for(vector<int>::iterator it = wires.begin(); it != wires.end(); it++)
	{
		printf("%d ", *it);
	}
	printf("\npushes: ");
	for(set<int>::iterator it = willPush.begin(); it != willPush.end(); it++)
	{
		printf("%d ", *it);
	}
	printf("\npushed by: ");
	for(set<int>::iterator it = pushedBy.begin(); it != pushedBy.end(); it++)
	{
		printf("%d ", *it);
	}
	printf("\n-------\n");
}

void recyclegate::addWillPush(int nr)
{
	if(willPush.find(nr) == willPush.end())
	{
		willPush.insert(nr);
	}
}

void recyclegate::addPushedBy(int nr)
{
	pushedBy.insert(nr);
}

void recyclegate::getMinMax(int& min, int& max)
{
	if(wires.size() == 0)
		return;

	min = INT_MAX;
	max = INT_MIN;
	for(vector<int>::iterator it = wires.begin(); it != wires.end(); it++)
	{
		if(min > *it)
			min = *it;
		if(max < *it)
			max = *it;
	}
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

	int minq;
	int maxq;
	getMinMax(minq, maxq);
	for(int i=minq; i<=maxq + additionalWireCost; i++)
	{
		wiresToUseForLinks.push_back(i);
	}
}

void recyclegate::computeOnlyWires()
{
	wiresToUseForLinks.clear();
	wiresToUseForLinks = wires;
}

/*from circgate.cpp*/

recyclegate::recyclegate(string& s)
{
	gateCost = 0;
	additionalWireCost = 0;

	istringstream in(trim(s));
	string typ;
	in >>  typ;

	type = typ[0];//a single char..not good?

	while(!in.eof())
	{
		int nr;
		in >> nr;
		wires.push_back(nr);
	}
}

void recyclegate::updateTypeAndCost(bool isInitialisation, char ctype, costmodel& model)
{
	//Default values from the model?
	gateCost = model.gateTypeCosts['d'].gatecost;
	additionalWireCost = model.gateTypeCosts['d'].wirecost;

	if(isInitialisation)
	{
		if(ctype != '-')
		{
			if(ctype == 'a')
			{
				type = AA;//TODO:temp

				gateCost = model.gateTypeCosts[ctype].gatecost;
				additionalWireCost = model.gateTypeCosts[ctype].wirecost;
			}
			else if(ctype == 'y')
			{
				type = YY;//todo:temp

				gateCost = model.gateTypeCosts[ctype].gatecost;
				additionalWireCost = model.gateTypeCosts[ctype].wirecost;
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

void recyclegate::replaceWires(map<int, int>& dict)
{
	for(vector<int>::iterator wit = wires.begin(); wit != wires.end(); wit++)
	{
		if(dict.find(*wit) != dict.end())
			*wit = dict[*wit];
	}
}
