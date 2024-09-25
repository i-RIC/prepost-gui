#ifndef GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORVTK_H
#define GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORVTK_H

#include "geodatapolygongrouppolygon_triangulatori.h"

class vtkIdList;
class vtkPolygon;

class GeoDataPolygonGroupPolygon::TriangulatorVtk : public TriangulatorI
{
public:
	TriangulatorVtk();
	~TriangulatorVtk();

	std::vector<unsigned int> triangulate(GeoDataPolygonGroupPolygon* polygon) override;

private:
	vtkPolygon* m_polygon;
	vtkIdList* m_idList;
};

#endif // GEODATAPOLYGONGROUPPOLYGON_TRIANGULATORVTK_H
