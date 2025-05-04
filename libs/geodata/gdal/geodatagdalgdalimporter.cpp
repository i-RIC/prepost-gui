#include "geodatagdal.h"
#include "geodatagdalfilelistdialog.h"
#include "geodatagdalfilenamematcher.h"
#include "geodatagdalfilenamepatterndialog.h"
#include "geodatagdalgdalimporter.h"
#include "private/geodatagdal_impl.h"
#include "private/geodatagdalgdalimporter_importersetting.h"

#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemselectdialog.h>
#include <cs/gdalutil.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataitemi.h>
#include <guicore/pre/base/preprocessorgriddataitemi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodataimportersetting.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/grid/v4inputgrid.h>
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

	dialog->setRange(0, static_cast<int> (filenames.size()) - 1);
	dialog->setProgress(0);
	dialog->setMessage(GeoDataGdalGdalImporter::tr("Importing data..."));
	dialog->showProgressBar();
	dialog->show();
	qApp->processEvents();
}

} // namespace

GeoDataGdalGdalImporter::GeoDataGdalGdalImporter(GeoDataCreator* creator) :
	GeoDataImporter {"gdal", tr("GDAL"), creator},
	m_timeZone {QTimeZone::utc()},
	m_matcher	{nullptr}
{}

GeoDataGdalGdalImporter::~GeoDataGdalGdalImporter()
{
	delete m_matcher;
}

const QStringList GeoDataGdalGdalImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("GeoTiff files(*.tif *.tiff)"));
	ret.append(tr("ArcInfo ASCII files(*.asc)"));
	return ret;
}

const QStringList GeoDataGdalGdalImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("tiff");
	ret.append("tif");
	ret.append("asc");
	return ret;
}

GeoDataImporterSetting* GeoDataGdalGdalImporter::createSetting() const
{
	return new ImporterSetting();
}

void GeoDataGdalGdalImporter::cancel()
{
	m_canceled = true;
}

bool GeoDataGdalGdalImporter::doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	clear();
	GDALAllRegister();

	bool ok = setMode(condition, w);
	if (! ok) {return false;}

	if (m_mode == Mode::Single) {
		ok = doInitForSingleMode(count, condition, item, w);
	} else if (m_mode == Mode::Time) {
		ok = doInitForTimeMode(count, condition, item, w);
	}

	if (! ok) {return false;}

	auto s = dynamic_cast<ImporterSetting*> (setting());
	s->timeMode = (m_mode == Mode::Time);
	s->csName = m_coordinateSystem->name();
	if (m_matcher != nullptr) {
		s->fileNamePattern = m_matcher->pattern();
	} else {
		s->fileNamePattern = "";
	}
	s->timeZone = m_timeZone.id();
	QStringList tmpFileNames;
	for (const auto& name : m_filenames) {
		tmpFileNames.append(name);
	}
	s->fileNames = tmpFileNames.join("\n");

	return true;
}

bool GeoDataGdalGdalImporter::doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	clear();
	GDALAllRegister();

	auto s = dynamic_cast<ImporterSetting*> (setting());
	if (s->timeMode) {
		m_mode = Mode::Time;
	} else {
		m_mode = Mode::Single;
	}

	*count = 1;

	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();
	m_coordinateSystem = csBuilder->system(s->csName);
	if (m_coordinateSystem == nullptr) {
		m_coordinateSystem = item->iricMainWindow()->coordinateSystemBuilder()->buildFromProj4String(s->csName);
	}

	delete m_matcher;
	m_matcher = new GeoDataGdalFileNameMatcher(s->fileNamePattern);
	m_timeZone = QTimeZone(s->timeZone.value().toUtf8());
	auto names = s->fileNames.value();
	m_filenames.clear();
	for (const auto& name : names.split("\n")) {
		m_filenames.push_back(name);
	}

	if (m_mode == Mode::Single) {
		doInitWithSettingForSingleMode(count, condition, item, w);
	} else {
		doInitWithSettingForTimeMode(count, condition, item, w);
	}
	return true;
}

