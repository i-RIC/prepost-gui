#ifndef IRICPYTHON_GEODATA_H
#define IRICPYTHON_GEODATA_H

#include "iricpython_api.h"

class PreProcessorGeoDataDataItemI;

namespace iRICPython {

class IRICPYTHON_API GeoData
{
public:
	GeoData(PreProcessorGeoDataDataItemI* dataItem);

	PreProcessorGeoDataDataItemI* geoDataItem() const;

private:
	PreProcessorGeoDataDataItemI* m_geoDataItem;
};

} // iRICPython

#endif // GEODATA_H
