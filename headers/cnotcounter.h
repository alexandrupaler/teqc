#ifndef CNOTCOUNTER_H__
#define CNOTCOUNTER_H__

#include "gatenumbers.h"

class cnotcounter{
public:
	/**
	 * Default constructor.
	 */
	cnotcounter();

	/**
	 * Checks if a value represents a CNOT element.
	 * @param val the value to check
	 * @return either CTRL or TGT
	 */
	static int getCnotPart(int val);

	/**
	 * Checks if a value represents a CNOT element.
	 * @param val
	 * @return true if CNOT, false otherwise
	 */
	static bool isCnot(int val);

	/**
	 * Returns the next CNOT number which represents the control CTRL
	 * The target TGT will be the returned value + 1
	 * @return the number
	 */
	int getNextCnotNumber();

	/**
	 * Checks if the two values correspond to the same CNOT
	 * @param ctrlval assumed control value
	 * @param targetval assumed target value
	 * @return true, if same CNOT, false otherwise
	 */
	static bool sameCnot(int ctrlval, int targetval);

	/**
	 * Cheks if a value represents a control.
	 * @param value
	 * @return true if control, false otherwise
	 */
	static bool isControl(int value);

	/**
	 * Checks if a value represents a target.
	 * @param value
	 * @return true if target, false otherwise
	 */
	static bool isTarget(int value);

private:
	/**
	 * The starting number of a CNOT.
	 */
	static int const startCnotNumber = 10000;

	/**
	 * The current CNOT number.
	 */
	int numberCnot;
};

#endif


