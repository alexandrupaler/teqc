#include <stdio.h>
#include <vector>

#include "processraw.h"
#include "fileformats/rawfilereader.h"
#include "fileformats/infilewriter.h"

using namespace std;

int main(int argc, char** argv)
{
	printf("Hello World!\n");
	if(argc == 1)
	{
		printf(".raw file name parameter missing");
		return 0;
	}

	printf("processing %s\n", argv[1]);

	rawfilereader rawreader;
	FILE* rawf = fopen(argv[1], "r");
	rawdata raw = rawreader.readRawFile(rawf);
	fclose(rawf);

	processraw praw(raw);

	infilewriter inwriter;
	string fname = infilewriter::getInFileName(argv[1]);
	FILE* file = fopen (fname.c_str(), "w");
	inwriter.writeInFile(file, praw.getCircuit());
	fclose(file);

	return 1;
}
