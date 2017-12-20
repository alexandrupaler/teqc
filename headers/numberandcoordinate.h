#ifndef _NUMBERANDCOORDINATE_H__
#define _NUMBERANDCOORDINATE_H__

#include <vector>
#include <string>

#include "pins/pinpair.h"

#define TYPE 0
#define INJNR 1
#define ROWCOORD 2

#define ATYPE 0
#define YTYPE 1

#define OFFSETNONCOORD 2

#define FAKEID 10000

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

	std::vector<pinpair> inputList;

	numberandcoordinate()
	{
		nrAStates = 0;
		nrYStates = 0;
		minInjRow = INT_MAX;
		maxInjRow = INT_MIN;
	}

	numberandcoordinate(std::vector<pinpair>& pins)
	{
		//slow. one by one. TODO: Replace entire class, do not use this method. Boxworld2
		for(std::vector<pinpair>::iterator it = pins.begin(); it != pins.end(); it++)
		{
			addEntry(*it);
		}
	}

	numberandcoordinate(const numberandcoordinate& other)
	{
		inputList = other.inputList;
		nrAStates = other.nrAStates;
		nrYStates = other.nrYStates;
		minInjRow = other.minInjRow;
		maxInjRow = other.maxInjRow;
	}

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
	void addEntry(pinpair& numberAndCoords);

//	/**
//	 * The function is used to fill the collection with inputpins of a single type.
//	 * Multiple inputpins may have the same coordinate, e.g. when the interval between startCoord and stopCoord
//	 * has not enough space.
//	 * @param difStates the number of pins to insert
//	 * @param type the type of the associated circuit input or scheduler box
//	 * @param startCoord the minimal circuit width axis coordinate of the pin
//	 * @param stopCoord the maximal circuit width axis coordinate of the pin
//	 * @param circuitRowCoordIncrement the circuit width axis coordinate increment between two subsequent pins
//	 */
//	void addSingleTypeEntries(int difStates, int type, int startCoord, int stopCoord, int circuitRowCoordIncrement);

	/**
	 * Sorts the pins based on their depth coordinate. TODO: Allow other coordinates?
	 */
	void sortEntries();

	struct myclass {
	  bool operator() (pinpair i, pinpair j)
	  {
		  return (i[ROWCOORD + 1] < j[ROWCOORD + 1]);
	  }
	} compareWidthCooord;
};

#endif
