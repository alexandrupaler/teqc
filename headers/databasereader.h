#ifndef DATABASEREADER_H_
#define DATABASEREADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string.h>

#include "gatenumbers.h"
#include "decomposition.h"

/**
 * The class is used for gate decompositions from a test database. The database contains ICM and non-ICM gate decompositions.
 * For example, the Toffoli decomposition into seven T gates is non-ICM. Each decomposition contains operations applied on qubits.
 * An operation is an initialisation, measurement or a gate. Therefore, non-ICM decopositions contain only operations
 * representing gates, and ICM decompositions initialisation and measurement operations.
 */
class databasereader
{
public:
	std::map<std::string, int> nameToInt;
	std::map<int, std::string> intToName;
	std::map<std::string, decomposition> decomp;

	/**
	 * The default constructor.
	 */
	databasereader();

	/**
	 * The strange part is that for each gate there is an equivalent integer value,
	 * and these values are used throughout the source code and in the database.
	 * A header file *.h containing the #defines is read and interpreted. Results are stored
	 * in nameToInt and intToName.
	 */
	void fillTranslations();

	/**
	 * Each gate/circuit element has an associated integer value, and the function returns it.
	 * @param key the gate name
	 * @return the gate integer value
	 */
	int getNumber(char* key);

	/**
	 * Read the operations
	 * @param fp file handler of the database
	 * @param list list to contain the read operations
	 */
	void readOperations(FILE* fp, std::vector<int>& list);

	/**
	 * The method is structurally very similar to readOperations, but specific for CNOTs.
	 * @param fp  file handler of the database
	 * @param list the list where the qubits are to be stored
	 * @param valoffset qubit numbers can be offset down when e.g. they represent indices in a vector
	 * @return
	 */
	bool readCnot(FILE* fp, std::vector<int>& list, int valoffset);

	/**
	 * Reads a decomposition from the database and creates returns an object containing it.
	 * @param file the file handler of the database
	 * @param name the name of the decomposition
	 * @return the decomposition object
	 */
	decomposition readDecomposition(FILE* file, std::string name);

	/**
	 * Advances the file reading process until a decomposition name is found in the file.
	 * @param file the file handler of the database
	 * @return the name of the decomposition
	 */
	std::string findDecomposition(FILE* file);

	/**
	 * Read the entire database into a std::map indexed by the decomposition names.
	 * @param decomp the destination map where the decompositions are stored
	 */
	void readDatabase(std::map<std::string, decomposition>& decomp);
};


#endif /* DATABASEREADER_H_ */
