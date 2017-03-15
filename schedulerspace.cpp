#include <stdio.h>
#include <climits>

#include "numberandcoordinate.h"
#include "schedulerspace.h"

schedulerspace::schedulerspace(numberandcoordinate& nandc, int maxBoxHeight)
{
	//stabileste spatiile libere pe randuri
	interval liber;
	liber.first = 0;
	liber.second = INT_MAX;

	freeSpaceOnHeight.clear();
	for (vector<pinpair>::iterator it = nandc.inputList.begin(); it != nandc.inputList.end(); it++)
	{
		freeSpaceOnHeight[(*it)[OFFSETNONCOORD + CIRCUITWIDTH]].push_back(liber);
	}
	//the maximum of boxes that could exist on a column is the number of inputs
	//a circuit has times the height of the highest box
	freeSpaceOnWidth.clear();
	for (int i = 0; i < nandc.size() * maxBoxHeight; i += DELTA)
	{
		//vector<vector<int> > libere;
		//libere.push_back(liber);
		freeSpaceOnWidth[i].push_back(liber);
	}
}

void schedulerspace::replaceInterval(vector<interval> prs, int pos, int collectionidx, bool heightOrWidth)
{
	map<int, vector<interval> >::iterator collection =
			(heightOrWidth == HEIGHTINTERVALS) ?
					freeSpaceOnHeight.find(collectionidx)
					: freeSpaceOnWidth.find(collectionidx);

	//reserve the space such that iterators are not changed after element insertion, deletion
	collection->second.reserve(collection->second.size() + prs.size());

	vector<interval>::iterator lit = collection->second.begin() + pos;

	collection->second.insert(lit, prs.begin(), prs.end());
	collection->second.erase(collection->second.begin() + prs.size() + pos);
}

int schedulerspace::getNumberOfIntervals(int index, bool heightOrWidth)
{
	return (heightOrWidth == HEIGHTINTERVALS) ? freeSpaceOnHeight[index].size() : freeSpaceOnWidth[index].size();
}


bool schedulerspace::computeCoordinates(int reqWidth, int reqHeight, int positionWidthAxis, int& positionHeightAxis)
{
	vector<interval>::iterator heightInterval = freeSpaceOnHeight[positionWidthAxis].begin();

	positionHeightAxis = NOSPACE;
	int lastPositionHeightAxis = positionHeightAxis;

	while(positionHeightAxis == NOSPACE)
	{
		int nrPossiblePositions;
		//found at least a position on the HeightAxis
		lastPositionHeightAxis = findHeightAxisPosition(heightInterval, positionWidthAxis, reqHeight, nrPossiblePositions);
		if(lastPositionHeightAxis == NOSPACE)
		{
			//because of "something wrong", there is no space in the field
			//in theory and practice, if the field is initialised correctly, this situation will not happen
			return false;
		}

		//check that using a given height position, all the widths contain the reqWidth space
		for(int i=0; i<nrPossiblePositions; i++)
		{
			bool fitsEveryWhere = boxFitsIntoWidthInterval(positionWidthAxis, lastPositionHeightAxis, reqWidth, reqHeight);
			if(fitsEveryWhere)
			{
				//found coordinates
				positionHeightAxis = lastPositionHeightAxis;
				break;
			}
			lastPositionHeightAxis++;
		}
		//if the last interval was used, the by incrementing the iterator the end will be reached -> not good
		if(positionHeightAxis == NOSPACE)
		{
			heightInterval++;
		}
	}

	vector<pair<int,int> > prs = splitInterval(*heightInterval, positionHeightAxis, reqHeight);
	replaceInterval(prs, heightInterval - freeSpaceOnHeight[positionWidthAxis].begin(), positionWidthAxis, true);

	occupyWidthIntervals(positionWidthAxis, positionHeightAxis, reqWidth, reqHeight);

	return true;
}


bool schedulerspace::constructInterval(int val1, int val2, interval& p1)
{
	if (val1 != val2 && val2 - val1 >= 0)
	{
		int e1 = val1;
		p1.first = e1;
		int e2 = val2;
		p1.second = e2;

		return true;
	}
	return false;
}

