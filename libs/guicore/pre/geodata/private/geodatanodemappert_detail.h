#ifndef GEODATANODEMAPPERT_DETAIL_H
#define GEODATANODEMAPPERT_DETAIL_H

#include "../geodatanodemappert.h"

template <class V, class DA>
GeoDataNodeMapperT<V, DA>::GeoDataNodeMapperT(const QString& caption, QObject* parent) :
	GeoDataMapperT<V> (caption, parent)
{}

template <class V, class DA>
GeoDataNodeMapperT<V, DA>::~GeoDataNodeMapperT()
{}

template <class V, class DA>
GridAttributeNodeContainerT<V, DA>* GeoDataNodeMapperT<V, DA>::container() const
{
	return dynamic_cast<GridAttributeNodeContainerT<V, DA>* >(GeoDataMapperT<V>::container());
}

#endif // GEODATANODEMAPPERT_DETAIL_H
