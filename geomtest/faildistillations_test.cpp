#include <gtest/gtest.h>

#include "faildistillations.h"
#include "extendedfd.h"

TEST(faildistillationstest, computepinpairs)
{
	extendedFD fd(1, 1);
	//a single input of ATYPE

	vector<pair<int, int> > pp = fd.computePinIndexPairs(0);
	EXPECT_EQ(pp.size(), 2);

	EXPECT_EQ(pp[0].first, 0);
	EXPECT_EQ(pp[0].second, 0);

	EXPECT_EQ(pp[1].first, 1);
	EXPECT_EQ(pp[1].second, 1);


	fd.makeHorizontalPin(0);

	pp = fd.computePinIndexPairs(0);
	EXPECT_EQ(pp.size(), 2);

	EXPECT_EQ(pp[0].first, 0);
	EXPECT_EQ(pp[0].second, 1);

	EXPECT_EQ(pp[1].first, 1);
	EXPECT_EQ(pp[1].second, 0);
}

TEST(faildistillationstest, storeconnectentry)
{
	extendedFD fd(1, 1);

	int lastpos = -2;
	double prob = 0;
	int ioindex = 0;

	//connect first circuit IO, to first distillation box
	int size = fd.storeConnectEntry(0, 0);
	EXPECT_EQ(size, 2);

	//if NONBOXAVAIL is offered, no connections are inserted, size unchanged
	size = fd.storeConnectEntry(0, -1);
	EXPECT_EQ(size, 2);
}

TEST(faildistillationtest, decidewithnoadditional)
{
	extendedFD fd(1, 1);

	double prob = 0;
	int origpos = 0;
	int lastpos = -2;

	//test never fail
	int currpos1 = fd.decide(prob, origpos, lastpos);
	EXPECT_EQ(currpos1, 0);
	EXPECT_EQ(lastpos, -2);

	//test always fail
	prob = 1;
	int currpos2 = fd.decide(prob, origpos, lastpos);
	EXPECT_EQ(currpos2, NOBOXESAVAIL);
}

TEST(faildistillationtest, decidewithadditional)
{
	extendedFD fd(1, 5);

	double prob = 1;
	int origpos = 0;
	int lastpos = -2;

	//test always fail
	int currpos2 = fd.decide(prob, origpos, lastpos);
	EXPECT_EQ(currpos2, NOBOXESAVAIL);
}
