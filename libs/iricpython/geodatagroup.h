#ifndef IRICPYTHON_GEODATAGROUP_H
#define IRICPYTHON_GEODATAGROUP_H

#include "iricpython_api.h"

#include <string>

class PreProcessorGeoDataGroupDataItem;

namespace iRICPython {

class GeoData;
class GeoDataImporter;

class IRICPYTHON_API GeoDataGroup
{
public:
	GeoDataGroup(PreProcessorGeoDataGroupDataItem* item);
	~GeoDataGroup();

	std::string name() const;

	GeoDataImporter* importer(const std::string& name) const;
	void add(GeoData* data);

private:
	PreProcessorGeoDataGroupDataItem* m_item;
};

} // iRICPython

#endif // IRICPYTHON_GEODATAGROUP_H
