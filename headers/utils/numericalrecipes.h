
#ifndef NUMERICALRECIPES_H__
#define NUMERICALRECIPES_H__

extern "C" {
	float pchisq(float a, float x);

	float gammln(float xx);

	void gcf(float *gammcf, float a, float x, float *gln);

	void gser(float *gamser, float a, float x, float *gln);

	float betai(float a, float b, float x);

	float betacf(float a, float b, float x);
}
#endif
