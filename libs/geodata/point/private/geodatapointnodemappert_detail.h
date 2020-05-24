#ifndef GEODATAPOINTNODEMAPPERT_DETAIL_H
#define GEODATAPOINTNODEMAPPERT_DETAIL_H

#include "../geodatapointnodemappert.h"

template <class V, class DA>
GeoDataPointNodeMapperT<V, DA>::GeoDataPointNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("Point node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPointNodeMapperT<V, DA>::initialize(bool* /*boolMap*/)
{
	GeoDataPointNodeMapperSetting* s = new GeoDataPointNodeMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPointNodeMapperT<V, DA>::map(bool* /*boolMap*/, GeoDataMapperSettingI* /*s*/)
{}

template <class V, class DA>
void GeoDataPointNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOINTNODEMAPPERT_DETAIL_H
