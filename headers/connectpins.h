#ifndef CONNECTPINS_H_
#define CONNECTPINS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "numberandcoordinate.h"
#include "geometry.h"
#include "faildistillations.h"

#define DISTPIN 1
#define CIRCPIN 0

#define NONFAILEDBOX 2
#define ABOXADD 0
#define YBOXADD 1
#define INDEX 0

using namespace std;

class connectpins
{
public:
	geometry geom;

public:
	void processPins(char* fname);

	int getCircuitPinIndex(coordinate& coord);

	int getAfterFail2(coordinate& coordDist, coordinate& coordCirc);

	coordinate extractPoint(int part, pinpair& cl);

	int offsetChangeIndexAndStore(coordinate& p, int pos, int off);

	void connect2(pinpair& coordline);

};
#endif /* CONNECTPINS_H_ */
