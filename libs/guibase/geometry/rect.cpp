#include "rect.h"

Rect::Rect() :
	Rect(0, 0, 0, 0)
{}

Rect::Rect(double xmin, double xmax, double ymin, double ymax) :
	m_xMin {xmin},
	m_xMax {xmax},
	m_yMin {ymin},
	m_yMax {ymax}
{}

bool Rect::operator<(const Rect& rect)
{
	return
			rect.m_xMin < m_xMin &&
			rect.m_xMax > m_xMax &&
			rect.m_yMin < m_yMin &&
			rect.m_yMax > m_yMax;
}

bool Rect::operator<=(const Rect& rect)
{
	return
			rect.m_xMin <= m_xMin &&
			rect.m_xMax >= m_xMax &&
			rect.m_yMin <= m_yMin &&
			rect.m_yMax >= m_yMax;
}
