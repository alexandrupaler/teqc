#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "fileformats/generaldefines.h"
#include "connectpins.h"

using namespace std;

int connectpins::getCircuitPinIndex(coordinate& coord)
{
	//assumes coordinates are only 0+2, 4+6, 8+10
	return (coord[CIRCUITHEIGHT]%4)/2;//functioneaza?
}

//returns 1 if Y: on the left side
//2 if initial distillation: normal
// 0 if A: on the right side
int connectpins::getAfterFail2(coordinate& coordDist, coordinate& coordCirc)
{
	//dist pin X - coord pin X
	int dif = coordDist[CIRCUITWIDTH] - coordCirc[CIRCUITWIDTH];
	int sign = ( dif==0 ? 0 : dif/abs(dif));

	if(sign == -1)
		return YBOXADD;
	else if(sign == 0)
		return NONFAILEDBOX;
	return ABOXADD;
}

coordinate connectpins::extractPoint(int part, pinpair& cl)
{
	coordinate ret(cl.getCoordinate(part));

	geom.addCoordinate(ret);

	return ret;
}

int connectpins::offsetChangeIndexAndStore(coordinate& p, int pos, int off)
{
	if(off == 0)
		return geom.coordMap[p.toString(',')];

	p[pos] += off;
	return geom.addCoordinate(p);
}

void connectpins::connect2(pinpair& coordline)
{
	coordinate circCoord = extractPoint(CIRCPIN, coordline);//from the input
	coordinate distCoord = extractPoint(DISTPIN, coordline);//from the distillationbox

	int pin = getCircuitPinIndex(circCoord);
	int additionalPin = getAfterFail2(distCoord, circCoord);

	int i1 = geom.coordMap[circCoord.toString(',')];//p1[INDEX];
	int i2 = geom.coordMap[distCoord.toString(',')];//p2[INDEX];

	int previ1 = i1;
	i1 = offsetChangeIndexAndStore(circCoord, CIRCUITDEPTH, -additionalPin*4 - DELTA*(pin==0));
	geom.addSegment(previ1, i1);

	//move upwards
	previ1 = i1;
	i1 = offsetChangeIndexAndStore(circCoord, CIRCUITHEIGHT, distCoord[CIRCUITHEIGHT] - circCoord[CIRCUITHEIGHT]);//to move to layer
	geom.addSegment(previ1, i1);

	//move horiz
	previ1 = i1;
	i1 = offsetChangeIndexAndStore(circCoord, CIRCUITWIDTH, distCoord[CIRCUITWIDTH] - circCoord[CIRCUITWIDTH]);//to move to layer
	geom.addSegment(previ1, i1);

	geom.addSegment(i2, i1);
}

void connectpins::processPins(char* fname)
{
	FILE* fp = fopen(fname, "r");

	while(!feof(fp))
	{
		pinpair cl;
		for(int i=0; i<6; i++)
		{
			long nr = -10000;

			fscanf(fp, "%ld", &nr);
			cl[OFFSETNONCOORD + i] = nr;
		}

		if(!feof(fp))
			connect2(cl);
	}

	fclose(fp);
}
