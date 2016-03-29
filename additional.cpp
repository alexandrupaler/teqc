#include <stdio.h>
#include <math.h>
#include "computeadditional.h"

int main(int argc, char** argv)
{
	int tgates = 7;
	int astates = tgates;

	double targetprob = .000001;
	double afail = 0.2;

	computeadditional cp;
	int total = cp.findParam(tgates, afail, targetprob);

	printf("next total %d\n", total);
}
