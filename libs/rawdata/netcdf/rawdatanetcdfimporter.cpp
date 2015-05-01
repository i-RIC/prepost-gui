#include "rawdatanetcdf.h"
#include "rawdatanetcdfimporter.h"
#include "rawdatanetcdfimportersettingdialog.h"
#include "rawdatanetcdfimporterdateselectdialog.h"
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiondimensiont.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensioncontainer.h>

#include <misc/stringtool.h>
#include <misc/filesystemfunction.h>

#include <QStringList>
#include <QString>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QRegExp>
#include <QDateTime>

const QStringList RawDataNetcdfImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("NetCDF file (*.nc)"));
	return ret;
}

const QStringList RawDataNetcdfImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("nc");
	return ret;
}

bool RawDataNetcdfImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w)
{
	m_groupDataItem = item;

	char nameBuffer[200];

	std::string fname = iRIC::toStr(filename);
	int ncid;
	int ndims, nvars, ngatts, unlimdimid;

	m_xDimId = -1;
	m_yDimId = -1;
	m_lonDimId = -1;
	m_latDimId = -1;

	int ret = nc_open(fname.c_str(), NC_NOWRITE, &ncid);
	if (ret != 0) {return false;}
	ret = nc_inq(ncid, &ndims, &nvars, &ngatts, &unlimdimid);
	if (ret != 0) {return false;}

	// investigate dimensions
	int* dimids = new int[ndims];
	ret = nc_inq_dimids(ncid, &ndims, dimids, 0);
	if (ret != 0) {return false;}

	QList<QString> dims;
	QList<int> dimIds;

	for (int i = 0; i < ndims; ++i) {
		int dimid = *(dimids + i);
		ret = nc_inq_dimname(ncid, dimid, &(nameBuffer[0]));
		if (ret != 0) {return false;}
		QString name = QString(nameBuffer);
		if (name.toLower() == "x") {
			// x found
			m_xDimId = dimid;
		} else if (name.toLower() == "y") {
			// y found
			m_yDimId = dimid;
		} else if (name.toLower() == "lon" || name.toLower() == "longitude") {
			// longitude found
			m_lonDimId = dimid;
		} else if (name.toLower() == "lat" || name.toLower() == "latitude") {
			// latitude found
			m_latDimId = dimid;
		}	else {
			dims.append(name);
			dimIds.append(dimid);
		}
	}
	delete dimids;

	if (m_lonDimId == -1 || m_latDimId == -1) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not have longitude and latitude data.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	if (m_xDimId != -1 && m_yDimId != -1) {
		m_csType = RawDataNetcdf::XY;
	} else {
		m_csType = RawDataNetcdf::LonLat;
	}

	int* varids = new int[nvars];
	ret = nc_inq_varids(ncid, &nvars, varids);
	nc_type ncType;
	int nDims;
	dimids = new int[10];
	int nAtts;

	QList<RawDataNetcdfImporterSettingDialog::NcVariable> variables;
	for (int i = 0; i < nvars; ++i) {
		ret = nc_inq_var(ncid, *(varids + i), &(nameBuffer[0]), &ncType, &nDims, &(dimids[0]), &nAtts);
		if (nDims != 2 + condition->dimensions().size()) {
			// this is not a variable for value.
			continue;
		}
		RawDataNetcdfImporterSettingDialog::NcVariable v;
		bool xOk = false;
		bool yOk = false;
		v.name = nameBuffer;
		for (int j = 0; j < nDims; ++j) {
			int dimid = dimids[j];
			if (m_csType == RawDataNetcdf::XY && dimid == m_xDimId) {xOk = true;}
			else if (m_csType == RawDataNetcdf::XY && dimid == m_yDimId) {yOk = true;}
			else if (m_csType == RawDataNetcdf::LonLat && dimid == m_lonDimId) {xOk = true;}
			else if (m_csType == RawDataNetcdf::LonLat && dimid == m_latDimId) {yOk = true;}
			int dimIdx = dimIds.indexOf(dimid);
			if (dimIdx != -1) {
				v.dimensions.append(dims.at(dimIdx));
			}
		}
		if (xOk && yOk) {
			variables.append(v);
		}
	}
	delete dimids;
	delete varids;

	if (variables.size() == 0) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not have variable that can be imported.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	RawDataNetcdfImporterSettingDialog dialog(w);
	dialog.setCondition(condition);
	dialog.setVariables(variables);

	if (dialog.needToShow()) {
		ret = dialog.exec();
		if (ret == QDialog::Rejected) {return false;}
	}

	m_valueVariable = dialog.variableName();
	m_dims = dialog.dimensionMappingSetting();

	return true;
}

