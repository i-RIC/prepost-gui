#ifndef IRICPYTHON_PRE_H
#define IRICPYTHON_PRE_H

#include "iricpython_api.h"

#include <vector>

class PreProcessorWindow;

namespace iRICPython {

class Application;
class GeoDataGroup;
class GridType;

class IRICPYTHON_API Pre
{
public:
	Pre(const Application* app);
	~Pre();

	GridType* gridType(const char* name) const;
	std::vector<GridType*> gridTypes() const;

	GeoDataGroup* geoDataGroup(const char* name) const;
	std::vector<GeoDataGroup*> geoDataGroups() const;

	void mapAllGeoData();
	void mapGeoData(const char* name);

	void clearGridTypes();

private:
	GridType* primaryGridType() const;
	PreProcessorWindow* preWindow() const;
	void buildGridTypes() const;

	const Application* m_app;

	mutable bool m_gridTypesBuilt;
	mutable std::vector<GridType*> m_gridTypes;
};

} // iRICPython

#endif // IRICPYTHON_PRE_H
