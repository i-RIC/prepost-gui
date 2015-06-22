#ifndef GEODATAPOINTMAPTEMPLATEMAPPINGSETTING_H
#define GEODATAPOINTMAPTEMPLATEMAPPINGSETTING_H

#include "gd_pointmap_global.h"

class GD_POINTMAP_EXPORT GeoDataPointmapTemplateMappingSetting
{

public:
	bool tempAutoMode;
	double tempStreamWiseLength;
	double tempCrossStreamLength;
	int tempNumExpansion;
	double tempWeightExponent;

	static GeoDataPointmapTemplateMappingSetting setting;

private:
	GeoDataPointmapTemplateMappingSetting();
};

#endif // GEODATAPOINTMAPTEMPLATEMAPPINGSETTING_H
