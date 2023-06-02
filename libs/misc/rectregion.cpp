#include "rectregion.h"

RectRegion::RectRegion(double xmin, double xmax, double ymin, double ymax):
	xMin {xmin},
	xMax {xmax},
	yMin {ymin},
	yMax {ymax}
{}

bool RectRegion::pointIsInside(double x, double y) const
{
	if (x < xMin) {return false;}
	if (x > xMax) {return false;}
	if (y < yMin) {return false;}
	if (y > yMax) {return false;}

	return true;
}

bool RectRegion::intersect(const QLineF& line) const
{
	bool isInside1 = pointIsInside(line.x1(), line.y1());
	bool isInside2 = pointIsInside(line.x2(), line.y2());
	return (isInside1 != isInside2);
}
