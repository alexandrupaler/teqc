#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
#include <climits>
#include <vector>
#include <string>

#include "gatenumbers.h"
#include "circconvert.h"
#include "utils/trim.h"

#include "fileformats/infilereader.h"
#include "fileformats/gmlfilewriter.h"

circconvert::circconvert()
{

}

/**
 * The method does not add any wires
 * @param causal
 */
void circconvert::replaceNonICM2(causalgraph& causal)
{

	/*
	 * 02.01.2018
	 * test: move the "r" gates to the left two positions
	 */
	bool firstR = true;

	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
			it != causal.tmpCircuit.end(); it++)
	{
		char type = (*it)->type;
		bool replace = false;
		std::vector<std::string> decomposition;

		if(type == 'T')
		{
			std::string g[] = {"h 2", "c 1 2", "t 2", "c 0 2", "t 2", "c 1 2", "t 2", "c 0 2", "t 1", "t 2", "c 0 1", "h 2", "t 1", "c 0 1", "t 0", "p 1"};
			decomposition = std::vector<std::string> (g, g + 16);
			replace = true;
		}
		else if(type == 'h')
		{
			std::string g[] = {"p 0", "v 0", "p 0"};
			decomposition = std::vector<std::string> (g, g + 3);
			replace = false;
		}
		else if (type == 'K')
		{
			//compute logical AND from Gidney adder
			/*
			 * r is a t gate without S gate correction
			 */
//			std::string g[] = {"r 2", "c 0 2", "c 1 2", "c 2 1 0", "t 0", "t 1", "t 2", "c 2 1 0", "h 2", "p 2"};
//			std::string g[] = {"r 2", "c 0 2", "c 1 2", "c 2 1 0", "t 0", "t 1", "c 2 1 0", "t 2", "h 2", "p 2"};
//			std::string g[] = {"r 2", "c 1 2", "c 0 1 2", "t 1", "c 0 2", "t 2", "c 0 1 2", "t 2", "h 2", "p 2"};

			//versiune 02.01.2018
//			std::string g[] = {"r 2", "c 0 2", "t 2 ", "c 1 2", "c 0 2", "t 2", "c 0 2", "t 2", "h 2", "p 2"};
			//versiunea 03.01.2018
//			std::string g[] = {"r 2", "c 0 2", "t 2 ", "c 1 2", "t 2", "c 0 2", "t 2", "c 0 2", "h 2", "p 2"};
//			std::string g[] = {"r 2", "c 0 2", "c 1 2", "c 2 1 0", "r 0", "r 1", "r 2", "c 2 1 0", "h 2", "p 2"};
			std::string g[] = {"r 2", "c 0 2", "c 1 2", "c 2 1 0", "r 0", "r 1", "r 2", "s 2", "c 2 1 0", "v 2"};/*h is like a V gate*/


			decomposition = std::vector<std::string> (g, g + 10);
			replace = true;
		}
		else if(type == 'U')
		{
			//uncompute logical AND from Gidney adder
			//the CZ is not conditional on any measurement. it is just added.
			std::string g[] = {"h 1", "c 0 1", "h 1"};
			decomposition = std::vector<std::string> (g, g + 3);
			replace = true;
		}

		if(replace)
		{
			for(size_t i = 0; i < decomposition.size(); i++)
			{
				std::string command = decomposition[i];
				if(decomposition[i][0] == 'r')
				{
					command[0] = 't';
				}
				//replaced above with this
				causal.constructGate2(command, (*it)->wirePointers, it);


//				02jan2018
//				if(decomposition[i][0] == 'r')
//				{
//					if(!firstR)
//					{
//						std::list<recyclegate*>::iterator itfirst  = it;
//						itfirst--;
//						std::list<recyclegate*>::iterator itsecond  = itfirst;
//						itsecond--;
//						itsecond--;
//						std::iter_swap(itfirst, itsecond);
//					}
//					firstR = false;
//				}

				/*add potential S gate correction for the T gate*/
				if(decomposition[i][0] == 't')
				{
					std::string correction = decomposition[i];
					correction[0] = 'p';
					causal.constructGate2(correction, (*it)->wirePointers, it);
				}

			}

			//delete the old gate
			it = causal.tmpCircuit.erase(it);//advances the iterator
			it--;
		}
	}
}

