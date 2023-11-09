#ifndef GEODATAPOLYLINEGROUPJFACEMAPPERT_H
#define GEODATAPOLYLINEGROUPJFACEMAPPERT_H

#include "geodatapolylinegroup.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatajfacemappert.h>
#include <misc/rangecontainert.h>

template <typename V, typename DA>
class GeoDataPolyLineGroupJFaceMapperT : public GeoDataJFaceMapperT<V, DA>
{
public:
	GeoDataPolyLineGroupJFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapolylinegroupjfacemappert_detail.h"

#endif // GEODATAPOLYLINEGROUPJFACEMAPPERT_H
