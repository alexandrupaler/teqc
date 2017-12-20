#ifndef PROCESSRAW_H_
#define PROCESSRAW_H_

#include <vector>

#include "cnotcounter.h"
#include "databasereader.h"
#include "fileformats/rawfilereader.h"
#include "fileformats/generaldefines.h"

/**
 * Class used for replacing gates with their non-ICM decompositions stored in
 * a database, and for generating a quantum circuit representation that is very
 * similar to a matrix.
 */
class processraw{

public:
	/**
	 * Constructor takes a raw file data representation as parameter.
	 * @param rw the rawfile object
	 */
	processraw(rawdata& rw);

	/**
	 * @return the matrix representation of the transformed circuit
	 */
	std::vector<qubitline>& getCircuit(){return circuit;}

	/**
	 * Each gate type in the raw file will be translated into one or multiple
	 * integer values. The CNOT is transformed into a control and a target integer
	 * using the cnotcounter class.
	 * @param gateType the raw file character for the gate types. See the #defines
	 * in the rawfilereader class header file.
	 * @return vetor of integer values representing the gate
	 */
	std::vector<int> getOpNumber(char gateType);

	/**
	 * A raw file line starts with a char followed by a list of integer values.
	 * In the case of the CNOT the integers indicate the qubits of the control
	 * and target (in this order). Placing a CNOT in the matrix representation
	 * requires knowing at which column what lines (control target) to update
	 * with the CNOT operation numbers. See getOpNumber().
	 * @param currNrGate column number in the circuit matrix representation
	 * @param opnums list of integers representing the CTRL and TGT
	 * @param rawFileLine contains the list of qubits affected by the CNOT
	 */
	void placeCnot(int currNrGate, std::vector<int>& opnums, fileline& rawFileLine);

	/**
	 * Replaces a gate with its non-ICM decomposition read from the database,
	 * e.g. "toffoli".
	 * @param name the name of the gate and at the same time of the decomposition
	 * from the database
	 * @param currNrGate column in the circuit matrix representation where to place
	 * the decomposition
	 * @param rawFileLine
	 * @return the length of the decomposition in terms of gates, thus the offset
	 * after currNrGate
	 */
	int placeNonICMDecomposition(const char* name, int currNrGate, fileline& rawFileLine);

private:
	std::vector<qubitline> circuit;

	databasereader dbReader;

	cnotcounter cncounter;
};


#endif /* PROCESSRAW_H_ */