//void circconvert::configInputs(int qubit, std::string ins)
//{
//	for(size_t i=qubit; i<qubit + ins.size(); i++)
//	{
//		if(ins[i - qubit] != '_')
//		{
//			std::list<char>::iterator it = inputs.begin();
//			std::advance(it, i);
//			*it = ins[i - qubit];
////			inputs[i] = ins[i - qubit];
//		}
//	}
//}
//
//void circconvert::configOutputs(int qubit, std::string outs)
//{
//	for(size_t i=qubit; i<qubit + outs.size(); i++)
//	{
//		if(outs[i - qubit] != '_')
//		{
////			outputs[i] = outs[i - qubit];
//			std::list<char>::iterator it = outputs.begin();
//			std::advance(it, i);
//			*it = outs[i - qubit];
//		}
//	}
//}
//
//void circconvert::replaceICM()
//{
//	for(std::list<recyclegate>::iterator it=gates.begin(); it != gates.end(); it++)
//	{
//		if(it->type == 'p')
//		{
//			int qubit = it->wires[0];
//
//			int nrqubits = 1;
//
//			std::string ins = "_y";
//			std::string outs = "x_";
//
//			includeWires(nrqubits, qubit + 1);
//			updateWiresStartingFromGate(it, qubit, 1);
//
//			//a bagat inainte de qubit
//			configInputs(qubit, ins);
//			configOutputs(qubit, outs);
//
//			std::string g[] = {"c 1 0"};
//			recyclegate gate(g[0], it->wirePointers);
//
//			std::map<int, int> dict;
//			dict[0] = qubit;
//			dict[1] = qubit + 1;
//			gate.replaceWires(dict);
//
//			gates.insert(it, gate);
//
//			it = gates.erase(it);
//			it--;
//		}
//		else if(it->type == 'v')
//		{
//			int qubit = it->wires[0];
//
//			std::string ins = "_y";
//			std::string outs = "z_";
//
//			includeWires(1, qubit + 1);
//			updateWiresStartingFromGate(it, qubit, 1);
//
//			configInputs(qubit, ins);
//			configOutputs(qubit, outs);
//
//			std::string g[] = {"c 0 1"};
//			recyclegate gate(g[0], it->wirePointers);
//
//			std::map<int, int> dict;
//			dict[0] = qubit;
//			dict[1] = qubit + 1;
//			gate.replaceWires(dict);
//
//			gates.insert(it, gate);
//
//			it = gates.erase(it);
//			it--;
//		}
//		else if(it->type == 't')
//		{
//			int qubit = it->wires[0];
//
//			std::string ins = "_a0y0+";
//			std::string outs = "x*#*#_";
//
//			includeWires(5, qubit + 1);
//			updateWiresStartingFromGate(it, qubit, 5);
//
//			configInputs(qubit, ins);
//			configOutputs(qubit, outs);
//
//			std::map<int, int> dict;
//			for(int i=0; i<=5; i++)
//				dict[i] = qubit + i;
//
//			std::string g[] = {"c 1 0", "c 1 2", "c 3 1", "c 4 2", "c 3 5", "c 4 5"};
//			for(int i=0; i<6; i++)
//			{
//				recyclegate gate(g[i], it->wirePointers);
//				gate.replaceWires(dict);
//				gates.insert(it, gate);
//			}
//
//			it = gates.erase(it);
//			it--;
//		}
//	}
//}

