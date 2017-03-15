#include <stdio.h>
#include <algorithm>

#include "circuitmatrix.h"
#include "gatenumbers.h"
#include "cnotcounter.h"

bool circuitmatrix::isInput(int i, int j)
{
	return circ.at(i).at(j) == INPUT;
}

bool circuitmatrix::isOutput(int i, int j)
{
	return circ.at(i).at(j) == OUTPUT;
}

bool circuitmatrix::isWire(int i, int j)
{
	return circ.at(i).at(j) == WIRE;
}

bool circuitmatrix::isInitialisation(int i, int j)
{
	return gatenumbers::getInstance().isInitialisationNumber(circ.at(i).at(j));
}

bool circuitmatrix::isMeasurement(int i, int j)
{
	return gatenumbers::getInstance().isMeasurementNumber(circ.at(i).at(j));
}

bool circuitmatrix::isDistillationAncillaInput(int i, int j)
{
	return circ.at(i).at(j) == AA || circ.at(i).at(j) == YY;
}

bool circuitmatrix::isEmpty(int i, int j)
{
	return circ.at(i).at(j) == EMPTY;
}

bool circuitmatrix::indexLessThanSize(size_t i, size_t index)
{
	return circ.at(i).size() > index;
}

int circuitmatrix::getNrLines()
{
	return circ.size();
}

size_t circuitmatrix::getMaxColumn()
{
	size_t max = 0;
	for (size_t i = 0; i < circ.size(); i++)
	{
		max = max < circ.at(i).size() ? circ.at(i).size() : max;
	}
	return max;
}

bool circuitmatrix::hasInjections()
{
	for(size_t i=0; i<circ.size(); i++)
	{
		for(size_t j=0; j<circ.at(i).size(); j++)
		{
			if(isDistillationAncillaInput(i, j))
				return true;
		}
	}
	return false;
}

vector<int> circuitmatrix::findTarget(int i, int j)
{
	vector<int> ret;
	int controlnumber = circ.at(i).at(j);

	for(size_t k=0; k<circ.size(); k++)
	{
		if(indexLessThanSize(k, j) && cnotcounter::sameCnot(controlnumber, circ.at(k).at(j)))
			ret.push_back(k);
	}
	return ret;
}

vector<int> circuitmatrix::findControl(int i, int j)
{
	vector<int> ret;
	int tartgetnumber = circ.at(i).at(j);

	for(size_t k=0; k < circ.size(); k++)
	{
		if(indexLessThanSize(k, j) && cnotcounter::sameCnot(circ.at(k).at(j), tartgetnumber))
			ret.push_back(k);
	}
	return ret;
}

void circuitmatrix::printCirc()
{
	printf("------\n");
	for(size_t i=0; i < circ.size(); i++)
	{
		for(size_t j=0; j < (circ.at(i).size()); j++)
		{
			int val = circ.at(i).at(j);
			printf("%6d ", val);
		}
		printf("\n");
	}
}

void circuitmatrix::removeEmptyColumns()
{
	size_t max = getMaxColumn();

	for(size_t j=0; j<max; j++)
	{
		bool columnIsEmpty = true;
		for(size_t i=0; i < circ.size(); i++)
		{
			columnIsEmpty = columnIsEmpty && ( !indexLessThanSize(i, j) || isWire(i, j) || isEmpty(i, j));
		}

		if(columnIsEmpty)
		{
			bool atLeastOneCell = false;
			for(size_t i=0; i < circ.size(); i++)
			{
				if(indexLessThanSize(i, j))
				{
					circ.at(i).erase(circ.at(i).begin() + j);
					atLeastOneCell = true;
				}
			}
			//printf("remove column %d %d\n", j, atLeastOneCell);
			if(atLeastOneCell)
			{
				j--;//go back
			}
		}
	}
}

void circuitmatrix::removeEmptyRows()
{
	bool emptyline = true;
	{
		for(size_t i=0; i < circ.size(); i++)
		{
			emptyline = true;
			for(size_t j=0; j < circ.at(i).size(); j++)
			{
				emptyline = emptyline && (isEmpty(i, j));
			}
			if(emptyline)
			{
				circ.erase(circ.begin() + i);
				i--;
			}
		}
	}
}

void circuitmatrix::insertRows(int beforePosition, vector<qubitline>& rows)
{
	circ.insert(circ.begin() + beforePosition, rows.begin(), rows.end());
}

void circuitmatrix::insertColumns(int beforePosition, int nrColumns)
{
	for (int k = 0; k < nrColumns; k++)
	{
		for (size_t i = 0; i < circ.size(); i++)
		{
			circ.at(i).insert(circ.at(i).begin() + beforePosition, WIRE);
		}
	}
}

qubitline& circuitmatrix::operator[](int i)
{
	//no checking, until now
	return circ[i];
}

qubitline& circuitmatrix::at(int i)
{
	//no checking, until now
	return circ[i];
}

circuitmatrix::circuitmatrix(vector<qubitline> orig)
{
	circ = orig;
	init();
}

circuitmatrix::circuitmatrix()
{
	init();
}

bool circuitmatrix::checkForCnotOnColumn(int column)
{
	for(size_t i=0; i < circ.size(); i++)
	{
		if(cnotcounter::isCnot(circ[i][column]))
		{
			return true;
		}
	}
	return false;
}

void circuitmatrix::init()
{
//	empty
}
