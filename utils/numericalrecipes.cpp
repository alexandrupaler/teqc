/* (C) Copr. 1986-92 Numerical Recipes Software 3#Q)$. */


#include <math.h>
#include <stdlib.h>

#include "utils/numericalrecipes.h"

#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

	float pchisq(float a, float x) {
		void gcf(float *gammcf, float a, float x, float *gln);
		void gser(float *gamser, float a, float x, float *gln);
		void nrerror(char error_text[]);
		float gamser, gammcf, gln;
		/*
		 if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine gammq");
		 */
		if (x < (a + 1.0)) {
			gser(&gamser, a, x, &gln);
			return 1.0 - gamser;
		} else {
			gcf(&gammcf, a, x, &gln);
			return gammcf;
		}
	}

	float gammln(float xx) {
		double x, y, tmp, ser;
		static double cof[6] = { 76.18009172947146, -86.50532032941677,
				24.01409824083091, -1.231739572450155, 0.1208650973866179e-2,
				-0.5395239384953e-5 };
		int j;

		y = x = xx;
		tmp = x + 5.5;
		tmp -= (x + 0.5) * log(tmp);
		ser = 1.000000000190015;
		for (j = 0; j <= 5; j++)
			ser += cof[j] / ++y;
		return -tmp + log(2.5066282746310005 * ser / x);
	}

	void gcf(float *gammcf, float a, float x, float *gln) {
		float gammln(float xx);
		/*
		 void nrerror(char error_text[]);
		 */
		int i;
		float an, b, c, d, del, h;

		*gln = gammln(a);
		b = x + 1.0 - a;
		c = 1.0 / FPMIN;
		d = 1.0 / b;
		h = d;
		for (i = 1; i <= ITMAX; i++) {
			an = -i * (i - a);
			b += 2.0;
			d = an * d + b;
			if (fabs(d) < FPMIN)
				d = FPMIN;
			c = b + an / c;
			if (fabs(c) < FPMIN)
				c = FPMIN;
			d = 1.0 / d;
			del = d * c;
			h *= del;
			if (fabs(del - 1.0) < EPS)
				break;
		}
		/*
		 if (i > ITMAX) nrerror("a too large, ITMAX too small in gcf");
		 */
		*gammcf = exp(-x + a * log(x) - (*gln)) * h;
	}

	void gser(float *gamser, float a, float x, float *gln) {
		float gammln(float xx);
		/*
		 void nrerror(char error_text[]);
		 */
		int n;
		float sum, del, ap;

		*gln = gammln(a);
		if (x <= 0.0) {
			/*
			 if (x < 0.0) nrerror("x less than 0 in routine gser");
			 */
			*gamser = 0.0;
			return;
		} else {
			ap = a;
			del = sum = 1.0 / a;
			for (n = 1; n <= ITMAX; n++) {
				++ap;
				del *= x / ap;
				sum += del;
				if (fabs(del) < fabs(sum) * EPS) {
					*gamser = sum * exp(-x + a * log(x) - (*gln));
					return;
				}
			}
			/*
			 nrerror("a too large, ITMAX too small in routine gser");
			 */
			return;
		}
	}

	/**
	 * Returns the incomplete beta function Ix(a, b).
	 */
	float betai(float a, float b, float x) {
		float betacf(float a, float b, float x);
		float gammln(float xx);
		void nrerror(char error_text[]);
		float bt;
//		if (x < 0.0 || x > 1.0)
//			nrerror("Bad x in routine betai");
		if (x == 0.0 || x == 1.0)
			bt = 0.0;
		else
			//Factors in front of the continued fraction.
			bt = exp(
					gammln(a + b) - gammln(a) - gammln(b) + a * log(x)
							+ b * log(1.0 - x));
		if (x < (a + 1.0) / (a + b + 2.0)) //Use continued fraction directly.
			return bt * betacf(a, b, x) / a;
		else
			//Use continued fraction after making the sym
			return 1.0 - bt * betacf(b, a, 1.0 - x) / b; //metry transformation.
	}

	float betacf(float a, float b, float x)
	//Used by betai: Evaluates continued fraction for incomplete beta function by modified Lentz’s
	//method (§5.2).
	{
		void nrerror(char error_text[]);
		int m, m2;
		float aa, c, d, del, h, qab, qam, qap;
		qab = a + b; // These q’s will be used in factors that occur
		qap = a + 1.0; // in the coefficients (6.4.6).
		qam = a - 1.0;
		c = 1.0; //First step of Lentz’s method.
		d = 1.0 - qab * x / qap;
		if (fabs(d) < FPMIN)
			d = FPMIN;
		d = 1.0 / d;
		h = d;
		for (m = 1; m <= ITMAX; m++) {
			m2 = 2 * m;
			aa = m * (b - m) * x / ((qam + m2) * (a + m2));
			d = 1.0 + aa * d; // One step (the even one) of the recurrence.
			if (fabs(d) < FPMIN)
				d = FPMIN;
			c = 1.0 + aa / c;
			if (fabs(c) < FPMIN)
				c = FPMIN;
			d = 1.0 / d;
			h *= d * c;
			aa = -(a + m) * (qab + m) * x / ((a + m2) * (qap + m2));
			d = 1.0 + aa * d; // Next step of the recurrence (the odd one).
			if (fabs(d) < FPMIN)
				d = FPMIN;
			c = 1.0 + aa / c;
			if (fabs(c) < FPMIN)
				c = FPMIN;
			d = 1.0 / d;
			del = d * c;
			h *= del;
			if (fabs(del - 1.0) < EPS)
				break; // Are we done?
		}
//		if (m > ITMAX)
//			nrerror("a or b too big, or MAXIT too small in betacf");
		return h;
	}
