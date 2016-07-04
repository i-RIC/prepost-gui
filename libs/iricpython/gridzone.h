#ifndef IRICPYTHON_GRIDZONE_H
#define IRICPYTHON_GRIDZONE_H

#include "iricpython_api.h"

class PreProcessorGridAndGridCreatingConditionDataItem;

namespace iRICPython {

class IRICPYTHON_API GridZone
{
public:
	GridZone(PreProcessorGridAndGridCreatingConditionDataItem* item);
	~GridZone();

	void mapAllGeoData();
	void mapGeoData(const char* name);

private:
	PreProcessorGridAndGridCreatingConditionDataItem* m_item;
};

} // iRICPython

#endif // IRICPYTHON_GRIDZONE_H
