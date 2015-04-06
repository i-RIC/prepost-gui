#include "hydraulicdataimporterfactory.h"
#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationimporter.h>

HydraulicDataImporterFactory* HydraulicDataImporterFactory::m_instance = 0;

HydraulicDataImporterFactory::HydraulicDataImporterFactory()
	: QObject(0)
{
	m_importers.append(new HydraulicDataRiverSurveyWaterElevationImporter());
}
