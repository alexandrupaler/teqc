#include <stdio.h>
#include <vector>

#include "gatenumbers.h"
#include "fileformats/infilereader.h"
#include "fileformats/generaldefines.h"

using namespace std;

vector<qubitline> infilereader::readInFile(FILE* file)
{
	int nrlines = -1;
	int nrcols = -1;

	fscanf(file, "%d", &nrlines);
	fscanf(file, "%d", &nrcols);

	vector<qubitline> circuit(nrlines);

	for(int i=0; i<nrlines; i++)
	{
		//circuit[i].reserve(nrcols);
		circuit[i].resize(nrcols, EMPTY);

		for(int j=0; j<nrcols; j++)
		{
			int v = EMPTY;
			fscanf(file, "%d", &v);
			circuit[i][j] = v;
		}
	}

	return circuit;
}
