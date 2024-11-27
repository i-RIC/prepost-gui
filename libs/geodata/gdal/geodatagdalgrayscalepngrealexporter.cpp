#include "geodatagdalreal.h"
#include "geodatagdalgrayscalepngrealexporter.h"

#include <misc/stringtool.h>

#include <netcdf.h>
#include <png.h>
#include <stdio.h>

#include <cmath>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QTextStream>

GeoDataGdalGrayscalePngRealExporter::GeoDataGdalGrayscalePngRealExporter(GeoDataCreator* creator) :
	GeoDataExporter("Grayscale PNG", creator)
{}

bool GeoDataGdalGrayscalePngRealExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* /*pd*/)
{
	auto gdal = dynamic_cast<GeoDataGdalReal*> (data);

	if (! doExportPng(gdal, filename, w)) {return false;}
	if (! doExportPgw(gdal, filename, w)) {return false;}
	if (! doExportMeta(gdal, filename, w)) {return false;}

	return true;
}

const QStringList GeoDataGdalGrayscalePngRealExporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("Grayscale 16bit PNG files(*.png)"));
	return ret;
}

bool GeoDataGdalGrayscalePngRealExporter::doExportPng(GeoDataGdalReal* gdal, const QString& filename, QWidget* w)
{
	if (! gdal->geoTransformExists() || ! gdal->baseAndResolutionExists()) {
		QMessageBox::critical(w, tr("Error"), tr("This data cannot be exported. It seems that it was not imported from *.png."));
		return false;
	}

	if (gdal->dimensions()->containers().size() > 0) {
		QMessageBox::critical(w, tr("Error"), tr("This data cannot be exported. Data with \"Time\" dimension cannot be exported to *.png"));
		return false;
	}

	int ncid, varid, ret;
	ret = nc_open(iRIC::toStr(gdal->filename()).c_str(), NC_NOWRITE, &ncid);
	size_t starts[2], ends[2];
	starts[0] = 0;
	starts[1] = 0;
	ends[0] = gdal->ySize();
	ends[1] = gdal->xSize();
	gdal->getValueVarId(ncid, &varid);
	size_t bufferSize = gdal->xSize() * gdal->ySize();
	std::vector<double> buffer(bufferSize);
	ret = nc_get_vara_double(ncid, varid, starts, ends, buffer.data());
	nc_close(ncid);

	auto fname = iRIC::toStr(filename);

	FILE *fp = fopen(fname.c_str(), "wb");
	if (!fp) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (! png_ptr) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while initializing libpng."));
		return false;
	}
	png_info* png_info = png_create_info_struct(png_ptr);
	if (! png_info) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while initializing libpng."));
		return false;
	}
	png_init_io(png_ptr, fp);

	// write header
	png_set_IHDR(png_ptr, png_info, gdal->xSize(), gdal->ySize(), 16, PNG_COLOR_TYPE_GRAY,
							 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, png_info);
	png_set_swap(png_ptr);

	// write bytes
	double base = gdal->base();
	double resolution = gdal->resolution();

	std::vector<png_int_16> row_buffer(gdal->xSize());
	for (int j = 0; j < gdal->ySize(); ++j) {
		for (int i = 0; i < gdal->xSize(); ++i) {
			row_buffer[i] = static_cast<png_int_16> ((buffer.at((gdal->ySize() - 1 - j) * gdal->xSize() + i) - base) / resolution);
		}
		png_write_row(png_ptr, reinterpret_cast<png_const_bytep>(row_buffer.data()));
	}

	png_write_end(png_ptr, nullptr);

	png_destroy_write_struct(&png_ptr, &png_info);

	fclose(fp);
	return true;
}

bool GeoDataGdalGrayscalePngRealExporter::doExportPgw(GeoDataGdalReal* gdal, const QString& filename, QWidget* w)
{
	QString pgwFilename = filename;
	pgwFilename.replace(QRegExp("png$"), "pgw");
	QFile f(pgwFilename);
	if (! f.open(QFile::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(pgwFilename)));
		return false;
	}
	QTextStream stream(&f);
	double* transform = gdal->geoTransform();
	double t[6];
	for (int i = 0; i < 6; ++i) {
		t[i] = *(transform + i);
	}
	stream << t[1] << endl;
	stream << 0.0 << endl;
	stream << 0.0 << endl;
	stream << t[5] << endl;
	stream << t[0] + t[1] * 0.5 << endl;
	stream << t[3] + t[5] * 0.5 << endl;
	f.close();
	return true;
}

bool GeoDataGdalGrayscalePngRealExporter::doExportMeta(GeoDataGdalReal* gdal, const QString& filename, QWidget* w)
{
	QString metaFilename = filename + ".meta";
	QFile f(metaFilename);
	if (! f.open(QFile::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(metaFilename)));
		return false;
	}
	QTextStream stream(&f);
	stream << "base: " << gdal->base() << endl;
	stream << "resolution: " << gdal->resolution() << endl;
	f.close();
	return true;
}
