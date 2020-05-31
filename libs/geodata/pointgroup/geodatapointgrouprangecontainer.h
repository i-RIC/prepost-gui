#ifndef GEODATAPOINTGROUPRANGECONTAINER_H
#define GEODATAPOINTGROUPRANGECONTAINER_H

#include <vtkIdList.h>
#include <vector>

class GeoDataPointGroupPoint;

class GeoDataPointGroupRangeContainer
{
public:
	GeoDataPointGroupRangeContainer();

	void add(GeoDataPointGroupPoint* point);

	const std::vector<GeoDataPointGroupPoint*>& points() const;
	const std::vector<vtkIdType>& maxIndices() const;

private:
	std::vector<GeoDataPointGroupPoint*> m_points;
	std::vector<vtkIdType> m_maxIndices;
};

#endif // GEODATAPOINTGROUPRANGECONTAINER_H
