#include <stdio.h>
#include "fileformats/toconnectfilewriter.h"

string toconnectfilewriter::getToConnectFileName(const char* basisname)
{
	string ret(basisname);
	ret += ".toconnect";
	return ret;
}

void toconnectfilewriter::writeToConnectFileName(FILE* file, vector<pinpair>& pairs)
{
	for(vector<pinpair>::iterator it = pairs.begin(); it != pairs.end(); it++)
	{
		for(int i=0; i<2; i++)
		{
			fprintf(file, "%s ", it->getCoordinate(i).toString(' ').c_str());
		}
		fprintf(file, "\n");
	}
}
