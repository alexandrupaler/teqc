#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cuccaro.h"

string cuccaro::cnot(int c, int t)
{
	char g[1024];
	sprintf(g, "c %d %d\n", c, t);
	circuit.push_back(g);
	return g;
}

string cuccaro::tof(int c1, int c2, int t)
{
	char g[2014];
	sprintf(g, "T %d %d %d\n", c1, c2, t);
	circuit.push_back(g);
	return g;
}

string cuccaro::xnot(int t)
{
	char g[1024];
	sprintf(g, "X %d\n", t);
	circuit.push_back(g);
	return g;
}

int cuccaro::b(int i)
{
	return 2*i + 1;// + 1;//+1 because of indexing from 1
}

int cuccaro::a(int i)
{
	return 2*i;// + 1;//+1 because of indexing from 1
}

vector<string> cuccaro::makeCircuit(int nrb, int doxnot)
{
	circuit.clear();

	int total = 2*nrb;//doua numere cu nrb biti

	//positions of ancilla and sum bits
	int ancilla = total;// + 1;//+1 because of indexing from 1
	int sum = total + 1;//+1 because of indexing from 1

	string input(total + 2, '-');
	input[sum] = '0';
	input = "in " + input;
	circuit.push_back(input);

	string output(total + 2, '-');
	output[ancilla] = '1';
	output = "out " + output;
	circuit.push_back(output);

	for(int i=1; i<=nrb-1; i++)
	{
		cnot(a(i), b(i));
	}

	cnot(a(1), ancilla);

	tof(a(0), b(0), ancilla); cnot(a(2), a(1));
	tof(ancilla, b(1), a(1)); cnot(a(3), a(2));

	for(int i=2; i<=nrb-3; i++)
	{
		tof(a(i-1), b(i), a(i));
		cnot(a(i+2), a(i+1));
	}

	tof(a(nrb-3),b(nrb-2),a(nrb-2)); cnot(a(nrb-1), sum);

	tof(a(nrb-2), b(nrb-1), sum);

	for(int i=1; i<=nrb-2; i++)
		if(doxnot) xnot(b(i));

	cnot(ancilla, b(1));

	for(int i=2; i<=nrb-1; i++)
		cnot(a(i-1), b(i));

	tof(b(nrb-2), a(nrb-3), a(nrb-2));

	for(int i=nrb-3; i>=2; i--)
	{
		tof(a(i-1), b(i), a(i));
		cnot(a(i+2), a(i+1));
		if(doxnot) xnot(b(i+1));
	}

	tof(ancilla, b(1), a(1)); cnot(a(3), a(2)); if(doxnot) xnot(b(2));

	tof(a(0), b(0), ancilla); cnot(a(2), a(1)); if(doxnot) xnot(b(1));

	cnot(a(1), ancilla);

	for(int i=0; i<=nrb-1; i++)
		cnot(a(i), b(i));

	return circuit;
}
