#include "coordinatesystem.h"
#include "webmercatorutil.h"
#include "private/webmercatorutil_impl.h"

#include <misc/iricrootpath.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QImage>
#include <QProcess>
#include <QString>

#include <gdal_priv.h>
#include <gdalwarper.h>
#include <cpl_vsi.h>
#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#include <proj_api.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>
#include <memory>

namespace {

const char* EPSG3857STR = "PROJCS[\"WGS 84 / Pseudo-Mercator\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Mercator_1SP\"],PARAMETER[\"central_meridian\",0],PARAMETER[\"scale_factor\",1],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH],EXTENSION[\"PROJ4\",\"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs\"],AUTHORITY[\"EPSG\",\"3857\"]]";
const char* EPSG4326STR = "GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]";

CoordinateSystem webMercatorCS("WebMercator", "WebMercator",
															 "+proj=latlong +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0", "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");


/// Convert Web Mercator georeferenced dataset to Long-Lat in-memory using GDAL VRT warping
GDALDataset* convertToLongLatInMemory(GDALDataset* srcDataset, double pixelSize)
{
	GDALDatasetH dstDataset = GDALAutoCreateWarpedVRT(reinterpret_cast<GDALDatasetH> (srcDataset), EPSG3857STR, EPSG4326STR, GRA_NearestNeighbour, 1.0, NULL);
	return reinterpret_cast<GDALDataset*> (dstDataset);
}

/// Create in-memory dataset from QImage
GDALDataset* createInMemoryDataset(const QRectF& rect, const QImage& img)
{
	QImage rgbImg = img.convertToFormat(QImage::Format_RGB888);

	GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("MEM");
	if (!driver) { return nullptr; }

	GDALDataset* ds = driver->Create("", rgbImg.width(), rgbImg.height(), 3, GDT_Byte, nullptr);
	if (ds == nullptr) { return nullptr; }

	int bitCount = rgbImg.width() * rgbImg.height();
	std::vector<unsigned char> rBits, gBits, bBits;
	rBits.assign(bitCount, 0);
	gBits.assign(bitCount, 0);
	bBits.assign(bitCount, 0);

	for (int i = 0; i < rgbImg.height(); ++i) {
		const uchar* head = rgbImg.scanLine(i);
		for (int j = 0; j < rgbImg.width(); ++j) {
			int idx = j + i * rgbImg.width();
			rBits[idx] = *(head + j * 3);
			gBits[idx] = *(head + j * 3 + 1);
			bBits[idx] = *(head + j * 3 + 2);
		}
	}

	GDALRasterBand* band = nullptr;
	band = ds->GetRasterBand(1);
	band->RasterIO(GF_Write, 0, 0, rgbImg.width(), rgbImg.height(), (void*)(rBits.data()),
		rgbImg.width(), rgbImg.height(), GDT_Byte, 1, rgbImg.width());
	band = ds->GetRasterBand(2);
	band->RasterIO(GF_Write, 0, 0, rgbImg.width(), rgbImg.height(), (void*)(gBits.data()),
		rgbImg.width(), rgbImg.height(), GDT_Byte, 1, rgbImg.width());
	band = ds->GetRasterBand(3);
	band->RasterIO(GF_Write, 0, 0, rgbImg.width(), rgbImg.height(), (void*)(bBits.data()),
		rgbImg.width(), rgbImg.height(), GDT_Byte, 1, rgbImg.width());

	double xmin, ymax, xmax;
	webMercatorCS.init();
	webMercatorCS.mapGeoToGrid(rect.left(), rect.bottom(), &xmin, &ymax);
	webMercatorCS.mapGeoToGrid(rect.right(), rect.bottom(), &xmax, &ymax);
	double delta = (xmax - xmin) / img.width();

	double transform[6] = { xmin, delta, 0, ymax, 0, -delta };
	ds->SetGeoTransform(transform);
	ds->SetProjection(EPSG3857STR);

	return ds;
}

/// Read data from warped dataset back to QImage
bool readWarpedDatasetToImage(GDALDataset* warpedDataset, QImage* img)
{
	int width = warpedDataset->GetRasterXSize();
	int height = warpedDataset->GetRasterYSize();

	std::vector<unsigned char> rBits(width * height);
	std::vector<unsigned char> gBits(width * height);
	std::vector<unsigned char> bBits(width * height);

	GDALRasterBand* band = nullptr;
	band = warpedDataset->GetRasterBand(1);
	band->RasterIO(GF_Read, 0, 0, width, height, (void*)(rBits.data()),
		width, height, GDT_Byte, 1, width);
	band = warpedDataset->GetRasterBand(2);
	band->RasterIO(GF_Read, 0, 0, width, height, (void*)(gBits.data()),
		width, height, GDT_Byte, 1, width);
	band = warpedDataset->GetRasterBand(3);
	band->RasterIO(GF_Read, 0, 0, width, height, (void*)(bBits.data()),
		width, height, GDT_Byte, 1, width);

	*img = QImage(width, height, QImage::Format_RGB888);
	for (int i = 0; i < height; ++i) {
		uchar* scanLine = img->scanLine(i);
		for (int j = 0; j < width; ++j) {
			int idx = j + i * width;
			*(scanLine + j * 3) = rBits[idx];
			*(scanLine + j * 3 + 1) = gBits[idx];
			*(scanLine + j * 3 + 2) = bBits[idx];
		}
	}

	return true;
}

} // namespace

