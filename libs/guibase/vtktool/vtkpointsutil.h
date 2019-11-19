#ifndef VTKPOINTSUTIL_H
#define VTKPOINTSUTIL_H

#include "../guibase_global.h"
#include <vector>

class QPointF;

class vtkCell;
class vtkPoints;
class vtkPointSet;

class GUIBASEDLL_EXPORT vtkPointsUtil
{
public:
	static std::vector<QPointF> getCoordinates(vtkPoints* points);
	static std::vector<QPointF> getCoordinates(vtkPointSet* pointset);
	static std::vector<QPointF> getCoordinates(vtkCell* cell);
	static QPointF getCenter(vtkCell* cell);

	static void setCoordinates(vtkPoints* points, const std::vector<QPointF>& coords);
	static void setCoordinates(vtkPointSet* pointset, const std::vector<QPointF>& coords);
	static void setCoordinates(vtkCell* cell, const std::vector<QPointF>& coords);

private:
	vtkPointsUtil();
};

#endif // VTKPOINTSUTIL_H
