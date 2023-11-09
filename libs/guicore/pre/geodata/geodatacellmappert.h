#ifndef GEODATACELLMAPPERT_H
#define GEODATACELLMAPPERT_H

#include "geodatamappert.h"

template <class V, class DA>
class GeoDataCellMapperT : public GeoDataMapperT<V, DA>
{
public:
	GeoDataCellMapperT(const QString& caption, QObject* parent);
	~GeoDataCellMapperT();
};

#include "private/geodatacellmappert_detail.h"

#endif // GEODATACELLMAPPERT_H
