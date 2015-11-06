#include "hydraulicdataimporterfactory.h"

#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationimporter.h>

HydraulicDataImporterFactory* HydraulicDataImporterFactory::m_instance = nullptr;

HydraulicDataImporterFactory::HydraulicDataImporterFactory() :
	QObject(nullptr)
{
	m_importers.append(new HydraulicDataRiverSurveyWaterElevationImporter());
}

HydraulicDataImporterFactory& HydraulicDataImporterFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new HydraulicDataImporterFactory();
	}
	return *m_instance;
}

const QList<HydraulicDataImporter*> HydraulicDataImporterFactory::importers() const
{
	return m_importers;
}
