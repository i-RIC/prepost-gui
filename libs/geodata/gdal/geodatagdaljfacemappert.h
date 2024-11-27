#ifndef GEODATAGDALJFACEMAPPERT_H
#define GEODATAGDALJFACEMAPPERT_H

#include "geodatagdalt.h"

#include <guicore/pre/geodata/geodatajfacemappert.h>

class GeoDataMapperSettingI;

template <class V, class DA>
class GeoDataGdalJFaceMapperT : public GeoDataJFaceMapperT<V, DA>
{
public:
	GeoDataGdalJFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatagdaljfacemappert_detail.h"

#endif // GEODATAGDALJFACEMAPPERT_H
