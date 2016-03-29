#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#define DUALUPCOORD 1
#define DUALDOWNCOORD -1

#include <vector>
#include <map>
#include <string>
#include "fileformats/generaldefines.h"
#include "numberandcoordinate.h"

using namespace std;

/**
 * Class used for storing a geometry which is based on vertices having three
 * dimensional coordinates and segments connecting the vertices. Segments are
 * stored as pairs of vertex indices.
 */
class geometry{
public:

	/**
	 * Adds a segment. Does not check if the indices are already existing.
	 * If the segment existed, it is not inserted again in the
	 * segment collection.
	 * @param idx1
	 * @param idx2
	 */
	void addSegment(int idx1, int idx2);

	/**
	 * Adds the coordinate of a vertex, and returns its index from the
	 * collection. If the coordinate existed, it is not inserted again, and the
	 * existing index is returned.
	 * @param c the coordinate to insert
	 * @return index of the coordinate in the vertex collection
	 */
	int addCoordinate(coordinate& c);

	/**
	 * Adds a vertex coordinate and marks the vertex as io.
	 * @param ioc - the coordinate
	 * @param isInit - not used, to be removed?
	 * @return
	 */
	int addIOPoint(coordinate& ioc, bool isInit);

	/**
	 * Updates the bounding box member after considering the input coordinate.
	 * @param c the coordinate
	 */
	void updateBoundingBox(coordinate& c);

public:
	/**
	 * The collection of vertex coordinates.
	 */
	vector<coordinate> coords;

	/**
	 * The collection of coordinate index pairs representing segments.
	 */
	vector<pair<long, long> > segs;

	/**
	 * A collection of vertex indices that are marked as "special". For
	 * the case of circuits, the indices represent input/output points.
	 */
	vector<long> io;//input outputs

	/**
	 * The map uses a serialised version of the coordinates as key for
	 * retrieving their index in the coordinate collection.
	 */
	map<string, long> coordMap;

	/**
	 * The map uses a serialised version of the segments as key for
	 * retrieving their index from the segments collection.
	 */
	map<string, long> segMap;

	/**
	 * The bounding box of the generated geometry
	 */
	coordinate boundingbox;
};


#endif /* GEOMETRY_H_ */
