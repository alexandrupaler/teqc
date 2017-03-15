#include "heuristicparameters.h"

void heuristicparameters::train()
{

}

void heuristicparameters::init()
{
	/*
	 * Geometry related values are units of DELTA
	 */
	connPoolStart = 8;
	maxConnections = 10;

	connectionBufferTimeLength = 8;//20
	connectionHeight = 9;//4;
	manhattanMultiplier = 150;
	timeBeforePoolEnd = /*20*/ 6;//ASAP 6
	pointJournalLength = 6;

//	connectionBoxHeight = 2 * connectionHeight + 1;//9;
	connectionBoxHeight = connectionHeight + 1;//9;
	connectionBoxWidthMargin = 4;
}
