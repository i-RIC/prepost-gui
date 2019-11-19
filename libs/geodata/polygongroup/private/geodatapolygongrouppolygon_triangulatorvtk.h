#ifndef GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORVTK_H
#define GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORVTK_H

#include "geodatapolygongrouppolygon_triangulatori.h"

class GeoDataPolygonGroupPolygon::TriangulatorVtk : public TriangulatorI
{
public:
	std::vector<unsigned int> triangulate(GeoDataPolygonGroupPolygon* polygon) override;
};

#endif // GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORVTK_H
