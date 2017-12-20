#include <stdio.h>
#include "fileformats/chpfilewriter.h"
#include "gatenumbers.h"
#include <cnotcounter.h>

std::string chpfilewriter::getCHPFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".chp";
	return ret;
}

void chpfilewriter::writeCHPFile(FILE* file, circuitmatrix& circ)
{
	fprintf(file, "#\n");

	//sunt toate liniile in circ de aceeasi lungime? banuiesc ca da...
	for(size_t j=0; j<circ.at(0).size(); j++)
	{
		for(size_t i=0; i<circ.size(); i++)
		{
			if(j==0 && circ.at(i).at(j) == PLUS)
			{
				fprintf(file, "h %lu\n", i);
			}
			if(cnotcounter::getCnotPart(circ.at(i).at(j)) == CTRL)
			{
				std::vector<int> targets = circ.findTarget(i,j);
				for(size_t k=0; k<targets.size(); k++)
				{
					fprintf(file, "c %lu %d\n", i, targets.at(k));
				}
			}

		}
	}
}
