#include "heuristicparameters.h"

void heuristicparameters::train()
{

}

void heuristicparameters::init()
{
	/*
	 * Geometry related values are units of DELTA
	 */
	connPoolStart = 3;
	maxConnections = 10000;

	connectionBufferTimeLength = 1;//8;//20
	connectionHeight = 1/*9*/;//4;
	manhattanMultiplier = 150;//150
	timeBeforePoolEnd = /*20*/ 1;//ASAP 6
	pointJournalLength = 0/*6*/;//no journaling

	connectionBoxHeight = connectionHeight - 1; //connectionHeight + 1;//9;
	connectionBoxWidthMargin = 4;
}
