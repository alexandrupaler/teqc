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

struct gridPosition
{
	long gridRow;
	long gridColumn;
	gridPosition();
	gridPosition(const gridPosition& c);
	gridPosition& operator= (const gridPosition& c);

	bool isEvenGridRow();
	bool isEvenGridColumn();

	static long totalWires;
	static long logicalQubitsPerLine;

	bool isControl;
	bool isFakeTarget;
};

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
	std::vector<long> lastDualIndices;
	bool useBridge;

	std::vector<int> lastDualIndicesAxisSegment;
	std::vector<convertcoordinate> lastTmpCoord;

	convertcoordinate minCoords;
	convertcoordinate maxCoords;

public:
	circuitgeometry()
	{
		inputConnectionDown = false;
		useBridge = false;

		resetMinMaxCoords();
	}

	void resetMinMaxCoords();

	void liSetAxisValueAndAddSegment(int wire, int indexPos, int axis,
			int value);
	void liAddAxisOffsetAndAddSegment(int wire, int indexPos, int axis,
			int offset);
	void liAddAxisOffset(int wire, int indexPos, int axis, int offset);
	void liInitCurrPrimal(int i, int j);
	void liInitCurrDual(long i, long j);
	void liCopyCurrsOverPrevs(int wire);
	void liAddPinPair(int wire, int ioIndex, int type);
	void addPinPair(int ioIndex, circuitmatrix& paramCirc, int i, int j);
	int getDepthShift();
	int getPrevHalfDelta(int direction);
	int getInputCoordAxis();
	int liAddIOPoint2(int wire);
	void makeGeometryFromCircuit(bfsState& state);
	gridPosition getWirePositionInGrid(long wire);
	void liDualAddAxisOffsetAndAddSegment(int indexPos, int axis, int offset);
	void liDualAddAxisOffsetAndAddSegment2(int indexPos, int axis, int offset);
	void liDualAddAxisOffset(int indexPos, int axis, int offset);
	int drawCtrlTgt(gridPosition& position0, bool previousWasDirectlyOnThisColumn, bool moveDirectlyOnThisColumn, bool changeColumn);

private:
	int liInsertIOPointUsingCurr1(int wire, bool isInit);
	void liConnectPrevsWithCurrs(int wire);
	void liConnectIOPointToCurrs(int wire, int& ioIndex, bool connectionDown);
	void addCnotPrimal(int& wire);
    void addCnotDual(std::vector<long>& targets, long level, bool constructFirstSegment);
    std::vector<gridPosition> computePositionsForCnotDual(std::vector<long>& targets);
	void addHadamard(int& wire, int level);
	void addSGate(int& wire, int level);
	void addVGate(int& wire, int level);

	void checkAndMoveBackOnWidth(bool& moveBackToNormalWidthAfterAControl,
			bool& isControl, long move1);

/*
 * 3 NOV
 */
	convertcoordinate addLoopAround(int coordsIndexPos, int direction, convertcoordinate& startcoord);
	convertcoordinate addGoThrough(int coordsIndexPos, int direction, convertcoordinate& startcoord);
	convertcoordinate moveToGridPosition(int coordsIndexPos, gridPosition& current, gridPosition& target, convertcoordinate& start);


};


#endif /* CIRCUITGEOMETRY_H_ */
