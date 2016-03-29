#include <gtest/gtest.h>

#include "cnotcounter.h"
#include "circuitmatrix.h"
#include "gatenumbers.h"

TEST(circuitmatrixtest, findtests)
{
	circuitmatrix cm;
	qubitline line(4, EMPTY);
	vector<qubitline> rows;
	rows.push_back(line);
	rows.push_back(line);
	rows.push_back(line);
	rows.push_back(line);

	cm.insertRows(0, rows);

	//generate four cnots
	cnotcounter cc;
	for(int i=0; i<4; i++)
	{
		int control = cc.getNextCnotNumber();
		int target = control + 1;

		cm[i][i] = control;
		cm[3 - i][i] = target;
	}

	//tests only cnots: single control, single target
	EXPECT_EQ(cm.findControl(3, 0)[0], 0);
	EXPECT_EQ(cm.findTarget(0, 0)[0], 3);

	EXPECT_EQ(cm.findControl(2, 1)[0], 1);
	EXPECT_EQ(cm.findTarget(1, 1)[0], 2);

	EXPECT_EQ(cm.findControl(1, 2)[0], 2);
	EXPECT_EQ(cm.findTarget(2, 2)[0], 1);

	EXPECT_EQ(cm.findControl(0, 3)[0], 3);
	EXPECT_EQ(cm.findTarget(3, 3)[0], 0);
}

TEST(circuitmatrixtest, isTests)
{
	circuitmatrix cm;
	qubitline line(3, WIRE);
	vector<qubitline> rows;
	rows.push_back(line);

	cm.insertRows(0, rows);

	EXPECT_EQ(cm.indexLessThanSize(0, 2), true);
	EXPECT_EQ(cm.indexLessThanSize(0, 3), false);

	cm[0][0] = INPUT;
	EXPECT_EQ(cm.isInput(0, 0), true);

	cm[0][2] = OUTPUT;
	EXPECT_EQ(cm.isOutput(0, 2), true);

	cm[0][1] = WIRE;
	EXPECT_EQ(cm.isWire(0, 1), true);

	cm[0][0] = AA;
	EXPECT_EQ(cm.isInitialisation(0, 0), true);
	EXPECT_EQ(cm.isMeasurement(0, 0), false);

	cm[0][2] = MA;
	EXPECT_EQ(cm.isInitialisation(0, 2), false);
	EXPECT_EQ(cm.isMeasurement(0, 2), true);

	cm[0][1] = EMPTY;
	EXPECT_EQ(cm.isEmpty(0, 1), true);
}

TEST(circuitmatrixtest, insertstest)
{
	circuitmatrix cm;

	qubitline line;
	vector<qubitline> rows;
	rows.push_back(line);

	cm.insertRows(0, rows);
	EXPECT_EQ(cm.getNrLines(), 1);

	cm.insertColumns(0, 3);
	EXPECT_EQ(cm.getMaxColumn(), 3);

	cm.removeEmptyColumns();
	EXPECT_EQ(cm.getMaxColumn(), 0);

	cm.removeEmptyRows();
	EXPECT_EQ(cm.getNrLines(), 0);

	qubitline line2(5, WIRE);
	rows.push_back(line2);
	cm.insertRows(0, rows);
	cm.removeEmptyRows();
	EXPECT_EQ(cm.getNrLines(), 1);
	EXPECT_EQ(cm.getMaxColumn(), 5);
}
