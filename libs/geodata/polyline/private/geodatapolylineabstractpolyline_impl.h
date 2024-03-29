#ifndef GEODATAPOLYLINEABSTRACTPOLYLINE_IMPL_H
#define GEODATAPOLYLINEABSTRACTPOLYLINE_IMPL_H

#include "../geodatapolylineabstractpolyline.h"
#include "geodatapolyline_displaysetting.h"

#include <guibase/polyline/polylinecontroller.h>

class vtkActor;
class vtkDoubleArray;

class GeoDataPolyLineAbstractPolyLine::Impl
{
public:
	Impl(GeoDataPolyLine* parent);
	~Impl();

	void setupScalarValues();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	GeoDataPolyLine* m_parent;
	vtkActor* m_linesActor;
	vtkActor* m_pointsActor;

	PolyLineController m_polylineController;
	vtkDoubleArray* m_linesScalarValues;
	vtkDoubleArray* m_pointsScalarValues;
};

#endif // GEODATAPOLYLINEABSTRACTPOLYLINE_IMPL_H
