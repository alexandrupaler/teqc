#ifndef CIRCUITGEOMETRY_H_
#define CIRCUITGEOMETRY_H_

#include "recycling/recyclegate.h"
#include "recycling/bfsstate.h"
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
    bool inputConnectionDown;
    std::vector<std::vector<long > > lastIndex;
    //use a lastindex for duals
    std::vector<long> lastDualIndices;

    bool useBridge;

public:
    circuitgeometry()
    {
        inputConnectionDown = false;
        useBridge = false;
    }

    void liSetAxisValueAndAddSegment(int wire, int indexPos, int axis, int value);
    void liAddAxisOffsetAndAddSegment(int wire, int indexPos, int axis, int offset);
    void liAddAxisOffset(int wire, int indexPos, int axis, int offset);
    void liInitCurrPrimal(int i, int j);
    void liInitCurrDual(int i, int j);
    void liCopyCurrsOverPrevs(int wire);
    void liAddPinPair(int wire, int ioIndex, int type);

    void addPinPair(int ioIndex, circuitmatrix & paramCirc, int i, int j);
    int getDepthShift();
    int getPrevHalfDelta(int direction);
    int getInputCoordAxis();
    int liAddIOPoint2(int wire);

//    void makeGeometryFromCircuit(circuitmatrix & paramCirc);
//    void makeGeometryFromCircuit(causalgraph& causal, int minlevel, int maxlevel);
//    void makeGeometryFromCircuit(causalgraph& causal, inputLevelInfo& mininfo, inputLevelInfo& maxinfo);
    void makeGeometryFromCircuit(bfsState& state);
    void addDual(recyclegate* operationPtr);



    void liDualAddAxisOffsetAndAddSegment(int indexPos, int axis, int offset);
    void liDualAddAxisOffset(int indexPos, int axis, int offset);

private:
	int liInsertIOPointUsingCurr1(int wire, bool isInit);
	void liConnectPrevsWithCurrs(int wire);
	void liConnectIOPointToCurrs(int wire, int& ioIndex, bool connectionDown);
};


#endif /* CIRCUITGEOMETRY_H_ */
