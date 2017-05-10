#ifndef RECT_DETAIL_H
#define RECT_DETAIL_H

#include "rect.h"

inline Rect::Rect() :
	m_isValid {false}
{}

inline Rect::Rect(double x, double y) :
	m_isValid {true}
{
	m_xMin = x;
	m_xMax = x;
	m_yMin = y;
	m_yMax = y;
}

inline Rect::Rect(double x1, double y1, double x2, double y2) :
	m_isValid {true}
{
	m_xMin = x1 < x2 ? x1 : x2;
	m_xMax = x1 > x2 ? x1 : x2;
	m_yMin = y1 < y2 ? y1 : y2;
	m_yMax = y1 > y2 ? y1 : y2;
}

inline double Rect::xMin() const
{
	return m_xMin;
}

inline double Rect::xMax() const
{
	return m_xMax;
}

inline double Rect::yMin() const
{
	return m_yMin;
}

inline double Rect::yMax() const
{
	return m_yMax;
}

inline bool Rect::isValid() const
{
	return m_isValid;
}

inline bool Rect::intersects(const Rect& r) const
{
	if (! m_isValid) {return false;}
	if (! r.isValid()) {return false;}
	return xMin() <= r.xMax() && xMax() >= r.xMin() && yMin() <= r.yMax() && yMax() >= r.yMin();
}

#endif // RECT_DETAIL_H
