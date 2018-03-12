#include "computeadditional.h"
#include "utils/numericalrecipes.h"

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

//extern "C"
//{
//	float computeadditional::gammln(float xx)
//	{
//		double x,y,tmp,ser;
//		static double cof[6]={76.18009172947146,-86.50532032941677,
//		24.01409824083091,-1.231739572450155,
//		0.1208650973866179e-2,-0.5395239384953e-5};
//		int j;
//		y=x=xx;
//		tmp=x+5.5;
//		tmp -= (x+0.5)*log(tmp);
//		ser=1.000000000190015;
//		for (j=0;j<=5;j++) ser += cof[j]/++y;
//		return -tmp+log(2.5066282746310005*ser/x);
//	}
//
//	double computeadditional::bico (int n, int k)
//	{
//		return floor(0.5 + exp(factln(n) - factln(k) - factln(n-k)));
//	}
//
//	double computeadditional::factln(int n)
//	{
//		static double a[101];
//		if(n < 0)
//		{
//			printf("COMPUTEADDITIONAL with factln negative %d\n", n);
//			return -1;
//		}
//		else if(n <= 1)
//		{
//			return 0.0;
//		}
//		else if(n <= 100)
//		{
//			return a[n] ? a[n] : (a[n] = gammln(n + 1.0));
//		}
//		else
//		{
//			return gammln(n + 1.0);
//		}
//	}
//}

double computeadditional::sum2(int nrGates, int nrDistillations, double failProb)
{
//	double prob = 0;
//
//	printf("%f %f\n", factln(4), exp(factln(4)));
//
//	for (int i = nrGates; i <= nrDistillations; i++)
//	{
//		float log1 = log(failProb) * nrDistillations - i;//log(pow(failProb, nrDistillations - i));
//		float log2 = log(1- failProb) * i;//log(pow(1 - failProb, i));
//
//		float term = exp(factln(nrDistillations) - factln(i) - factln(nrDistillations-i) + log1 + log2);
//
//		prob += term;
//	}
//
//	return prob;

	/* Suppose an event occurs with probability p per trial. Then the probability P of
its occurring k or more times in n trials is termed a cumulative binomial probability,
and is related to the incomplete beta function Ix(a, b) as follows: Ip(k, n − k + 1)

	float betai(float a, float b, float x)

	will be

	float betai(float k, float n - k + 1, float p)

	where

	n trials = nrDistillations

	k or more times = nrGates
	 */

	return betai( (float)nrGates, (float)(nrDistillations - nrGates + 1), (float)failProb);
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
//	double prob = sum(nrGates, astates, failProb);
	double prob = sum2(nrGates, astates, failProb);

	while(prob < targetprob2)
	{
		astates++;
		prob = sum2(nrGates, astates, failProb);
	}
	int total = astates;

	return total;
}
