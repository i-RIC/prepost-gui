#include "geodatanetcdfreal.h"
#include "geodatanetcdfgrayscalepngrealexporter.h"

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

GeoDataNetcdfGrayscalePngRealExporter::GeoDataNetcdfGrayscalePngRealExporter(GeoDataCreator* creator) :
	GeoDataExporter("Grayscale PNG", creator)
{}

bool GeoDataNetcdfGrayscalePngRealExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* /*pd*/)
{
	auto netcdf = dynamic_cast<GeoDataNetcdfReal*> (data);

	if (! doExportPng(netcdf, filename, w)) {return false;}
	if (! doExportPgw(netcdf, filename, w)) {return false;}
	if (! doExportMeta(netcdf, filename, w)) {return false;}

	return true;
}

const QStringList GeoDataNetcdfGrayscalePngRealExporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("Grayscale 16bit PNG files(*.png)"));
	return ret;
}

bool GeoDataNetcdfGrayscalePngRealExporter::doExportPng(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w)
{
	if (! netcdf->geoTransformExists() || ! netcdf->baseAndResolutionExists()) {
		QMessageBox::critical(w, tr("Error"), tr("This data can not be exported. It seems that is it not imported from *.png."));
		return false;
	}

	if (netcdf->dimensions()->containers().size() > 0) {
		QMessageBox::critical(w, tr("Error"), tr("This data can not be exported. Data with \"Time\" dimension can not be exported to *.png"));
		return false;
	}

	int ncid, varid, ret;
	ret = nc_open(iRIC::toStr(netcdf->filename()).c_str(), NC_NOWRITE, &ncid);
	size_t starts[2], ends[2];
	starts[0] = 0;
	starts[1] = 0;
	ends[0] = netcdf->ySize();
	ends[1] = netcdf->xSize();
	netcdf->getValueVarId(ncid, &varid);
	size_t bufferSize = netcdf->xSize() * netcdf->ySize();
	std::vector<double> buffer(bufferSize);
	ret = nc_get_vara_double(ncid, varid, starts, ends, buffer.data());
	nc_close(ncid);

	auto fname = iRIC::toStr(filename);

	FILE *fp = fopen(fname.c_str(), "wb");
	if (!fp) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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
	png_set_IHDR(png_ptr, png_info, netcdf->xSize(), netcdf->ySize(), 16, PNG_COLOR_TYPE_GRAY,
							 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, png_info);
	png_set_swap(png_ptr);

	// write bytes
	double base = netcdf->base();
	double resolution = netcdf->resolution();

	std::vector<png_int_16> row_buffer(netcdf->xSize());
	for (int j = 0; j < netcdf->ySize(); ++j) {
		for (int i = 0; i < netcdf->xSize(); ++i) {
			row_buffer[i] = static_cast<png_int_16> ((buffer.at((netcdf->ySize() - 1 - j) * netcdf->xSize() + i) - base) / resolution);
		}
		png_write_row(png_ptr, reinterpret_cast<png_const_bytep>(row_buffer.data()));
	}

	png_write_end(png_ptr, NULL);

	png_destroy_write_struct(&png_ptr, &png_info);

	fclose(fp);
	return true;
}

bool GeoDataNetcdfGrayscalePngRealExporter::doExportPgw(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w)
{
	QString pgwFilename = filename;
	pgwFilename.replace(QRegExp("png$"), "pgw");
	QFile f(pgwFilename);
	if (! f.open(QFile::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(pgwFilename)));
		return false;
	}
	QTextStream stream(&f);
	double* transform = netcdf->geoTransform();
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

bool GeoDataNetcdfGrayscalePngRealExporter::doExportMeta(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w)
{
	QString metaFilename = filename + ".meta";
	QFile f(metaFilename);
	if (! f.open(QFile::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(metaFilename)));
		return false;
	}
	QTextStream stream(&f);
	stream << "base: " << netcdf->base() << endl;
	stream << "resolution: " << netcdf->resolution() << endl;
	f.close();
	return true;
}
