#ifndef GEODATAPOLYLINEGROUPNODEMAPPERT_DETAIL_H
#define GEODATAPOLYLINEGROUPNODEMAPPERT_DETAIL_H

#include "../geodatapolylinegroupnodemappert.h"

template <class V, class DA>
GeoDataPolyLineGroupNodeMapperT<V, DA>::GeoDataPolyLineGroupNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("PolyLineGroup node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolyLineGroupNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	auto s = new GeoDataPolyLineGroupNodeMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPolyLineGroupNodeMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{}

template <class V, class DA>
void GeoDataPolyLineGroupNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYLINEGROUPNODEMAPPERT_DETAIL_H
