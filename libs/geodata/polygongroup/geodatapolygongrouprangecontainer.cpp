#include "geodatapolygongrouprangecontainer.h"

GeoDataPolygonGroupRangeContainer::GeoDataPolygonGroupRangeContainer()
{}

void GeoDataPolygonGroupRangeContainer::add(GeoDataPolygonGroupPolygon* polygon)
{
	if (m_polygons.size() == 0) {
		m_polygons.push_back(polygon);
		m_maxIndices.push_back(0);
		return;
	}
	auto lastPolygon = m_polygons.back();
	if (lastPolygon == polygon) {
		++ (m_maxIndices.back());
	} else {
		m_polygons.push_back(polygon);
		m_maxIndices.push_back(m_maxIndices.back() + 1);
	}
}

const std::vector<GeoDataPolygonGroupPolygon*>& GeoDataPolygonGroupRangeContainer::polygons() const
{
	return m_polygons;
}

const std::vector<vtkIdType>& GeoDataPolygonGroupRangeContainer::maxIndices() const
{
	return m_maxIndices;
}
