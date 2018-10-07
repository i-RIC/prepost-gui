#ifndef GEODATAPOLYGONABSTRACTPOLYGON_IMPL_H
#define GEODATAPOLYGONABSTRACTPOLYGON_IMPL_H

#include "../geodatapolygonabstractpolygon.h"

#include <guibase/polygon/polygoncontroller.h>

class vtkDoubleArray;

class GeoDataPolygonAbstractPolygon::Impl
{
public:
	Impl(GeoDataPolygon* parent);

	void setupScalarValues();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	GeoDataPolygon* m_parent;
	PolygonController m_polygonController;
	vtkDoubleArray* m_scalarValues;
};

#endif // GEODATAPOLYGONABSTRACTPOLYGON_IMPL_H
