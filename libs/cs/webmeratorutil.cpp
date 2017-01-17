#include "webmeratorutil.h"
#include "coordinatesystem.h"

#include <cmath>

WebMeratorUtil::WebMeratorUtil(int zoomLevel)
{
	QString wgs84("+proj=longlat +datum=WGS84 +no_defs");

	double t_p2 = std::pow(2.0, zoomLevel - 1);
	double t_p1 = t_p2 / 3.141592;
	QString tile = QString("+proj=merc +a=%1 +b=%1 +lat_ts=0.0 +lon_0=0.0 +x_0=%2 +y_0=-%2 +k=1.0 +axis=esu +units=m +nadgrids=@null +no_defs").arg(t_p1).arg(t_p2);

	double p_p2 = std::pow(2.0, zoomLevel + 7);
	double p_p1 = p_p2 / 3.141592;
	QString pixel = QString("+proj=merc +a=%1 +b=%1 +lat_ts=0.0 +lon_0=0.0 +x_0=%2 +y_0=-%2 +k=1.0 +axis=esu +units=m +nadgrids=@null +no_defs").arg(p_p1).arg(p_p2);

	m_tileCS = new CoordinateSystem("", "", wgs84, tile);
	m_tileCS->init();

	m_pixelCS = new CoordinateSystem("", "", wgs84, pixel);
	m_pixelCS->init();
}

WebMeratorUtil::~WebMeratorUtil()
{
	delete m_pixelCS;
	delete m_tileCS;
}

void WebMeratorUtil::getCoordinates(int tilex, int tiley, int pixelx, int pixely, double* lon, double* lat)
{
	double X = tilex * 256 + pixelx + 0.5;
	double Y = tiley * 256 + pixely + 0.5;

	m_pixelCS->mapGridToGeo(X, Y, lon, lat);
}

void WebMeratorUtil::getTileRegion(double topLeftLon, double topLeftLat, double bottomRightLon, double bottomRightLat, int* xMin, int* xMax, int* yMin, int *yMax)
{
	// @todo region between -180 line not supported now.
	double xmin, ymin, xmax, ymax;

	m_tileCS->mapGeoToGrid(topLeftLon, topLeftLat, &xmin, &ymin);
	m_tileCS->mapGeoToGrid(bottomRightLon, bottomRightLat, &xmax, &ymax);

	*xMin = static_cast<int>(std::floor(xmin));
	*yMin = static_cast<int>(std::floor(ymin));
	*xMax = static_cast<int>(std::ceil(xmax));
	*yMax = static_cast<int>(std::ceil(ymax));
}
