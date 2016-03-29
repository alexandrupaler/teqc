#include <stdio.h>
#include <string>

#include "cnotcounter.h"
#include "geometry.h"
#include "gatenumbers.h"
#include "circuitmatrix.h"

int geometry::addIOPoint(coordinate& ioc, bool isInit)
{
	int ioIndex = addCoordinate(ioc);
	io.push_back(ioIndex);
	return ioIndex;
}

void geometry::addSegment(int idx1, int idx2)
{
	pair<int,int> s1;
	if(idx1 == idx2)
	{
		//printf("zero distance...do not add\n");
		return;
	}

	//indicele mic e primul, al doilea e indicele mare
	s1.first = idx1 < idx2 ? idx1 : idx2;
	s1.second = idx1 > idx2 ? idx1 : idx2;

	/*new linear search*/
	string key;
	char numstr[100]; // enough to hold all numbers up to 64-bits
	//for(int i=0; i<2; i++)
	{
		sprintf(numstr, "%d,%d", s1.first, s1.second);
		key = key + numstr;
	}

	//key = s1.first + "," + s1.second;

	map<string, long>::iterator it = segMap.find(key);
	if(it != segMap.end())
	{
		return;
	}

	segs.push_back(s1);

	segMap[key] = segs.size() - 1;
}

int geometry::addCoordinate(coordinate& coord)
{
	/*new linear search*/
	string key = coord.toString(',');
//	for(int i=0; i<3; i++)
//	{
//		char numstr[21]; // enough to hold all numbers up to 64-bits
//		sprintf(numstr, ",%ld", coord[i]);
//		key = key + numstr;
//	}

	map<string, long>::iterator it = coordMap.find(key);
	if(it != coordMap.end())
	{
		return it->second;
	}

	//compute bounding box
	updateBoundingBox(coord);

	coords.push_back(coord);
	coordMap[key] = coords.size() - 1;

	return coords.size() - 1;
}

void geometry::updateBoundingBox(coordinate& coord)
{
	//compute bounding box
	for (int i = 0; i < 3; i++)
	{
		if (boundingbox[i] < coord[i])
			boundingbox[i] = coord[i];
	}
}
