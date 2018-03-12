#include <fileformats/recycledcircuitwriter.h>
#include <vector>

//void recycledcircuitwriter::writeGateText(FILE* f, recyclegate* current)
//{
//	int controlline = current->wirePointers[0]->getUpdatedWire(current->orderNrInGateList)->number;
//	int operation = current->type;
//
//	if(current->wirePointers.size() > 1)
//	{
//		/*almost certain this is a cnot, because only this should be left*/
//		int targetline = current->wirePointers[1]->getUpdatedWire(current->orderNrInGateList)->number;
//
//		fprintf(f, "%d %d %d\n", operation, controlline, targetline);
//	}
//	else if(current->wirePointers.size() == 1)
//	{
//		fprintf(f, "%d %d\n", operation, controlline);
//	}
//}

void recycledcircuitwriter::writeRecycledCircuitFile(FILE* f, causalgraph& causal)
{
	std::vector<recyclegate*> order = causal.bfs();

	long length = causal.getMaxLevel() + 1;

	std::vector<recyclegate*> inputs = causal.getRoots();
	int nrLines = inputs.size();

	for(std::vector<recyclegate*>::iterator it = order.begin(); it != order.end(); it++)
	{
//		writeGateText(f, *it);
		std::string gateText = (*it)->toString();
		fprintf(f, "%s\n", gateText.c_str());
	}
}

std::string recycledcircuitwriter::getRecycledFileName(const char* basisfilename)
{
	std::string ret(basisfilename);
	ret += ".rec";
	return ret;
}
