#ifndef GEODATANODEMAPPERT_DETAIL_H
#define GEODATANODEMAPPERT_DETAIL_H

#include "../geodatanodemappert.h"

template <class V, class DA>
GeoDataNodeMapperT<V, DA>::GeoDataNodeMapperT(const QString& caption, QObject* parent) :
	GeoDataMapperT<V, DA> (caption, parent)
{}

template <class V, class DA>
GeoDataNodeMapperT<V, DA>::~GeoDataNodeMapperT()
{}

#endif // GEODATANODEMAPPERT_DETAIL_H
