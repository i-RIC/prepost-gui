#ifndef POINTRING_H
#define POINTRING_H

#include "../guibase_global.h"

#include <vtkType.h>
#include <unordered_set>
#include <vector>

class Rect;
class vtkCell;
class vtkPoints;

class GUIBASEDLL_EXPORT PointRing
{
public:
	PointRing(const std::vector<vtkIdType>& points, vtkPoints* vtkPs);
	PointRing(vtkCell* cell, vtkPoints* vtkPs);

	const std::vector<vtkIdType>& points() const;
	vtkPoints* getVtkPoints() const;
	Rect boundingRect() const;
	bool merge(PointRing* otherRing, PointRing** another, int rotateCount = 0);
	void reOrder(vtkIdType first, vtkIdType second);
	std::vector<PointRing*> clean();

private:
	void clean(std::unordered_set<vtkIdType> finished);
	void exportHoles(std::vector<PointRing*>* rings, std::unordered_set<vtkIdType> finished);
	bool contains(vtkIdType point);
	void updateSortedPoints();

	std::vector<vtkIdType> m_points;
	std::vector<vtkIdType> m_sortedPoints;
	vtkPoints* m_vtkPoints;
};

#endif // POINTRING_H
