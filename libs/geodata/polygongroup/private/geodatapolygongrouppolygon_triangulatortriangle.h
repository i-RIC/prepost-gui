#ifndef GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORTRIANGLE_H
#define GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORTRIANGLE_H

#include "geodatapolygongrouppolygon_triangulatori.h"


class GeoDataPolygonGroupPolygon::TriangulatorTriangle : public TriangulatorI
{
public:
	std::vector<unsigned int> triangulate(GeoDataPolygonGroupPolygon* polygon) override;
};

#endif // GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORTRIANGLE_H
