#include "computeadditional.h"

double computeadditional::comb(int n, int k)
{
	//calculeaza n!/k!*(n-k)! si returneaza drept double
	double ret = 1;

	for(int i=1; i<=k; i++)
	{
		double mult = ((double)(n-(i-1))/(double)i);
		ret = ret * mult;
	}

	return ret;
}

double computeadditional::sum(int nrGates, int nrDistillations, double failProb)
{
//	int dif = nrDistillations - nrGates;
	double prob = 0;
	for (int i = nrGates; i <= nrDistillations; i++)
	{
		double cmb = comb(nrDistillations, i);
		double term = cmb * pow(failProb, nrDistillations - i) * pow(1 - failProb, i);
		prob += term;
	}
	return prob;
}

int computeadditional::findParam(int nrGates, double failProb, double targetProb)
{
	int astates = nrGates;
	//while (sum(tgates, astates, afail) <= 1 - targetprob)
	//{
	//	astates *= 2;
	//}
	//int total = findMiddle(tgates, astates, afail, 1 - targetprob);


	double targetprob2 = 1 - targetProb;
	double prob = sum(nrGates, astates, failProb);

	while(prob < targetprob2)
	{
		astates++;
		prob = sum(nrGates, astates, failProb);
	}
	int total = astates;

	return total;
}
