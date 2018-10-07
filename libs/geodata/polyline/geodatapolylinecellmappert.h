#ifndef GEODATAPOLYLINECELLMAPPERT_H
#define GEODATAPOLYLINECELLMAPPERT_H

#include "geodatapolyline.h"

#include <guicore/pre/geodata/geodatacellmappert.h>

template <class V, class DA>
class GeoDataPolyLineCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPolyLineCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapolylinecellmappert_detail.h"

#endif // GEODATAPOLYLINECELLMAPPERT_H
