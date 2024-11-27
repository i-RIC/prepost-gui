#ifndef GEODATAGDALNODEMAPPERT_H
#define GEODATAGDALNODEMAPPERT_H

#include "geodatagdalt.h"

#include <guicore/pre/geodata/geodatanodemappert.h>

class GeoDataMapperSettingI;

template <class V, class DA>
class GeoDataGdalNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataGdalNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatagdalnodemappert_detail.h"

#endif // GEODATAGDALNODEMAPPERT_H
