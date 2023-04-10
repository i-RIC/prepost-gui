#include "geodatanetcdfgrayscalepngrealimporter.h"
#include "geodatanetcdfreal.h"
#include "private/geodatanetcdf_impl.h"

#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemselectdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/filesystemfunction.h>

#include <yaml-cpp/yaml.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <netcdf.h>
#include <png.h>
#include <stdio.h>

#include <vector>

GeoDataNetcdfGrayscalePngRealImporter::GeoDataNetcdfGrayscalePngRealImporter(GeoDataCreator* creator) :
	GeoDataImporter {"grayscalepng", tr("Grayscale 16bit PNG (for Unreal Engine 4)"), creator}
{}

const QStringList GeoDataNetcdfGrayscalePngRealImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("Grayscale 16bit PNG files(*.png)"));
	return ret;
}

const QStringList GeoDataNetcdfGrayscalePngRealImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("png");
	return ret;
}

bool GeoDataNetcdfGrayscalePngRealImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto netcdf = dynamic_cast<GeoDataNetcdfReal*> (data);

	if (! importPgw(netcdf, filename(), w)) {return false;}
	if (! importMeta(netcdf, filename(), w)) {return false;}
	if (! importPng(netcdf, filename(), w)) {return false;}

	return true;
}

bool GeoDataNetcdfGrayscalePngRealImporter::doInit(const QString& /*filename*/, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	if (condition->dimensions().size() > 0) {
		QMessageBox::warning(w, tr("Warning"), tr("Grayscale 16bit PNG files can be imported for grid conditions without dimensions."));
		return false;
	}
	return setCs(item, w);
}

