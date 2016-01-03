#ifndef GEODATACELLMAPPERT_DETAIL_H
#define GEODATACELLMAPPERT_DETAIL_H

#include "../geodatacellmappert.h"

template <class V, class DA>
GeoDataCellMapperT<V, DA>::GeoDataCellMapperT(const QString& caption, GeoDataCreator* parent) :
	GeoDataMapperT<V> (caption, parent)
{}

template <class V, class DA>
GeoDataCellMapperT<V, DA>::~GeoDataCellMapperT()
{}

template <class V, class DA>
GridAttributeCellContainerT<V, DA>* GeoDataCellMapperT<V, DA>::container() const
{
	return dynamic_cast<GridAttributeCellContainerT<V, DA>* >(GeoDataMapperT<V>::container());
}

#endif // GEODATACELLMAPPERT_DETAIL_H

