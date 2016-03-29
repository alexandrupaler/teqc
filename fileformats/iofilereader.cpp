#include <stdio.h>
#include "fileformats/iofilereader.h"
#include "numberandcoordinate.h"

numberandcoordinate iofilereader::readIOFile(FILE* file)
{
	numberandcoordinate ret;

	char type[1];
	int line = 0;//for debug purposes

	while (!feof(file))
	{
		if (fscanf(file, "%s", type) == 0)
			continue;

		if (feof(file))
			break;

		inputpin numberAndCoords;
		int itype = (type[TYPE] == 'A' ? 0 : 1);
		numberAndCoords[TYPE] = itype;
//		numberAndCoords.push_back(itype);

		for (int i = 0; i < 7; i++)
		{
			long r;
			fscanf(file, "%ld", &r); //daca nu a citit nimic
			//numberAndCoords.push_back(r);
			numberAndCoords[1 + i] = r;
		}

		//compute min/max inj coordinate
		ret.addEntry(numberAndCoords);

		if (feof(file))
			break;

		//printf("line %c %d\n", type[0], line++);
	}

	return ret;
}
