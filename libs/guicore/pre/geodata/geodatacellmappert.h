#ifndef GEODATACELLMAPPERT_H
#define GEODATACELLMAPPERT_H

#include "geodatamappert.h"
#include "../gridcond/base/gridattributecellcontainert.h"

template <class V, class DA>
class GeoDataCellMapperT : public GeoDataMapperT<V>
{
public:
	GeoDataCellMapperT(const QString& caption, GeoDataCreator* parent);
	virtual ~GeoDataCellMapperT();

protected:
	GridAttributeCellContainerT<V, DA>* container() const;
};

#include "private/geodatacellmappert_detail.h"

#endif // GEODATACELLMAPPERT_H
