#include "recycling/wireelement.h"
#include <climits>

wireelement* wireelement::getUpdatedWire(long x)
{
	wireelement* current = this;
//	printf("%d %d\n", x, current->threshold);

	while(current->threshold != LONG_MIN)
	{
		if(current->threshold > x)
		{
			current = current->ngh[0];
		}
		else if(current->threshold < x)
		{
			current = current->ngh[1];
		}
		else if(current->threshold == x)
		{
			//nothing
		}
	}

	return current;
}

wireelement::wireelement()
{
	ngh[0] = NULL;
	ngh[1] = NULL;
	prev = NULL;
	next = NULL;

	threshold = LONG_MIN;
	number = -1;

//	start = NULL;
//	stop = NULL;
}

void wireelement::linkForward(wireelement* fwd)
{
	this->next = fwd;
	if(fwd != NULL)
	{
		fwd->prev = this;
	}
}

wireelement* wireelement::cutForward()
{
	wireelement* oldNext = this->next;
	if(this->next != NULL)
	{
		this->next->prev = NULL;
	}
	this->next = NULL;

	return oldNext;
}

wireelement* wireelement::cutBackward()
{
	wireelement* oldPrev = this->prev;

	if(this->prev != NULL)
	{
		this->prev->next = NULL;
	}
	this->prev = NULL;


	return oldPrev;
}
