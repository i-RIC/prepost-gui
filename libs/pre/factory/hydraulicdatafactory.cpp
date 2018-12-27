#include "hydraulicdatafactory.h"

#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationcreator.h>

HydraulicDataFactory* HydraulicDataFactory::m_instance = nullptr;

HydraulicDataFactory::HydraulicDataFactory() :
	QObject(nullptr)
{
	m_creators.push_back(new HydraulicDataRiverSurveyWaterElevationCreator());
}

const std::vector<HydraulicDataCreator*>& HydraulicDataFactory::creators() const
{
	return m_creators;
}

HydraulicDataFactory& HydraulicDataFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new HydraulicDataFactory();
	}

	return *m_instance;
}
