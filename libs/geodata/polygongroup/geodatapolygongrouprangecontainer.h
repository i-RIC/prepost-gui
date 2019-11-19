#ifndef GEODATAPOLYGONGROUPRANGECONTAINER_H
#define GEODATAPOLYGONGROUPRANGECONTAINER_H

#include <vtkIdList.h>
#include <vector>

class GeoDataPolygonGroupPolygon;

class GeoDataPolygonGroupRangeContainer
{
public:
	GeoDataPolygonGroupRangeContainer();

	void add(GeoDataPolygonGroupPolygon* polygon);

	const std::vector<GeoDataPolygonGroupPolygon*>& polygons() const;
	const std::vector<vtkIdType>& maxIndices() const;

private:
	std::vector<GeoDataPolygonGroupPolygon*> m_polygons;
	std::vector<vtkIdType> m_maxIndices;
};

#endif // GEODATAPOLYGONGROUPRANGECONTAINER_H
