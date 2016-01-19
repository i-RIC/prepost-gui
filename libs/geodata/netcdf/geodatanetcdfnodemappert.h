#ifndef GEODATANETCDFNODEMAPPERT_H
#define GEODATANETCDFNODEMAPPERT_H

#include "geodatanetcdft.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/doublemappingsetting.h>

template <class V, class DA>
class GeoDataNetcdfNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataNetcdfNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatanetcdfnodemappert_detail.h"

#endif // GEODATANETCDFNODEMAPPERT_H
