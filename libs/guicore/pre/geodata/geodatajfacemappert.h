#ifndef GEODATAJFACEMAPPERT_H
#define GEODATAJFACEMAPPERT_H

#include "geodatamappert.h"

template <class V, class DA>
class GeoDataJFaceMapperT : public GeoDataMapperT<V, DA>
{
public:
	GeoDataJFaceMapperT(const QString& caption, QObject* parent);
	~GeoDataJFaceMapperT();
};

#include "private/geodatajfacemappert_detail.h"

#endif // GEODATAJFACEMAPPERT_H
