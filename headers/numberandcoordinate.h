#ifndef _NUMBERANDCOORDINATE_H__
#define _NUMBERANDCOORDINATE_H__

#include <vector>
#include <string>
#include <sstream>
#include <limits.h>

using namespace std;

#define CIRCUITWIDTH 0
#define CIRCUITDEPTH 1
#define CIRCUITHEIGHT 2

#define TYPE 0
#define INJNR 1
#define ROWCOORD 2

#define ATYPE 0
#define YTYPE 1

#define OFFSETNONCOORD 2

#define FAKEID 10000

/**
 * A class used for storing three dimensional coordinates
 */
class coordinate{
public:
	coordinate()
	{
		coord.resize(3, -100);
	};

	coordinate(const coordinate& other)
	{
		coord = other.coord;
	};

	long& operator[](int i){return coord[i];};
	long& at(int i){return coord[i];};

	coordinate& operator=(const coordinate other)
	{
		coord = other.coord;

		return *this;
	};

	/**
	 * Note: I am mixing the printing of coordinates: sometimes I use direct access, sometimes the toString
	 * in the future I may move completely away from toString
	 */
	string toString(char separator)
	{
		stringstream ss;
		ss << coord[0] << separator << coord[1] << separator << coord[2];

		return ss.str();
	};

private:
	vector<long> coord;
};

/**
 * The class represents a collection of two coordinates, a type and an id. It is used for multiple purposes:
 * - in circuits, each input or output vertex has exactly two neighbouring normal vertices. These are pins,
 * the points where defects coming out from distillation boxes are joined with the circuit geometry
 * - for distillation boxes, the pins are the points where the defects connecting the geometry with the box are joined
 * Because the class includes the members type and id, by means of typedefs it is further used for defining
 * the coordinates of the distillation boxes in the scheduler (the id is not necessary), and the pairs of vertices to
 * be connected in connectpins (type and id are not necessary).
 */
class inputpin{
public:
	inputpin()
	{
		id = FAKEID;
		type = ATYPE;
	};

	inputpin(const inputpin& other)
	{
		coord[0]= other.coord[0];
		coord[1]= other.coord[1];
		id = other.id;
		type = other.type;
	};

	inputpin& operator=(const inputpin other)
	{
		coord[0] = other.coord[0];
		coord[1] = other.coord[1];
		id = other.id;
		type = other.type;

		return *this;
	};

	long& operator[](int i)
	{
		if(i == TYPE)
		{
			return type;
		}
		else if(i == INJNR)
		{
			return id;
		}
		int coordnr = (i - OFFSETNONCOORD) / 3;
		int coordidx = (i - OFFSETNONCOORD) % 3;
		return coord[coordnr][coordidx];
	};

	void setCoordinate(int idx, coordinate& c)
	{
		coord[idx] = c;
	};

	coordinate& getCoordinate(int idx)
	{
		return coord[idx];
	}

protected:
	coordinate coord[2];
	long id;
	long type;
};

/**
 * The class stores a collection of inputpins.
 * It is used for the circuit or the distillation box schedulers.
 */
class numberandcoordinate
{
public:
	/**
	 * The number of inputpins related to A states.
	 */
	int nrAStates;

	/**
	 * The number of inputpins related to A states.
	 */
	int nrYStates;

	/**
	 * The minimum circuit width axis coordinate of the pins.
	 */
	int minInjRow;

	/**
	 * The maximum circuit width axis coordinate of the pins.
	 */
	int maxInjRow;

	numberandcoordinate()
	{
		nrAStates = 0;
		nrYStates = 0;
		minInjRow = INT_MAX;
		maxInjRow = INT_MIN;
	}

	numberandcoordinate(const numberandcoordinate& other)
	{
		inputList = other.inputList;
		nrAStates = other.nrAStates;
		nrYStates = other.nrYStates;
		minInjRow = other.minInjRow;
		maxInjRow = other.maxInjRow;
	}

	vector<inputpin> inputList;

	numberandcoordinate& operator=(const numberandcoordinate other)
	{
		inputList = other.inputList;
		nrAStates = other.nrAStates;
		nrYStates = other.nrYStates;
		minInjRow = other.minInjRow;
		maxInjRow = other.maxInjRow;

		return *this;
	};

	/**
	 * @return the number of inputpins in the collection
	 */
	int size();

	/**
	 * @param index the inputpin index in the collection
	 * @return the type of the input or box associated to the pin
	 */

	int getIOType(int index);

	/**
	 * Inserts a new pin to the collection.
	 * @param numberAndCoords the pin to insert
	 */
	void addEntry(inputpin& numberAndCoords);

	/**
	 * The function is used to fill the collection with inputpins of a single type.
	 * Multiple inputpins may have the same coordinate, e.g. when the interval between startCoord and stopCoord
	 * has not enough space.
	 * @param difStates the number of pins to insert
	 * @param type the type of the associated circuit input or scheduler box
	 * @param startCoord the minimal circuit width axis coordinate of the pin
	 * @param stopCoord the maximal circuit width axis coordinate of the pin
	 * @param circuitRowCoordIncrement the circuit width axis coordinate increment between two subsequent pins
	 */
	void addSingleTypeEntries(int difStates, int type, int startCoord, int stopCoord, int circuitRowCoordIncrement);
};


#endif
