#ifndef GEODATAPOLYLINEABSTRACTPOLYLINE_IMPL_H
#define GEODATAPOLYLINEABSTRACTPOLYLINE_IMPL_H

#include "../geodatapolylineabstractpolyline.h"

#include <guibase/polyline/polylinecontroller.h>

class vtkDoubleArray;

class GeoDataPolyLineAbstractPolyLine::Impl
{
public:
	Impl(GeoDataPolyLine* parent);

	void setupScalarValues();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	GeoDataPolyLine* m_parent;
	PolyLineController m_polylineController;
	vtkDoubleArray* m_scalarValues;
};

#endif // GEODATAPOLYLINEABSTRACTPOLYLINE_IMPL_H
