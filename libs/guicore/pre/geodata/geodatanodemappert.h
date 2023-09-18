#ifndef GEODATANODEMAPPERT_H
#define GEODATANODEMAPPERT_H

#include "geodatamappert.h"

template <class V, class DA>
class GeoDataNodeMapperT : public GeoDataMapperT<V, DA>
{
public:
	GeoDataNodeMapperT(const QString& caption, QObject* parent);
	~GeoDataNodeMapperT();
};

#include "private/geodatanodemappert_detail.h"

#endif // GEODATANODEMAPPERT_H
