#ifndef BOXWORLD_H_
#define BOXWORLD_H_

#include <vector>
#include <utility>
#include <map>

#include "fileformats/iofilereader.h"
#include "computeadditional.h"
#include "numberandcoordinate.h"
#include "schedulerspace.h"

#define PINSVERTICAL 0
#define PINSHORIZRIGHT 1
#define PINSHORIZLEFT 2

#define NOMIRROR 1
#define MIRROR -1

using namespace std;

//inputpin has two coordinates, but I need only one
//instead of constructing a new class, or making inputpin parameterisable
//I will just have to make sure, that only the first coordinate is used
typedef inputpin boxcoord;

class twobbox
{
public:
	int maxWidth;
	int maxHeight;
	int minWidth;
	int minHeight;

	void updateBBox(coordinate& boxCoord, int type, vector<vector<int> >& boxSizes)
	{
		if(maxHeight < boxCoord[CIRCUITHEIGHT] + boxSizes[type][CIRCUITHEIGHT])
		{
			maxHeight = boxCoord[CIRCUITHEIGHT] + boxSizes[type][CIRCUITHEIGHT];
		}
		if(maxWidth < boxCoord[CIRCUITWIDTH] + boxSizes[type][CIRCUITWIDTH])
		{
			maxWidth = boxCoord[CIRCUITWIDTH] + boxSizes[type][CIRCUITWIDTH];
		}
		if(minHeight > boxCoord[CIRCUITHEIGHT])
		{
			minHeight = boxCoord[CIRCUITHEIGHT];
		}
		if(minWidth > boxCoord[CIRCUITWIDTH])
		{
			minWidth = boxCoord[CIRCUITWIDTH];
		}
	};

	twobbox()
	{
		maxWidth = INT_MIN;
		maxHeight = INT_MIN;
		minWidth = INT_MAX;
		minHeight = INT_MAX;
	};
};

/**
 * A simple distillation box scheduler
 */
class boxworld2
{
public:
	/**
	 * Computes the coordinates of the pins when layed out vertically.
	 * @param nr the pin index
	 * @param bindex the type of the distillation box
	 * @param x the returned x coordinate
	 * @param y the returned y coordinate
	 * @param z the returned z coordinate
	 */
	void pinsVertical(int nr, int bindex, long& x, long& y, long& z);

	/**
	 * Computes the coordinates of the pins when layed out horizontally on the right of a box.
	 * @param nr the pin index
	 * @param bindex the type of the distillation box
	 * @param x the returned x coordinate
	 * @param y the returned y coordinate
	 * @param z the returned z coordinate
	 */
	void pinsHorizontalRight(int nr, int bindex, long& x, long& y, long& z);

	/**
	 * Computes the coordinates of the pins when layed out horizontally on the left of a box.
	 * @param nr the pin index
	 * @param bindex the type of the distillation box
	 * @param x the returned x coordinate
	 * @param y the returned y coordinate
	 * @param z the returned z coordinate
	 */
	void pinsHorizontalLeft(int nr, int bindex, long& x, long& y, long& z);

	/**
	 * Takes the current box coordinates, computes the pin coordinates and stores them.
	 * @param bindex the type of the box
	 */
	void savePinCoord(int bindex/*, inputpin& circpins*/);

	/**
	 * Takes the current box coordinates and stores them.
	 * @param nr the number of the circuit input (see the geometry vertex indices)
	 * @param bindex the type of the box
	 */
	void saveBoxCoord(int nr, int bindex);

	/**
	 * Aftre computing a schedule, the box and pin coordinates are offset and stored in the final vectors.
	 * @param offset the distances in each dimension for moving boxes and pins
	 */
	void pushTempInFinal(vector<int> offset);

	/**
	 * Computes the bounding box of a distillation box schedule.
	 * @return the two dimensional bounding box
	 */
	twobbox computeBBox();

	/**
	 * Adds a new box to an existing scheduler.
	 * @param sp the volume representing the free spaces in the scheduler
	 * @param bindex the type of the box to be added
	 * @param circuitRow the coordinate on the ciruit width axis where the box should be placed
	 * @return
	 */
	bool advance(schedulerspace& sp, int bindex, int circuitRow);

	/**
	 * Computes a distillation box schedule for the specified list of circuit inputs.
	 */
	void computeSchedule();

	/**
	 * If boxes are placed step wise using stepsize, the height of a box
	 * will determine the maximum number of
	 * boxes that can placed along the width axis.
	 * @param type the type of the box to be added
	 * @param stepsize the height axis coordinate increment between  two subsequent boxes
	 * @return maximum number of possible boxes that can be placed
	 */
	int computeMaxNrBoxes(int type, int stepsize);

	/**
	 *
	 * @param nrInitialStates
	 * @param stateFail
	 * @param gateFail
	 * @param startCoord
	 * @param stopCoord
	 * @param type
	 * @return
	 */
	int prepareAdditionalSingleTypeNandc(int nrInitialStates, double stateFail,
			double gateFail, int startCoord, int stopCoord, int type);

	/**
	 * Constructor
	 * @param iofile the file representing the circuit's inputs
	 * @param bboxfile the file representing the circuit's geometry bounding box
	 */
	boxworld2(char* iofile, char* bboxfile);

public:
	/**
	 * The layout of the distillation box pins.
	 */
	int _pinScenario;

	/**
	 * After a distillation box was placed, its position on the height axis can be shifted.
	 */
	int _heightStepSize;

	/**
	 * The position of a distillation box can be mirrored. Possible values 1 (no mirror) and -1 (mirror).
	 */
	int _mirrorAboutCircuitWidthAxis;

	/**
	 * The distillation box sizes.
	 */
	vector<vector<int> > boxSize;

	/**
	 * The coordinates computed after each call of advance()
	 */
	coordinate currBoxCoords;

	/**
	 * The distances between the scheduled boxes. These are expected to be in DELTA increments.
	 */
	vector<int>  distBetweenBoxes; //totul este in increment de 2, deoarece sunt coordonate de celule primare/duale

	/**
	 * The final vector containing distillation box coordinates.
	 */
	vector<boxcoord> boxCoords;

	/**
	 * The final vector containing the pins of all the distillation boxes.
	 */
	vector<inputpin> boxPins;

	/**
	 * Before calling pushTempInFinal(), the computed box coordinates are stored here.
	 */
	vector<boxcoord> tempBoxCoords;

	/**
	 * Before calling pushTempInFinal(), the computed box pin coordinates are stored here.
	 */
	vector<inputpin> tempBoxPins;

	/**
	 * A list of circuit inputs for which a schedule of distillation boxes has to be computed.
	 */
	numberandcoordinate nandc;
};
#endif /* BOXWORLD_H_ */
