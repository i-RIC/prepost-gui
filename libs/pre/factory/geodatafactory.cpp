#include "geodatafactory.h"

#include <guicore/pre/geodata/geodatacreator.h>
#include <geodata/netcdf/geodatanetcdfintegercreator.h>
#include <geodata/netcdf/geodatanetcdfrealcreator.h>
#include <geodata/pointmap/geodatapointmaprealcreator.h>
#include <geodata/polygon/geodatapolygonintegercreator.h>
#include <geodata/polygon/geodatapolygonrealcreator.h>
#include <geodata/riversurvey/geodatariversurveycreator.h>

GeoDataFactory* GeoDataFactory::m_instance = nullptr;

GeoDataFactory::GeoDataFactory()
	: QObject(nullptr)
{
	// @todo add GeoDataCreator instances into
	// m_creators here when you added new
	// classes those inherits GeoDataCreator.
	m_creators.append(new GeoDataRiverSurveyCreator());
	m_creators.append(new GeoDataPointmapRealCreator());
	m_creators.append(new GeoDataPolygonIntegerCreator());
	m_creators.append(new GeoDataPolygonRealCreator());
	m_creators.append(new GeoDataNetcdfIntegerCreator());
	m_creators.append(new GeoDataNetcdfRealCreator());
}

const QList<GeoDataCreator*> GeoDataFactory::compatibleCreators(SolverDefinitionGridAttribute* condition) const
{
	QList<GeoDataCreator*> ret;
	for (GeoDataCreator* c : m_creators) {
		if (c->isCompatibleWith(condition)) {
			ret.append(c);
		}
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