bool RawDataNetcdfImporter::importData(RawData* data, int /*index*/, QWidget* w)
{
	RawDataNetcdf* netcdf = dynamic_cast<RawDataNetcdf*>(data);

	int ncid_in, ncid_out;
	int ret;
	char nameBuffer[200];

	ret = nc_open(iRIC::toStr(m_filename).c_str(), NC_NOWRITE, &ncid_in);
	if (ret != NC_NOERR) {return false;}
	QFileInfo finfo(netcdf->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(netcdf->filename());
	f.remove();

	ret = nc_create(iRIC::toStr(netcdf->filename()).c_str(), NC_NETCDF4, &ncid_out);
	if (ret != NC_NOERR) {return false;}

	netcdf->m_coordinateSystemType = m_csType;

	// load coordinate values

	if (m_csType == RawDataNetcdf::XY) {
		// load X and Y
		size_t xlen, ylen;
		ret = nc_inq_dimlen(ncid_in, m_xDimId, &xlen);
		ret = nc_inq_dimlen(ncid_in, m_yDimId, &ylen);

		double* xs = new double[xlen];
		double* ys = new double[ylen];

		int varid;
		ret = nc_inq_dimname(ncid_in, m_xDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, xlen, xs);

		ret = nc_inq_dimname(ncid_in, m_yDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, ylen, ys);

		netcdf->m_xValues.clear();
		for (int i = 0; i < xlen; ++i) {
			netcdf->m_xValues.append(*(xs + i));
		}
		netcdf->m_yValues.clear();
		for (int i = 0; i < ylen; ++i) {
			netcdf->m_yValues.append(*(ys + i));
		}

		delete xs;
		delete ys;

		// load Lon and Lat
		size_t lonLen, latLen;

		ret = nc_inq_dimlen(ncid_in, m_lonDimId, &lonLen);
		ret = nc_inq_dimlen(ncid_in, m_latDimId, &latLen);

		double* lons = new double[lonLen];
		double* lats = new double[latLen];

		ret = nc_inq_dimname(ncid_in, m_lonDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, lonLen, lons);

		ret = nc_inq_dimname(ncid_in, m_latDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, latLen, lats);

		netcdf->m_lonValues.clear();
		for (int i = 0; i < lonLen; ++i) {
			netcdf->m_lonValues.append(*(lons + i));
		}
		netcdf->m_latValues.clear();
		for (int i = 0; i < latLen; ++i) {
			netcdf->m_latValues.append(*(lats + i));
		}

		delete lons;
		delete lats;
	} else if (m_csType == RawDataNetcdf::LonLat) {
		// load Lon and Lat
		size_t lonLen, latLen;

		ret = nc_inq_dimlen(ncid_in, m_lonDimId, &lonLen);
		ret = nc_inq_dimlen(ncid_in, m_latDimId, &latLen);

		double* lons = new double[lonLen];
		double* lats = new double[latLen];

		int varid;
		ret = nc_inq_dimname(ncid_in, m_lonDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, lonLen, lons);

		ret = nc_inq_dimname(ncid_in, m_latDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, latLen, lats);

		netcdf->m_lonValues.clear();
		for (int i = 0; i < lonLen; ++i) {
			netcdf->m_lonValues.append(*(lons + i));
		}
		netcdf->m_latValues.clear();
		for (int i = 0; i < latLen; ++i) {
			netcdf->m_latValues.append(*(lats + i));
		}

		delete lons;
		delete lats;
	}

	// load dimension values
	GridRelatedConditionDimensionsContainer* dims = m_groupDataItem->dimensions();
	for (int i = 0; i < dims->containers().size(); ++i) {
		QString dim = m_dims.at(i);
		int dimid;
		int varid;
		int ret;
		size_t dimlen;

		ret = nc_inq_dimid(ncid_in, iRIC::toStr(dim).c_str(), &dimid);
		ret = nc_inq_dimlen(ncid_in, dimid, &dimlen);
		ret = nc_inq_varid(ncid_in, iRIC::toStr(dim).c_str(), &varid);

		QList<QVariant> vals;
		QList<QVariant> convertedVals;
		ret = ncGetVariableAsQVariant(ncid_in, varid, dimlen, vals);
		convertedVals = vals;
		GridRelatedConditionDimensionContainer* c = dims->containers().at(i);
		if (c->definition()->name() == "Time") {
			// if the dimension is time, convert the value using units information.
			char unitBuffer[200];
			ret = nc_get_att_text(ncid_in, varid, "units", unitBuffer);
			if (ret != NC_NOERR) {
				// no units information. do nothing;
			} else {
				bool canceled = false;
				convertedVals = convertTimeValues(unitBuffer, vals, w, &canceled);
				if (canceled) {
					return false;
				}
			}
		}
		if (c->variantValues().size() == 0) {
			c->setVariantValues(convertedVals);
		} else {
			if (c->variantValues() != convertedVals) {
				QMessageBox::critical(w, tr("Error"), tr("Dimension values for %1 mismatch.").arg(c->definition()->caption()));
				return false;
			}
		}
	}
	// save coordinates and dimensions to the netcdf file.
	int out_xDimId, out_yDimId, out_lonDimId, out_latDimId;
	int out_xVarId, out_yVarId, out_lonVarId, out_latVarId;
	QList<int> dimIds;
	QList<int> varIds;
	int varOutId;

	ret = nc_redef(ncid_out);
	netcdf->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_lonDimId, &out_latDimId, &out_xVarId, &out_yVarId, &out_lonVarId, &out_latVarId);
	netcdf->defineDimensions(ncid_out, dimIds, varIds);
	if (m_csType == RawDataNetcdf::XY) {
		ret = netcdf->defineValue(ncid_out, out_xDimId, out_yDimId, dimIds, &varOutId);
	} else if (m_csType == RawDataNetcdf::LonLat) {
		ret = netcdf->defineValue(ncid_out, out_lonDimId, out_latDimId, dimIds, &varOutId);
	}

	ret = nc_enddef(ncid_out);
	netcdf->outputCoords(ncid_out, out_xVarId, out_yVarId, out_lonVarId, out_latVarId);
	netcdf->outputDimensions(ncid_out, varIds);

	ret = importValues(ncid_in, ncid_out, varOutId, m_xDimId, m_yDimId, m_lonDimId, m_latDimId, dimIds, netcdf);

	nc_close(ncid_in);
	nc_close(ncid_out);

	netcdf->updateShapeData();
	netcdf->doHandleDimensionCurrentIndexChange(0, dims->currentIndex());

	return true;
}

