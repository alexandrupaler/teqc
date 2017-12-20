#include <stdio.h>
#include "fileformats/infilewriter.h"

void infilewriter::writeInFile(FILE* file, std::vector<qubitline>& circuit)
{
	fprintf(file, "%d\n", (int) circuit.size());
	fprintf(file, "%d\n", (int) circuit.at(0).size());

	for(int i=0; i<circuit.size(); i++)
	{
		for(int j=0; j<circuit.at(i).size(); j++)
		{
			fprintf(file, "%5d ", circuit.at(i).at(j));
		}
		fprintf(file, "\n");
	}
}

std::string infilewriter::getInFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".in";
	return ret;
}
