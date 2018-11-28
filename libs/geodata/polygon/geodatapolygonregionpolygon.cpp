#include "geodatapolygonregionpolygon.h"

#include <guibase/polygon/polygoncontroller.h>

#include <vtkActor.h>
#include <vtkProperty.h>

GeoDataPolygonRegionPolygon::GeoDataPolygonRegionPolygon(GeoDataPolygon* parent) :
	GeoDataPolygonAbstractPolygon {parent}
{
	const auto& ctrl = polygonController();
	auto paintProp = ctrl.paintActor()->GetProperty();
	paintProp->SetColor(0, 0, 0);
	paintProp->SetOpacity(0.);

	ctrl.linesActor()->GetProperty()->SetColor(0, 0, 0);
	ctrl.pointsActor()->GetProperty()->SetColor(0, 0, 0);
}
