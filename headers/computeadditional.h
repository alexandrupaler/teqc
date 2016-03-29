#ifndef ADDITIONAL_H__
#define ADDITIONAL_H__

#include <stdio.h>
#include <math.h>

/**
 * The class computes the number of additional distillation boxes required
 * in a circuit if the failure rate per distillation box is known, and for
 * each gate using distillations a certain failure rate needs to be achieved.
 */
class computeadditional
{
public:
	/**
	 * Computes the number of combinations.
	 * @param n size of large set
	 * @param k size of sub-set
	 * @return the number C_n^k
	 */
	double comb(int n, int k);

	/**
	 * Computes the cumulative distribution function.
	 * @param nrGates is the size of the sub-set
	 * @param nrDistillations is the size of the large set
	 * @param failProb the failure probability of an element in the large set
	 * @return the cmf
	 */
	double sum(int nrGates, int nrDistillations, double failProb);

//	int findMiddle( int tgates, int astates, double afail, double targetprob);

	/**
	 * Computes the number of distillation boxes required for achieving a lower
	 * per gate failure probability.
	 * @param nrGates the number of gates existing in the circuit
	 * @param failProb the probability that a distillation box fails
	 * @param targeProb the target probability to achieve
	 * @return the number of necessary distillation boxes required for achieving targetProb
	 */
	int findParam(int nrGates, double failProb, double targeProb);
};

#endif
