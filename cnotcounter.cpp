#include <cnotcounter.h>

cnotcounter::cnotcounter()
{
	numberCnot = 0;
}

int cnotcounter::getCnotPart(int val)
{
	if(val > cnotcounter::startCnotNumber)
	{
		int type = val%2;
		type += (type == 0 ? TGT : 0);
		return type;
	}
	return -1;
}

bool cnotcounter::isCnot(int val)
{
	bool is = (getCnotPart(val) != -1);

	return is;
}

int cnotcounter::getNextCnotNumber()
{
	//this will be the number of the control
	int ret = startCnotNumber + 2*numberCnot + 1;
	//all the targets of a CNOT will have the same number(id) 10000+2*numberCnot + 2;
	numberCnot++;

	return ret;
}

bool cnotcounter::sameCnot(int ctrlval, int targetval)
{
	return (ctrlval + 1 == targetval);
}

bool cnotcounter::isControl(int value)
{
	return isCnot(value) && (getCnotPart(value) == CTRL);
}

bool cnotcounter::isTarget(int value)
{
	return isCnot(value) && (getCnotPart(value) == TGT);
}
