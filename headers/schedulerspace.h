#ifndef SCHEDULERSPACE_H_
#define SCHEDULERSPACE_H_

#include <vector>
#include <utility>
#include <map>

#include "numberandcoordinate.h"
#include "fileformats/generaldefines.h"

/**
 * An interval is a pair of integers, with .firsts < .second
 */
typedef pair<int, int> interval;

using namespace std;

#define NOSPACE -999
#define HEIGHTINTERVALS true
#define WIDTHINTERVALS false

/**
 * The class is a data structure similar to a two dimensional map (for the moment)
 * of coordinates used for marking the space required by the distillation boxes placed by schedulers.
 * It is constructed from a collection of inputpins under the assumption that
 * there is a maximum box size. The space is organised into lines (along the height axis)
 * and columns (along the width axis).
 * The columns have the circuit width axis coordinates of the inputs, and the lines
 * are constructed at coordinate increments of DELTA(=2). Each line and column is a
 * collection of intervals, initially (0, inf) meaning line/column is free. Once
 * a box is placed, the initial interval is split into at most two intervals which remain free:
 * the one before and the one after the occupied interval, and the line/column collection is updated.
 */
class schedulerspace
{
public:
	/**
	 * Constructor
	 * @param nandc the collection of inputpins for which the schedulerspace is to be constructed
	 * @param maxBoxHeight the maximum size a distillation box will have
	 */
	schedulerspace(numberandcoordinate& nandc, int maxBoxHeight);

	/**
	 * Constructs a interval object from two integer values. If the values are
	 * equal, the interval is not constructed.
	 * @param p1 the interval that is passed as a reference and will be updated
	 * @return boolean value if the interval was constructed
	 */
	bool constructInterval(int val1, int val2, interval& p1);

	/**
	 * Splits an input interval into a collection of intervals, after removing
	 * a specified sub interval
	 * @param pai the initial input interval
	 * @param boxPosition the beginning of the sub interval
	 * @param intervalLength the length of the sub interval
	 * @return the collection of the remaining intervals
	 */
	vector<pair<int, int> > splitInterval(pair<int, int>& pai, int boxPosition, int intervalLength);

	void printInterval(pair<int,int>& it);

	/**
	 * Replaces an interval on a specified line/column with a collection of intervals
	 * @param prs the interval collection used to replace
	 * @param pos the index of the initial interval that will be replaced
	 * @param collectionindex the  of the line/column in the list
	 * @param heightOrWidth true indicates that a column interval (height axis)
	 * false indicates that a line interval (width axis) will be replaced
	 */
	void replaceInterval(vector<interval> prs, int pos, int collectionindex, bool heightOrWidth);

//debugging purposes only?
	int getNumberOfIntervals(int index, bool heightOrWidth);

	pair<int,int> getHeightInterval(int widthindex, int pos){return *(freeSpaceOnHeight[widthindex].begin() + pos);}

	pair<int,int> getWidthInterval(int heightindex, int pos){return *(freeSpaceOnWidth[heightindex].begin() + pos);}

	/**
	 * Searches for a large enough interval on a specified column (height axis) that contains
	 * a sub interval of a given length. The method updates an iterator of a
	 * vector of intervals and a parameter that holds the number of possible
	 * positions the sub interval can take. For example, considering the interval
	 * (1,5) and a sub interval of length 2, there are 3 possible positions
	 * corresponding to the sub intervals (1,3), (2,4) and (3,5).
	 * @param heightInterval the vector iterator to update
	 * @param positionCircuitWidthAxis the index of the vector in the columns collection
	 * @param reqHeight the length of the subinterval
	 * @param nrPossiblePositions the number of positions the sub interval can occupy
	 * @return the beginning coordinate of the interval that can hold the sub interval, NOSPACE otherwise
	 */
	int findHeightAxisPosition(vector<interval>::iterator& heightInterval, int positionCircuitWidthAxis, int reqHeight, int& nrPossiblePositions);

	/**
	 * Checks that a sub interval of a specified length and starting from a
	 * specified width axis coordinate fits on a specified line
	 * @param positionCircuitWidthAxis the width axis coordinate the sub interval
	 * is required to start from
	 * @param positionCircuitHeightAxis the line coordinate on the height axis
	 * @param reqWidth the required length of the interval
	 * @return true if the sub interval fits on the line, fals otherwise
	 */
	bool checkWidthInterval(int positionCircuitWidthAxis, int positionCircuitHeightAxis, int reqWidth);

	/**
	 * A box is specified by two intervals: for width and for height. Once it was
	 * established that the box fits into a height interval, it is checked if
	 * it fits on all the width intervals that are incident on the height interval.
	 * For example, a box that needs to be place at a width axis coordinate 22,
	 * and which fits on the height (1,3), has a required width of 5. Therefore,
	 * it has to be checked if starting from the coordinates (22,1), (22,2) and (22,3)
	 * there are intervals of length 5 on the width axis. It is effectively checked
	 * that the interval (22,27) fits three times on the lines having coordinates
	 * 1, 2 and 3.
	 * @param positionCircuitWidthAxis the width axis starting position of the box
	 * @param positionCircuitHeightAxis the height axis starting position of the box
	 * @param reqWidth
	 * @param reqHeight
	 * @return true if a box fits at the specified coordinates, false otherwise
	 */
	bool boxFitsIntoWidthInterval(int positionCircuitWidthAxis, int positionCircuitHeightAxis, int reqWidth, int reqHeight);

	/**
	 * After verifying that a box fits at the specified coordinates, the line and
	 * column intervals are updated (split). The occupied intervals are removed from
	 * the collections of available intervals. See boxFitsIntoWidthInterval().
	 * @param positionCircuitWidthAxis the width axis starting position of the box
	 * @param positionCircuitHeightAxis the height axis starting position of the box
	 * @param reqWidth
	 * @param reqHeight
	 */
	void occupyWidthIntervals(int positionCircuitWidthAxis, int positionCircuitHeightAxis, int reqWidth, int reqHeight);

	/**
	 * A box having known dimensions needs to be placed at a specified width axis coordinate. The method
	 * computes the circuit height axis coordinate where the box can be placed.
	 * @param reqWidth box width
	 * @param reqHeight box height
	 * @param positionWidthAxis required position on the width axis
	 * @param positionHeightAxis will be updated by the method with the
	 * found height axis position of the placed box
	 * @return true if a box was succesfully placed, false otherwise
	 */
	bool computeCoordinates(int reqWidth, int reqHeight, int positionWidthAxis, int& positionHeightAxis);

protected:
	map<int, vector<interval> > freeSpaceOnWidth;
	map<int, vector<interval> > freeSpaceOnHeight;
};

#endif /* SCHEDULERSPACE_H_ */
