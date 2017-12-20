#ifndef PINBLOCKJOURNALENTRY_H_
#define PINBLOCKJOURNALENTRY_H_

#include <string>

struct pinblockjournalentry
{
	int blockType;
	int blockPriority;
	int synthesisStepNumber;
	std::string operationType;/*e.g. connect, extend, finalise*/
	int poolConnectionNumber;

	pinblockjournalentry();

	std::string toString();
};


#endif /* PINBLOCKJOURNALENTRY_H_ */
