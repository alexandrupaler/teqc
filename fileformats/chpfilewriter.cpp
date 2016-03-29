#include <stdio.h>
#include "fileformats/chpfilewriter.h"
#include "gatenumbers.h"
#include <cnotcounter.h>

string chpfilewriter::getCHPFileName(const char* basisfilename)
{
	string ret(basisfilename);
	ret += ".chp";
	return ret;
}

void chpfilewriter::writeCHPFile(FILE* file, circuitmatrix& circ)
{
	fprintf(file, "#\n");

	//sunt toate liniile in circ de aceeasi lungime? banuiesc ca da...
	for(int j=0; j<circ.at(0).size(); j++)
	{
		for(int i=0; i<circ.size(); i++)
		{
			if(j==0 && circ.at(i).at(j) == PLUS)
			{
				fprintf(file, "h %d\n", i);
			}
			if(cnotcounter::getCnotPart(circ.at(i).at(j)) == CTRL)
			{
				vector<int> targets = circ.findTarget(i,j);
				for(int k=0; k<targets.size(); k++)
				{
					fprintf(file, "c %d %d\n", i, targets.at(k));
				}
			}

		}
	}
}
