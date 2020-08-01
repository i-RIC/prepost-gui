#ifndef GEODATAPOLYDATAGROUPPOLYDATAWITHBOUNDINGRECT_H
#define GEODATAPOLYDATAGROUPPOLYDATAWITHBOUNDINGRECT_H

#include "gd_polydatagroup_global.h"

#include "geodatapolydatagrouppolydata.h"

class GD_POLYDATAGROUP_EXPORT GeoDataPolyDataGroupPolyDataWithBoundingRect : public GeoDataPolyDataGroupPolyData
{
public:
	GeoDataPolyDataGroupPolyDataWithBoundingRect(GeoDataPolyDataGroup* group);

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) override;

protected:
	double m_xmin;
	double m_xmax;
	double m_ymin;
	double m_ymax;
};

#endif // GEODATAPOLYDATAGROUPPOLYDATAWITHBOUNDINGRECT_H
