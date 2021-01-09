#include "geodatanetcdf.h"
#include "geodatanetcdffilelistdialog.h"
#include "geodatanetcdffilenamematcher.h"
#include "geodatanetcdffilenamepatterndialog.h"
#include "geodatanetcdfgdalimporter.h"

#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemselectdialog.h>
#include <cs/gdalutil.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionrealcontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QProcess>

#include <gdal_priv.h>
#include <ogr_spatialref.h>

#include <memory>
#include <string>

namespace {

void setupWaitDialog(WaitDialog* dialog, const std::vector<QString>& filenames)
{
	if (filenames.size() < 5) {return;}

	dialog->setRange(0, filenames.size() - 1);
	dialog->setProgress(0);
	dialog->setMessage(GeoDataNetcdfGdalImporter::tr("Importing data..."));
	dialog->showProgressBar();
	dialog->show();
	qApp->processEvents();
}

} // namespace

GeoDataNetcdfGdalImporter::GeoDataNetcdfGdalImporter(GeoDataCreator* creator) :
	GeoDataImporter {"gdal", tr("GDAL"), creator},
	m_matcher	{nullptr}
{}

GeoDataNetcdfGdalImporter::~GeoDataNetcdfGdalImporter()
{
	delete m_matcher;
}

const QStringList GeoDataNetcdfGdalImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("GeoTiff files(*.tif)"));
	ret.append(tr("ArcInfo ASCII files(*.asc)"));
	return ret;
}

const QStringList GeoDataNetcdfGdalImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("tif");
	ret.append("asc");
	return ret;
}

void GeoDataNetcdfGdalImporter::cancel()
{
	m_canceled = true;
}

bool GeoDataNetcdfGdalImporter::doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	clear();
	GDALAllRegister();

	bool ok = setMode(condition, w);
	if (! ok) {return false;}

	if (m_mode == Mode::Single) {
		return doInitForSingleMode(filename, selectedFilter, count, condition, item, w);
	}	 else if (m_mode == Mode::Time) {
		return doInitForTimeMode(filename, selectedFilter, count, condition, item, w);
	}
}

bool GeoDataNetcdfGdalImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	if (m_mode == Mode::Single) {
		return importDataForSingleMode(data, w);
	} else {
		return importDataForTimeMode(data, w);
	}
}

bool GeoDataNetcdfGdalImporter::setMode(SolverDefinitionGridAttribute* condition, QWidget* w)
{
	// investigate the condition.
	const auto& dims = condition->dimensions();
	if (dims.size() == 0) {
		m_mode = Mode::Single;
		return true;
	}
	if (dims.size() > 1) {
		QMessageBox::warning(w, tr("Warning"), tr("GDAL data files can be imported for grid conditions without dimensions, and Time dimension only."));
		return false;
	}
	SolverDefinitionGridAttributeDimension* dim = condition->dimensions().at(0);
	if (dim->name() != "Time") {
		QMessageBox::warning(w, tr("Warning"), tr("GDAL data files can be imported for grid conditions without dimensions, and Time dimension only."));
		return false;
	}
	m_mode = Mode::Time;
	return true;
}

bool GeoDataNetcdfGdalImporter::doInitForSingleMode(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {
		// failed opening.
		return false;
	}

	*count = 1;

	bool ok = setCoordinateSystem(filename, dataset, item, w);
	if (! ok) {return false;}

	ok = setTransform(dataset);
	if (! ok) {return false;}

	GDALClose(dataset);

	m_filenames.push_back(filename);
	return true;
}

bool GeoDataNetcdfGdalImporter::doInitForTimeMode(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {
		// failed opening.
		return false;
	}

	*count = 1;

	bool ok = setCoordinateSystem(filename, dataset, item, w);
	if (! ok) {return false;}

	ok = setTransform(dataset);
	if (! ok) {return false;}

	GDALClose(dataset);

	// find pattern to parse time value from filename
	QFileInfo finfo(filename);
	ok = setupFileNamePattern(finfo.fileName(), w);
	if (! ok) {return false;}

	ok = setupFilenames(filename, w);
	if (! ok) {return false;}

	std::vector<double> timeVals;
	for (const auto& fname : m_filenames) {
		QFileInfo finfo(fname);
		bool ok;
		QDateTime dt = m_matcher->getDateTime(finfo.fileName(), &ok);
		timeVals.push_back(dt.toMSecsSinceEpoch() / 1000.0);
	}
	auto timeContainer = dynamic_cast<GridAttributeDimensionRealContainer*> (item->dimensions()->containers().at(0));
	if (timeContainer->values().size() != 0) {
		// the time values should coincide
		if (timeContainer->values() != timeVals) {
			QMessageBox::critical(w, tr("Error"), tr("Dimension values for time mismatch."));
			return false;
		}
	} else {
		timeContainer->setValues(timeVals);
	}

	return true;
}

