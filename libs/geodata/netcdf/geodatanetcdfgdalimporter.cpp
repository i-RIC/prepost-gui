#include "geodatanetcdf.h"
#include "geodatanetcdfgdalimporter.h"

#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <cs/gdalutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QApplication>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QProcess>

#include <gdal_priv.h>
#include <ogr_spatialref.h>

GeoDataNetcdfGdalImporter::GeoDataNetcdfGdalImporter(GeoDataCreator* creator) :
	GeoDataImporter {"gdal", tr("GDAL"), creator}
{}

GeoDataNetcdfGdalImporter::~GeoDataNetcdfGdalImporter()
{}

const QStringList GeoDataNetcdfGdalImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("GeoTiff files(*.tif)"));
	return ret;
}

const QStringList GeoDataNetcdfGdalImporter::acceptableExtensions()
{
	QStringList ret;
	ret.append("tif");
	return ret;
}

bool GeoDataNetcdfGdalImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	// investigate the condition.
	const auto& dims = condition->dimensions();
	if (dims.size() != 0) {
		QMessageBox::warning(w, tr("Warning"), tr("GDAL data files can be imported for grid conditions without dimensions."));
		return false;
	}

	GDALAllRegister();

	m_dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (m_dataset == NULL) {
		// failed opening.
		return false;
	}

	*count = m_dataset->GetRasterCount();
	m_count = *count;

	m_sr = new OGRSpatialReference();
	std::string geoRef = m_dataset->GetProjectionRef();
	char* geoRefPointer = const_cast<char*>(geoRef.c_str());
	OGRErr err = m_sr->importFromWkt(&geoRefPointer);
	if (err != OGRERR_NONE) {return false;}

	char* projDef;

	m_sr->exportToProj4(&projDef);
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

	CPLFree(projDef);

	err = m_dataset->GetGeoTransform(m_transform);
	if (err != CE_None) {
		return false;
	}

	return true;
}

bool GeoDataNetcdfGdalImporter::importData(GeoData* data, int index, QWidget* /*w*/)
{
	GeoDataNetcdf* netcdf = dynamic_cast<GeoDataNetcdf*>(data);

	GDALRasterBand* band = m_dataset->GetRasterBand(index + 1);
	int xsize, ysize;
	xsize = band->GetXSize();
	ysize = band->GetYSize();
	netcdf->m_coordinateSystemType = GeoDataNetcdf::XY;
	netcdf->m_coordinateSystemName = m_coordinateSystem->name();
	netcdf->m_xValues.clear();
	for (int i = 0; i < xsize; ++i) {
		netcdf->m_xValues.push_back(m_transform[0] + m_transform[1] * i);
	}
	netcdf->m_yValues.clear();
	for (int i = 0; i < ysize; ++i) {
		netcdf->m_yValues.push_back(m_transform[3] + m_transform[5] * (ysize - 1 - i));
	}

	netcdf->m_lonValues.clear();
	for (int j = 0; j < netcdf->m_yValues.size(); ++j) {
		double y = netcdf->m_yValues.at(j);
		for (int i = 0; i < netcdf->m_xValues.size(); ++i) {
			double x = netcdf->m_xValues.at(i);
			double lon, lat;
			m_coordinateSystem->mapGridToGeo(x, y, &lon, &lat);
			netcdf->m_lonValues.push_back(lon);
			netcdf->m_latValues.push_back(lat);
		}
	}

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

	if (index == m_count - 1) {
		GDALClose(m_dataset);
	}
	netcdf->updateShapeData();
	netcdf->handleDimensionCurrentIndexChange(0, 0);

	return true;
}
