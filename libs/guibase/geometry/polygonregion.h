#ifndef POLYGONREGION_H
#define POLYGONREGION_H

#include "../guibase_global.h"
#include <vtkType.h>
#include <vector>

class vtkCell;
class vtkPoints;
class PointRing;

class GUIBASEDLL_EXPORT PolygonRegion
{
public:
	PolygonRegion(PointRing* region);
	PolygonRegion(const std::vector<vtkIdType>& points, vtkPoints* vtkPs);
	PolygonRegion(vtkCell* cell, vtkPoints* vtkPs);
	~PolygonRegion();

	PointRing* region() const;
	const std::vector<PointRing*>& holes() const;
	bool merge(PolygonRegion* region);
	void clean();

private:
	PointRing* m_region;
	std::vector<PointRing*> m_holes;
};

#endif // POLYGONREGION_H
