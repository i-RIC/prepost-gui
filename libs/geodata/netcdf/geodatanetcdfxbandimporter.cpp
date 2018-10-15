#include "geodatanetcdfreal.h"
#include "geodatanetcdfxbandimporter.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
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

GeoDataNetcdfXbandImporter::GeoDataNetcdfXbandImporter(GeoDataCreator* creator) :
	GeoDataImporter {"xband", tr("XBand MP RADER data"), creator}
{}

GeoDataNetcdfXbandImporter::~GeoDataNetcdfXbandImporter()
{}

const QStringList GeoDataNetcdfXbandImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("X band MP rader data (*.*)"));
	return ret;
}

const QStringList GeoDataNetcdfXbandImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("*");
	return ret;
}

bool GeoDataNetcdfXbandImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	m_groupDataItem = item;

	// investigate the condition.
	const auto& dims = condition->dimensions();
	if (! (dims.size() == 1 && dims.at(0)->name() == "Time")) {
		QMessageBox::warning(w, tr("Warning"), tr("X band MP rader data can be imported for grid conditions with dimension \"Time\"."));
		return false;
	}

	QFileInfo finfo(filename);
	QDir dir = finfo.absoluteDir();
	QDir workDir = dir;
	QStringList filter;
	QStringList filenames = dir.entryList(filter, QDir::Files, QDir::Name);
	m_tmpFileName = dir.absoluteFilePath(TMP_NC);

	QString workTmpFileName = m_tmpFileName;
	QTemporaryDir* tempDir = nullptr;

	QTextCodec* latin1 = QTextCodec::codecForName("latin1");
	if (! latin1->canEncode(dir.absolutePath()) || dir.absolutePath().length() > 90) {
		// the name contains non-ASCII characters.
		// we should copy to temporary directory.
		QDir wsDir(item->projectData()->workDirectory());
		tempDir = new QTemporaryDir(wsDir.absoluteFilePath("xband"));
		workDir = QDir(tempDir->path());
		for (QString fname : filenames) {
			QFile::copy(dir.absoluteFilePath(fname), workDir.absoluteFilePath(fname));
		}
		workTmpFileName = workDir.absoluteFilePath(TMP_NC);
	}

	if (! item->iricMainWindow()->cuiMode()) {
		int ret = QMessageBox::information(w, tr("Information"), tr("%1 files in the folder %2 are imported.").arg(filenames.size()).arg(QDir::toNativeSeparators(dir.absolutePath())), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {return false;}
	}

	// use mlitx2nc to convert files into NetCDF file.
	QString exepath = iRICRootPath::get();
	QString exeName = QDir(exepath).absoluteFilePath("mlitx2nc.exe");
	QProcess* mlitx2ncProcess = new QProcess();
	QStringList args;
	args << workTmpFileName << QString::number(filenames.size());

	mlitx2ncProcess->start(exeName, args);
	for (QString fname : filenames) {
		QString fullName = QDir::toNativeSeparators(workDir.absoluteFilePath(fname));
		fullName.append("\r\n");
		mlitx2ncProcess->write(iRIC::toStr(fullName).c_str());
	}
	mlitx2ncProcess->waitForFinished(-1);

	// mlitx2nc process finished.

	if (tempDir != nullptr) {

		std::string workTmpFileNameStr = iRIC::toStr(workTmpFileName);

		QFile::copy(workTmpFileName, m_tmpFileName);
		delete tempDir;
	}

	return true;
}

bool GeoDataNetcdfXbandImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	GeoDataNetcdfReal* netcdf = dynamic_cast<GeoDataNetcdfReal*>(data);

	int ncid_in, ncid_out;
	int ret;

	ret = nc_open(iRIC::toStr(m_tmpFileName).c_str(), NC_NOWRITE, &ncid_in);
	if (ret != NC_NOERR) {return false;}
	QFileInfo finfo(netcdf->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(netcdf->filename());
	f.remove();

	ret = nc_create(iRIC::toStr(netcdf->filename()).c_str(), NC_NETCDF4, &ncid_out);
	if (ret != NC_NOERR) {return false;}

	netcdf->m_coordinateSystemType = GeoDataNetcdf::LonLat;

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

	std::vector<double> lons(lonLen);
	std::vector<double> lats(latLen);
	std::vector<double> times(timeLen);

	nc_get_var_double(ncid_in, lonVarId, lons.data());
	nc_get_var_double(ncid_in, latVarId, lats.data());
	nc_get_var_double(ncid_in,timeVarId, times.data());

	netcdf->m_lonValues.clear();
	for (size_t i = 0; i < lonLen; ++i) {
		netcdf->m_lonValues.push_back(lons[i]);
	}
	netcdf->m_latValues.clear();
	for (size_t i = 0; i < latLen; ++i) {
		netcdf->m_latValues.push_back(lats[i]);
	}

	// Xband x rader time valuesa are already unix time stamp. No conversion.
	GridAttributeDimensionsContainer* dims = m_groupDataItem->dimensions();
	GridAttributeDimensionContainer* c = dims->containers().at(0);
	std::vector<QVariant> timeVals;
	for (size_t i = 0; i < timeLen; ++i) {
		timeVals.push_back(times[i]);
	}

	if (c->variantValues().size() == 0) {
		c->setVariantValues(timeVals);
	} else {
		if (c->variantValues() != timeVals) {
			QMessageBox::critical(w, tr("Error"), tr("Dimension values for time mismatch.").arg(c->definition()->caption()));
			return false;
		}
	}

	// save coordinates and dimensions to the netcdf file.
	int out_xDimId, out_yDimId, out_lonDimId, out_latDimId;
	int out_xVarId, out_yVarId, out_lonVarId, out_latVarId;
	std::vector<int> dimIds;
	std::vector<int> varIds;
	int varOutId;

	ret = nc_redef(ncid_out);
	netcdf->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_lonDimId, &out_latDimId, &out_xVarId, &out_yVarId, &out_lonVarId, &out_latVarId);
	netcdf->defineDimensions(ncid_out, &dimIds, &varIds);
	ret = netcdf->defineValue(ncid_out, out_lonDimId, out_latDimId, dimIds, &varOutId);

	ret = nc_enddef(ncid_out);
	netcdf->outputCoords(ncid_out, out_xVarId, out_yVarId, out_lonVarId, out_latVarId);
	netcdf->outputDimensions(ncid_out, varIds);

	size_t start_in[3];
	size_t start_out[3];
	size_t len_in[3];
	size_t len_out[3];
	size_t bufferSize = 0;

	// setup len_in, len_out
	len_in[0] = 1;
	len_in[1] = netcdf->latValues().size();
	len_in[2] = netcdf->lonValues().size();
	len_out[0] = 1;
	len_out[1] = netcdf->latValues().size();
	len_out[2] = netcdf->lonValues().size();
	bufferSize = netcdf->lonValues().size() * netcdf->latValues().size();

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

	for (size_t i = 0; i < timeLen; ++i) {
		start_in[0] = i;
		start_out[0] = i;
		nc_get_vara_float(ncid_in, rrVarId, start_in, len_in, floatBuffer.data());
		for (size_t j = 0; j < bufferSize; ++j) {
			if (floatBuffer[j] == missingValue) {
				doubleBuffer[j] = netcdf->missingValue();
			} else {
				doubleBuffer[j] = floatBuffer[j] * scaleFactor + addOffset;
			}
		}
		nc_put_vara_double(ncid_out, varOutId, start_out, len_out, doubleBuffer.data());
	}

	nc_close(ncid_in);
	nc_close(ncid_out);

	netcdf->updateShapeData();
	netcdf->handleDimensionCurrentIndexChange(0, dims->currentIndex());

	QFile f2(m_tmpFileName);
	f2.remove();

	return true;
}
