#include "hydraulicdataimporterfactory.h"

#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationimporter.h>

HydraulicDataImporterFactory* HydraulicDataImporterFactory::m_instance = nullptr;

HydraulicDataImporterFactory::HydraulicDataImporterFactory()
	: QObject(nullptr)
{
	m_importers.append(new HydraulicDataRiverSurveyWaterElevationImporter());
}
