#include "recycling/wireorder.h"
#include <stdlib.h>
#include <stdio.h>

wireorder::wireorder()
{
	prev = NULL;
	next = NULL;

	number = 0;
}

wireorder* wireorder::cutFromNext()
{
	wireorder* nextO = this->next;
	if(nextO != NULL)
	{
		nextO->prev = NULL;
	}
	this->next = NULL;

	return nextO;
}

wireorder* wireorder::cutFromPrev()
{
	wireorder* prevO = this->prev;
	if(prevO != NULL)
	{
		prevO->next = NULL;
	}
	this->prev = NULL;

	return prevO;
}

void wireorder::linkNext(wireorder* newNext)
{
	if(newNext != NULL)
	{
		newNext->prev = this;
	}
	this->next = newNext;
}

void wireorder::linkPrev(wireorder* newPrev)
{
	if(newPrev != NULL)
	{
		newPrev->next = this;
	}
	this->prev = newPrev;
}

void wireorder::makeNext(wireorder* first, wireorder* last)
{
	//cut first and last from the list
	wireorder* firstPrev = first->cutFromPrev();
	wireorder* lastNext = last->cutFromNext();

	//join firstPrev and lastNext
	if(firstPrev != NULL)
	{
		firstPrev->linkNext(lastNext);
	}

	//cut this from the next
	wireorder* thisNext = this->cutFromNext();

	//insert first and last after this
	this->linkNext(first);
	last->linkNext(thisNext);
}

bool wireorder::operator <(wireorder const& rhs)
{
	wireorder* current = this;
	if(rhs.prev == NULL)
	{
		return false;
	}

	wireorder* target = rhs.prev->next;

	if(current == target)
		return false;

	while(current != target && current != NULL)
	{
		current = current->next;
	}

	return current != NULL;
}

std::set<wireorder*> wireorder::areInIntervalStartingFromHere(wireorder* first, wireorder* last, wireorder* stop)
{
	wireorder* current = this;

	std::set<wireorder*> ret;

	std::set<wireorder*> visit;

	while(current != NULL && current->prev != stop)
	{
		if(current == first)
			ret.insert(first);
		else if (current == last)
			ret.insert(last);

//		if(visit.find(current) != visit.end())
//		{
//			printf("???? %p %p %p %p\n", current, first, last, stop);
//			break;
//		}
//		visit.insert(current);
		current = current->next;
	}

	return ret;
}

std::vector<wireorder*> wireorder::joinWithInterval(wireorder* thisEnd, wireorder* startInterval, wireorder* stopInterval)
{
	std::vector<wireorder*> ret(2, NULL);

	//check {this, thisEnd} in [startInterval, stopInterval]
	std::set<wireorder*> ret1 = this->areInIntervalStartingFromHere(startInterval, stopInterval, thisEnd);

	//check {startInterval, stopInterval} in [this, thisEnd]
	std::set<wireorder*> ret2 = startInterval->areInIntervalStartingFromHere(this, thisEnd, stopInterval);

	if(ret1.size() == 0 && ret2.size() == 0)
	{
//		if(stopInterval->next == this)
//		{
//			ret[0] = startInterval;
//			ret[1] = thisEnd;
//		}
//		else if(thisEnd->next == startInterval)
//		{
//			ret[0] = this;
//			ret[1] = stopInterval;
//		}
//		else
//		{
			//intervals do not intersect
			thisEnd->makeNext(startInterval, stopInterval);
			ret[0] = this;
			ret[1] = stopInterval;
//		}
	}
	else if(ret1.find(startInterval) != ret1.end() && ret1.size() == 1)
	{
		//this < startInterval < thisEnd < stopInterval
		ret[0] = this;
		ret[1] = stopInterval;
	}
	else if(ret1.find(stopInterval) != ret1.end() && ret1.size() == 1)
	{
		//startInterval < this < stopInterval < thisEnd
		ret[0] = startInterval;
		ret[1] = thisEnd;
	}
	else if(ret1.find(startInterval) != ret1.end() && ret1.size() == 2)
	{
		//this < startInterval < stopInterval < thisEnd
		ret[0] = this;
		ret[1] = thisEnd;
	}

	else if(ret2.find(this) != ret2.end() && ret2.size() == 1)
	{
		//startInterval < this < stopInterval < thisEnd
		ret[0] = startInterval;
		ret[1] = thisEnd;
	}
	else if(ret2.find(thisEnd) != ret2.end() && ret2.size() == 1)
	{
		//this < startInterval < thisEnd < stopInterval
		ret[0] = this;
		ret[1] = stopInterval;
	}
	else if(ret2.find(this) != ret2.end() && ret2.size() == 2)
	{
		//startInterval < this < thisEnd < stopInterval
		ret[0] = startInterval;
		ret[1] = stopInterval;
	}

	return ret;
}


