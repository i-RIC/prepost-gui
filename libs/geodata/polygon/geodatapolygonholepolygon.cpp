#include "geodatapolygonholepolygon.h"

#include <guibase/polygon/polygoncontroller.h>

#include <vtkActor.h>
#include <vtkProperty.h>

GeoDataPolygonHolePolygon::GeoDataPolygonHolePolygon(GeoDataPolygon* parent) :
	GeoDataPolygonAbstractPolygon {parent}
{
	const auto& ctrl = polygonController();
	auto paintProp = ctrl.paintActor()->GetProperty();
	paintProp->SetColor(0, 0, 0);
	paintProp->SetOpacity(0.1);

	ctrl.linesActor()->GetProperty()->SetColor(0, 0, 1);
	ctrl.pointsActor()->GetProperty()->SetColor(0, 0, 1);
}
