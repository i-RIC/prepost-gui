#ifndef WEBMERCATORUTIL_IMPL_H
#define WEBMERCATORUTIL_IMPL_H

#include "../webmercatorutil.h"

class WebMercatorUtil::Impl
{
public:
	void init(int zoomlevel);
	void project_plxel(double lon, double lat, double* x, double* y);
	void unproject_pixel(double x, double y, double* lon, double* lat);

	double Bc;
	double Cc;
	double zcx;
	double zcy;
};

#endif // WEBMERCATORUTIL_IMPL_H
