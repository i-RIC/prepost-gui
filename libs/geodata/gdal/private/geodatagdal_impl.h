#ifndef GEODATAGDAL_IMPL_H
#define GEODATAGDAL_IMPL_H

#include "../geodatagdal.h"
#include "geodatagdal_displaysetting.h"

class GeoDataGdal::Impl
{
public:
	Impl();
	~Impl();

	std::vector<double> m_xValues;
	std::vector<double> m_yValues;

	bool m_geoTransformExists;
	double m_geoTransform[6];

	bool m_baseAndResolutionExists;
	double m_base;
	double m_resolution;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAGDAL_IMPL_H