int RawDataNetcdfImporter::ncGetVariableAsDouble(int ncid, int varid, size_t len, double* buffer)
{
	int ret;
	ret = nc_get_var_double(ncid, varid, buffer);
	if (ret != NC_NOERR) { return ret; }

	return NC_NOERR;
}

int RawDataNetcdfImporter::ncGetVariableAsQVariant(int ncid, int varid, size_t len, QList<QVariant>& list)
{
	int ret;
	nc_type ncType;
	list.clear();
	ret = nc_inq_vartype(ncid, varid, &ncType);
	if (ncType == NC_INT) {
		int* tmpbuffer = new int[len];
		ret = nc_get_var_int(ncid, varid, tmpbuffer);
		if (ret != NC_NOERR) { return ret; }
		for (int i = 0; i < len; ++i) {
			list.append(QVariant(*(tmpbuffer + i)));
		}
		delete tmpbuffer;
	} else if (ncType == NC_UINT) {
		unsigned int* tmpbuffer = new unsigned int[len];
		ret = nc_get_var_uint(ncid, varid, tmpbuffer);
		if (ret != NC_NOERR) { return ret; }
		for (int i = 0; i < len; ++i) {
			list.append(QVariant(*(tmpbuffer + i)));
		}
		delete tmpbuffer;
	} else if (ncType == NC_INT64) {
		long long* tmpbuffer = new long long[len];
		ret = nc_get_var_longlong(ncid, varid, tmpbuffer);
		if (ret != NC_NOERR) { return ret; }
		for (int i = 0; i < len; ++i) {
			list.append(QVariant(*(tmpbuffer + i)));
		}
		delete tmpbuffer;
	} else if (ncType == NC_UINT64) {
		unsigned long long* tmpbuffer = new unsigned long long[len];
		ret = nc_get_var_ulonglong(ncid, varid, tmpbuffer);
		if (ret != NC_NOERR) { return ret; }
		for (int i = 0; i < len; ++i) {
			list.append(QVariant(*(tmpbuffer + i)));
		}
		delete tmpbuffer;
	}	else if (ncType == NC_FLOAT) {
		float* tmpbuffer = new float[len];
		ret = nc_get_var_float(ncid, varid, tmpbuffer);
		if (ret != NC_NOERR) { return ret; }
		for (int i = 0; i < len; ++i) {
			list.append(QVariant(*(tmpbuffer + i)));
		}
		delete tmpbuffer;
	} else if (ncType == NC_DOUBLE) {
		double* tmpbuffer = new double[len];
		ret = nc_get_var_double(ncid, varid, tmpbuffer);
		if (ret != NC_NOERR) { return ret; }
		for (int i = 0; i < len; ++i) {
			list.append(QVariant(*(tmpbuffer + i)));
		}
		delete tmpbuffer;
	}
	return NC_NOERR;
}

