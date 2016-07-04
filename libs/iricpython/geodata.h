#ifndef IRICPYTHON_GEODATA_H
#define IRICPYTHON_GEODATA_H

#include "iricpython_api.h"

class PreProcessorGeoDataDataItemInterface;

namespace iRICPython {

class IRICPYTHON_API GeoData
{
public:
	GeoData(PreProcessorGeoDataDataItemInterface* dataItem);

	PreProcessorGeoDataDataItemInterface* geoDataItem() const;

private:
	PreProcessorGeoDataDataItemInterface* m_geoDataItem;
};

} // iRICPython

#endif // GEODATA_H
