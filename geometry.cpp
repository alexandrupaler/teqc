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

	std::pair<long, long> s1 = computePair(idx1, idx2);
	std::string key = computeKey(s1);

	//do not assume the segment exists
	std::map<std::string, long>::iterator it = segMap.find(key);
	if(it == segMap.end())
	{
		return false;
	}

	std::vector<std::pair<long, long> >::iterator it2 = find(segs.begin(), segs.end(), s1);
	segs.erase(it2);
	segMap.erase(it);

	return true;
}

std::pair<long, long> geometry::computePair(int idx1, int idx2)
{
	std::pair<long, long> s1;


	//indicele mic e primul, al doilea e indicele mare
	s1.first = idx1 < idx2 ? idx1 : idx2;
	s1.second = idx1 > idx2 ? idx1 : idx2;

	return s1;
}

std::string geometry::computeKey(std::pair<long, long>& p)
{
	/*new linear search*/
	std::string key;
	char numstr[100]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "%ld,%ld", p.first, p.second);
	key = key + numstr;

	return key;
}

bool geometry::addSegment(int idx1, int idx2)
{
	return addSegment(idx1, idx2, DONOTADDEXISTINGSEGMENT);
}

bool geometry::addSegment(int idx1, int idx2, int operationTypeForExistingSegment)
{
	if(idx1 == idx2)
	{
		//printf("zero distance...do not add\n");
		return false;
	}

	std::pair<long, long> s1 = computePair(idx1, idx2);

	/*new linear search*/
	std::string key = computeKey(s1);
//	char numstr[100]; // enough to hold all numbers up to 64-bits
//	sprintf(numstr, "%d,%d", s1.first, s1.second);
//	key = key + numstr;

	std::map<std::string, long>::iterator it = segMap.find(key);
	if(it != segMap.end())
	{
		if(operationTypeForExistingSegment == DONOTADDEXISTINGSEGMENT)
		{
			//do not nothing for now
		}
		if(operationTypeForExistingSegment == REMOVEEXISTINGSEGMENT)
		{
			segs.erase(segs.begin() + it->second);
			segMap.erase(it);
		}
		return false;
	}

	segs.push_back(s1);

	segMap[key] = segs.size() - 1;

	return true;
}

int geometry::addCoordinate(convertcoordinate& coord)
{
	/*new linear search*/
	std::string key = coord.toString(',');

	std::map<std::string, long>::iterator it = coordMap.find(key);
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

void geometry::appendGeometry(geometry& other, convertcoordinate& offset)
{
	std::map<long, long> indexOfIndex;
	for(size_t i = 0; i < other.coords.size(); i++)
	{
		convertcoordinate c2 = other.coords[i];
		c2[CIRCUITWIDTH] += offset[CIRCUITWIDTH];
		c2[CIRCUITDEPTH] += offset[CIRCUITDEPTH];
		c2[CIRCUITHEIGHT] += offset[CIRCUITHEIGHT];

		long cindex = addCoordinate(c2);
		indexOfIndex[i] = cindex;
	}

	for(size_t i = 0; i < other.segs.size(); i++)
	{
		addSegment(indexOfIndex[other.segs[i].first], indexOfIndex[other.segs[i].second], DONOTADDEXISTINGSEGMENT);
	}

	for(size_t i = 0; i < other.io.size(); i++)
	{
		io.push_back(indexOfIndex[other.io[i]]);
	}
}
