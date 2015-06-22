#ifndef GEODATAPOINTMAPMAPPINGMODE_H
#define GEODATAPOINTMAPMAPPINGMODE_H

#include "gd_pointmap_global.h"

class GD_POINTMAP_EXPORT GeoDataPointmapMappingMode
{

public:
	enum Mode {
		mTIN = 0,      ///< Mapping is done with TIN.
		mTemplate = 1  ///< Mapping is done with template.
	};
	static Mode mode;

private:
	GeoDataPointmapMappingMode();
};

#endif // GEODATAPOINTMAPMAPPINGMODE_H