void WebMercatorUtil::Impl::init(int zoomlevel)
{
	zoomLevel = zoomlevel;

	double c = 256;
	for (int i = 0; i < zoomlevel; ++i) {
		c *= 2;
	}
	Bc = c / 360.0;
	Cc = c / (2 * M_PI);
	double e = c / 2;
	zcx = e;
	zcy = e;
}

void WebMercatorUtil::Impl::project_pixel(double lon, double lat, double* x, double* y)
{
	*x = zcx + lon * Bc;
	double f = std::sin(DEG_TO_RAD * lat);
	*y = zcy + 0.5 * std::log((1 + f) / (1 - f)) * (-Cc);
}

void WebMercatorUtil::Impl::unproject_pixel(double x, double y, double* lon, double* lat)
{
	*lon = (x - zcx) / Bc;
	double g = (y - zcy) / (- Cc);
	*lat = RAD_TO_DEG * (2 * std::atan(std::exp(g)) - 0.5 * M_PI);
}

WebMercatorUtil::WebMercatorUtil(int zoomLevel) :
	impl {new Impl ()}
{
	impl->init(zoomLevel);
}

WebMercatorUtil::~WebMercatorUtil()
{
	delete impl;
}

void WebMercatorUtil::getCoordinates(int tilex, int tiley, int pixelx, int pixely, double* lon, double* lat)
{
	double X = tilex * 256 + pixelx + 0.5;
	double Y = tiley * 256 + pixely + 0.5;

	impl->unproject_pixel(X, Y, lon, lat);
}

void WebMercatorUtil::getTileRegion(double topLeftLon, double topLeftLat, double bottomRightLon, double bottomRightLat, int* xMin, int* xMax, int* yMin, int *yMax)
{
	int maxNum = 1;
	for (int i = 0; i < impl->zoomLevel; ++i) {
		maxNum *= 2;
	}
	maxNum -= 1;

	double xmin, ymin, xmax, ymax;

	impl->project_pixel(topLeftLon, topLeftLat, &xmin, &ymin);
	impl->project_pixel(bottomRightLon, bottomRightLat, &xmax, &ymax);

	xmin /= 256;
	ymin /= 256;
	xmax /= 256;
	ymax /= 256;

	*xMin = std::max(static_cast<int>(std::floor(xmin)), 0);
	*yMin = std::max(static_cast<int>(std::floor(ymin)), 0);
	*xMax = std::min(static_cast<int>(std::ceil(xmax)), maxNum);
	*yMax = std::min(static_cast<int>(std::ceil(ymax)), maxNum);
}


void WebMercatorUtil::calcImageZoomAndSize(double lonMin, double latMin, double lonMax, double latMax, double imgWidth,
																					 double* lonCenter, double* latCenter, int* zoomLevel, int* width, int* height)
{
	double xmin, ymin, xmax, ymax, xcenter, ycenter;

	Impl impl;
	impl.init(0);

	impl.project_pixel(lonMin, latMin, &xmin, &ymin);
	impl.project_pixel(lonMax, latMax, &xmax, &ymax);

	xcenter = (xmin + xmax) * 0.5;
	ycenter = (ymin + ymax) * 0.5;
	impl.unproject_pixel(xcenter, ycenter, lonCenter, latCenter);

	double xwidth = qAbs(xmax - xmin);
	double ywidth = qAbs(ymax - ymin);
	*zoomLevel = 0;
	while (xwidth < imgWidth) {
		*zoomLevel += 1;
		xwidth *= 2;
		ywidth *= 2;
	}
	// decrement zoomlevel
	if (*zoomLevel > 1) {
		*zoomLevel -= 1;
		xwidth /= 2;
		ywidth /= 2;
	}
	*width = static_cast<int> (xwidth);
	*height = static_cast<int> (ywidth);
}

QImage WebMercatorUtil::convertWebMercatorToLongLat(const QRectF& rect, const QImage& image, const QString& workDir)
{
	GDALAllRegister();

	// Create in-memory georeferenced dataset from QImage
	GDALDataset* srcDataset = createInMemoryDataset(rect, image);
	if (!srcDataset) {
		return image; // Return original on error
	}

	// Perform coordinate transformation in-memory using GDAL VRT warping
	double pixelSize = rect.width() / image.width();
	GDALDataset* warpedDataset = convertToLongLatInMemory(srcDataset, pixelSize);
	if (!warpedDataset) {
		GDALClose(static_cast<GDALDatasetH>(srcDataset));
		return image; // Return original on error
	}

	// Read warped data back to QImage
	QImage result;
	readWarpedDatasetToImage(warpedDataset, &result);

	// Scale result to match original image width if needed
	int w = image.width();
	int h = static_cast<double>(result.height()) / result.width() * w;
	result = result.scaled(w, h);

	// Clean up
	GDALClose(static_cast<GDALDatasetH>(warpedDataset));
	GDALClose(static_cast<GDALDatasetH>(srcDataset));

	return result;
}

void WebMercatorUtil::project(double lon, double lat, double* x, double* y)
{
	Impl impl;
	impl.init(0);

	impl.project_pixel(lon, lat, x, y);
}

void WebMercatorUtil::unproject(double x, double y, double* lon, double* lat)
{
	Impl impl;
	impl.init(0);

	impl.unproject_pixel(x, y, lon, lat);
}

double WebMercatorUtil::pixelSize(double zoomLevel)
{
	// pixelSize = 1 for zoomlevel 0.
	return 1 / std::exp(std::log(2.0) * zoomLevel);
}
