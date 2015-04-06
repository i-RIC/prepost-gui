#ifndef RAWDATAPOINTMAPTEMPLATEMAPPINGSETTING_H
#define RAWDATAPOINTMAPTEMPLATEMAPPINGSETTING_H

#include "rd_pointmap_global.h"

class RD_POINTMAP_EXPORT RawDataPointmapTemplateMappingSetting
{
public:
	bool tempAutoMode;
	double tempStreamWiseLength;
	double tempCrossStreamLength;
	int tempNumExpansion;
	double tempWeightExponent;

	static RawDataPointmapTemplateMappingSetting setting;

private:
	RawDataPointmapTemplateMappingSetting();
};

#endif // RAWDATAPOINTMAPTEMPLATEMAPPINGSETTING_H