void circconvert::replaceICM2(causalgraph& causal)
{
	int roundx = 0;

//	gmlfilewriter gwr;
//	std::string fname22 = gmlfilewriter::getGMLFileName("test", roundx++);
//	FILE* filegwr = fopen(fname22.c_str(), "w");
//	gwr.writeGMLFile(filegwr, causal);
//	fclose(filegwr);


	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
				it != causal.tmpCircuit.end(); it++)
	{
		bool replaceICMGate = false;

		size_t currentGateId = (*it)->orderNrInGateList;
		std::string ins = "";
		std::string outs = "";
		std::vector<std::string> gateFormation;
		int nrCuts = -1;

		if( ((*it)->type == 'p') || ((*it)->type == 'v') || ((*it)->type == 't'))
		{
			replaceICMGate = true;
		}

		if((*it)->type == 'p')
		{
			nrCuts = 1;
			ins = "_y";
			outs = "x_";

			std::string g[] = {"c 1 0"};
			gateFormation = std::vector<std::string>(g, g + 1);
		}
		else if((*it)->type == 'v')
		{
			nrCuts = 1;
			ins = "_y";
			outs = "z_";

			std::string g[] = {"c 0 1"};
			gateFormation = std::vector<std::string>(g, g + 1);
		}
		else if((*it)->type == 't')
		{
			nrCuts = 5;
			ins = "_a0y0+";
			outs = "x*#*#_";

			std::string g[] = {"c 1 0", "c 1 2", "c 3 1", "c 4 2", "c 3 5", "c 4 5"};
			gateFormation = std::vector<std::string>(g, g + 6);
		}

		if(replaceICMGate)
		{
			/*
			 * WIRE OPERATIONS
			 */
			wireelement* wireToCut = (*it)->wirePointers[0]->getUpdatedWire(currentGateId);
			std::vector<wireelement*> newWires;
			for(int i=0; i < 1 /*nrCuts*/; i++)
			{
				std::vector<wireelement*> tmpWires = causal.splitWire2(wireToCut, it);
				newWires.push_back(tmpWires[0]);
				wireToCut = tmpWires[1];
			}
			newWires.push_back(wireToCut);

			/*
			 * INSERT ADDITIONAL WIRES: which is better?
			 */
			causal.insertWires2(newWires, nrCuts - 1);

			/*
			 * CONFIGURE INITS AND MEAS
			 */
			std::vector<recyclegate*> newGates = causal.insertMeasurementAndInitialisation2(newWires, ins, outs, it);

			/*
			 * AFTER insertMeasurementAndInitialisation2 the iterator it
			 * points to the first measurement. complicat de explicat?
			 */
			causal.insertGates2(gateFormation, newWires, it);

//			gmlfilewriter gwr;//	gmlfilewriter gwr;
//			//	std::string fname22 = gmlfilewriter::getGMLFileName(argv[1], 0);
//			//	FILE* filegwr = fopen(fname22.c_str(), "w");
//			//	gwr.writeGMLFile(filegwr, causalG);
//			//	fclose(filegwr);
//			//
//			//	fname22 = gmlfilewriter::getGMLFileName(argv[1], 1);
//			//	filegwr = fopen(fname22.c_str(), "w");
//			//	gwr.writeGMLFile2(filegwr, causalG);
//			//	fclose(filegwr);
//			std::string fname22 = gmlfilewriter::getGMLFileName("test", roundx++);
//			FILE* filegwr = fopen(fname22.c_str(), "w");
//			gwr.writeGMLFile(filegwr, causal);
//			fclose(filegwr);
//
//			causal.printGateList2();
		}
	}
}

//void circconvert::includeWires(int howMany, int where)
//{
////	vector<char> empty(howMany, '-');
////	inputs.insert(inputs.begin() + where, empty.begin(), empty.end());
////	outputs.insert(outputs.begin() + where, empty.begin(), empty.end());
//
//	std::list<char> empty(howMany, '-');
//	std::list<char>::iterator it = inputs.begin();
//	std::advance(it, where);
//	inputs.insert(it, empty.begin(), empty.end());
//
//	it = outputs.begin();
//	advance(it, where);
//	outputs.insert(it, empty.begin(), empty.end());
//}
//
//void circconvert::includeWires2(int howMany, int where)
//{
//	std::list<char> empty(howMany, '-');
//	std::list<char>::iterator it = inputs.begin();
//	std::advance(it, where);
//	inputs.insert(it, empty.begin(), empty.end());
//
//	it = outputs.begin();
//	std::advance(it, where);
//	outputs.insert(it, empty.begin(), empty.end());
//}

//void circconvert::makeAncillaInput(int qubit)
//{
//	configInputs(qubit, "0");
//}
//
//void circconvert::makeAncillaOutput(int qubit)
//{
//	configInputs(qubit, "-");
//}
//
//void circconvert::makeComputeInput(int qubit)
//{
//	configInputs(qubit, "1");
//}
//
//void circconvert::makeComputeOutput(int qubit)
//{
//	configInputs(qubit, "-");
//}

//TODO: Use in INFILEREADER
//void circconvert::print()
//{
//	std::string ins(inputs.size(), ' ');
//	std::string outs(outputs.size(), ' ');
//
//	int i=0;
//	for(std::list<char>::iterator it = inputs.begin();
//			it != inputs.end(); it++)
//	{
//		ins[i] = *it;
//		i++;
//	}
//
//	i=0;
//	for(std::list<char>::iterator it = outputs.begin();
//			it != outputs.end(); it++)
//	{
//		outs[i] = *it;
//		i++;
//	}
//
//	printf("inputs %s\n", ins.c_str());
//	printf("outputs %s\n", outs.c_str());
//
//	for(std::list<recyclegate>::iterator it=gates.begin(); it!=gates.end(); it++)
//	{
//		printf("%c ", it->type);
//		//for(int i=0; i<it->wires.size(); i++)
//		for(std::vector<int>::iterator wit = it->wires.begin(); wit != it->wires.end(); wit++)
//			printf("%d ", *wit);
//		printf("\n");
//	}
//}
