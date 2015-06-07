#ifndef RAWDATAPOINTMAPMAPPINGMODE_H
#define RAWDATAPOINTMAPMAPPINGMODE_H

#include "rd_pointmap_global.h"

class RD_POINTMAP_EXPORT RawDataPointmapMappingMode
{

public:
	enum Mode {
		mTIN = 0,      ///< Mapping is done with TIN.
		mTemplate = 1  ///< Mapping is done with template.
	};
	static Mode mode;

private:
	RawDataPointmapMappingMode();
};

#endif // RAWDATAPOINTMAPMAPPINGMODE_H
