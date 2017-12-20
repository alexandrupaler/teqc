#include <vector>
#include <map>
#include "numberandcoordinate.h"
#include "fileformats/iofilewriter.h"

std::string iofilewriter::getIOFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".io";
	return ret;
}

//void iofilewriter::writeIOFile(FILE* file, vector<vector<int> >& allpins, map<int, int>& inputs)
void iofilewriter::writeIOFile(FILE* file, std::vector<pinpair>& allpins)
{
	for(size_t i=0; i < allpins.size(); i++)
	{
		int type = allpins.at(i)[TYPE];
		int ioIndex = allpins.at(i)[INJNR] + 1;

		fprintf(file, "%c %d ", type == 0 ? 'A' : 'Y', ioIndex);//nr
		for(int j=0; j<6; j++)
		{
			fprintf(file, "%ld ", allpins.at(i)[OFFSETNONCOORD + j]);
		}
		fprintf(file, "\n");
	}
}
