#include "geodatapolydatagrouppolydatawithboundingrect.h"

GeoDataPolyDataGroupPolyDataWithBoundingRect::GeoDataPolyDataGroupPolyDataWithBoundingRect(GeoDataPolyDataGroup* group) :
	GeoDataPolyDataGroupPolyData(group),
	m_xmin {0},
	m_xmax {0},
	m_ymin {0},
	m_ymax {0}
{}

void GeoDataPolyDataGroupPolyDataWithBoundingRect::getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax)
{
	*xmin = m_xmin;
	*xmax = m_xmax;
	*ymin = m_ymin;
	*ymax = m_ymax;
}
