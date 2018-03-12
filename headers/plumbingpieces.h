#ifndef PLUMBINGPIECES_H_
#define PLUMBINGPIECES_H_

#include "numberandcoordinate.h"
#include "geometry.h"

#include <vector>
#include <string>
#include <map>

class plumbingpiece
{
public:
	convertcoordinate position;
	//BIT interpretation: EXIST, XAXIS, YAXIS, ZAXIS
	//0000, 1100, 1010, 1001,
	unsigned int primal;
	unsigned int dual;

	plumbingpiece();

	std::string toString();

	void setMask(bool isPrimal, unsigned int mask);
};


class plumbingpiecesgenerator
{
public:
//	std::map<std::string, long> plumbMap;
	std::map<long, std::map<long, std::map<long, long> > > plumbMap;

	std::vector<plumbingpiece> pieces;

	std::map<int, int> potentialCornerCounts;

	void removeWrongCorners();

	void generateFromGeometry(geometry& geom);

	int getPlumbingPieceIndex(convertcoordinate& coord);

	bool countKey(long x, long y, long z);
};


#endif /* PLUMBINGPIECES_H_ */
