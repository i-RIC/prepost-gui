#ifndef GEODATAPOLYLINEGROUPCELLMAPPERT_DETAIL_H
#define GEODATAPOLYLINEGROUPCELLMAPPERT_DETAIL_H

#include "../geodatapolylinegroupcellmappert.h"

#include <guibase/vtktool/vtkpointsutil.h>

template <class V, class DA>
GeoDataPolyLineGroupCellMapperT<V, DA>::GeoDataPolyLineGroupCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("PolyLineGroup cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolyLineGroupCellMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataPolyLineGroupNodeMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPolyLineGroupCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{}

template <class V, class DA>
void GeoDataPolyLineGroupCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYLINEGROUPCELLMAPPERT_DETAIL_H
