#ifndef PINBLOCKJOURNALENTRY_H_
#define PINBLOCKJOURNALENTRY_H_

#include <string>

using namespace std;

struct pinblockjournalentry
{
	int blockType;
	int blockPriority;
	int synthesisStepNumber;
	string operationType;/*e.g. connect, extend, finalise*/
	int poolConnectionNumber;

	pinblockjournalentry();

	string toString();
};


#endif /* PINBLOCKJOURNALENTRY_H_ */
