#ifndef GEODATACELLMAPPERT_H
#define GEODATACELLMAPPERT_H

#include "geodatamappert.h"
#include "../gridcond/base/gridattributecellcontainert.h"

template <class V, class DA>
class GeoDataCellMapperT : public GeoDataMapperT<V>
{

public:
	GeoDataCellMapperT(GeoDataCreator* parent)
		: GeoDataMapperT<V>(parent)
	{}

protected:
	GridAttributeCellContainerT<V, DA>* container() {
		return dynamic_cast<GridAttributeCellContainerT<V, DA>* >(GeoDataMapperT<V>::container());
	}
};

#endif // GEODATACELLMAPPERT_H
