#include "pins/pindetails.h"

void pindetails::addPinBlock(int offset, int direction, int distance, int blockType, pinblockjournalentry& journal)
{
	pinblocker bl;
	bl.offset = offset;
	bl.axis = direction;
	bl.distance = distance;

	bl.journal = journal;

	bl.blockId = bl.getNextAvailableId();
	bl.blockType = blockType;

	blocks.push_back(bl);
}

void pindetails::removeBlocks()
{
	for(vector<pinblocker>::iterator it = blocks.begin();
			it != blocks.end(); it++)
	{
		//reuse this id
		pinblocker::availableIds.push_back(it->blockId);
	}

	blocks.clear();
}
