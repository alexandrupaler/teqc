#include <fstream>

#include "fileformats/infilereader.h"

std::vector<std::string> infilereader::readInFile(char* fname)
{
	std::vector<std::string> ret;

	std::ifstream ifs (fname, std::ifstream::in);

	std::string line;

	//citeste inputuri
	getline(ifs, line);

	ret.push_back(line);

	getline(ifs, line);

	ret.push_back(line);

	while(ifs.good())
	{
		std::string line2;
		getline(ifs, line2);

		if(line2.size() == 0)
		{
			continue;
		}
		ret.push_back(line2);
	}

	return ret;
}
