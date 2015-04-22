#include "rawdatafactory.h"
#include <guicore/pre/rawdata/rawdatacreator.h>
#include <rawdata/polygon/rawdatapolygonintegercreator.h>
#include <rawdata/polygon/rawdatapolygonrealcreator.h>
#include <rawdata/pointmap/rawdatapointmaprealcreator.h>
#include <rawdata/riversurvey/rawdatariversurveycreator.h>
#include <rawdata/netcdf/rawdatanetcdfintegercreator.h>
#include <rawdata/netcdf/rawdatanetcdfrealcreator.h>

RawDataFactory* RawDataFactory::m_instance = 0;

RawDataFactory::RawDataFactory()
	: QObject(0)
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

const QList<RawDataCreator*> RawDataFactory::compatibleCreators(SolverDefinitionGridRelatedCondition* condition) const
{
	QList<RawDataCreator*> ret;
	for (auto it = m_creators.begin(); it != m_creators.end(); ++it){
		if ((*it)->isCompatibleWith(condition)){
			ret.append(*it);
		}
	}
	return ret;
}

RawData* RawDataFactory::restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridRelatedCondition* cond) const
{
	for (auto it = m_creators.begin(); it != m_creators.end(); ++it){
		RawData* ret = (*it)->restore(node, item, cond);
		if (ret){return ret;}
	}
	return 0;
}
