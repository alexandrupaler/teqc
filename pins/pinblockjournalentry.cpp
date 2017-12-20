#include <sstream>

#include "pins/pinblockjournalentry.h"
#include "pins/pinconstants.h"

pinblockjournalentry::pinblockjournalentry()
{
	blockType = WALKFREE;
	blockPriority = NOPRIORITY;
	synthesisStepNumber = -1;
	operationType = "n/a";
	poolConnectionNumber = -1;
}

std::string pinblockjournalentry::toString()
{
	std::stringstream ret;

	std::string walk;
	switch(blockType)
	{
	case WALKFREE:
		walk = "FREE";
		break;
	case WALKBLOCKED_OCCUPY:
		walk ="OCCUPY";
		break;
	case WALKBLOCKED_GUIDE:
		walk = "GUIDE";
		break;
	case WALKUSED:
		walk = "USED";
		break;
	case WALKUSEDEND:
		walk = "USEDEND";
		break;
	}

	ret << " op:" << this->operationType
			<< " walk:" << walk
			<< " prio:" << this->blockPriority
			<< " step:" << this->synthesisStepNumber
			<< " conn:" << this->poolConnectionNumber;

	return ret.str();
}
