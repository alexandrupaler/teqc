#ifndef CONVERTCOORDINATE_H_
#define CONVERTCOORDINATE_H_

#define CIRCUITWIDTH 0
#define CIRCUITDEPTH 1
#define CIRCUITHEIGHT 2

#include <vector>
#include <string>
#include <climits>

using namespace std;

/**
 * A class used for storing three dimensional coordinates
 */
class convertcoordinate{
public:
	convertcoordinate()
	{
		values.resize(3, INT_MIN);
	};

	convertcoordinate(const convertcoordinate& other)
	{
		values = other.values;
	};

	long& operator[](int i){return values[i];};
	long& at(int i){return values[i];};

	convertcoordinate& operator=(const convertcoordinate other)
	{
		values = other.values;

		return *this;
	};

	/**
	 * Note: I am mixing the printing of coordinates: sometimes I use direct access, sometimes the toString
	 * in the future I may move completely away from toString
	 */
	string toString(char separator);

	bool isPrimalCenter(int val);

	bool isDualCenter(int val);

	bool isPrimalCorner(int val);

	bool isDualCorner(int val);

	long manhattanDistance(convertcoordinate& other);

	bool isColinear(convertcoordinate& other);

	void reset();

private:
	vector<long> values;
};


#endif /* CONVERTCOORDINATE_H_ */
