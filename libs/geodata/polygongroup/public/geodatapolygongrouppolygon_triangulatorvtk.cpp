#include "../private/geodatapolygongrouppolygon_impl.h"
#include "geodatapolygongrouppolygon_triangulatorvtk.h"

#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>

#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>

GeoDataPolygonGroupPolygon::TriangulatorVtk::TriangulatorVtk() :
	m_polygon {vtkPolygon::New()},
	m_idList {vtkIdList::New()}
{}

GeoDataPolygonGroupPolygon::TriangulatorVtk::~TriangulatorVtk()
{
	m_polygon->Delete();
	m_idList->Delete();
}

std::vector<unsigned int> GeoDataPolygonGroupPolygon::TriangulatorVtk::triangulate(GeoDataPolygonGroupPolygon* polygon)
{
	m_polygon->Initialize();
	m_idList->Initialize();

	auto points = m_polygon->GetPoints();
	auto ids = m_polygon->GetPointIds();

	geos::geom::Polygon* geosPol = polygon->impl->m_polygon.get();
	const geos::geom::LineString* ls = geosPol->getExteriorRing();
	points->Allocate(ls->getNumPoints() - 1);
	ids->Allocate(ls->getNumPoints() - 1);
	const auto& coord1 = ls->getCoordinateN(0);
	for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
		const auto& coord = ls->getCoordinateN(i);
		points->InsertNextPoint(coord.x - coord1.x, coord.y - coord1.y, 0);
		ids->InsertNextId(i);
	}
	m_polygon->Triangulate(m_idList);

	std::vector<unsigned int> ret;
	for (int i = 0; i < m_idList->GetNumberOfIds(); ++i) {
		ret.push_back(m_idList->GetId(i));
	}
	return ret;
}
