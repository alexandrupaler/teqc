#include <gtest/gtest.h>

#include "computeadditional.h"

TEST(computeadditional, combinatorial)
{
	computeadditional c;
	EXPECT_EQ(3, c.comb(3, 1));
	EXPECT_EQ(1, c.comb(3, 0));
}

TEST(computeadditional, binomialsums)
{
	computeadditional c;
	EXPECT_DOUBLE_EQ(0.50331648, c.sum(7, 8, 0.2));
	EXPECT_NEAR(0.9999775473976, c.sum(7, 18, 0.2), 0.0000001);
}

TEST(computeadditional, findparameter)
{
	computeadditional c;
	EXPECT_EQ(21, c.findParam(7, 0.2, 0.000001));
	EXPECT_EQ(26, c.findParam(10, 0.2, 0.000001));
	EXPECT_EQ(53, c.findParam(10, 0.5, 0.000001));
}
