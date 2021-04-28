#include "geodatafactory.h"

#include <guicore/pre/geodata/geodatacreator.h>
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

GeoDataFactory* GeoDataFactory::m_instance = nullptr;

GeoDataFactory::GeoDataFactory() :
	GeoDataFactoryInterface {}
{
	// @todo add GeoDataCreator instances into
	// m_creators here when you added new
	// classes those inherits GeoDataCreator.

	// cross-section data, point cloud data, rasterdata
	m_creators.push_back(new GeoDataPointmapRealCreator());
	m_creators.push_back(new GeoDataPointmapTINCreator());
	m_creators.push_back(new GeoDataRiverSurveyCreator());
	m_creators.push_back(new GeoDataNetcdfIntegerCreator());
	m_creators.push_back(new GeoDataNetcdfRealCreator());
	m_creators.push_back(new GeoDataNetcdfTimeSeriesIntegerCreator());
	m_creators.push_back(new GeoDataNetcdfTimeSeriesRealCreator());

	// vector data
	m_creators.push_back(new GeoDataPolygonIntegerCreator());
	m_creators.push_back(new GeoDataPolygonRealCreator());
	m_creators.push_back(new GeoDataPolygonGroupIntegerCreator());
	m_creators.push_back(new GeoDataPolygonGroupRealCreator());

	m_creators.push_back(new GeoDataPolyLineIntegerCreator());
	m_creators.push_back(new GeoDataPolyLineRealCreator());
	m_creators.push_back(new GeoDataPolyLineGroupIntegerCreator);
	m_creators.push_back(new GeoDataPolyLineGroupRealCreator);

	m_creators.push_back(new GeoDataPointRealCreator());
	m_creators.push_back(new GeoDataPointGroupIntegerCreator());
	m_creators.push_back(new GeoDataPointGroupRealCreator());
}

GeoDataFactory& GeoDataFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new GeoDataFactory();
	}

	return *m_instance;
}

const std::vector<GeoDataCreator*>& GeoDataFactory::creators() const
{
	return m_creators;
}

std::vector<GeoDataCreator*> GeoDataFactory::compatibleCreators(SolverDefinitionGridAttribute* condition) const
{
	std::vector<GeoDataCreator*> ret;
	for (GeoDataCreator* c : m_creators) {
		if (! c->isCompatibleWith(condition)) {continue;}

		ret.push_back(c);
	}
	return ret;
}

GeoData* GeoDataFactory::restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const
{
	for (GeoDataCreator* c : m_creators) {
		GeoData* ret = c->restore(node, item, cond);
		if (ret) {return ret;}
	}
	return nullptr;
}