//vector<pair<int, int> > schedulerspace::splitInterval(vector<pair<int, int> >::iterator& it,
//		int boxPosition, int intervalLength,
//		vector<pair<int,int> >& freeIntervals)
vector<interval> schedulerspace::splitInterval(interval& pai, int boxPosition, int intervalLength)
{
	vector<interval> ret;

	int pp1 = pai.first;
	int pp2 = pai.second;

	//vector<pair<int,int> >::iterator curr = it;
	//int iteratorOffset = 0;

	//get the interval on the left side
	interval p1;
	if (constructInterval(pp1, boxPosition, p1))
	{
		//curr = freeIntervals.insert(it, p1);
		//iteratorOffset = 1;

		ret.push_back(p1);
	}

	//get the interval on the right side
	interval p2;
	if (constructInterval(boxPosition + intervalLength, pp2, p2))
	{
		//curr = freeIntervals.insert(curr + iteratorOffset, p2);
		//iteratorOffset = 1;
		ret.push_back(p2);
	}

	//remove the big interval
	//freeIntervals.erase(curr + iteratorOffset);

	return ret;
}

int schedulerspace::findHeightAxisPosition(vector<interval>::iterator& heightInterval, int positionWidthAxis, int reqHeight, int& nrPossiblePositions)
{
	//vector<pair<int, int> >::iterator heightInterval;

	for (/*heightInterval = freeSpaceOnHeight[positionCircuitWidthAxis].begin()*/;
			heightInterval != freeSpaceOnHeight[positionWidthAxis].end(); heightInterval++)
	{
		int availHeight = (*heightInterval).second - (*heightInterval).first;
		int localPossiblePositions = availHeight - reqHeight;
		if( localPossiblePositions > 0)//should be >= ?
		{
			nrPossiblePositions = localPossiblePositions;
			return heightInterval->first;
		}
	}

	return NOSPACE;
}

bool schedulerspace::checkWidthInterval(int positionWidthAxis, int positionHeightAxis, int reqWidth)
{
	vector<pair<int, int> >::iterator widthInterval;

	for (widthInterval = freeSpaceOnWidth[positionHeightAxis].begin(); widthInterval != freeSpaceOnWidth[positionHeightAxis].end(); widthInterval++)
	{
		if ((*widthInterval).first <= positionWidthAxis && positionWidthAxis <= (*widthInterval).second)
		{
			//printInterval(*widthInterval);

			//contine row + latime?
			//int availWidth = (*nit).second - (*nit).first;
			int availWidth = (*widthInterval).second - positionWidthAxis;
			//printf("avail:%d %d\n", availWidth, reqWidth);
			//am gasit intervalul liber
			if (availWidth >= reqWidth)
			{
				return true;
			}
		}
	}

	return false;
}

bool schedulerspace::boxFitsIntoWidthInterval(int positionWidthAxis, int positionHeightAxis, int reqWidth, int reqHeight)
{
	//cauta pe coloane spatiul
	bool fitsEverywhere = true;
	int currHeightPos = NOSPACE;

	//for (currHeightPos = positionHeightAxis; currHeightPos <= positionHeightAxis + reqHeight; currHeightPos += DELTA)
	for (currHeightPos = positionHeightAxis; currHeightPos < positionHeightAxis + reqHeight; currHeightPos += DELTA)
	{
//		bool fitsIntoWidthInterval = checkWidthInterval(positionWidthAxis, positionHeightAxis, reqWidth);
		bool fitsIntoWidthInterval = checkWidthInterval(positionWidthAxis, currHeightPos, reqWidth);

		if(!fitsIntoWidthInterval)
			return false;

		//fitsEverywhere = (fitsEverywhere && fitsIntoWidthInterval);
	}

	return fitsEverywhere;// ? currHeightPos : NOSPACE;
}

void schedulerspace::occupyWidthIntervals(int positionWidthAxis, int positionHeightAxis, int reqWidth, int reqHeight)
{
	//sparge spatiul pe toate coloanele
	//for (int j = positionHeightAxis; j <= positionHeightAxis + reqHeight; j += DELTA)
	for (int j = positionHeightAxis; j < positionHeightAxis + reqHeight; j += DELTA)
	{
		for (vector<pair<int, int> >::iterator nit = freeSpaceOnWidth[j].begin(); nit != freeSpaceOnWidth[j].end(); nit++)
		{
			if (positionWidthAxis >= (*nit).first && positionWidthAxis <= (*nit).second)
			{
				vector<pair<int,int> > prs;
				prs = splitInterval(*nit, positionWidthAxis, reqWidth);
				replaceInterval(prs, nit - freeSpaceOnWidth[j].begin(), j, false);
				break;
			}
		}
	}
}

void schedulerspace::printInterval(interval& it)
{
	printf("%d:%d\n", it.first, it.second);
}
