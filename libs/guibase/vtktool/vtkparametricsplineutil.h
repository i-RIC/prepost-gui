#ifndef VTKPARAMETRICSPLINEUTIL_H
#define VTKPARAMETRICSPLINEUTIL_H

#include "../guibase_global.h"

#include <QPointF>

#include <vector>

class vtkPoints;

class GUIBASEDLL_EXPORT vtkParametricSplineUtil
{
public:
	static std::vector<QPointF> buildSplinePointsWithSameDistances(vtkPoints* points, int divNum);
	static std::vector<QPointF> buildSplinePoints(vtkPoints* points, int subDivNum);

private:
	vtkParametricSplineUtil();
};

#endif // VTKPARAMETRICSPLINEUTIL_H
