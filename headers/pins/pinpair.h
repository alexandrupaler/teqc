#ifndef PINPAIR_H_
#define PINPAIR_H_

#include "pins/pindetails.h"


#define SOURCEPIN 1
#define DESTPIN 0

/**
 * The class represents a collection of two coordinates, a type and an id. It is used for multiple purposes:
 * - in circuits, each input or output vertex has exactly two neighbouring normal vertices. These are pins,
 * the points where defects coming out from distillation boxes are joined with the circuit geometry
 * - for distillation boxes, the pins are the points where the defects connecting the geometry with the box are joined
 * Because the class includes the members type and id, by means of typedefs it is further used for defining
 * the coordinates of the distillation boxes in the scheduler (the id is not necessary), and the pairs of vertices to
 * be connected in connectpins (type and id are not necessary).
 */
class pinpair
{
public:
	pinpair();

	pinpair(const pinpair& other);

	pinpair& operator=(const pinpair& other);

	long& operator[](int i);

	void setPinDetail(int idx, pindetails& c);

	pindetails& getPinDetail(int idx);

	long minDistBetweenPins();

	bool isColinear();

	int getType();

	/**
	 * The source is always earlier in time than the destination when
	 * pinpairs are used by A* to compute connections between boxes and connection
	 * pool, or connection pool and circuit
	 * However, it is easier/faster for A* to compute pathes from dense
	 * regions to undense regions. Therefore, for connections between boxes and
	 * pool, source and destination are reversed, and A* has to know this
	 */
	bool hasSourceAndDestinationReversed;

	/**
	 * Some connections should be allowed to go through the connections channel
	 */
	bool allowConnectionThroughChannel;

protected:
	pindetails pins[2];
	long id;
	long type;
};


#endif /* PINPAIR_H_ */