bool GeoDataGdalGdalImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto gdal = dynamic_cast<GeoDataGdal*> (data);
	gdal->setGeoTransform(m_tgtTransform);

	if (m_mode == Mode::Single) {
		return importDataForSingleMode(gdal, w);
	} else {
		return importDataForTimeMode(gdal, w);
	}
}

bool GeoDataGdalGdalImporter::setMode(SolverDefinitionGridAttribute* condition, QWidget* w)
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

bool GeoDataGdalGdalImporter::doInitForSingleMode(int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto filename = setting()->fileName();
	if (! iRIC::isAscii(filename)) {
		QMessageBox::critical(w, tr("Error"), tr("The file name contains non-ASCII characters. Please move or rename the file."));
		return false;
	}

	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {
		QMessageBox::critical(w, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	*count = 1;

	bool ok = setCoordinateSystem(filename, dataset, item, w);
	if (! ok) {return false;}

	ok = setTransform(dataset);
	if (! ok) {return false;}

	int srcISize = dataset->GetRasterXSize();
	int srcJSize = dataset->GetRasterYSize();

	GDALClose(dataset);

	m_filenames.push_back(filename);

	GeoDataGdal::buildWarpMatrix(srcISize, srcJSize, m_srcTransform, m_coordinateSystem, item->projectData()->mainfile()->coordinateSystem(),
															 &m_tgtISize, &m_tgtJSize, m_tgtTransform, &m_matrix);

	return true;
}

bool GeoDataGdalGdalImporter::doInitForTimeMode(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto filename = setting()->fileName();
	m_timeZone = item->projectData()->mainfile()->timeZone();

	if (item->geoDatas().size() > 1) {
		QMessageBox::critical(w, tr("Error"), tr("Time series raster data is already imported. If you want to import other data, please delete the data already imported first."));
		return false;
	}

	// grid attributes cleared
	auto conds = item->geoDataTopDataItem()->gridTypeDataItem()->conditions();
	for (auto cond : conds) {
		auto grid = cond->gridDataItem()->grid();
		if (grid == nullptr) {continue;}

		auto att = grid->attribute(condition->name());
		att->clearTemporaryData();
		att->setDefaultValue();
	}

	if (! iRIC::isAscii(filename)) {
		QMessageBox::critical(w, tr("Error"), tr("The file name contains non-ASCII characters. Please move or rename the file."));
		return false;
	}

	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {
		QMessageBox::critical(w, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	*count = 1;

	bool ok = setCoordinateSystem(filename, dataset, item, w);
	if (! ok) {return false;}

	ok = setTransform(dataset);
	if (! ok) {return false;}

	int srcISize = dataset->GetRasterXSize();
	int srcJSize = dataset->GetRasterYSize();

	GDALClose(dataset);

	GeoDataGdal::buildWarpMatrix(srcISize, srcJSize, m_srcTransform, m_coordinateSystem, item->projectData()->mainfile()->coordinateSystem(),
															 &m_tgtISize, &m_tgtJSize, m_tgtTransform, &m_matrix);

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
		dt.setTimeZone(m_timeZone);
		timeVals.push_back(dt.toMSecsSinceEpoch() / 1000.0);
	}
	auto timeContainer = dynamic_cast<GridAttributeDimensionRealContainer*> (item->dimensions()->containers().at(0));
	timeContainer->setValues(timeVals);

	return true;
}

bool GeoDataGdalGdalImporter::doInitWithSettingForSingleMode(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto filename = setting()->fileName();
	if (! iRIC::isAscii(filename)) {
		QMessageBox::critical(w, tr("Error"), tr("The file name contains non-ASCII characters. Please move or rename the file."));
		return false;
	}

	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {
		QMessageBox::critical(w, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	*count = 1;

	bool ok = setTransform(dataset);
	if (! ok) {return false;}

	int srcISize = dataset->GetRasterXSize();
	int srcJSize = dataset->GetRasterYSize();

	GDALClose(dataset);

	m_filenames.push_back(filename);

	GeoDataGdal::buildWarpMatrix(srcISize, srcJSize, m_srcTransform, m_coordinateSystem, item->projectData()->mainfile()->coordinateSystem(),
															 &m_tgtISize, &m_tgtJSize, m_tgtTransform, &m_matrix);

	return true;
}

bool GeoDataGdalGdalImporter::doInitWithSettingForTimeMode(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	auto filename = setting()->fileName();

	// grid attributes cleared
	auto conds = item->geoDataTopDataItem()->gridTypeDataItem()->conditions();
	for (auto cond : conds) {
		auto grid = cond->gridDataItem()->grid();
		if (grid == nullptr) {continue;}

		auto att = grid->attribute(condition->name());
		att->clearTemporaryData();
		att->setDefaultValue();
	}

	if (! iRIC::isAscii(filename)) {
		QMessageBox::critical(w, tr("Error"), tr("The file name contains non-ASCII characters. Please move or rename the file."));
		return false;
	}

	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {
		QMessageBox::critical(w, tr("Error"), tr("Opening %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	*count = 1;

	bool ok = setTransform(dataset);
	if (! ok) {return false;}

	int srcISize = dataset->GetRasterXSize();
	int srcJSize = dataset->GetRasterYSize();

	GDALClose(dataset);

	GeoDataGdal::buildWarpMatrix(srcISize, srcJSize, m_srcTransform, m_coordinateSystem, item->projectData()->mainfile()->coordinateSystem(),
															 &m_tgtISize, &m_tgtJSize, m_tgtTransform, &m_matrix);

	std::vector<double> timeVals;
	for (const auto& fname : m_filenames) {
		QFileInfo finfo(fname);
		bool ok;
		QDateTime dt = m_matcher->getDateTime(finfo.fileName(), &ok);
		dt.setTimeZone(m_timeZone);
		timeVals.push_back(dt.toMSecsSinceEpoch() / 1000.0);
	}
	auto timeContainer = dynamic_cast<GridAttributeDimensionRealContainer*> (item->dimensions()->containers().at(0));
	timeContainer->setValues(timeVals);

	return true;
}

bool GeoDataGdalGdalImporter::importDataForSingleMode(GeoDataGdal* gdal, QWidget* /*w*/)
{
	setupCoordinates(gdal);

	auto filename = *(m_filenames.begin());
	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {return false;}

	GDALRasterBand* band = dataset->GetRasterBand(1);

	QFileInfo finfo(gdal->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(gdal->filename());
	f.remove();

	int ncid_out, ret;

	ret = nc_create(iRIC::toStr(gdal->filename()).c_str(), NC_NETCDF4, &ncid_out);

	// save coordinates and dimensions to the gdal file.
	int out_xDimId, out_yDimId;
	int out_xVarId, out_yVarId;
	std::vector<int> dimIds;

	int varOutId;

	ret = nc_redef(ncid_out);
	gdal->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_xVarId, &out_yVarId);
	gdal->defineValue(ncid_out, out_xDimId, out_yDimId, dimIds, &varOutId);

	ret = nc_enddef(ncid_out);
	gdal->outputCoords(ncid_out, out_xVarId, out_yVarId);

	outputValues(ncid_out, varOutId, dataset->GetRasterXSize(), dataset->GetRasterYSize(), band, gdal);

	nc_close(ncid_out);

	GDALClose(dataset);

	gdal->updateShapeData();
	gdal->handleDimensionCurrentIndexChange(0, 0);

	if (gdal->isReadOnly()) {
		// delete the needless file
		f.remove();
	}

	return true;
}

bool GeoDataGdalGdalImporter::importDataForTimeMode(GeoDataGdal* gdal, QWidget* w)
{
	// Get Time dimension container here

	int ncid_out;
	int ret;

	QFileInfo finfo(gdal->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(gdal->filename());
	f.remove();

	ret = nc_create(iRIC::toStr(gdal->filename()).c_str(), NC_NETCDF4, &ncid_out);
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
			setupCoordinates(gdal);

			// save coordinates and dimensions to the gdal file.
			int out_xDimId, out_yDimId;
			int out_xVarId, out_yVarId;

			std::vector<int> dimIds, varIds;

			ret = nc_redef(ncid_out);
			gdal->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_xVarId, &out_yVarId);
			gdal->defineDimensions(ncid_out, &dimIds, &varIds);
			gdal->defineValue(ncid_out, out_xDimId, out_yDimId, dimIds, &varOutId);

			ret = nc_enddef(ncid_out);
			gdal->outputCoords(ncid_out, out_xVarId, out_yVarId);
			gdal->outputDimensions(ncid_out, varIds);
		}
		outputValuesWithTime(ncid_out, varOutId, timeId, dataset->GetRasterXSize(), dataset->GetRasterYSize(), band, gdal);

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

	gdal->updateShapeData();
	gdal->handleDimensionCurrentIndexChange(0, 0);

	return true;
}

bool GeoDataGdalGdalImporter::setCoordinateSystem(const QString& filename, GDALDataset* dataset, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
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

bool GeoDataGdalGdalImporter::setTransform(GDALDataset* dataset)
{
	OGRErr err = dataset->GetGeoTransform(m_srcTransform);

	return (err == CE_None);
}

void GeoDataGdalGdalImporter::setupCoordinates(GeoDataGdal* data)
{
	data->impl->m_xValues.clear();
	for (int i = 0; i < m_tgtISize; ++i) {
		data->impl->m_xValues.push_back(m_tgtTransform[0] + m_tgtTransform[1] * (i + 0.5));
	}
	data->impl->m_yValues.clear();
	for (int i = 0; i < m_tgtJSize; ++i) {
		data->impl->m_yValues.push_back(m_tgtTransform[3] + m_tgtTransform[5] * (m_tgtJSize - i - 0.5));
	}
}

bool GeoDataGdalGdalImporter::setupFileNamePattern(const QString& filename, QWidget* w)
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
		auto matcher = new GeoDataGdalFileNameMatcher(p);
		bool ok = matcher->setup(filename);
		if (! ok) {continue;}

		m_matcher = matcher;
		break;
	}

	GeoDataGdalFileNamePatternDialog dialog(w);
	dialog.setTimeZone(m_timeZone);
	dialog.setFilename(filename);
	if (m_matcher != nullptr) {
		dialog.setPattern(m_matcher->pattern());
	}

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_matcher = dialog.matcher();
	m_timeZone = dialog.timeZone();

	return (m_matcher != nullptr);
}

bool GeoDataGdalGdalImporter::setupFilenames(const QString& filename, QWidget *w)
{
	QFileInfo finfo(filename);
	QDir dir = finfo.absoluteDir();
	auto list = dir.entryList(QDir::Files, QDir::Name);

	std::vector<QString> fnames;

	for (const QString& fname : list) {
		bool ok;
		auto dt = m_matcher->getDateTime(fname, &ok);
		if (! ok) {continue;}

		QFileInfo finfo2(fname);
		if (finfo.suffix() != finfo2.suffix()) {continue;}

		fnames.push_back(dir.absoluteFilePath(fname));
	}
	GeoDataGdalFileListDialog dialog(w);
	dialog.setFileNameMatcher(m_matcher);
	dialog.setFileNames(fnames);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_filenames = dialog.selectedFilenames();

	return true;
}

void GeoDataGdalGdalImporter::clear()
{
	m_coordinateSystem = nullptr;
	m_filenames.clear();
	m_canceled = false;
}
