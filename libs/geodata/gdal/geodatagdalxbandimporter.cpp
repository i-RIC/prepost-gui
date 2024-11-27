#include "geodatagdalreal.h"
#include "geodatagdalxbandimporter.h"
#include "private/geodatagdal_impl.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <misc/filesystemfunction.h>
#include <misc/iricrootpath.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QProcess>
#include <QTemporaryDir>
#include <QTextCodec>

#include <vtkDoubleArray.h>

#include <string>

namespace {
	const QString TMP_NC = "tmp.nc";
} // namespace

GeoDataGdalXbandImporter::GeoDataGdalXbandImporter(GeoDataCreator* creator) :
	GeoDataImporter {"xband", tr("XBand MP RADER data"), creator}
{}

GeoDataGdalXbandImporter::~GeoDataGdalXbandImporter()
{}

const QStringList GeoDataGdalXbandImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("X band MP rader data (*.*)"));
	return ret;
}

const QStringList GeoDataGdalXbandImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("*");
	return ret;
}

bool GeoDataGdalXbandImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	m_groupDataItem = item;

	// investigate the condition.
	const auto& dims = condition->dimensions();
	if (! (dims.size() == 1 && dims.at(0)->name() == "Time")) {
		QMessageBox::warning(w, tr("Warning"), tr("X band MP rader data can be imported for grid conditions with dimension \"Time\"."));
		return false;
	}

	GridAttributeDimensionsContainer* dimVals = m_groupDataItem->dimensions();
	GridAttributeDimensionContainer* c = dimVals->containers().at(0);

	if (c->variantValues().size() != 0) {
		QMessageBox::critical(w, tr("Error"), tr("Time series raster data is already imported. If you want to import other data, please delete the data already imported first."));
		return false;
	}

	QFileInfo finfo(filename);
	QDir dir = finfo.absoluteDir();

	m_dirName = dir.path();
	QStringList filter;
	m_fileNames = dir.entryList(filter, QDir::Files, QDir::Name);

	return true;
}

