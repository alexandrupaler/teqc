#ifndef DECOMPOSITION_H__
#define DECOMPOSITION_H__

#include <string>
#include <vector>

#include "gatenumbers.h"

namespace DecompType{
	enum Value { icm = 0, nonicm = 1, disticm = 2};
}

class decomposition
{
public:
	std::string name;

	//int type;//0-icm, 1-nonicm, 2-disticm
	DecompType::Value type;

	int nrAncilla;

	//for icm
	std::vector<int> inits;
	std::vector<std::vector<int> > cnots;
	std::vector<int> meas;

	//for nonicm
	std::vector<std::vector<int> > gates;

	void toString();

	/**
	 * @return the maximum number of columns used in the circuitmatrix representation of the decomposition
	 */
	size_t getMaxCols();

	/**
	 * @return true if it is an ICM decomposition, false otherwise
	 */
	bool isicm();

	/**
	 * @return true if is a distillation sub circuit, false otherwise
	 */
	bool isdist();

	/**
	 * @return the number of CNOTs existing in the decomposition
	 */
	int getNumberOfCnots();
};

#endif
