#ifndef BOXWORLD_H_
#define BOXWORLD_H_

#include <vector>
#include <utility>
#include <map>
#include <queue>

#include "fileformats/iofilereader.h"
#include "computeadditional.h"
#include "numberandcoordinate.h"
#include "fileformats/generaldefines.h"
#include "heuristicparameters.h"

#include "faildistillations.h"
#include "rtree/RStarTree.h"
#include "recycling/causalgraph.h"

#include "heuristicparameters.h"

#define PINSVERTICAL 0
#define PINSHORIZRIGHT 1
#define PINSHORIZLEFT 2

//#define LOGPLACING

//pinpair has two coordinates, but I need only one
//instead of constructing a new class, or making pinpair parameterisable
//I will just have to make sure, that only the first coordinate is used
typedef pinpair boxcoord;

struct boxConfiguration
{
    int _pinScenario;
    int _heightStepSize;

    std:: vector<std::vector<int> > boxSize;
    std::vector<int> distBetweenBoxes;

    boxConfiguration()
    {
    	_pinScenario = PINSVERTICAL;
    	_heightStepSize = 0;

    	boxSize.resize(2);
    	int bsizetmp[] = {60, 60, 60, 30, 30, 30};
		boxSize[0] = std::vector<int>(bsizetmp + 0, bsizetmp + 3);
		boxSize[1] = std::vector<int>(bsizetmp + 3, bsizetmp + 6);

		int distatmp[] = { DELTA, DELTA, DELTA };
		distBetweenBoxes = std::vector<int>(distatmp + 0, distatmp + 4);
    }

    int getBoxTotalDimension(int boxType, int axis);
};

class twobbox
{
public:
	int maxWidth;
	int maxHeight;
	int minWidth;
	int minHeight;

	void updateBBox(convertcoordinate& boxCoord, int type, std::vector<std::vector<int> >& boxSizes)
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

	int getMinWidthDistance() { return minWidth - 1;}
	int getMaxWidthDistance() { return maxWidth - 1;}
	int getMinHeightDistance() { return minHeight - 1;}
	int getMaxHeightDistance() { return maxHeight - 1;}
};

/**
 * A simple distillation box scheduler
 */

struct schedulerLevelInfo
{
	long depth;
	BoundingBox lastBounding;
	long maxDepth;
	int lastMoveIndex;

	/**
	 * The first box of the first round is placed at in the circuit
	 * geometry. During calibration the box is moved out of the geometry
	 * and the connection channel which encompasses it.
	 */
	bool calibration;

	/**
	 * The last type of scheduled boxes
	 */
	int lastScheduledBoxType;

	double aStateFail;
	double yStateFail;
	double tGateFail;
	double pGateFail;

	int greedyAxis[4];
	int greedyDirection[4];

	int getMoveIndex(int offset);
	int getDirection(int offset);
	int getAxis(int offset);
};

class boxworld2
{
public:
    boxworld2(char *iofile, char* boundingfile);
    boxworld2();

    void pinsVertical(int nr, int bindex, long  & x, long  & y, long  & z);
    void pinsHorizontalRight(int nr, int bindex, long  & x, long  & y, long  & z);
    void pinsHorizontalLeft(int nr, int bindex, long  & x, long  & y, long  & z);

    void saveBoxAndPinCoords(int nr, int bindex);

    BoundingBox generateBounds(int x, int y, int z, int boxTypeIndex);
//    BoundingBox generateBounds(convertcoordinate coord, int boxTypeIndex);
    BoundingBox initNewBox(schedulerLevelInfo& level, int boxType);

    twobbox computeBBox();

    bool canSlide(BoundingBox bBox, int axis, int direction);
    bool overlapsConnectionChannel(BoundingBox bBox, int axis, int direction);

    bool overlappedAndWasSlided(BoundingBox& bBox, int axis, int direction);
    void addBoxSlidingHeight(BoundingBox& bBox, int boxTypeIndex);
    void addBoxSlidingDepth(BoundingBox& bBox, int boxTypeIndex);
    void addBoxSlidingDepth2(BoundingBox& bBox, int boxTypeIndex);

    void placeAdditionalsAlongWidthAxis(int total, int boxType, int direction, int offset[3]);
    void computeAdditionalATypeScheduler(long boxStartTimeCoordinate, int difStates);
    void computeAdditionalYTypeScheduler(long boxStartTimeCoordinate, int difStates);

    void switchLayoutConfig(int configNumber);

    std::queue<int> computeScheduleCanonical(long boxStartTimeCoordinate, causalgraph& causal);
    std::queue<int> computeScheduleALAPT(long boxStartTimeCoordinate, int boxType, int available, int necessary);

	int preSimulateFailuresInGreedy(std::queue<int>& boxPinIds, int totalToSim, int boxType, schedulerLevelInfo& greedyLevel);

	void initGeomBoundingBox(long wf, long ws, long df, long ds, long hf, long hs);
	void initScheduleGreedy(double aStateFail, double yStateFail, double tGateFail, double pGateFail);
	std::queue<int> greedyScheduleBoxes(long boxStartTimeCoordinate, int boxType, int available, int necessary);

	bool setConnectionBoxWidth(int totalWidth);
	bool setConnectionBoxHeight(int totalHeight);

	long getTimeWhenBoxesEnd();

public:
	faildistillations fd;

    convertcoordinate currBoxCoords;

    std::vector<boxcoord> boxCoords;
    std::vector<pinpair> boxPins;

    //this is not used in the greedy scheduler
    numberandcoordinate nandc;

    boxConfiguration currentConfig;

    RTree rtree;
    BoundingBox geomBoundingBox;
    BoundingBox connectionsBox;

    //objects used by greedy scheduling
	schedulerLevelInfo greedyLevel;

	heuristicparameters* heuristicParam;

	void setCalibration(bool value);

private:
    void init();
	void placeBoxesInGreedy(long boxStartTimeCoordinate /*bfsState& state*/, int& boxType, int nrBoxesToAdd);
};

#endif /* BOXWORLD_H_ */
