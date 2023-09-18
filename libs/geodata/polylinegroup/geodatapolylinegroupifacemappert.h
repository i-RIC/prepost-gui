#ifndef GEODATAPOLYLINEGROUPIFACEMAPPERT_H
#define GEODATAPOLYLINEGROUPIFACEMAPPERT_H

#include "geodatapolylinegroup.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodataifacemappert.h>
#include <misc/rangecontainert.h>

template <typename V, typename DA>
class GeoDataPolyLineGroupIFaceMapperT : public GeoDataIFaceMapperT<V, DA>
{
public:
	GeoDataPolyLineGroupIFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapolylinegroupifacemappert_detail.h"

#endif // GEODATAPOLYLINEGROUPIFACEMAPPERT_H
