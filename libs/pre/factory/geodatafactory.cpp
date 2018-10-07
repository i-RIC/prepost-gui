#include "geodatafactory.h"

#include <guicore/pre/geodata/geodatacreator.h>
#include <geodata/netcdf/geodatanetcdfintegercreator.h>
#include <geodata/netcdf/geodatanetcdfrealcreator.h>
#include <geodata/pointmap/geodatapointmaprealcreator.h>
#include <geodata/polygon/geodatapolygonintegercreator.h>
#include <geodata/polygon/geodatapolygonrealcreator.h>
#include <geodata/polyline/geodatapolylineintegercreator.h>
#include <geodata/polyline/geodatapolylinerealcreator.h>
#include <geodata/riversurvey/geodatariversurveycreator.h>

GeoDataFactory* GeoDataFactory::m_instance = nullptr;

GeoDataFactory::GeoDataFactory() :
	QObject(nullptr)
{
	// @todo add GeoDataCreator instances into
	// m_creators here when you added new
	// classes those inherits GeoDataCreator.
	m_creators.push_back(new GeoDataRiverSurveyCreator());
	m_creators.push_back(new GeoDataPointmapRealCreator());
	m_creators.push_back(new GeoDataPolygonIntegerCreator());
	m_creators.push_back(new GeoDataPolygonRealCreator());
	m_creators.push_back(new GeoDataPolyLineIntegerCreator());
	m_creators.push_back(new GeoDataPolyLineRealCreator());
	m_creators.push_back(new GeoDataNetcdfIntegerCreator());
	m_creators.push_back(new GeoDataNetcdfRealCreator());
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
