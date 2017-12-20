#include "fileformats/gmlfilewriter.h"

std::string gmlfilewriter::getGMLFileName(const char* basisfilename, int nrconnect)
{
	char fname[1024];
	sprintf(fname, "%s_%03d.raw.gml", basisfilename, nrconnect);

	return std::string(fname);
}

void gmlfilewriter::writeGMLFile(FILE* fileg, causalgraph& causal)
{
	causal.computeLevels();
	causal.updateLabels();

	fprintf(fileg, "graph [\n");
	fprintf(fileg, "directed 1\n");

	std::map<void*, int> ids;

	for(std::list<wireelement*>::iterator it=causal.memWireElements.begin();
			it!=causal.memWireElements.end(); it++)
	{
		ids[*it] = ids.size();

		fprintf(fileg, "node [\n");
		fprintf(fileg, " id %d\n label \"%d\" \n", ids[*it], (*it)->threshold);
		fprintf(fileg, "]\n");
	}

	for(std::list<recyclegate*>::iterator it=causal.tmpCircuit.begin();
				it!=causal.tmpCircuit.end(); it++)
	{
		ids[*it] = ids.size();

		fprintf(fileg, "node [\n");
		fprintf(fileg, " id %d\n label \"g%d_%d_%d_%d\" \n", ids[*it], (*it)->orderNrInGateList,
				(*it)->type,
				(*it)->willPush.size(),
				(*it)->pushedBy.size());
		fprintf(fileg, "]\n");
	}

	for(std::list<wireelement*>::iterator it=causal.memWireElements.begin();
			it!=causal.memWireElements.end(); it++)
	{
		for(int i=0; i<2; i++)
		{
			if((*it)->ngh[i] != NULL)
			{
				fprintf(fileg, "edge [\n");
				fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[(*it)->ngh[i]]);
				fprintf(fileg, " label \"ngh\"\n");
				fprintf(fileg, "]\n");
			}
		}
		if((*it)->prev != NULL)
		{
			fprintf(fileg, "edge [\n");
			fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[(*it)->prev]);
			fprintf(fileg, "]\n");
		}
		if((*it)->next != NULL)
		{
			fprintf(fileg, "edge [\n");
			fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[(*it)->next]);
			fprintf(fileg, "]\n");
		}
	}

	for(std::list<recyclegate*>::iterator it=causal.tmpCircuit.begin();
				it!=causal.tmpCircuit.end(); it++)
	{
		fprintf(fileg, "edge [\n");
		fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[(*it)->wirePointers[0]]);
		fprintf(fileg, "]\n");
	}

	fprintf(fileg, "]");
}

//void gmlfilewriter::writeGMLFileOrder(FILE* fileg, causalgraph& causal)
//{
//	fprintf(fileg, "graph [\n");
//	fprintf(fileg, "directed 1\n");
//
//	std::map<wireorder*, int> ids;
//
//	for(std::vector<wireorder*>::iterator it=causal.memWireOrder.begin();
//			it!=causal.memWireOrder.end(); it++)
//	{
//		ids[*it] = ids.size();
//
//		fprintf(fileg, "node [\n");
//		fprintf(fileg, " id %d\n label \"%p\" \n", ids[*it], *it);
//		fprintf(fileg, "]\n");
//	}
//
//	for(std::vector<wireorder*>::iterator it=causal.memWireOrder.begin();
//				it!=causal.memWireOrder.end(); it++)
//	{
//		if((*it)->next != NULL)
//		{
//			fprintf(fileg, "edge [\n");
//			fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[(*it)->next]);
//			fprintf(fileg, "]\n");
//		}
//
//		if((*it)->prev != NULL)
//		{
//			fprintf(fileg, "edge [\n");
//			fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[(*it)->prev]);
//			fprintf(fileg, "]\n");
//		}
//	}
//}

void gmlfilewriter::writeGMLFile2(FILE* fileg, causalgraph& causal)
{
	causal.computeLevels();
	causal.updateLabels();

	fprintf(fileg, "graph [\n");
	fprintf(fileg, "directed 1\n");

	std::map<recyclegate*, int> ids;

	for(std::list<recyclegate*>::iterator it=causal.tmpCircuit.begin();
			it!=causal.tmpCircuit.end(); it++)
	{
		ids[*it] = ids.size();

		fprintf(fileg, "node [\n");
		fprintf(fileg, " id %d\n label \"%d_%d\" \n", ids[*it],
				(*it)->type,
				(*it)->orderNrInGateList);
		fprintf(fileg, "]\n");
	}

	for(std::list<recyclegate*>::iterator it=causal.tmpCircuit.begin();
			it!=causal.tmpCircuit.end(); it++)
	{
		for(std::map<int, recyclegate*>::iterator it2 = (*it)->willPush.begin();
				it2 != (*it)->willPush.end(); it2++)
		{
			fprintf(fileg, "edge [\n");
//			fprintf(fileg, " source %d\n target %d \n", (*it)->id, it2->second->id);
			//use pointer as id?
			fprintf(fileg, " source %d\n target %d \n", ids[*it], ids[it2->second]);

			fprintf(fileg, "]\n");
		}
	}

	fprintf(fileg, "]");
}
