#ifndef GDALUTIL_H
#define GDALUTIL_H

#include "cs_api.h"

class CS_API GdalUtil
{
public:
	static int wkt2Epsg(const char* wkt);

private:
	GdalUtil();
};

#endif // GDALUTIL_H
