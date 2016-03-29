#ifndef CIRCUITGEOMETRY_H_
#define CIRCUITGEOMETRY_H_

#include "geometry.h"
#include "circuitmatrix.h"
#include "gatenumbers.h"

#define PREV1 0
#define PREV2 1
#define CURR1 2
#define CURR2 3

/**
 * The class is a wrapper over the geometry class, and is used for
 * constructing circuit geometries. It uses a vector of size 4, called
 * lastindex, to represent vertex indices currently used in the geometry
 * construction. The method names start with the prefix "li" as a reference to
 * the indices vector.
 */
class circuitgeometry{
public:
	geometry simplegeom;

	numberandcoordinate allpins;

	//legacy
	bool inputConnectionDown;

	/**
	 * vector of 4 indices
	 */
	vector<long> lastindex;

public:

	/**
	 * Constructor.
	 */
	circuitgeometry()
	{
		inputConnectionDown = false;
		lastindex = vector<long>(4, -100);
	}

	void liSetAxisValueAndAddSegment(int indexPos, int axis, int value);
	void liAddAxisOffsetAndAddSegment(int indexPos, int axis, int offset);
	void liAddAxisOffset(int indexPos, int axis, int offset);
	void liInitCurrPrimal(int i, int j);
	void liInitCurrDual(int i, int j, circuitmatrix& circ);
	void liCopyCurrsOverPrevs();

	void makeGeometryFromCircuit(circuitmatrix& paramCirc);
	void addDual(circuitmatrix& circ, int ctrli, int ctrlj);

	void addInputPins(int ioIndex, circuitmatrix& paramCirc, int i, int j);

	int getDepthShift();

	int getInputCoordAxis();

private:
	int liInsertIOPointUsingCurr1(bool isInit);
	void liConnectPrevsWithCurrs();
	void liConnectIOPointToCurrs(int& ioIndex, bool connectionDown);
};


#endif /* CIRCUITGEOMETRY_H_ */
