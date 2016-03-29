#include <stdio.h>

#include <vector>

#include "fileformats/rawfilereader.h"

long rawfilereader::getMaxQubit(long currMaxQubit, vector<long> qubitList)
{
	//the reader reads indices, whereas I need numbers, such that +1
	long ret = currMaxQubit;
	for(vector<long>::iterator it = qubitList.begin(); it != qubitList.end(); it++)
	{
		if(ret < *it + 1)
			ret = *it + 1;
	}
	return ret;
}

rawdata rawfilereader::readRawFile(FILE* file)
{
	rawdata raw;
	raw.nrGates = 0;
	raw.nrToffoli = 0;
	raw.nrQubits = 0;

	while (!feof(file))
	{
		char gate = readGate(file);
		if(gate == CH_NOGATE)
			break;

		long qubit =readFirstQubit(file);
		if(qubit == -1)
			break;

		raw.nrGates++;
		raw.nrQubits = (qubit + 1) > raw.nrQubits ? (qubit + 1) : raw.nrQubits;

		vector<long> others;
		//if cnot or Toffoli, there are other qubits after it
		if (gate == CH_CNOT || gate == CH_TOFFOLI)
		{
			readOtherQubits(file, others);
			raw.nrQubits = getMaxQubit(raw.nrQubits, others);
			//nrQubits = getMaxQubit(file, nrQubits);
			raw.nrGates++;//why?
		}
		if (gate == CH_TOFFOLI)
			raw.nrToffoli++;

		//append line from file to the rawfile
		fileline line;
		line.push_back(gate);
		line.push_back(qubit);
		line.insert(line.end(), others.begin(), others.end());
		raw.fileLines.push_back(line);
	}

	return raw;
}

char rawfilereader::readGate(FILE* file)
{
	char gate = CH_NOGATE;

	while(!feof(file) && gate == CH_NOGATE)
	{
		fscanf(file, "%c", &gate);
		if ((gate=='\r') || (gate=='\n') || feof(file))
		{
			//continue;//empty line
			gate = CH_NOGATE;
		}
	}

	return gate;
}

long rawfilereader::readFirstQubit(FILE* file)
{
	long qubit = -1;

	fscanf(file, "%ld", &qubit);
	qubit -= 1 ;//indices from zero?

	if(feof(file))
		return -1;

	return qubit;
}

void rawfilereader::readOtherQubits(FILE* file, vector<long>& qubitNumbers)
{
	char c = '?';
	while (!feof(file) && (c!='\r') && (c!='\n'))
	{
		long val = 0;
		c = getc(file);
		if(c == EOF || (c=='\r') || (c=='\n'))
			continue;

		fscanf(file, "%ld", &val);
		qubitNumbers.push_back(val - 1);//indices from zero?
	}
}

//rawfile rawfilereader::readOldRawFile(const char* fname)
//{
//	int nrQubits = 0;
//	int nrToffoli = 0;
//	int nrGates = 0;
//
//	rawfile raw;
//
//	//read the file just to know the number of qubits and gates;
//	//readNrQubits(fname, raw.nrGates, raw.nrQubits, raw.nrToffoli);
//
//	FILE* file = fopen(fname, "r");
//
//	//vector<long> qubitNumbers;
//	//int currGateNr = 0;
//	while(!feof(file))
//	{
//		//qubitNumbers.clear();
//
//		char gate = readGate(file);
//		if(gate == '?')
//			break;
//
//		//long qubit = readFirstQubit(file);
//		//if(qubit == -1)
//			//break;
//
//		vector<long> qubs;
//		readOtherQubits(file, qubs);
//
//		//currGateNr++;
//
//		vector<long> line;
//		line.push_back(gate);
//		line.insert(line.end(), qubs.begin(), qubs.end());
//
//		raw.fileLines.push_back(line);
//	}
//
//	fclose(file);
//
//	return raw;
//}
