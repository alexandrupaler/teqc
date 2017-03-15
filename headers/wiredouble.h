#ifndef WIREDOUBLE_H_
#define WIREDOUBLE_H_

/*
 * used for a circular double linked list
 */
class wireDouble
{
public:
	wireDouble()
	{
		number = 0;
		prev = 0;
		next = 0;
		isStart = false;
	};

	unsigned int number;
	wireDouble* prev;
	wireDouble* next;
	bool isStart;
};

#endif /* WIREDOUBLE_H_ */
