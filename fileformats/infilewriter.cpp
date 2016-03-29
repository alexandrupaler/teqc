#include <stdio.h>
#include "fileformats/infilewriter.h"

void infilewriter::writeInFile(FILE* file, vector<qubitline>& circuit)
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

string infilewriter::getInFileName(const char* basisfilename)
{
	string ret(basisfilename);
	ret += ".in";
	return ret;
}
