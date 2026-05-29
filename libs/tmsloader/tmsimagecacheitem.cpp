#include "tmsimagecacheitem.h"

#include <cs/coordinatesystem.h>
#include <cs/webmercatorutil.h>
#include <misc/stringtool.h>

#include <gdal_priv.h>
#include <gdal_utils.h>
#include <gdalwarper.h>

namespace {

const char* EPSG3857STR = "PROJCS[\"WGS 84 / Pseudo-Mercator\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Mercator_1SP\"],PARAMETER[\"central_meridian\",0],PARAMETER[\"scale_factor\",1],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH],EXTENSION[\"PROJ4\",\"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs\"],AUTHORITY[\"EPSG\",\"3857\"]]";
const char* EPSG4326STR = "GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]";

const double R = 6378137.0; // radius of the Earth
const double originShift = 20037508.342789244; // PI * R

} // namespace

TmsImageCacheItem::TmsImageCacheItem(int z, int x, int y, const QPixmap& image, bool lonLat) :
	m_z {z},
	m_x {x},
	m_y {y},
	m_original {image}
{
	if (lonLat) {
		setupWgs84();
	}
}

TmsImageCacheItem::TmsImageCacheItem(int z, int x, int y, const QPixmap& original, const QPixmap& wgs84, QRectF wgs84Rect) :
	m_z {z},
	m_x {x},
	m_y {y},
	m_original {original},
	m_wgs84 {wgs84},
	m_wgs84Rect {wgs84Rect}
{}

TmsImageCacheItem::~TmsImageCacheItem()
{}

QPixmap TmsImageCacheItem::original() const
{
	return m_original;
}

QPixmap TmsImageCacheItem::wgs84()
{
	if (! m_wgs84.isNull()) {
		return m_wgs84;
	}

	setupWgs84();

	return m_wgs84;
}

QRectF TmsImageCacheItem::wgs84Rect() const
{
	return m_wgs84Rect;
}

void TmsImageCacheItem::setupWgs84()
{
	const auto& img = m_original.toImage().convertToFormat(QImage::Format_RGB888);

	auto driver = GetGDALDriverManager()->GetDriverByName("MEM");
	auto ds_in= driver->Create("", img.width(), img.height(), 3, GDT_Byte, nullptr);

	int bitCount = img.width() * img.height();
	std::vector<unsigned char> rBits, gBits, bBits;
	rBits.assign(bitCount, 0);
	gBits.assign(bitCount, 0);
	bBits.assign(bitCount, 0);

	for (int j = 0; j < img.height(); ++j) {
		const uchar* head = img.scanLine(j);
		for (int i = 0; i < img.width(); ++i) {
			int idx = i + j * img.width();
			rBits[idx] = *(head + i * 3);
			gBits[idx] = *(head + i * 3 + 1);
			bBits[idx] = *(head + i * 3 + 2);
		}
	}

	GDALRasterBand* band = nullptr;
	band = ds_in->GetRasterBand(1);
	band->RasterIO(GF_Write, 0, 0, img.width(), img.height(), (void*)(rBits.data()),
		img.width(), img.height(), GDT_Byte, 1, img.width());
	band = ds_in->GetRasterBand(2);
	band->RasterIO(GF_Write, 0, 0, img.width(), img.height(), (void*)(gBits.data()),
		img.width(), img.height(), GDT_Byte, 1, img.width());
	band = ds_in->GetRasterBand(3);
	band->RasterIO(GF_Write, 0, 0, img.width(), img.height(), (void*)(bBits.data()),
		img.width(), img.height(), GDT_Byte, 1, img.width());

	double n = 1;
	for (auto zz = 0; zz < m_z; ++zz) {
		n *= 2;
	}

	double minX = m_x / n * 2 * originShift - originShift;
	double maxX = (m_x + 1) / n * 2 * originShift - originShift;
	double maxY = originShift - m_y / n * 2 * originShift;

	double delta = (maxX - minX) / img.width();
	double transform[6] = { minX, delta, 0, maxY, 0, -delta };
	ds_in->SetGeoTransform(transform);
	ds_in->SetProjection(EPSG3857STR);

	auto ds_out = reinterpret_cast<GDALDataset*> (GDALAutoCreateWarpedVRT(reinterpret_cast<GDALDatasetH>(ds_in), EPSG3857STR, EPSG4326STR, GRA_NearestNeighbour, 1.0, NULL));

	int w = ds_out->GetRasterXSize();
	int h = ds_out->GetRasterYSize();
	int size = w * h;

	std::vector<unsigned char> r, g, b;
	r.assign(size, 0);
	g.assign(size, 0);
	b.assign(size, 0);

	band = ds_out->GetRasterBand(1);
	band->RasterIO(GF_Read, 0, 0, w, h, (void*)(r.data()), w, h, GDT_Byte, 1, w);
	band = ds_out->GetRasterBand(2);
	band->RasterIO(GF_Read, 0, 0, w, h, (void*)(g.data()), w, h, GDT_Byte, 1, w);
	band = ds_out->GetRasterBand(3);
	band->RasterIO(GF_Read, 0, 0, w, h, (void*)(b.data()), w, h, GDT_Byte, 1, w);

	QImage image(w, h, QImage::Format_RGB888);

	for (int j = 0; j < h; ++j) {
		uchar* head = image.scanLine(j);
		for (int i = 0; i < w; ++i) {
			int idx = i + j * w;
			*(head + i * 3 + 0) = r[idx];
			*(head + i * 3 + 1) = g[idx];
			*(head + i * 3 + 2) = b[idx];
		}
	}
	double t[6];
	ds_out->GetGeoTransform(t);
	m_wgs84Rect = QRectF(t[0] - 0.5 * t[1], -(t[3] - 0.5 * t[5]), t[1] * (w + 1), -t[5] * (h + 1)); // latitude is negative

	GDALClose(ds_out);
	GDALClose(ds_in);

	m_wgs84 = QPixmap::fromImage(image);
}
