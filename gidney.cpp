#include "gidney.h"
#include "utils/trim.h"
#include "stdio.h"

std::vector<std::string> gidney::makeCircuit(int nrqubits)
{
	std::vector<std::string> circuit;

	/*
	 * first half of the circuit
	 */
	std::vector<std::string> finalCnots;
	for(int i = 0; i < nrqubits; i++)
	{
		std::string prevCarry = intToString(3 * i - 1);
		std::string bit1 = intToString(3 * i + 0);
		std::string bit2 = intToString(3 * i + 1);
		std::string bit3 = intToString(3 * i + 2);

		if(i == 0)
		{
			circuit.push_back("K " + bit1 + " " + bit2 + " " + bit3);
		}
		else if(i == (nrqubits - 1))
		{
			circuit.push_back("c " + prevCarry + " " + bit2);
		}
		else
		{
			circuit.push_back("c " + prevCarry + " " + bit1 + " " + bit2);
			circuit.push_back("K " + bit1 + " " + bit2 + " " + bit3);
			circuit.push_back("c " + prevCarry + " " + bit3);
		}

		finalCnots.push_back("c " + bit1 + " " + bit2);
	}
	/*
	 * second half of the circuit, in reversed order
	 * and replacing the K (compute logical AND) with U (uncompute logical AND)
	 */
	int totalgates = circuit.size();
	for(int i = totalgates - 1; i >= 0; i--)
	{
		std::string gate = circuit[i];
		if(gate[0] == 'K')
		{
			gate[0] = 'U';
			printf("%s\n", gate.c_str());
		}
		circuit.push_back(gate);
	}

	circuit.insert(circuit.end(), finalCnots.begin(), finalCnots.end());

	//add input and ouput
	std::string ins = "in ";
	std::string out = "out ";

	for(int i = 0; i < nrqubits; i++)
	{
		ins += "--";
		out += "--";
		if(i != nrqubits - 1)
		{
			ins += "+";
			out += "x";
		}
	}
	circuit.insert(circuit.begin(), out);
	circuit.insert(circuit.begin(), ins);

	return circuit;
}
