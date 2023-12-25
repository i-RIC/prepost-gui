#include "geodatanetcdf.h"
#include "geodatanetcdfimporter.h"
#include "geodatanetcdfimporterdateselectdialog.h"
#include "geodatanetcdfimportersettingdialog.h"
#include "private/geodatanetcdf_impl.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataitemi.h>
#include <guicore/pre/base/preprocessorgriddataitemi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/grid/v4inputgrid.h>
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
#include <QTimeZone>

#include <udunits2.h>

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

int getVarLen(int ncid, int varid)
{
	char nameBuffer[200];
	nc_type ncType;
	int ndims;
	int dimids[10];
	int natts;

	nc_inq_var(ncid, varid, nameBuffer, &ncType, &ndims, &(dimids[0]), &natts);
	int size = 1;
	for (int i = 0; i < ndims; ++i) {
		int dimid = dimids[i];
		size_t s;
		nc_inq_dimlen(ncid, dimid, &s);
		size = size * s;
	}
	return size;
}

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

bool GeoDataNetcdfImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	if (item->geoDatas().size() > 1) {
		QMessageBox::critical(w, tr("Error"), tr("Time series raster data is already imported. If you want to import other data, please delete the data already imported first."));
		return false;
	}
	// grid attributes cleared
	auto conds = item->geoDataTopDataItem()->gridTypeDataItem()->conditions();
	for (auto cond : conds) {
		auto grid = cond->gridDataItem()->grid();
		auto att = grid->attribute(condition->name());
		att->clearTemporaryData();
		att->setDefaultValue();
	}

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

	if (m_xDimId != -1 && m_yDimId != -1) {
		m_csType = GeoDataNetcdf::XY;
	} else if (m_latDimId != -1 && m_lonDimId != -1){
		m_csType = GeoDataNetcdf::LonLat;
	} else {
		QMessageBox::critical(w, tr("Error"), tr("%1 does not have longitude, latitude nor x, y data.").arg(QDir::toNativeSeparators(filename)));
		return false;
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
		QString name = QString(nameBuffer).toLower();
		if (m_csType == GeoDataNetcdf::XY && (name == "lon" || name == "longitude")) {
			m_lonVarId = i;
			continue;
		}
		if (m_csType == GeoDataNetcdf::XY && (name == "lat" || name == "latitude")) {
			m_latVarId = i;
			continue;
		}
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

	netcdf->impl->m_coordinateSystemType = m_csType;

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

		netcdf->impl->m_xValues.clear();
		for (size_t i = 0; i < xlen; ++i) {
			netcdf->impl->m_xValues.push_back(xs[i]);
		}
		netcdf->impl->m_yValues.clear();
		for (size_t i = 0; i < ylen; ++i) {
			netcdf->impl->m_yValues.push_back(ys[i]);
		}

		// load Lon and Lat
		size_t lonLen, latLen;

		lonLen = getVarLen(ncid_in, m_lonVarId);
		latLen = getVarLen(ncid_in, m_latVarId);

		std::vector<double> lons(lonLen);
		std::vector<double> lats(latLen);

		ret = ncGetVariableAsDouble(ncid_in, m_lonVarId, lonLen, lons.data());
		ret = ncGetVariableAsDouble(ncid_in, m_latVarId, latLen, lats.data());

		netcdf->impl->m_lonValues.clear();
		for (size_t i = 0; i < lonLen; ++i) {
			netcdf->impl->m_lonValues.push_back(lons[i]);
		}
		netcdf->impl->m_latValues.clear();
		for (size_t i = 0; i < latLen; ++i) {
			netcdf->impl->m_latValues.push_back(lats[i]);
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

		netcdf->impl->m_lonValues.clear();
		for (size_t i = 0; i < lonLen; ++i) {
			netcdf->impl->m_lonValues.push_back(lons[i]);
		}
		netcdf->impl->m_latValues.clear();
		for (size_t i = 0; i < latLen; ++i) {
			netcdf->impl->m_latValues.push_back(lats[i]);
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
		c->setVariantValues(convertedVals);
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

	ut_system* unitSystem = ut_read_xml(nullptr);

	ut_unit* second = ut_get_unit_by_name(unitSystem, "second");
	ut_unit* unixTime = ut_offset_by_time(second, ut_encode_time(1970, 1, 1, 0, 0, 0.0));

	auto unitsStr = iRIC::toStr(units);
	ut_unit* unit = ut_parse(unitSystem, unitsStr.c_str(), UT_ASCII);
	if (unit != nullptr) {
		std::vector<QVariant> ret;
		cv_converter* converter = ut_get_converter(unit, unixTime);

		for (int i = 0; i < values.size(); ++i) {
			double val = values.at(i).toDouble();
			double unixTimeVal = cv_convert_double(converter, val);
			ret.push_back(unixTimeVal);
		}
		return ret;
	}

	GeoDataNetcdfImporterDateSelectDialog dialog(parent);
	dialog.setUnit(units);
	int result = dialog.exec();

	if (result == QDialog::Rejected) {
		*canceled = true;
		std::vector<QVariant> empty;
		return empty;
	}

	QDateTime zeroDate = dialog.originalDateTime();
	auto timeUnit = dialog.timeUnit();
	auto timeZone = dialog.timeZone();

	std::vector<QVariant> ret;
	for (int i = 0; i < values.size(); ++i) {
		QVariant val = values.at(i);
		QDateTime d = zeroDate;
		if (timeUnit == GeoDataNetcdfImporterDateSelectDialog::TimeUnit::Years) {
			d = d.addYears(val.toInt());
		} else if (timeUnit == GeoDataNetcdfImporterDateSelectDialog::TimeUnit::Days) {
			qlonglong days = val.toLongLong();
			int secs = static_cast<int>((val.toDouble() - days) * 24 * 60 * 60);
			d = d.addDays(days);
			d = d.addSecs(secs);
		} else if (timeUnit == GeoDataNetcdfImporterDateSelectDialog::TimeUnit::Hours) {
			d = d.addSecs(val.toDouble() * 60 * 60);
		} else if (timeUnit == GeoDataNetcdfImporterDateSelectDialog::TimeUnit::Minutes) {
			d = d.addSecs(val.toDouble() * 60);
		} else if (timeUnit == GeoDataNetcdfImporterDateSelectDialog::TimeUnit::Seconds) {
			d = d.addSecs(val.toDouble());
		}
		d.setTimeZone(timeZone);

		ret.push_back(d.toTime_t());
	}

	return ret;
}