bool GeoDataGdalXbandImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	GeoDataGdalReal* gdal = dynamic_cast<GeoDataGdalReal*>(data);

	int ncid_in, ncid_out;
	int ret;

	QString exepath = iRICRootPath::get();
	QString exeName = QDir(exepath).absoluteFilePath("mlitx2nc.exe");

	QDir workDir = gdal->projectData()->workDirectory();
	QString tmpFileName = workDir.absoluteFilePath(TMP_NC);

	QDir dir(m_dirName);

	int varOutId;
	std::vector<int> varIds;
	std::vector<QVariant> timeValues;

	timeValues.assign(m_fileNames.size(), 0);

	for (int i = 0; i < m_fileNames.size(); ++i) {
		auto inputFile = dir.absoluteFilePath(m_fileNames.at(i));
		QStringList args;
		args << tmpFileName << "1";

		QProcess mlitx2ncProcess;
		mlitx2ncProcess.start(exeName, args);

		QString argFileName = QDir::toNativeSeparators(inputFile);
		argFileName.append("\r\n");
		mlitx2ncProcess.write(iRIC::toStr(argFileName).c_str());
		auto argFileNameStr = iRIC::toStr(argFileName);
		mlitx2ncProcess.waitForFinished(-1);

		ret = nc_open(iRIC::toStr(tmpFileName).c_str(), NC_NOWRITE, &ncid_in);
		if (ret != NC_NOERR) {
			QMessageBox::critical(w, tr("Error"), tr("Error occured when importing data converted from %1.").arg(m_fileNames.at(i)));
			return false;
		}

		// load Lon, Lat, and Time
		int lonDimId, latDimId, timeDimId;
		int lonVarId, latVarId, timeVarId;
		int rrVarId;

		nc_inq_dimid(ncid_in, "LON", &lonDimId);
		nc_inq_dimid(ncid_in, "LAT", &latDimId);
		nc_inq_dimid(ncid_in, "TIME", &timeDimId);
		nc_inq_varid(ncid_in, "LON", &lonVarId);
		nc_inq_varid(ncid_in, "LAT", &latVarId);
		nc_inq_varid(ncid_in, "TIME", &timeVarId);
		nc_inq_varid(ncid_in, "RR", &rrVarId);

		size_t lonLen, latLen, timeLen;
		ret = nc_inq_dimlen(ncid_in, lonDimId, &lonLen);
		ret = nc_inq_dimlen(ncid_in, latDimId, &latLen);
		ret = nc_inq_dimlen(ncid_in, timeDimId, &timeLen);

		if (i == 0) {
			QFileInfo finfo(gdal->filename());
			iRIC::mkdirRecursively(finfo.absolutePath());

			// delete the file if it already exists.
			QFile f(gdal->filename());
			f.remove();

			ret = nc_create(iRIC::toStr(gdal->filename()).c_str(), NC_NETCDF4, &ncid_out);
			if (ret != NC_NOERR) {
				QMessageBox::critical(w, tr("Error"), tr("Error occured when opening %1.").arg(QDir::toNativeSeparators(gdal->filename())));
				return false;
			}

			gdal->impl->m_coordinateSystemType = GeoDataGdal::LonLat;

			std::vector<double> lons(lonLen);
			std::vector<double> lats(latLen);

			nc_get_var_double(ncid_in, lonVarId, lons.data());
			nc_get_var_double(ncid_in, latVarId, lats.data());

			// set lon and lat data
			gdal->impl->m_lonValues.clear();
			for (size_t i = 0; i < lonLen; ++i) {
				gdal->impl->m_lonValues.push_back(lons[i]);
			}
			gdal->impl->m_latValues.clear();
			for (size_t i = 0; i < latLen; ++i) {
				gdal->impl->m_latValues.push_back(lats[i]);
			}
			// set time dummy data
			GridAttributeDimensionsContainer* dims = m_groupDataItem->dimensions();
			GridAttributeDimensionContainer* c = dims->containers().at(0);
			std::vector<QVariant> timeVals;
			for (size_t i = 0; i < m_fileNames.size(); ++i) {
				timeVals.push_back(0);
			}
			c->setVariantValues(timeVals);

			// save coordinates and dimensions to the gdal file.
			int out_xDimId, out_yDimId, out_lonDimId, out_latDimId;
			int out_xVarId, out_yVarId, out_lonVarId, out_latVarId;
			std::vector<int> dimIds;

			ret = nc_redef(ncid_out);
			gdal->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_lonDimId, &out_latDimId, &out_xVarId, &out_yVarId, &out_lonVarId, &out_latVarId);
			gdal->defineDimensions(ncid_out, &dimIds, &varIds);
			ret = gdal->defineValue(ncid_out, out_lonDimId, out_latDimId, dimIds, &varOutId);

			ret = nc_enddef(ncid_out);
			gdal->outputCoords(ncid_out, out_xVarId, out_yVarId, out_lonVarId, out_latVarId);
		}

		// read time value
		double timeVal;
		nc_get_var_double(ncid_in, timeVarId, &timeVal);
		timeValues[i] = timeVal;

		size_t start_in[3];
		size_t start_out[3];
		size_t len_in[3];
		size_t len_out[3];
		size_t bufferSize = 0;

		// setup len_in, len_out
		len_in[0] = 1;
		len_in[1] = latLen;
		len_in[2] = lonLen;
		len_out[0] = 1;
		len_out[1] = latLen;
		len_out[2] = lonLen;
		bufferSize = lonLen * latLen;

		// setup start_in, start_out partially
		start_in[1] = 0;
		start_in[2] = 0;
		start_out[1] = 0;
		start_out[2] = 0;

		std::vector<float> floatBuffer(bufferSize);
		std::vector<double> doubleBuffer(bufferSize);
		float missingValue;
		float scaleFactor = 1;
		float addOffset = 0;

		ret = nc_get_att_float(ncid_in, rrVarId, "missing_value", &missingValue);
		ret = nc_get_att_float(ncid_in, rrVarId, "scale_factor", &scaleFactor);
		ret = nc_get_att_float(ncid_in, rrVarId, "add_offset", &addOffset);

		start_in[0] = 0;
		start_out[0] = i;
		nc_get_vara_float(ncid_in, rrVarId, start_in, len_in, floatBuffer.data());
		for (size_t j = 0; j < bufferSize; ++j) {
			if (floatBuffer[j] == missingValue) {
				doubleBuffer[j] = gdal->missingValue();
			} else {
				doubleBuffer[j] = floatBuffer[j] * scaleFactor + addOffset;
			}
		}
		nc_put_vara_double(ncid_out, varOutId, start_out, len_out, doubleBuffer.data());

		nc_close(ncid_in);

		QFile f(tmpFileName);
		f.remove();
	}

	auto timeContainer = m_groupDataItem->dimensions()->containers().at(0);
	timeContainer->setVariantValues(timeValues);
	gdal->outputDimensions(ncid_out, varIds);

	nc_close(ncid_out);

	gdal->updateShapeData();
	gdal->handleDimensionCurrentIndexChange(0, gdal->dimensions()->currentIndex());

	return true;
}
