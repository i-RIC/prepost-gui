#include "geodatapointgrouprangecontainer.h"

GeoDataPointGroupRangeContainer::GeoDataPointGroupRangeContainer()
{}

void GeoDataPointGroupRangeContainer::add(GeoDataPointGroupPoint* point)
{
	if (m_points.size() == 0) {
		m_points.push_back(point);
		m_maxIndices.push_back(0);
		return;
	}
	auto lastPoint = m_points.back();
	if (lastPoint == point) {
		++ (m_maxIndices.back());
	} else {
		m_points.push_back(point);
		m_maxIndices.push_back(m_maxIndices.back() + 1);
	}
}

const std::vector<GeoDataPointGroupPoint*>& GeoDataPointGroupRangeContainer::points() const
{
	return m_points;
}

const std::vector<vtkIdType>& GeoDataPointGroupRangeContainer::maxIndices() const
{
	return m_maxIndices;
}
