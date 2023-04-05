#ifndef GEODATANETCDF_IMPL_H
#define GEODATANETCDF_IMPL_H

#include "../geodatanetcdf.h"
#include "geodatanetcdf_displaysetting.h"

class GeoDataNetcdf::Impl
{
public:
	Impl();
	~Impl();

	std::vector<double> m_lonValues;
	std::vector<double> m_latValues;

	std::vector<double> m_xValues;
	std::vector<double> m_yValues;

	CoordinateSystemType m_coordinateSystemType;
	QString m_coordinateSystemName;

	bool m_geoTransformExists;
	double m_geoTransform[6];

	bool m_baseAndResolutionExists;
	double m_base;
	double m_resolution;

	DisplaySetting m_displaySetting;
};

#endif // GEODATANETCDF_IMPL_H
