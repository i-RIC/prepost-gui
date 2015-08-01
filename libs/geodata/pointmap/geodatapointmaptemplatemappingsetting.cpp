#include "geodatapointmaptemplatemappingsetting.h"

GeoDataPointmapTemplateMappingSetting GeoDataPointmapTemplateMappingSetting::setting;

GeoDataPointmapTemplateMappingSetting::GeoDataPointmapTemplateMappingSetting()
{
	tempAutoMode = true;
	tempStreamWiseLength = 4;
	tempCrossStreamLength = 1;
	tempNumExpansion = 3;
	tempWeightExponent = 1.;
}
