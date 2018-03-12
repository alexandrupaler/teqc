#ifndef CONVERTCOORDINATE_H_
#define CONVERTCOORDINATE_H_

#define CIRCUITNOAXIS -1
#define CIRCUITWIDTH 0
#define CIRCUITDEPTH 1
#define CIRCUITHEIGHT 2

#include <vector>
#include <string>
#include <climits>

/**
 * A class used for storing three dimensional coordinates
 */
class convertcoordinate{
public:
	convertcoordinate();

	convertcoordinate(const convertcoordinate& other);

	long& operator[](int i);

	long& at(int i);

	convertcoordinate& operator=(const convertcoordinate& other);
	bool operator==(const convertcoordinate& other);

	/**
	 * Note: I am mixing the printing of coordinates: sometimes I use direct access, sometimes the toString
	 * in the future I may move completely away from toString
	 */
	std::string toString(char separator);

	bool isPrimalCenter(int val);

	bool isDualCenter(int val);

	bool isPrimalCorner(int val);

	bool isDualCorner(int val);

	long manhattanDistance(convertcoordinate& other);

	bool isColinear(convertcoordinate& other);

	void reset();

private:
	std::vector<long> values;
};


#endif /* CONVERTCOORDINATE_H_ */
