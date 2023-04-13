#ifndef GEODATAPOLYGONABSTRACTPOLYGON_IMPL_H
#define GEODATAPOLYGONABSTRACTPOLYGON_IMPL_H

#include "../geodatapolygonabstractpolygon.h"

#include <guibase/polygon/polygoncontroller.h>

class vtkActor;
class vtkDoubleArray;

class GeoDataPolygonAbstractPolygon::Impl
{
public:
	Impl(GeoDataPolygon* parent);
	~Impl();

	void setupScalarValues();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	bool m_active;
	GeoDataPolygon* m_parent;

	vtkActor* m_linesActor;
	vtkActor* m_pointsActor;

	PolygonController m_polygonController;
	vtkDoubleArray* m_linesScalarValues;
	vtkDoubleArray* m_pointsScalarValues;
};

#endif // GEODATAPOLYGONABSTRACTPOLYGON_IMPL_H
