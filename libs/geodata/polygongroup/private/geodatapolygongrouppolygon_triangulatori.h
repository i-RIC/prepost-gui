#ifndef GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORI_H
#define GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORI_H

#include "../geodatapolygongrouppolygon.h"

#include <vector>

class GeoDataPolygonGroupPolygon::TriangulatorI
{
public:
	TriangulatorI() {}
	virtual ~TriangulatorI() {}

	virtual std::vector<unsigned int> triangulate(GeoDataPolygonGroupPolygon* polygon) = 0;
};

#endif // GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORI_H