QList<QVariant> RawDataNetcdfImporter::convertTimeValues(QString units, QList<QVariant>& values, QWidget* parent, bool* canceled)
{
	*canceled = false;
	QRegExp rx("(.+) since (.+)");
	if (rx.indexIn(units) == -1) {
		// not matched!
		return values;
	}
	QString unit = rx.cap(1);
	QString since = rx.cap(2);
	QDateTime zeroDate;

	QRegExp rxDateTime("([0-9]{4})-([0-9]{1,2})-([0-9]{1,2}) ([0-9]{1,2}):([0-9]{1,2}):([0-9]{1,2})(|\\.([0-9]+))");
	QRegExp rxDateTime2("([0-9]{4})/([0-9]{1,2})/([0-9]{1,2}) ([0-9]{1,2}):([0-9]{1,2}):([0-9]{1,2})(|\\.([0-9]+))");
	QRegExp rxDate("([0-9]{4})-([0-9]{1,2})-([0-9]{1,2})");
	QRegExp rxDate2("([0-9]{4})/([0-9]{1,2})/([0-9]{1,2})");

	if (rxDateTime.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(since, "yyyy-M-d H:m:s");
	} else if (rxDateTime2.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(since, "yyyy/M/d H:m:s");
	} else if (rxDate.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(since, "yyyy-M-d");
	} else if (rxDate2.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(since, "yyyy/M/d");
	}

	QList<QVariant> ret;
	if (!zeroDate.isValid()) {
		RawDataNetcdfImporterDateSelectDialog dialog(parent);
		dialog.setUnit(units);
		dialog.setOriginalDateTime(QDateTime::currentDateTime());
		int dialogRet = dialog.exec();
		if (dialogRet == QDialog::Rejected) {
			*canceled = true;
			return ret;
		}
		zeroDate = dialog.originalDateTime();
	}

	for (int i = 0; i < values.size(); ++i) {
		QVariant val = values.at(i);
		QDateTime d = zeroDate;
		if (unit == "seconds") {
			d = d.addSecs(val.toInt());
		} else if (unit == "days") {
			qlonglong days = val.toLongLong();
			int secs = static_cast<int>((val.toDouble() - days) * 24 * 60 * 60);
			d = d.addDays(days);
			d = d.addSecs(secs);
		} else if (unit == "months") {
			int months = val.toInt();
			d = d.addMonths(months);
		} else if (unit == "years") {
			int years = val.toInt();
			d = d.addYears(years);
		}
		ret.append(d.toTime_t());
	}
	return ret;
}
