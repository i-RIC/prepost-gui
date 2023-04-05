#include "geodatafactorysetup.h"

#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodatafactory.h>
#include <geodata/netcdf/geodatanetcdfintegercreator.h>
#include <geodata/netcdf/geodatanetcdfrealcreator.h>
#include <geodata/netcdf/geodatanetcdftimeseriesintegercreator.h>
#include <geodata/netcdf/geodatanetcdftimeseriesrealcreator.h>
#include <geodata/point/geodatapointrealcreator.h>
#include <geodata/pointgroup/geodatapointgroupintegercreator.h>
#include <geodata/pointgroup/geodatapointgrouprealcreator.h>
#include <geodata/pointmap/geodatapointmaprealcreator.h>
#include <geodata/pointmap/geodatapointmaptincreator.h>
#include <geodata/polygon/geodatapolygonintegercreator.h>
#include <geodata/polygon/geodatapolygonrealcreator.h>
#include <geodata/polygongroup/geodatapolygongroupintegercreator.h>
#include <geodata/polygongroup/geodatapolygongrouprealcreator.h>
#include <geodata/polyline/geodatapolylineintegercreator.h>
#include <geodata/polyline/geodatapolylinerealcreator.h>
#include <geodata/polylinegroup/geodatapolylinegroupintegercreator.h>
#include <geodata/polylinegroup/geodatapolylinegrouprealcreator.h>
#include <geodata/riversurvey/geodatariversurveycreator.h>

GeoDataFactorySetup::GeoDataFactorySetup()
{}

void GeoDataFactorySetup::setup()
{
	auto& factory = GeoDataFactory::instance();
	if (factory.isSetup()) {return;}

	// cross-section data, point cloud data, rasterdata
	factory.addCreator(new GeoDataPointmapRealCreator());
	factory.addCreator(new GeoDataRiverSurveyCreator());
	factory.addCreator(new GeoDataNetcdfIntegerCreator());
	factory.addCreator(new GeoDataNetcdfRealCreator());
	factory.addCreator(new GeoDataNetcdfTimeSeriesIntegerCreator());
	factory.addCreator(new GeoDataNetcdfTimeSeriesRealCreator());

	// vector data
	factory.addCreator(new GeoDataPolygonIntegerCreator());
	factory.addCreator(new GeoDataPolygonRealCreator());
	factory.addCreator(new GeoDataPolygonGroupIntegerCreator());
	factory.addCreator(new GeoDataPolygonGroupRealCreator());

	factory.addCreator(new GeoDataPolyLineIntegerCreator());
	factory.addCreator(new GeoDataPolyLineRealCreator());
	factory.addCreator(new GeoDataPolyLineGroupIntegerCreator);
	factory.addCreator(new GeoDataPolyLineGroupRealCreator);

	factory.addCreator(new GeoDataPointRealCreator());
	factory.addCreator(new GeoDataPointGroupIntegerCreator());
	factory.addCreator(new GeoDataPointGroupRealCreator());

	factory.addCreator(new GeoDataPointmapTINCreator());

	factory.setIsSetup(true);
}
