#ifndef GEODATAIFACEMAPPERT_H
#define GEODATAIFACEMAPPERT_H

#include "geodatamappert.h"

template <class V, class DA>
class GeoDataIFaceMapperT : public GeoDataMapperT<V, DA>
{
public:
	GeoDataIFaceMapperT(const QString& caption, QObject* parent);
	~GeoDataIFaceMapperT();
};

#include "private/geodataifacemappert_detail.h"

#endif // GEODATAIFACEMAPPERT_H
