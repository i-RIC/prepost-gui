#include "geodatanetcdf.h"
#include "geodatanetcdfimporter.h"
#include "geodatanetcdfimporterdateselectdialog.h"
#include "geodatanetcdfimportersettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattributedimensiont.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegExp>
#include <QString>
#include <QStringList>

namespace {

class nc_closer {
public:
	nc_closer(int id) {m_id = id;}
	~nc_closer()
	{
		nc_close(m_id);
	}

private:
	int m_id;
};

} // namespace

GeoDataNetcdfImporter::GeoDataNetcdfImporter(GeoDataCreator* creator) :
	GeoDataImporter("netcdf", tr("NetCDF"), creator)
{}

GeoDataNetcdfImporter::~GeoDataNetcdfImporter()
{}

const QStringList GeoDataNetcdfImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("NetCDF file (*.nc)"));
	return ret;
}

const QStringList GeoDataNetcdfImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("nc");
	return ret;
}

bool GeoDataNetcdfImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
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
	nc_closer closer(ncid);

	ret = nc_inq(ncid, &ndims, &nvars, &ngatts, &unlimdimid);
	if (ret != 0) {return false;}

	// investigate dimensions
	std::vector<int> dimids(ndims);
	ret = nc_inq_dimids(ncid, &ndims, dimids.data(), 0);
	if (ret != 0) {return false;}

	std::vector<QString> dims;
	std::vector<int> dimIds;

	for (int i = 0; i < ndims; ++i) {
		int dimid = dimids[i];
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
			dims.push_back(name);
			dimIds.push_back(dimid);
		}
	}

	if (m_lonDimId == -1 || m_latDimId == -1) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not have longitude and latitude data.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	if (m_xDimId != -1 && m_yDimId != -1) {
		m_csType = GeoDataNetcdf::XY;
	} else {
		m_csType = GeoDataNetcdf::LonLat;
	}

	std::vector<int> varids(nvars);
	ret = nc_inq_varids(ncid, &nvars, varids.data());
	nc_type ncType;
	int nDims;
	dimids.clear();
	dimids.assign(10, 0);
	int nAtts;

	std::vector<GeoDataNetcdfImporterSettingDialog::NcVariable> variables;
	for (int i = 0; i < nvars; ++i) {
		ret = nc_inq_var(ncid, varids[i], &(nameBuffer[0]), &ncType, &nDims, dimids.data(), &nAtts);
		if (nDims != 2 + condition->dimensions().size()) {
			// this is not a variable for value.
			continue;
		}
		GeoDataNetcdfImporterSettingDialog::NcVariable v;
		bool xOk = false;
		bool yOk = false;
		v.name = nameBuffer;
		for (int j = 0; j < nDims; ++j) {
			int dimid = dimids[j];
			if (m_csType == GeoDataNetcdf::XY && dimid == m_xDimId) {xOk = true;}
			else if (m_csType == GeoDataNetcdf::XY && dimid == m_yDimId) {yOk = true;}
			else if (m_csType == GeoDataNetcdf::LonLat && dimid == m_lonDimId) {xOk = true;}
			else if (m_csType == GeoDataNetcdf::LonLat && dimid == m_latDimId) {yOk = true;}

			auto it = std::find(dimids.begin(), dimids.end(), dimid);
			if (it != dimids.end()) {
				auto idx = static_cast<unsigned int>(it - dimids.begin());
				if (idx < dims.size()) {
					v.dimensions.push_back(dims[it - dimids.begin()]);
				}
			}
		}
		if (xOk && yOk) {
			variables.push_back(v);
		}
	}

	if (variables.size() == 0) {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not have variable that can be imported.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	GeoDataNetcdfImporterSettingDialog dialog(w);
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

bool GeoDataNetcdfImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	GeoDataNetcdf* netcdf = dynamic_cast<GeoDataNetcdf*>(data);

	int ncid_in, ncid_out;
	int ret;
	char nameBuffer[200];

	ret = nc_open(iRIC::toStr(filename()).c_str(), NC_NOWRITE, &ncid_in);
	if (ret != NC_NOERR) {return false;}
	nc_closer closer(ncid_in);

	QFileInfo finfo(netcdf->filename());
	iRIC::mkdirRecursively(finfo.absolutePath());

	// delete the file if it already exists.
	QFile f(netcdf->filename());
	f.remove();

	ret = nc_create(iRIC::toStr(netcdf->filename()).c_str(), NC_NETCDF4, &ncid_out);
	if (ret != NC_NOERR) {return false;}
	nc_closer closer_new(ncid_out);

	netcdf->m_coordinateSystemType = m_csType;

	// load coordinate values

	if (m_csType == GeoDataNetcdf::XY) {
		// load X and Y
		size_t xlen, ylen;
		ret = nc_inq_dimlen(ncid_in, m_xDimId, &xlen);
		ret = nc_inq_dimlen(ncid_in, m_yDimId, &ylen);

		std::vector<double> xs(xlen);
		std::vector<double> ys(ylen);

		int varid;
		ret = nc_inq_dimname(ncid_in, m_xDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, xlen, xs.data());

		ret = nc_inq_dimname(ncid_in, m_yDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, ylen, ys.data());

		netcdf->m_xValues.clear();
		for (size_t i = 0; i < xlen; ++i) {
			netcdf->m_xValues.push_back(xs[i]);
		}
		netcdf->m_yValues.clear();
		for (size_t i = 0; i < ylen; ++i) {
			netcdf->m_yValues.push_back(ys[i]);
		}

		// load Lon and Lat
		size_t lonLen, latLen;

		ret = nc_inq_dimlen(ncid_in, m_lonDimId, &lonLen);
		ret = nc_inq_dimlen(ncid_in, m_latDimId, &latLen);

		std::vector<double> lons(lonLen);
		std::vector<double> lats(latLen);

		ret = nc_inq_dimname(ncid_in, m_lonDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, lonLen, lons.data());

		ret = nc_inq_dimname(ncid_in, m_latDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, latLen, lats.data());

		netcdf->m_lonValues.clear();
		for (size_t i = 0; i < lonLen; ++i) {
			netcdf->m_lonValues.push_back(lons[i]);
		}
		netcdf->m_latValues.clear();
		for (size_t i = 0; i < latLen; ++i) {
			netcdf->m_latValues.push_back(lats[i]);
		}
	} else if (m_csType == GeoDataNetcdf::LonLat) {
		// load Lon and Lat
		size_t lonLen, latLen;

		ret = nc_inq_dimlen(ncid_in, m_lonDimId, &lonLen);
		ret = nc_inq_dimlen(ncid_in, m_latDimId, &latLen);

		std::vector<double> lons(lonLen);
		std::vector<double> lats(latLen);

		int varid;
		ret = nc_inq_dimname(ncid_in, m_lonDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, lonLen, lons.data());

		ret = nc_inq_dimname(ncid_in, m_latDimId, nameBuffer);
		ret = nc_inq_varid(ncid_in, nameBuffer, &varid);
		ret = ncGetVariableAsDouble(ncid_in, varid, latLen, lats.data());

		netcdf->m_lonValues.clear();
		for (size_t i = 0; i < lonLen; ++i) {
			netcdf->m_lonValues.push_back(lons[i]);
		}
		netcdf->m_latValues.clear();
		for (size_t i = 0; i < latLen; ++i) {
			netcdf->m_latValues.push_back(lats[i]);
		}
	}

	// load dimension values
	GridAttributeDimensionsContainer* dims = m_groupDataItem->dimensions();
	for (int i = 0; i < dims->containers().size(); ++i) {
		QString dim = m_dims.at(i);
		int dimid;
		int varid;
		int ret;
		size_t dimlen;

		ret = nc_inq_dimid(ncid_in, iRIC::toStr(dim).c_str(), &dimid);
		ret = nc_inq_dimlen(ncid_in, dimid, &dimlen);
		ret = nc_inq_varid(ncid_in, iRIC::toStr(dim).c_str(), &varid);

		std::vector<QVariant> vals;
		std::vector<QVariant> convertedVals;
		ret = ncGetVariableAsQVariant(ncid_in, varid, dimlen, vals);
		convertedVals = vals;
		GridAttributeDimensionContainer* c = dims->containers().at(i);
		if (c->definition()->name() == "Time") {
			// if the dimension is time, convert the value using units information.
			char unitBuffer[200] = "";
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
	std::vector<int> dimIds;
	std::vector<int> varIds;
	int varOutId;

	ret = nc_redef(ncid_out);
	netcdf->defineCoords(ncid_out, &out_xDimId, &out_yDimId, &out_lonDimId, &out_latDimId, &out_xVarId, &out_yVarId, &out_lonVarId, &out_latVarId);
	netcdf->defineDimensions(ncid_out, &dimIds, &varIds);
	if (m_csType == GeoDataNetcdf::XY) {
		ret = netcdf->defineValue(ncid_out, out_xDimId, out_yDimId, dimIds, &varOutId);
	} else if (m_csType == GeoDataNetcdf::LonLat) {
		ret = netcdf->defineValue(ncid_out, out_lonDimId, out_latDimId, dimIds, &varOutId);
	}

	ret = nc_enddef(ncid_out);
	netcdf->outputCoords(ncid_out, out_xVarId, out_yVarId, out_lonVarId, out_latVarId);
	netcdf->outputDimensions(ncid_out, varIds);

	ret = importValues(ncid_in, ncid_out, varOutId, m_xDimId, m_yDimId, m_lonDimId, m_latDimId, dimIds, netcdf);

	netcdf->updateShapeData();
	netcdf->doHandleDimensionCurrentIndexChange(0, dims->currentIndex());

	return true;
}

int GeoDataNetcdfImporter::ncGetVariableAsDouble(int ncid, int varid, size_t len, double* buffer)
{
	Q_UNUSED(len)

	int ret;
	ret = nc_get_var_double(ncid, varid, buffer);
	if (ret != NC_NOERR) { return ret; }

	double scaleFactor;
	double addOffset;

	ret = nc_get_att_double(ncid, varid, "scale_factor", &scaleFactor);
	if (ret != NC_NOERR) {scaleFactor = 1;}

	ret = nc_get_att_double(ncid, varid, "add_offset", &addOffset);
	if (ret != NC_NOERR) {addOffset = 0;}

	for (size_t i = 0; i < len; ++i) {
		*(buffer + i) = *(buffer + i) * scaleFactor + addOffset;
	}

	return NC_NOERR;
}

template<typename T>
int getVariableAsQVariant(int ncid, int varid, size_t len, int (*f)(int, int, T*), std::vector<QVariant>* list)
{
	std::vector<T> tmpbuffer(len);
	int ret = f(ncid, varid, tmpbuffer.data());
	if (ret != NC_NOERR) {return ret;}
	for (size_t i = 0; i < len; ++i) {
		list->push_back(QVariant(tmpbuffer[i]));
	}
	return NC_NOERR;
}

int GeoDataNetcdfImporter::ncGetVariableAsQVariant(int ncid, int varid, size_t len, std::vector<QVariant>& list)
{
	int ret;
	nc_type ncType;
	list.clear();
	ret = nc_inq_vartype(ncid, varid, &ncType);
	if (ncType == NC_BYTE) {
		return getVariableAsQVariant<signed char>(ncid, varid, len, nc_get_var_schar, &list);
	} else if (ncType == NC_SHORT) {
		return getVariableAsQVariant<short int>(ncid, varid, len, nc_get_var_short, &list);
	} else if (ncType == NC_INT) {
		return getVariableAsQVariant<int>(ncid, varid, len, nc_get_var_int, &list);
	} else if (ncType == NC_LONG) {
		return getVariableAsQVariant<long int>(ncid, varid, len, nc_get_var_long, &list);
	} else if (ncType == NC_FLOAT) {
		return getVariableAsQVariant<float>(ncid, varid, len, nc_get_var_float, &list);
	} else if (ncType == NC_DOUBLE) {
		return getVariableAsQVariant<double>(ncid, varid, len, nc_get_var_double, &list);
	} else if (ncType == NC_UBYTE) {
		return getVariableAsQVariant<unsigned char>(ncid, varid, len, nc_get_var_uchar, &list);
	} else if (ncType == NC_USHORT) {
		return getVariableAsQVariant<unsigned short int>(ncid, varid, len, nc_get_var_ushort, &list);
	} else if (ncType == NC_UINT) {
		return getVariableAsQVariant<unsigned int>(ncid, varid, len, nc_get_var_uint, &list);
	} else if (ncType == NC_INT64) {
		return getVariableAsQVariant<long long>(ncid, varid, len, nc_get_var_longlong, &list);
	} else if (ncType == NC_UINT64) {
		return getVariableAsQVariant<unsigned long long>(ncid, varid, len, nc_get_var_ulonglong, &list);
	}
	return NC_NOERR;
}

std::vector<QVariant> GeoDataNetcdfImporter::convertTimeValues(QString units, const std::vector<QVariant>& values, QWidget* parent, bool* canceled)
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

	QRegExp rxDateTime("(([0-9]{4})-([0-9]{1,2})-([0-9]{1,2}) ([0-9]{1,2}):([0-9]{1,2}):([0-9]{1,2})(|\\.([0-9]+)))(| +0:00)");
	QRegExp rxDateTime2("(([0-9]{4})/([0-9]{1,2})/([0-9]{1,2}) ([0-9]{1,2}):([0-9]{1,2}):([0-9]{1,2})(|\\.([0-9]+)))(| +0:00)");
	QRegExp rxDate("(([0-9]{4})-([0-9]{1,2})-([0-9]{1,2}))");
	QRegExp rxDate2("(([0-9]{4})/([0-9]{1,2})/([0-9]{1,2}))");

	if (rxDateTime.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(rxDateTime.cap(1), "yyyy-M-d H:m:s");
	} else if (rxDateTime2.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(rxDateTime2.cap(1), "yyyy/M/d H:m:s");
	} else if (rxDate.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(rxDate.cap(1), "yyyy-M-d");
	} else if (rxDate2.indexIn(since) != -1) {
		zeroDate = QDateTime::fromString(rxDate2.cap(1), "yyyy/M/d");
	}

	std::vector<QVariant> ret;
	if (!zeroDate.isValid()) {
		GeoDataNetcdfImporterDateSelectDialog dialog(parent);
		dialog.setUnit(units);
		// get current time with seconds=0
		QDateTime current = QDateTime::currentDateTime();
		QTime t = current.time();
		t.setHMS(0, 0, 0);
		current.setTime(t);
		dialog.setOriginalDateTime(current);
		int dialogRet = dialog.exec();
		if (dialogRet == QDialog::Rejected) {
			*canceled = true;
			return ret;
		}
		zeroDate = dialog.originalDateTime();
		unit = dialog.unit();
	}

	qDebug("zeroDate=%s\n", zeroDate.toString(Qt::ISODate).toStdString().c_str());
	const qlonglong LL_SECS_PER_DAY    = 86400LL;
	const qlonglong LL_SECS_PER_HOUR   = 3600LL;
	const qlonglong LL_SECS_PER_MINUTE = 60LL;
	for (int i = 0; i < values.size(); ++i) {
		QVariant val = values.at(i);
		QDateTime d = zeroDate;
		if (unit == "seconds") {
			d = d.addSecs(val.toLongLong());
		} else if (unit == "minutes") {
			d = d.addSecs(val.toLongLong() * LL_SECS_PER_MINUTE);
		} else if (unit == "hours") {
			d = d.addSecs(val.toLongLong() * LL_SECS_PER_HOUR);
		} else if (unit == "days") {
			qlonglong days = val.toLongLong();
			qlonglong secs = static_cast<qlonglong>((val.toDouble() - days) * LL_SECS_PER_DAY);
			d = d.addDays(days);
			d = d.addSecs(secs);
		} else if (unit == "months") {
			int months = val.toInt();
			d = d.addMonths(months);
		} else if (unit == "years") {
			int years = val.toInt();
			d = d.addYears(years);
		}
		qDebug("d=%s\n", d.toString(Qt::ISODate).toStdString().c_str());
		ret.push_back(d.toTime_t());
	}
	return ret;
}
