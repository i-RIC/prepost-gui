#include "rawdatafactory.h"

#include <guicore/pre/rawdata/rawdatacreator.h>
#include <rawdata/netcdf/rawdatanetcdfintegercreator.h>
#include <rawdata/netcdf/rawdatanetcdfrealcreator.h>
#include <rawdata/pointmap/rawdatapointmaprealcreator.h>
#include <rawdata/polygon/rawdatapolygonintegercreator.h>
#include <rawdata/polygon/rawdatapolygonrealcreator.h>
#include <rawdata/riversurvey/rawdatariversurveycreator.h>

RawDataFactory* RawDataFactory::m_instance = nullptr;

RawDataFactory::RawDataFactory()
	: QObject(nullptr)
{
	// @todo add RawDataCreator instances into
	// m_creators here when you added new
	// classes those inherits RawDataCreator.
	m_creators.append(new RawDataRiverSurveyCreator());
	m_creators.append(new RawDataPointmapRealCreator());
	m_creators.append(new RawDataPolygonIntegerCreator());
	m_creators.append(new RawDataPolygonRealCreator());
	m_creators.append(new RawDataNetcdfIntegerCreator());
	m_creators.append(new RawDataNetcdfRealCreator());
}

const QList<RawDataCreator*> RawDataFactory::compatibleCreators(SolverDefinitionGridAttribute* condition) const
{
	QList<RawDataCreator*> ret;
	for (RawDataCreator* c : m_creators) {
		if (c->isCompatibleWith(condition)) {
			ret.append(c);
		}
	}
	return ret;
}

RawData* RawDataFactory::restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const
{
	for (RawDataCreator* c : m_creators) {
		RawData* ret = c->restore(node, item, cond);
		if (ret) {return ret;}
	}
	return nullptr;
}
