#ifndef RECT_H
#define RECT_H

class Rect
{
public:
	Rect();
	Rect(double x, double y);
	Rect(double x1, double y1, double x2, double y2);

	double xMin() const;
	double xMax() const;
	double yMin() const;
	double yMax() const;

	bool isValid() const;
	bool intersects(const Rect& r) const;

private:
	double m_xMin;
	double m_xMax;
	double m_yMin;
	double m_yMax;
	bool m_isValid;
};

Rect intersectRect(const Rect& r1, const Rect& r2);
Rect unionRect(const Rect& r1, const Rect& r2);

#include "rect_detail.h"

#endif // RECT_H
