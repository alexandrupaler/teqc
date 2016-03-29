#ifndef RAWFILEREADER_H_
#define RAWFILEREADER_H_

#include <vector>
#include "fileformats/generaldefines.h"

#define CH_CNOT 'c'
#define CH_HADAMARD 'h'
#define CH_PHASE 'p'
#define CH_TGATE 't'
#define CH_RGATE 'v'
#define CH_TOFFOLI 'T'
#define CH_NOGATE '?'

using namespace std;

/**
 * The structure holds the result of reading a raw circuit file. Such a file is
 * the input to the complete chain of tools that generates a circuit geometry.
 * It consists of gates specified one per file line. The gates types are encoded
 * using characters, e.g. 'h' represents Hadamard. A file line has the format
 * [gate] [qubit1] ... [qubitn], such that multiqubit gates can also be defined.
 */
struct rawdata
{
	long nrQubits;
	long nrGates;
	long nrToffoli;

	/**
	 * The fileline vector
	 */
	vector<fileline> fileLines;
};

class rawfilereader{
public:
	/**
	 * The method returns the maximum qubit number referenced by a gate from the
	 * raw file.
	 * @param currMax the current maximum qubit number
	 * @param others a vector of other qubit numbers that
	 * will be compared with currMax
	 * @return the new maximum qubit number
	 */
	long getMaxQubit(long currMax, vector<long> others);

	/**
	 * @param file war file handler
	 * @return a rawdata structure filled with the file contents
	 */
	rawdata readRawFile(FILE* file);

	/**
	 * Reads a gate character from the file
	 * @param file raw file handler
	 * @return the read gate char
	 */
	char readGate(FILE* file);

	/**
	 * After each gate char the format requires an integer specifying the qubit
	 * number affecte by the gate
	 * @param file raw file handler
	 * @return the qubit number read
	 */
	long readFirstQubit(FILE* file);

	/**
	 * A gate can affect more than one qubit, e.g. CNOT and Toffoli. The method
	 * reads the qubits following the first one.
	 * @param fp raw file handler
	 * @param list the vector where the other qubits will be stored
	 */
	void readOtherQubits(FILE* file, vector<long>& list);
};


#endif /* RAWFILEREADER_H_ */
