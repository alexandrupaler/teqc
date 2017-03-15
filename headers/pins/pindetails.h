#ifndef PINDETAILS_H_
#define PINDETAILS_H_

#include "pinblocker.h"
#include "convertcoordinate.h"

#include <vector>

using namespace std;

struct pindetails
{
	convertcoordinate coord;
	vector<pinblocker> blocks;

	void addPinBlock(int offset, int direction, int distance, int blockType, pinblockjournalentry& journal);

	void removeBlocks();
};


#endif /* PINDETAILS_H_ */
