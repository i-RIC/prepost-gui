#ifndef GEODATAPOLYLINENODEMAPPERT_DETAIL_H
#define GEODATAPOLYLINENODEMAPPERT_DETAIL_H

#include "../geodatapolylinenodemappert.h"

#include <vtkPolyData.h>

template <class V, class DA>
GeoDataPolyLineNodeMapperT<V, DA>::GeoDataPolyLineNodeMapperT(GeoDataCreator* parent) :
	GeoDataNodeMapperT<V, DA> ("PolyLine node mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolyLineNodeMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPolyLineNodeMapperSetting* s = new GeoDataPolyLineNodeMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPolyLineNodeMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
}

template <class V, class DA>

void GeoDataPolyLineNodeMapperT<V, DA>::terminate(GeoDataMapperSettingI* setting)
{
	delete setting;
}

#endif // GEODATAPOLYLINENODEMAPPERT_DETAIL_H