bool GeoDataNetcdfGrayscalePngRealImporter::importPng(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w)
{
	std::string fname = iRIC::toStr(filename);

	FILE* fp = fopen(fname.c_str(), "rb");
	if (!fp) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	char header[8];
	fread(header, 1, 8, fp);
	if (png_sig_cmp(reinterpret_cast<png_const_bytep>(&(header[0])), 0, 8) != 0) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1. It seems not to be a PNG file.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_info* png_info = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, png_info);

	int width = png_get_image_width(png_ptr, png_info);
	int height = png_get_image_height(png_ptr, png_info);
	png_byte color_type = png_get_color_type(png_ptr, png_info);
	png_byte bit_depth = png_get_bit_depth(png_ptr, png_info);

	if (color_type != PNG_COLOR_TYPE_GRAY || bit_depth != 16) {
		QMessageBox::critical(w, tr("Error"), tr("%1 is not a 16bit grayscale image.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	size_t rowbytes = png_get_rowbytes(png_ptr, png_info);
	std::vector<png_byte*> row_pointers(height);
	std::vector<png_byte> buffer(height * rowbytes);

	for (int j = 0; j < height; ++j) {
		row_pointers[j] = buffer.data() + j * rowbytes;
	}
	png_read_image(png_ptr, row_pointers.data());

	netcdf->impl->m_coordinateSystemType = GeoDataNetcdf::XY;
	netcdf->impl->m_coordinateSystemName = m_coordinateSystem->name();

	netcdf->impl->m_xValues.clear();
	double* transform = netcdf->geoTransform();
	for (int i = 0; i < width; ++i) {
		netcdf->impl->m_xValues.push_back(*transform + *(transform + 1) * (i + 0.5));
	}
	netcdf->impl->m_yValues.clear();
	for (int i = 0; i < height; ++i) {
		netcdf->impl->m_yValues.push_back(*(transform + 3) + *(transform + 5) * (height - i - 0.5));
	}

	netcdf->impl->m_lonValues.clear();
	for (int j = 0; j < netcdf->impl->m_yValues.size(); ++j) {
		double y = netcdf->impl->m_yValues.at(j);
		for (int i = 0; i < netcdf->impl->m_xValues.size(); ++i) {
			double x = netcdf->impl->m_xValues.at(i);
			double lon, lat;
			if (m_coordinateSystem->isLongLat()) {
				lon = x;
				lat = y;
			} else {
				m_coordinateSystem->mapGridToGeo(x, y, &lon, &lat);
			}
			netcdf->impl->m_lonValues.push_back(lon);
			netcdf->impl->m_latValues.push_back(lat);
		}
	}

	QFileInfo finfo(netcdf->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(netcdf->filename());
	f.remove();

	int ncid_out, ret;

	ret = nc_create(iRIC::toStr(netcdf->filename()).c_str(), NC_NETCDF4, &ncid_out);

	// save coordinates and dimensions to the netcdf file.
	int out_xDimId, out_yDimId, out_lonDimId, out_latDimId;
	int out_xVarId, out_yVarId, out_lonVarId, out_latVarId;
	std::vector<int> dimIds;

	int varOutId;

	ret = nc_redef(ncid_out);
	netcdf->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_lonDimId, &out_latDimId, &out_xVarId, &out_yVarId, &out_lonVarId, &out_latVarId);
	netcdf->defineValue(ncid_out, out_xDimId, out_yDimId, dimIds, &varOutId);

	ret = nc_enddef(ncid_out);
	netcdf->outputCoords(ncid_out, out_xVarId, out_yVarId, out_lonVarId, out_latVarId);

	std::vector<double> valuesBuffer(width * height);

	double base = netcdf->base();
	double resolution = netcdf->resolution();

	for (int j = 0; j < netcdf->ySize(); ++j) {
		for (int i = 0; i < netcdf->xSize(); ++i) {
			int srcIndex = i + netcdf->xSize() * (netcdf->ySize() - 1 - j);
			int trgIndex = i + netcdf->xSize() * j;

			int intVal = 256 * *(buffer.data() + 2 * srcIndex) + *(buffer.data() + 2 * srcIndex + 1);
			double v = intVal * resolution + base;

			valuesBuffer[trgIndex] = v;
		}
	}

	ret = nc_put_var_double(ncid_out, varOutId, valuesBuffer.data());
	if (ret != NC_NOERR) {return false;}

	nc_close(ncid_out);

	png_destroy_read_struct(&png_ptr, &png_info, nullptr);

	netcdf->updateShapeData();
	netcdf->handleDimensionCurrentIndexChange(0, 0);

	return true;
}

bool GeoDataNetcdfGrayscalePngRealImporter::importPgw(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w)
{
	QString pgwFilename = filename;
	pgwFilename.replace(QRegExp("png$"), "pgw");
	QFile f(pgwFilename);
	if (! f.exists()) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not exists.").arg(QDir::toNativeSeparators(pgwFilename)));
		return false;
	}

	if (! f.open(QFile::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while opening %1").arg(QDir::toNativeSeparators(pgwFilename)));
		return false;
	}
	QTextStream stream(&f);
	double t[6];
	for (int i = 0; i < 6; ++i) {
		QString line = stream.readLine();
		if (line.isNull()) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading %1. It is not a valid world file.").arg(QDir::toNativeSeparators(pgwFilename)));
			return false;
		}
		bool ok;
		t[i] = line.toDouble(&ok);
		if (! ok) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured while reading %1. It is not a valid world file.").arg(QDir::toNativeSeparators(pgwFilename)));
			return false;
		}
	}
	f.close();
	double geotransform[6];
	geotransform[0] = t[4] - t[0] * 0.5;
	geotransform[1] = t[0];
	geotransform[2] = 0;
	geotransform[3] = t[5] - t[3] * 0.5;
	geotransform[4] = 0;
	geotransform[5] = t[3];

	netcdf->setGeoTransform(&(geotransform[0]));

	return true;
}

bool GeoDataNetcdfGrayscalePngRealImporter::importMeta(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w)
{
	QString metaFilename = filename + ".meta";
	QFile mf(metaFilename);
	if (! mf.exists()) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not exists.").arg(QDir::toNativeSeparators(metaFilename)));
		return false;
	}
	try {
		YAML::Node config = YAML::LoadFile(iRIC::toStr(metaFilename));
		double base = config["base"].as<double>();
		double resolution = config["resolution"].as<double>();
		netcdf->setBaseAndResolution(base, resolution);
		return true;
	} catch (YAML::Exception&) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while parsing %1.").arg(QDir::toNativeSeparators(metaFilename)));
		return false;
	}
}


bool GeoDataNetcdfGrayscalePngRealImporter::setCs(PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	auto csb = item->iricMainWindow()->coordinateSystemBuilder();

	CoordinateSystemSelectDialog csDialog(w);
	csDialog.setBuilder(csb);
	csDialog.setCoordinateSystem(item->projectData()->mainfile()->coordinateSystem());
	csDialog.setForceSelect(true);

	int ret = csDialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_coordinateSystem = csDialog.coordinateSystem();
	return true;
}
