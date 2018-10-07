#ifndef GEODATAPOLYLINECELLMAPPERT_DETAIL_H
#define GEODATAPOLYLINECELLMAPPERT_DETAIL_H

#include "../geodatapolylinecellmappert.h"
#include "../geodatapolylinecellmappersetting.h"

#include <vtkCell.h>

template <class V, class DA>
GeoDataPolyLineCellMapperT<V, DA>::GeoDataPolyLineCellMapperT(GeoDataCreator* parent) :
	GeoDataCellMapperT<V, DA> ("PolyLine cell mapper", parent)
{}

template <class V, class DA>
GeoDataMapperSettingI* GeoDataPolyLineCellMapperT<V, DA>::initialize(bool* boolMap)
{
	GeoDataPolyLineCellMapperSetting* s = new GeoDataPolyLineCellMapperSetting();
	return s;
}

template <class V, class DA>
void GeoDataPolyLineCellMapperT<V, DA>::map(bool* boolMap, GeoDataMapperSettingI* s)
{
}

template <class V, class DA>
void GeoDataPolyLineCellMapperT<V, DA>::terminate(GeoDataMapperSettingI* s)
{
	delete s;
}

#endif // GEODATAPOLYLINECELLMAPPERT_DETAIL_H
