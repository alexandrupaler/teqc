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
#include "recycling/trim.h"

#include "fileformats/infilereader.h"
#include "fileformats/gmlfilewriter.h"

//void circconvert::makeGates(std::vector<std::string> file)
//{
//	//citeste inputuri
//	std::istringstream inin(file[0]);
//	std::string s;
//	inin >> s;
////	inin >> inputs;
//	inin >> s;
//	for(size_t i=0; i<s.size(); i++)
//	{
//		inputs.push_back(s[i]);
//	}
//
//	std::istringstream outout(file[1]);
//	outout >> s;
////	outout >> outputs;
//	outout >> s;
//	for(size_t i=0; i<s.size(); i++)
//	{
//		outputs.push_back(s[i]);
//	}
//
//	std::vector<wireelement*> empty;
//	for(size_t i=2; i<file.size(); i++)
//	{
//		std::string line2 = file[i];
//
//		recyclegate currGate(line2, empty);
//
//		gates.push_back(currGate);
//	}
//
//}
//
//circconvert::circconvert(std::vector<std::string>& file)
//{
//	makeGates(file);
//}
//
//circconvert::circconvert(char* fname)
//{
//	infilereader inread;
//	std::vector<std::string> file = inread.readInFile(fname);
//	makeGates(file);
//}

circconvert::circconvert()
{

}
//
//void circconvert::updateWiresStartingFromGate(std::list<recyclegate>::iterator after, int minWireNumber, int incrValue)
//{
//	//after++;
//	for(std::list<recyclegate>::iterator it=gates.begin(); it!=after; it++)
//	{
//		for(size_t j=0; j<it->wires.size(); j++)
//		{
//			if(it->wires[j] > minWireNumber)
//				it->wires[j] += incrValue;
//		}
//	}
//	for(std::list<recyclegate>::iterator it=after; it!=gates.end(); it++)
//	{
//		for(size_t j=0; j<it->wires.size(); j++)
//		{
//			if(it->wires[j] >= minWireNumber)
//				it->wires[j] += incrValue;
//		}
//	}
//}
//
//void circconvert::replaceNonICM()
//{
//	for(std::list<recyclegate>::iterator it=gates.begin(); it != gates.end(); it++)
//	{
//		if(it->type == 'T')
//		{
//			std::vector<int> wires = it->wires;
//			std::map<int, int> dict;
//			dict[0] = wires[0];
//			dict[1] = wires[1];
//			dict[2] = wires[2];
//
////			WIRE WIRE WIRE CTRL WIRE WIRE WIRE CTRL WIRE CTRL WIRE WIRE CTRL TGATE
////			WIRE CTRL WIRE WIRE WIRE CTRL WIRE WIRE TGATE TGT WIRE TGATE TGT PGATE
////			HGATE TGT TGATE TGT TGATE TGT TGATE TGT TGATE WIRE HGATE WIRE WIRE WIRE
//
//			std::string g[] = {"h 2", "c 1 2", "t 2", "c 0 2", "t 2", "c 1 2", "t 2", "c 0 2", "t 1", "t 2", "c 0 1", "h 2", "t 1", "c 0 1", "t 0", "p 1"};
//
//			for(int i=0; i<16; i++)
//			{
//				recyclegate c(g[i], it->wirePointers);
//				c.replaceWires(dict);
//
//				gates.insert(it, c);
//			}
//
//			it = gates.erase(it);
//			it--;
//		}
//		else if(it->type == 'h')
//		{
////			int nrgates = 3;
//
//			std::map<int, int> dict;
//			dict[0] = it->wires[0];
//
//			std::string g[] = {"p 0", "v 0", "p 0"};
//			for(int i=0; i<3; i++)
//			{
//				recyclegate gate(g[i], it->wirePointers);
//				gate.replaceWires(dict);
//				gates.insert(it, gate);
//			}
//
//			//delete the old gate
//			it = gates.erase(it);//advances the iterator
//			it--;
//		}
//	}
//}

/**
 * The method does not add any wires
 * @param causal
 */
void circconvert::replaceNonICM2(causalgraph& causal)
{
	for(std::list<recyclegate*>::iterator it = causal.tmpCircuit.begin();
			it != causal.tmpCircuit.end(); it++)
	{
		char type = (*it)->type;
		bool replace = false;
		std::vector<std::string> decomposition;

		if(type == 'T')
		{
//			WIRE WIRE WIRE CTRL WIRE WIRE WIRE CTRL WIRE CTRL WIRE WIRE CTRL TGATE
//			WIRE CTRL WIRE WIRE WIRE CTRL WIRE WIRE TGATE TGT WIRE TGATE TGT PGATE
//			HGATE TGT TGATE TGT TGATE TGT TGATE TGT TGATE WIRE HGATE WIRE WIRE WIRE

			std::string g[] = {"h 2", "c 1 2", "t 2", "c 0 2", "t 2", "c 1 2", "t 2", "c 0 2", "t 1", "t 2", "c 0 1", "h 2", "t 1", "c 0 1", "t 0", "p 1"};
			decomposition = std::vector<std::string> (g, g + 16);
			replace = true;
		}
		else if(type == 'h')
		{
			std::string g[] = {"p 0", "v 0", "p 0"};
			decomposition = std::vector<std::string> (g, g + 3);
			replace = true;
		}

		if(replace)
		{
			for(size_t i=0; i<decomposition.size(); i++)
			{
				recyclegate* gate = new recyclegate(decomposition[i], (*it)->wirePointers);

				causal.tmpCircuit.insert(it, gate);
				causal.memTmpCircuit.push_back(gate);
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
			 * points to the first measurement. complicat de exeplicat?
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
