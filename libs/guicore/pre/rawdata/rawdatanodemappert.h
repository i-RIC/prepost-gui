#ifndef RAWDATANODEMAPPERT_H
#define RAWDATANODEMAPPERT_H

#include "rawdatamappert.h"
#include "../gridcond/base/gridattributenodecontainert.h"

template <class V, class DA>
class RawDataNodeMapperT : public RawDataMapperT<V>
{

public:
	RawDataNodeMapperT(RawDataCreator* parent)
		: RawDataMapperT<V>(parent)
	{}

protected:
	GridAttributeNodeContainerT<V, DA>* container() {
		return dynamic_cast<GridAttributeNodeContainerT<V, DA>* >(RawDataMapperT<V>::container());
	}
};

#endif // RAWDATANODEMAPPERT_H
