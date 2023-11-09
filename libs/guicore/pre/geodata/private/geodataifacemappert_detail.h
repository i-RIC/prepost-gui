#ifndef GEODATAIFACEMAPPERT_DETAIL_H
#define GEODATAIFACEMAPPERT_DETAIL_H

#include "../geodataifacemappert.h"

template <class V, class DA>
GeoDataIFaceMapperT<V, DA>::GeoDataIFaceMapperT(const QString& caption, QObject* parent) :
	GeoDataMapperT<V, DA> (caption, parent)
{}

template <class V, class DA>
GeoDataIFaceMapperT<V, DA>::~GeoDataIFaceMapperT()
{}

#endif // GEODATAIFACEMAPPERT_DETAIL_H
