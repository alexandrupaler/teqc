#include "fileformats/recycledcircuitreader.h"

#include <vector>
#include <fstream>

#include "gatenumbers.h"

void recycledcircuitreader::readRecycledFile(const char* fname, causalgraph& graph)
{
	std::vector<std::string> ret;

	std::ifstream ifs (fname, std::ifstream::in);

	std::string line;

	std::vector<wireelement*> tmpWires;
	std::vector<int> tmpWiresIndices;

	while(ifs.good())
	{
		getline(ifs, line);

		if(line.size() == 0)
		{
			continue;
		}

		tmpWiresIndices.clear();

		char type;
		recyclegate::fromString(line, type, tmpWiresIndices);

		//compute the maximum number of the wires used by this gate
		int maxWire = *std::max_element(tmpWiresIndices.begin(), tmpWiresIndices.end());
		maxWire += 1;//indices start from zero

		//how many wires are missing?
		if(maxWire > tmpWires.size())
		{
			long dif = maxWire - tmpWires.size();
			for(long wi = 0; wi < dif; wi++)
			{
				graph.constructWire2(tmpWires);//in caz ca nu exista
			}
		}

		if(tmpWiresIndices.size() == 1)
		{
			// this is a single qubit operation
			std::list<recyclegate*>::iterator newIter =
				graph.constructRecycleGate2(tmpWires, tmpWiresIndices, graph.tmpCircuit.end());

			bool isInitialisation = gatenumbers::getInstance().isInitialisationNumber(type);

			//set the type here, because the ctype is difficult to infer
			(*newIter)->type = type;
			graph.configureInputOutput2(isInitialisation, '*'/*force it to be nothing*/, newIter );
		}
		else
		{
			// this is a cnot gate
			graph.constructGate2(line, tmpWires, graph.tmpCircuit.end());
		}
	}
}
