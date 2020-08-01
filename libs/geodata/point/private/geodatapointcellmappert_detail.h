#ifndef GEODATAPOINTCELLMAPPERT_DETAIL_H
#define GEODATAPOINTCELLMAPPERT_DETAIL_H

#include "../geodatapointcellmappert.h"

#include <vtkCell.h>

template <class V, class DA>
GeoDataPointCellMapperT<V, DA>::GeoDataPointCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("Point cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPointCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPointCellMapperSetting* s = new GeoDataPointCellMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPointCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{}

template <class V, class DA>
void GeoDataPointCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAPOINTCELLMAPPERT_DETAIL_H
