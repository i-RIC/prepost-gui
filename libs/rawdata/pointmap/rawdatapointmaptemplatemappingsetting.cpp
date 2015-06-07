#include "rawdatapointmaptemplatemappingsetting.h"

RawDataPointmapTemplateMappingSetting RawDataPointmapTemplateMappingSetting::setting;

RawDataPointmapTemplateMappingSetting::RawDataPointmapTemplateMappingSetting()
{
	tempAutoMode = true;
	tempStreamWiseLength = 4;
	tempCrossStreamLength = 1;
	tempNumExpansion = 3;
	tempWeightExponent = 1.;
}
