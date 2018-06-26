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
#include <proj_api.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>

namespace {

const char* EPSG3857STR = "PROJCS[\"WGS 84 / Pseudo-Mercator\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Mercator_1SP\"],PARAMETER[\"central_meridian\",0],PARAMETER[\"scale_factor\",1],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH],EXTENSION[\"PROJ4\",\"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs\"],AUTHORITY[\"EPSG\",\"3857\"]]";
const char* EPSG4326STR = "GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]";

CoordinateSystem webMercatorCS("WebMercator", "WebMercator",
															 "+proj=latlong +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0", "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs");


bool saveGeoTIFF(const std::string& fname, const QRectF& rect, const QImage& img)
{
	QImage rgbImg = img.convertToFormat(QImage::Format_RGB888);

	GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
	char** options = NULL;
	GDALDataset *ds = driver->Create(fname.c_str(), rgbImg.width(), rgbImg.height(), 3, GDT_Byte, options);
	if (ds == NULL) { return false; }

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
	GDALClose(static_cast<GDALDatasetH> (ds));

	return true;
}

bool convertGeoTiffToLongLat(const std::string& from, const std::string& to, double res)
{
	QString exepath = iRICRootPath::get();
	QString exeName = QDir(exepath).absoluteFilePath("gdalwarp.exe");
	QString resstr = QString::number(res, 'g', 10);

	QStringList args;
	args << "-s_srs" << EPSG3857STR << "-t_srs" << EPSG4326STR;
	args << from.c_str() << to.c_str();

	int ret = QProcess::execute(exeName, args);
	return (ret == 0);
}

bool loadGeoTIFF(const std::string& fname, QImage* img)
{
	*img = QImage(fname.c_str());
	return true;
}

} // namespace

void WebMercatorUtil::Impl::init(int zoomlevel)
{
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

void WebMercatorUtil::Impl::project_plxel(double lon, double lat, double* x, double* y)
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
	double xmin, ymin, xmax, ymax;

	impl->project_plxel(topLeftLon, topLeftLat, &xmin, &ymin);
	impl->project_plxel(bottomRightLon, bottomRightLat, &xmax, &ymax);

	xmin /= 256;
	ymin /= 256;
	xmax /= 256;
	ymax /= 256;

	*xMin = static_cast<int>(std::floor(xmin));
	*yMin = static_cast<int>(std::floor(ymin));
	*xMax = static_cast<int>(std::ceil(xmax));
	*yMax = static_cast<int>(std::ceil(ymax));
}


void WebMercatorUtil::calcImageZoomAndSize(double lonMin, double latMin, double lonMax, double latMax, double imgWidth,
																					 double* lonCenter, double* latCenter, int* zoomLevel, int* width, int* height)
{
	double xmin, ymin, xmax, ymax, xcenter, ycenter;

	Impl impl;
	impl.init(0);

	impl.project_plxel(lonMin, latMin, &xmin, &ymin);
	impl.project_plxel(lonMax, latMax, &xmax, &ymax);

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
	*width = static_cast<int> (xwidth);
	*height = static_cast<int> (ywidth);
}

QImage WebMercatorUtil::convertWebMercatorToLongLat(const QRectF& rect, const QImage& image, const QString& workDir)
{
	static int idx = 0;
	GDALAllRegister();

	QDir dir(workDir);
	std::string tmpImg1 = iRIC::toStr(dir.absoluteFilePath(QString("img%1.tif").arg(++idx)));
	std::string tmpImg2 = iRIC::toStr(dir.absoluteFilePath(QString("img%1.tif").arg(++idx)));

	bool ok = saveGeoTIFF(tmpImg1, rect, image);
	ok = convertGeoTiffToLongLat(tmpImg1, tmpImg2, rect.width() / image.width());
	QImage result;
	ok = loadGeoTIFF(tmpImg2, &result);
	QFile::remove(tmpImg1.c_str());
	QFile::remove(tmpImg2.c_str());

	int w = image.width();
	int h = static_cast<double>(result.height()) / result.width() * w;
	result = result.scaled(w, h);

	return result;
}

void WebMercatorUtil::project(double lon, double lat, double* x, double* y)
{
	Impl impl;
	impl.init(0);

	impl.project_plxel(lon, lat, x, y);
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
