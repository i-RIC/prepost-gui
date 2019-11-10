#ifndef POLYLINEUTIL_H
#define POLYLINEUTIL_H

#include "../guibase_global.h"

#include <vector>

class vtkPoints;

class QPointF;

class GUIBASEDLL_EXPORT PolyLineUtil
{
public:
	static std::vector<QPointF> buildSplinePoints(vtkPoints* points, int divNum);
	static double length(std::vector<QPointF>& polyLine);

private:
	PolyLineUtil();
};

#endif // POLYLINEUTIL_H
