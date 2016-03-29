#include <gtest/gtest.h>

#include "connectpins.h"

TEST(connectpinstest, extractpoint)
{
	inputpin line;
	for(int i=0; i<6; i++)
		line[OFFSETNONCOORD + i] = i;

	connectpins cp;

	vector<int> point1 = cp.extractPoint(CIRCPIN, line);
	vector<int> point2 = cp.extractPoint(DISTPIN, line);
	for(int i=0; i<3; i++)
	{
		//first position is index in the coords list
		EXPECT_EQ(point1[i + 1], line[OFFSETNONCOORD + i]);
		EXPECT_EQ(point2[i + 1], line[OFFSETNONCOORD + 3 + i]);
	}

	//index of second extracted point is +1 from the first opint index
	EXPECT_EQ(point2[0], point1[0] + 1);
}

TEST(connectpinstest, getafterfail)
{
	//a circuit input pin should be aligned with the distillation output pin
	//if the distillation is not simulated to be failed
	//failed Y distillations are on the left of the circuit
	//failed A distillations are on the right of the circuit
	//dist coordinates are the last three elements

	connectpins cp;

//	int vec1[] = {1, 0, 0, 1, 0, 0};
//	vector<int> line(vec1, vec1 + 6);

	coordinate c1;
	c1[0] = 1;
	c1[1] = 0;
	c1[2] = 0;

	coordinate c2;
	c2 = c1;

	int g1 = cp.getAfterFail2(c1, c2);
	EXPECT_EQ(g1, NONFAILEDBOX);//NORMAL

	c2[0] = 2;
	int g2 = cp.getAfterFail2(c1, c2);
	EXPECT_EQ(g2, ABOXADD);//ASIDE

	c2[0] = -1;
	int g3 = cp.getAfterFail2(c1, c2);
	EXPECT_EQ(g3, YBOXADD);//YSIDE
}

TEST(connectpinstest, offsetchangeindexandstore)
{
	connectpins cp;

	int vec[] = {-1/*index*/, 1 /*coord1*/, 2/*coord2*/, 3/*coord3*/};
	//vector<int> point(vec, vec + 6);

	coordinate point;

	//offset first index
	int index = cp.offsetChangeIndexAndStore(point, CIRCUITWIDTH, 1);
	EXPECT_EQ(index, 0);
	EXPECT_EQ(point[CIRCUITWIDTH], 2);

	//offset second index
	index = cp.offsetChangeIndexAndStore(point, CIRCUITDEPTH, 1);
	EXPECT_EQ(index, 1);
	EXPECT_EQ(point[CIRCUITDEPTH], 3);

	//offset third index
	index = cp.offsetChangeIndexAndStore(point, CIRCUITHEIGHT, 1);
	EXPECT_EQ(index, 2);
	EXPECT_EQ(point[CIRCUITHEIGHT], 4);

	//offset zero test
	index = cp.offsetChangeIndexAndStore(point, CIRCUITHEIGHT, 0);
	EXPECT_EQ(index, 2);//left unchanged
	EXPECT_EQ(point[CIRCUITHEIGHT], 4);//left unchanged
}
