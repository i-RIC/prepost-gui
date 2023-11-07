#ifndef RECTREGION_H
#define RECTREGION_H

#include "misc_global.h"

class QLineF;

class MISCDLL_EXPORT RectRegion
{
public:
	RectRegion(double xmin, double xmax, double ymin, double ymax);

	bool pointIsInside(double x, double y) const;
	bool intersect(const QLineF& line) const;

	double xMin;
	double xMax;
	double yMin;
	double yMax;
};

#endif // RECTREGION_H
