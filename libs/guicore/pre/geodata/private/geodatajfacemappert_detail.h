#ifndef GEODATAJFACEMAPPERT_DETAIL_H
#define GEODATAJFACEMAPPERT_DETAIL_H

#include "../geodatajfacemappert.h"

template <class V, class DA>
GeoDataJFaceMapperT<V, DA>::GeoDataJFaceMapperT(const QString& caption, QObject* parent) :
	GeoDataMapperT<V, DA> (caption, parent)
{}

template <class V, class DA>
GeoDataJFaceMapperT<V, DA>::~GeoDataJFaceMapperT()
{}

#endif // GEODATAJFACEMAPPERT_DETAIL_H
