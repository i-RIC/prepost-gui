#include "geodatapolygongrouppolygon_impl.h"
#include "geodatapolygongrouppolygon_triangulatorvtk.h"

#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>

#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>

std::vector<unsigned int> GeoDataPolygonGroupPolygon::TriangulatorVtk::triangulate(GeoDataPolygonGroupPolygon* polygon)
{
	auto pol = vtkSmartPointer<vtkPolygon>::New();
	auto points = pol->GetPoints();
	auto ids = pol->GetPointIds();

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
	auto triIds = vtkSmartPointer<vtkIdList>::New();
	pol->Triangulate(triIds);

	std::vector<unsigned int> ret;
	for (int i = 0; i < triIds->GetNumberOfIds(); ++i) {
		ret.push_back(triIds->GetId(i));
	}
	return ret;
}