#ifndef GEODATACELLMAPPERT_DETAIL_H
#define GEODATACELLMAPPERT_DETAIL_H

#include "../geodatacellmappert.h"

template <class V, class DA>
GeoDataCellMapperT<V, DA>::GeoDataCellMapperT(const QString& caption, QObject* parent) :
	GeoDataMapperT<V, DA> (caption, parent)
{}

template <class V, class DA>
GeoDataCellMapperT<V, DA>::~GeoDataCellMapperT()
{}

#endif // GEODATACELLMAPPERT_DETAIL_H
