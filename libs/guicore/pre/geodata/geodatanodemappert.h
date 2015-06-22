#ifndef GEODATANODEMAPPERT_H
#define GEODATANODEMAPPERT_H

#include "geodatamappert.h"
#include "../gridcond/base/gridattributenodecontainert.h"

template <class V, class DA>
class GeoDataNodeMapperT : public GeoDataMapperT<V>
{

public:
	GeoDataNodeMapperT(GeoDataCreator* parent)
		: GeoDataMapperT<V>(parent)
	{}

protected:
	GridAttributeNodeContainerT<V, DA>* container() {
		return dynamic_cast<GridAttributeNodeContainerT<V, DA>* >(GeoDataMapperT<V>::container());
	}
};

#endif // GEODATANODEMAPPERT_H
