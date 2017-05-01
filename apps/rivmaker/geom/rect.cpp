#include "rect.h"

#include <algorithm>
#include <cmath>

Rect intersectRect(const Rect& r1, const Rect& r2)
{
	double x1 = std::max(r1.xMin(), r2.xMin());
	double x2 = std::min(r1.xMax(), r2.xMax());
	double y1 = std::max(r1.yMin(), r2.yMin());
	double y2 = std::min(r1.yMax(), r2.yMax());

	return Rect(x1, y1, x2, y2);
}

Rect unionRect(const Rect& r1, const Rect& r2)
{
	double x1 = std::min(r1.xMin(), r2.xMin());
	double x2 = std::max(r1.xMax(), r2.xMax());
	double y1 = std::min(r1.yMin(), r2.yMin());
	double y2 = std::max(r1.yMax(), r2.yMax());

	return Rect(x1, y1, x2, y2);
}
