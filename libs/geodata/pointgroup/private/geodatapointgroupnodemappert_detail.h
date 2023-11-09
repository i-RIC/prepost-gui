#ifndef GEODATAPOINTGROUPNODEMAPPERT_DETAIL_H
#define GEODATAPOINTGROUPNODEMAPPERT_DETAIL_H

#include "../geodatapointgroupnodemappert.h"

template <class V, class DA>
GeoDataPointGroupNodeMapperT<V, DA>::GeoDataPointGroupNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("PointGroup node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPointGroupNodeMapperT<V, DA>::initialize(bool* /*boolMap*/)
{
	auto s = new GeoDataPointGroupNodeMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPointGroupNodeMapperT<V, DA>::map(bool* /*boolMap*/, GeoDataMapperSettingI* /*s*/)
{}

template <class V, class DA>
void GeoDataPointGroupNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOINTGROUPNODEMAPPERT_DETAIL_H
