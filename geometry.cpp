#include <stdio.h>
#include <string>
#include <algorithm>

#include "cnotcounter.h"
#include "geometry.h"
#include "gatenumbers.h"
#include "circuitmatrix.h"

int geometry::addIOPoint(convertcoordinate& ioc, bool isInit)
{
	int ioIndex = addCoordinate(ioc);
	io.push_back(ioIndex);
	return ioIndex;
}

bool geometry::removeSegment(int idx1, int idx2)
{
	if(idx1 == idx2)
	{
		//printf("zero distance...do not add\n");
		return false;
	}

	pair<long, long> s1 = computePair(idx1, idx2);
	string key = computeKey(s1);

	//do not assume the segment exists
	map<string, long>::iterator it = segMap.find(key);
	if(it == segMap.end())
	{
		return false;
	}

	vector<pair<long, long> >::iterator it2 = find(segs.begin(), segs.end(), s1);
	segs.erase(it2);
	segMap.erase(it);

	return true;
}

pair<long, long> geometry::computePair(int idx1, int idx2)
{
	pair<long, long> s1;


	//indicele mic e primul, al doilea e indicele mare
	s1.first = idx1 < idx2 ? idx1 : idx2;
	s1.second = idx1 > idx2 ? idx1 : idx2;

	return s1;
}

string geometry::computeKey(pair<long, long>& p)
{
	/*new linear search*/
	string key;
	char numstr[100]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "%ld,%ld", p.first, p.second);
	key = key + numstr;

	return key;
}

bool geometry::addSegment(int idx1, int idx2)
{
	if(idx1 == idx2)
	{
		//printf("zero distance...do not add\n");
		return false;
	}

	pair<long, long> s1 = computePair(idx1, idx2);

	/*new linear search*/
	string key = computeKey(s1);
//	char numstr[100]; // enough to hold all numbers up to 64-bits
//	sprintf(numstr, "%d,%d", s1.first, s1.second);
//	key = key + numstr;

	map<string, long>::iterator it = segMap.find(key);
	if(it != segMap.end())
	{
		return false;
	}

	segs.push_back(s1);

	segMap[key] = segs.size() - 1;

	return true;
}

int geometry::addCoordinate(convertcoordinate& coord)
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

void geometry::updateBoundingBox(convertcoordinate& coord)
{
	//compute bounding box
	for (int i = 0; i < 3; i++)
	{
		if (boundingbox[i] < coord[i])
			boundingbox[i] = coord[i];
	}
}

void geometry::reset()
{
	coords.clear();
	coordMap.clear();

	segs.clear();
	segMap.clear();

	boundingbox.reset();
}