bool GeoDataNetcdfGdalImporter::importDataForSingleMode(GeoData* data, QWidget* /*w*/)
{
	auto filename = *(m_filenames.begin());
	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {return false;}

	GeoDataNetcdf* netcdf = dynamic_cast<GeoDataNetcdf*>(data);
	GDALRasterBand* band = dataset->GetRasterBand(1);
	setupCoordinates(netcdf, band);

	int ncid_out;
	int ret;

	Q_UNUSED(ret)

	QFileInfo finfo(netcdf->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(netcdf->filename());
	f.remove();

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

	outputValues(ncid_out, varOutId, band, netcdf);

	nc_close(ncid_out);

	GDALClose(dataset);

	netcdf->updateShapeData();
	netcdf->handleDimensionCurrentIndexChange(0, 0);

	return true;
}

bool GeoDataNetcdfGdalImporter::importDataForTimeMode(GeoData* data, QWidget* w)
{
	GeoDataNetcdf* netcdf = dynamic_cast<GeoDataNetcdf*>(data);
	// Get Time dimension container here

	int ncid_out;
	int ret;

	QFileInfo finfo(netcdf->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(netcdf->filename());
	f.remove();

	ret = nc_create(iRIC::toStr(netcdf->filename()).c_str(), NC_NETCDF4, &ncid_out);
	int varOutId;

	WaitDialog wDialog(w);
	setupWaitDialog(&wDialog, m_filenames);
	connect(&wDialog, SIGNAL(canceled()), this, SLOT(cancel()));

	for (int timeId = 0; timeId < m_filenames.size(); ++timeId) {
		auto filename = m_filenames.at(timeId);

		auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
		if (dataset == NULL) {return false;}
		GDALRasterBand* band = dataset->GetRasterBand(1);
		if (timeId == 0) {
			setupCoordinates(netcdf, band);

			// save coordinates and dimensions to the netcdf file.
			int out_xDimId, out_yDimId, out_lonDimId, out_latDimId;
			int out_xVarId, out_yVarId, out_lonVarId, out_latVarId;

			std::vector<int> dimIds, varIds;

			ret = nc_redef(ncid_out);
			netcdf->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_lonDimId, &out_latDimId, &out_xVarId, &out_yVarId, &out_lonVarId, &out_latVarId);
			netcdf->defineDimensions(ncid_out, &dimIds, &varIds);
			netcdf->defineValue(ncid_out, out_xDimId, out_yDimId, dimIds, &varOutId);

			ret = nc_enddef(ncid_out);
			netcdf->outputCoords(ncid_out, out_xVarId, out_yVarId, out_lonVarId, out_latVarId);
			netcdf->outputDimensions(ncid_out, varIds);
		}
		outputValuesWithTime(ncid_out, varOutId, timeId, band, netcdf);

		GDALClose(dataset);

		wDialog.setProgress(timeId + 1);
		qApp->processEvents();

		if (m_canceled) {
			nc_close(ncid_out);
			f.remove();
			return false;
		}
	}
	nc_close(ncid_out);

	netcdf->updateShapeData();
	netcdf->handleDimensionCurrentIndexChange(0, 0);

	return true;
}

bool GeoDataNetcdfGdalImporter::setCoordinateSystem(const QString& filename, GDALDataset* dataset, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	OGRSpatialReference* sr = new OGRSpatialReference();

	std::string geoRef = dataset->GetProjectionRef();
	char* geoRefPointer = const_cast<char*>(geoRef.c_str());
	OGRErr err = sr->importFromWkt(&geoRefPointer);
	if (err == OGRERR_NONE) {
		char* projDef;

		sr->exportToProj4(&projDef);
		if (*projDef != '\0') {
			m_coordinateSystem = item->iricMainWindow()->coordinateSystemBuilder()->buildFromProj4String(projDef);
			m_coordinateSystem->setName(projDef);
			if (m_coordinateSystem == nullptr) {return false;}
		} else {
			int epsg = GdalUtil::wkt2Epsg(geoRef.c_str());
			auto name = QString("EPSG:%1").arg(epsg);
			m_coordinateSystem = item->iricMainWindow()->coordinateSystemBuilder()->system(name);
			m_coordinateSystem->setName(name);
			if (m_coordinateSystem == nullptr) {return false;}
		}
	} else {
		// Failed to load coordinate system information.
		// This happens for *.asc or GeoTIFF without coordinate system information.
		// User should select coordinate system.

		auto csb = item->iricMainWindow()->coordinateSystemBuilder();
		QFileInfo finfo(filename);
		QMessageBox::warning(w, tr("Warning"), tr("Coordinate system is not specified for %1. Please select coordinate system manually.").arg(finfo.fileName()));

		CoordinateSystemSelectDialog csDialog(w);
		csDialog.setBuilder(csb);
		csDialog.setCoordinateSystem(item->projectData()->mainfile()->coordinateSystem());
		csDialog.setForceSelect(true);

		int ret = csDialog.exec();
		if (ret == QDialog::Rejected) {return false;}
		m_coordinateSystem = csDialog.coordinateSystem();
	}
	return true;
}

bool GeoDataNetcdfGdalImporter::setTransform(GDALDataset* dataset)
{
	OGRErr err = dataset->GetGeoTransform(m_transform);
	return (err == CE_None);
}

void GeoDataNetcdfGdalImporter::setupCoordinates(GeoDataNetcdf* data, GDALRasterBand* band)
{
	int xsize = band->GetXSize();
	int ysize = band->GetYSize();
	data->m_coordinateSystemType = GeoDataNetcdf::XY;
	data->m_coordinateSystemName = m_coordinateSystem->name();

	data->m_xValues.clear();
	for (int i = 0; i < xsize; ++i) {
		data->m_xValues.push_back(m_transform[0] + m_transform[1] * (i + 0.5));
	}
	data->m_yValues.clear();
	for (int i = 0; i < ysize; ++i) {
		data->m_yValues.push_back(m_transform[3] + m_transform[5] * (ysize - i - 0.5));
	}

	data->m_lonValues.clear();
	for (int j = 0; j < data->m_yValues.size(); ++j) {
		double y = data->m_yValues.at(j);
		for (int i = 0; i < data->m_xValues.size(); ++i) {
			double x = data->m_xValues.at(i);
			double lon, lat;
			if (m_coordinateSystem->isLongLat()) {
				lon = x;
				lat = y;
			} else {
				m_coordinateSystem->mapGridToGeo(x, y, &lon, &lat);
			}
			data->m_lonValues.push_back(lon);
			data->m_latValues.push_back(lat);
		}
	}
}

bool GeoDataNetcdfGdalImporter::setupFileNamePattern(const QString& filename, QWidget* w)
{
	delete m_matcher;
	m_matcher = nullptr;

	std::vector<QString> patterns;

	patterns.push_back("YYYY-MM-DD_hh:mm:ss");
	patterns.push_back("YYYY_MM_DD_hh:mm:ss");
	patterns.push_back("YYYYMMDD-hhmmss");
	patterns.push_back("YYYYMMDD_hhmmss");
	patterns.push_back("YYYYMMDD-hhmm");
	patterns.push_back("YYYYMMDD_hhmm");

	for (auto& p : patterns) {
		auto matcher = new GeoDataNetcdfFileNameMatcher(p);
		bool ok = matcher->setup(filename);
		if (! ok) {continue;}

		m_matcher = matcher;
		break;
	}
	if (m_matcher == nullptr) {
		GeoDataNetcdfFileNamePatternDialog dialog(w);
		dialog.setFilename(filename);

		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {return false;}

		m_matcher = dialog.matcher();
	}
	return (m_matcher != nullptr);
}

bool GeoDataNetcdfGdalImporter::setupFilenames(const QString& filename, QWidget *w)
{
	QFileInfo finfo(filename);
	QDir dir = finfo.absoluteDir();
	auto list = dir.entryList(QDir::Files, QDir::Name);

	m_filenames.clear();
	for (const QString& fname : list) {
		bool ok;
		auto dt = m_matcher->getDateTime(fname, &ok);
		if (! ok) {continue;}

		QFileInfo finfo2(fname);
		if (finfo.suffix() != finfo2.suffix()) {continue;}

		m_filenames.push_back(dir.absoluteFilePath(fname));
	}
	GeoDataNetcdfFileListDialog dialog(w);
	dialog.setFilenames(m_filenames);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	return true;
}

void GeoDataNetcdfGdalImporter::clear()
{
	m_coordinateSystem = nullptr;
	m_filenames.clear();
	m_canceled = false;
}
