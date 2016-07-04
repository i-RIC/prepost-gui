#ifndef IRICPYTHON_GEODATAIMPORTER_H
#define IRICPYTHON_GEODATAIMPORTER_H

#include "iricpython_api.h"

#include <string>

class GeoDataImporter;
class PreProcessorGeoDataGroupDataItemInterface;
class SolverDefinitionGridAttribute;

namespace iRICPython {

class GeoData;

class IRICPYTHON_API GeoDataImporter
{
public:
	GeoDataImporter(::GeoDataImporter* importer, SolverDefinitionGridAttribute* att, PreProcessorGeoDataGroupDataItemInterface *groupItem);

	GeoData* importGeoData(const std::string& filename);

private:
	::GeoDataImporter* m_importer;
	SolverDefinitionGridAttribute* m_att;
	PreProcessorGeoDataGroupDataItemInterface* m_groupItem;
};

} // iRICPython

#endif // IRICPYTHON_GEODATAIMPORTER_H
