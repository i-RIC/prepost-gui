#include "geodatanetcdf.h"
#include "geodatanetcdfgdalexporter.h"

#include <misc/stringtool.h>
#include <cs/coordinatesystem.h>
#include <cs/coordinatesystembuilder.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>

#include <QMessageBox>

#include <gdal_priv.h>
#include <netcdf.h>
#include <ogr_spatialref.h>

GeoDataNetcdfGdalExporter::GeoDataNetcdfGdalExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("GDAL"), creator)
{}

GeoDataNetcdfGdalExporter::~GeoDataNetcdfGdalExporter()
{}

bool GeoDataNetcdfGdalExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	GeoDataNetcdf* netcdf = dynamic_cast<GeoDataNetcdf*> (data);
	if (! netcdf->geoTransformExists()) {
		QMessageBox::critical(w, tr("Error"), tr("This data cannot be exported. It seems that it was not imported from *.tif or *.asc."));
		return false;
	}

	if (netcdf->dimensions()->containers().size() > 0) {
		QMessageBox::critical(w, tr("Error"), tr("This data cannot be exported. Data with \"Time\" dimension cannot be exported to *.tif or *.asc."));
		return false;
	}

	GDALAllRegister();
	char** papszOptions = nullptr;

	auto driverManager = GetGDALDriverManager();
	GDALDriver* memDriver = driverManager->GetDriverByName("MEM");
	GDALDriver* driver = nullptr;

	if (selectedFilter == tr("ArcInfo ASCII files(*.asc)")) {
		driver = driverManager->GetDriverByName("AAIGrid");
	} else {
		driver = driverManager->GetDriverByName("GTiff");
	}

	std::string fname = iRIC::toStr(filename);
	GDALDataset* dataset = memDriver->Create("dummy", netcdf->xSize(), netcdf->ySize(), 1, gdalDataType(), papszOptions);
	dataset->SetGeoTransform(netcdf->geoTransform());

	auto csName = netcdf->coordinateSystemName();

	CoordinateSystem* cs = netcdf->projectData()->mainWindow()->coordinateSystemBuilder()->system(csName);
	if (cs != nullptr) {
		OGRSpatialReference SRC;
		SRC.importFromProj4(iRIC::toStr(cs->proj4PlaneStr()).c_str());
		char* wktStr;
		SRC.exportToWkt(&wktStr);
		dataset->SetProjection(wktStr);
	} else {
		OGRSpatialReference SRC;
		SRC.importFromProj4(iRIC::toStr(csName).c_str());
		char* wktStr;
		SRC.exportToWkt(&wktStr);
		dataset->SetProjection(wktStr);
	}

	GDALRasterBand* band = dataset->GetRasterBand(1);

	int ncid, varid, ret;

	ret = nc_open(iRIC::toStr(netcdf->filename()).c_str(), NC_NOWRITE, &ncid);
	size_t starts[2], ends[2];
	starts[0] = 0;
	starts[1] = 0;
	ends[0] = netcdf->ySize();
	ends[1] = netcdf->xSize();
	netcdf->getValueVarId(ncid, &varid);

	copyData(netcdf, ncid, varid, starts, ends, band);
	nc_close(ncid);

	auto dataset2 = driver->CreateCopy(fname.c_str(), dataset, FALSE, nullptr, nullptr, nullptr);

	delete dataset2;
	delete dataset;

	return true;
}

const QStringList GeoDataNetcdfGdalExporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("GeoTiff files(*.tif *.tiff)"));
	ret.append(tr("ArcInfo ASCII files(*.asc)"));
	return ret;
}
