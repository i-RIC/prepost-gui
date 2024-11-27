#ifndef GEODATAGDALIFACEMAPPERT_H
#define GEODATAGDALIFACEMAPPERT_H

#include "geodatagdalt.h"

#include <guicore/pre/geodata/geodataifacemappert.h>

class GeoDataMapperSettingI;

template <class V, class DA>
class GeoDataGdalIFaceMapperT : public GeoDataIFaceMapperT<V, DA>
{
public:
	GeoDataGdalIFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatagdalifacemappert_detail.h"

#endif // GEODATAGDALIFACEMAPPERT_H
