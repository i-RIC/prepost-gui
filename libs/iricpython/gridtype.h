#ifndef IRICPYTHON_GRIDTYPE_H
#define IRICPYTHON_GRIDTYPE_H

#include "iricpython_api.h"

#include <vector>

class PreProcessorGridTypeDataItem;

namespace iRICPython {

class GeoDataGroup;
class GridZone;

class IRICPYTHON_API GridType
{
public:
	GridType(const PreProcessorGridTypeDataItem* gtItem);
	~GridType();

	std::string name() const;
	bool isPrimary() const;

	GeoDataGroup* geoDataGroup(const char* name) const;
	std::vector<GeoDataGroup*> geoDataGroups() const;

	void mapAllGeoData();
	void mapGeoData(const char* name);

	std::vector<GridZone*> gridZones() const;

private:
	void buildGeoDataGroups() const;
	void buildGridZones() const;

	const PreProcessorGridTypeDataItem* m_gtItem;

	mutable bool m_geoDataGroupsBuilt;
	mutable std::vector<GeoDataGroup*> m_geoDataGroups;

	mutable bool m_gridZonesBuilt;
	mutable std::vector<GridZone*> m_gridZones;
};

} // iRICPython

#endif // IRICPYTHON_GRIDTYPE_H
