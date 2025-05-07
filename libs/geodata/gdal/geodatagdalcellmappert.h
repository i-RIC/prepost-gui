#ifndef GEODATAGDALCELLMAPPERT_H
#define GEODATAGDALCELLMAPPERT_H

#include "geodatagdalt.h"

#include <guicore/pre/geodata/geodatacellmappert.h>

class GeoDataMapperSettingI;

template <class V, class DA>
class GeoDataGdalCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataGdalCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatagdalcellmappert_detail.h"

#endif // GEODATAGDALCELLMAPPERT_H
