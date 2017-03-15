#include <stdio.h>
#include "decomposition.h"

/**
 * Counts the CNOTs if this is a non-ICM decomposition, e.g. the Toffoli.
 * It is assumed that in a correct decomposition, a single CNOT per decomposition
 * column appears. A CNOT is allowed to have multiple targets, and
 * as a result on a column will be at most one control.
 * Therefore, the number of controls is counted.
 */
int decomposition::getNumberOfCnots()
{
	if(type == DecompType::icm || type == DecompType::disticm)
		return cnots.size();

	int height = gates.size();
	int length = gates.at(0).size();
	int nrcnots = 0;
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<length; j++)
		{
			if(gates[i][j] == CTRL)
			{
				nrcnots++;
			}
		}
	}
	return nrcnots;
}

void decomposition::toString()
{
	printf("name: %s\n", name.c_str());
}

size_t decomposition::getMaxCols()
{
	size_t max = 0;

	if(!isicm())
	{
		for(size_t i=0; i < gates.size(); i++)
		{
			if (max < gates[i].size())
			{
				max = gates[i].size();
			}
		}
	}
	else
	{
		max = cnots.size();
		max++;//the measurements
	}

	return max;
}

bool decomposition::isicm()
{
	return (type == DecompType::icm) || (type == DecompType::disticm);
}

bool decomposition::isdist()
{
	return type == DecompType::disticm;
}
