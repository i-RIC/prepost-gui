#include "zdepthrange.h"

ZDepthRange::ZDepthRange()
{
	m_min = 0;
	m_max = 1;
	m_itemCount = 0;
}

ZDepthRange::ZDepthRange(double min, double max)
{
	m_min = min;
	m_max = max;
	m_itemCount = 0;
}
