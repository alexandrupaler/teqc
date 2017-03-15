#include <stdio.h>
#include "fileformats/toconnectfilewriter.h"
#include "convertcoordinate.h"

string toconnectfilewriter::getToConnectFileName(const char* basisname)
{
	string ret(basisname);
	ret += ".toconnect";
	return ret;
}

void toconnectfilewriter::writeToConnectFile(FILE* file, vector<pinpair>& pairs)
{
	//for(vector<pinpair>::iterator it = pairs.begin(); it != pairs.end(); it++)
	for(size_t j=0; j < pairs.size(); j++)
	{
		for(int i=0; i < 2; i++)
		{
			fprintf(file, "%s ", pairs[j].getPinDetail(i).coord.toString(' ').c_str());
		}
		fprintf(file, "\n");
	}
}
