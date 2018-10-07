#include "geodatapolylineimplpolyline.h"

#include <guibase/polyline/polylinecontroller.h>

#include <vtkActor.h>
#include <vtkProperty.h>

GeoDataPolyLineImplPolyLine::GeoDataPolyLineImplPolyLine(GeoDataPolyLine* parent) :
	GeoDataPolyLineAbstractPolyLine {parent}
{
	const auto& ctrl = polylineController();
	ctrl.linesActor()->GetProperty()->SetColor(0, 0, 0);
	ctrl.pointsActor()->GetProperty()->SetColor(0, 0, 0);
}
