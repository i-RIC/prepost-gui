#include "geodatapolygonholepolygon.h"

#include <vtkProperty.h>

GeoDataPolygonHolePolygon::GeoDataPolygonHolePolygon(GeoDataPolygon* parent)
	: GeoDataPolygonAbstractPolygon(parent)
{
	m_paintActor->GetProperty()->SetColor(0, 0, 0);
	m_paintActor->GetProperty()->SetOpacity(0.1);
	m_edgeActor->GetProperty()->SetColor(0, 0, 1);
	m_vertexActor->GetProperty()->SetColor(0, 0, 1);
}
