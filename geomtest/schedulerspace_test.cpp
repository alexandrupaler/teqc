#include <gtest/gtest.h>

#include "schedulerspace.h"

TEST(schedulerspacetest, constructinterval)
{
	//empty nandc
	numberandcoordinate nandc;
	//no length
	schedulerspace sp(nandc, 0);

	//for noneq begin and end, interval is constructed
	interval pai;
	sp.constructInterval(0, 5, pai);
	EXPECT_EQ(pai.first, 0);
	EXPECT_EQ(pai.second, 5);

	//for equal begin and end values, the interval is not constructed
	interval pai2(11, 11);
	sp.constructInterval(9, 9, pai2);
	EXPECT_EQ(pai2.first, 11);
	EXPECT_EQ(pai2.second, 11);
}

TEST(schedulerspacetest, splitinterval)
{
	//empty nandc
	numberandcoordinate nandc;
	nandc.addSingleTypeEntries(1, ATYPE, 0, 4, 4);
	//no length
	schedulerspace sp(nandc, 4);

	interval big;
	big.first = 10;
	big.second = 20;

	vector<interval> res = sp.splitInterval(big, 11, 5);
	EXPECT_EQ(res.size(), 2);
	EXPECT_EQ(res[0].first, 10);
	EXPECT_EQ(res[0].second, 11);
	EXPECT_EQ(res[1].first, 16);
	EXPECT_EQ(res[1].second, 20);

	vector<interval> res2 = sp.splitInterval(big, 10, 5);
	EXPECT_EQ(res2.size(), 1);
	EXPECT_EQ(res2[0].first, 15);
	EXPECT_EQ(res2[0].second, 20);

	vector<interval> res3 = sp.splitInterval(big, 15, 5);
	EXPECT_EQ(res3.size(), 1);
	EXPECT_EQ(res3[0].first, 10);
	EXPECT_EQ(res3[0].second, 15);
}

TEST(schedulerspacetest, replaceinterval)
{
	//empty nandc
	numberandcoordinate nandc;
	nandc.addSingleTypeEntries(1, ATYPE, 0, 4, 4);
	//no length
	schedulerspace sp(nandc, 4);

	interval one;
	one.first = 10;
	one.second = 20;

	interval two;
	two.first = 30;
	two.second = 40;

	vector<interval> prs;
	prs.push_back(one);
	prs.push_back(two);

	sp.replaceInterval(prs, 0, 0, HEIGHTINTERVALS);

	EXPECT_EQ(sp.getNumberOfIntervals(0, HEIGHTINTERVALS), 2);

	interval i1 = sp.getHeightInterval(0,0);
	EXPECT_EQ(i1.first, 10);
	EXPECT_EQ(i1.second, 20);

	interval i2 = sp.getHeightInterval(0,1);
	EXPECT_EQ(i2.first, 30);
	EXPECT_EQ(i2.second, 40);
}

TEST(schedulerspacetest, occupywidthintervals)
{
	//empty nandc
	numberandcoordinate nandc;
	nandc.addSingleTypeEntries(1, ATYPE, 0, 4, 4);
	//no length
	schedulerspace sp(nandc, 4);

	sp.occupyWidthIntervals(0, 0, 4, 4);
	for(int i=0; i<4; i+=2)
	{
		EXPECT_EQ(sp.getNumberOfIntervals(i, WIDTHINTERVALS), 1);

		pair<int, int> interval = sp.getWidthInterval(i, 0);
		EXPECT_EQ(interval.first, 4);
		EXPECT_EQ(interval.second, INT_MAX);
	}
}

TEST(schedulerspacetest, checkinterval)
{
	//empty nandc
	numberandcoordinate nandc;
	nandc.addSingleTypeEntries(1, ATYPE, 0, 4, 4);
	//no length
	schedulerspace sp(nandc, 4);

	//occupy (2,8)
	sp.occupyWidthIntervals(2, 0, 6, 4);

	for(int i=0; i<4; i+=DELTA)
	{
		//check (4,8)
		bool busy = sp.checkWidthInterval(4, i, 4);
		EXPECT_EQ(busy, false);
		//check(0,2)
		bool notbusy = sp.checkWidthInterval(0, i, 2);
		EXPECT_EQ(notbusy, true);
		//check(0,4)
		busy = sp.checkWidthInterval(0, i, 4);
		EXPECT_EQ(busy, false);
	}

	bool doesnot = sp.boxFitsIntoWidthInterval(0, 0, 4, 4);
	EXPECT_EQ(doesnot, false);
}

TEST(schedulerspacetest, findheightgoesup)
{
	numberandcoordinate nandc;
	//add two elements of dimension 4
	nandc.addSingleTypeEntries(2, ATYPE, 0, 4, 4);

	schedulerspace sp(nandc, 4);

	int positionWidthAxis = 0;
	int positionHeightAxis = -1;//put something wrong

	// occupy interval (0, 8) on width
	bool found = sp.computeCoordinates(8, 4, positionWidthAxis, positionHeightAxis);
	EXPECT_EQ(found, true);
	EXPECT_EQ(positionHeightAxis, 0);
	EXPECT_EQ(positionWidthAxis, 0);

	//occupy interval (0, 4) on width -> no space -> height goes up to 4
	found = sp.computeCoordinates(4, 4, positionWidthAxis, positionHeightAxis);
	EXPECT_EQ(found, true);
	EXPECT_EQ(positionHeightAxis, 4);
	EXPECT_EQ(positionWidthAxis, 0);

	EXPECT_EQ(sp.getNumberOfIntervals(4, HEIGHTINTERVALS), 1);
	EXPECT_EQ(sp.getHeightInterval(4, 0).first, 0);
	EXPECT_EQ(sp.getHeightInterval(4, 0).second, INT_MAX);

	EXPECT_EQ(sp.getNumberOfIntervals(4, WIDTHINTERVALS), 1);
	EXPECT_EQ(sp.getWidthInterval(0, 0).first, 8);
	EXPECT_EQ(sp.getWidthInterval(0, 0).second, INT_MAX);

	//ocuppy interval (4, 8) on width ->no space -> height goes up to 4
	positionWidthAxis = 4;

	found = sp.computeCoordinates(4, 4, positionWidthAxis, positionHeightAxis);
	EXPECT_EQ(found, true);
	EXPECT_EQ(positionHeightAxis, 4);
	EXPECT_EQ(positionWidthAxis, 4);
}

TEST(schedulerspacetest, findheightsinglefit)
{
	numberandcoordinate nandc;
	//add a single element, of dimension 4
	nandc.addSingleTypeEntries(1, ATYPE, 0, 4, 4);

	schedulerspace sp(nandc, 4);

	int positionWidthAxis = 0;
	int positionHeightAxis = -1;//put something wrong

	bool found = sp.computeCoordinates(4, 4, positionWidthAxis, positionHeightAxis);

	EXPECT_EQ(found, true);
	EXPECT_EQ(positionHeightAxis, 0);
	EXPECT_EQ(positionWidthAxis, 0);
}
