#ifndef RAWDATACELLMAPPERT_H
#define RAWDATACELLMAPPERT_H

#include "rawdatamappert.h"
#include "../gridcond/base/gridattributecellcontainert.h"

template <class V, class DA>
class RawDataCellMapperT : public RawDataMapperT<V>
{
public:
	RawDataCellMapperT(RawDataCreator* parent)
		: RawDataMapperT<V>(parent)
	{}
protected:
	GridAttributeCellContainerT<V, DA>* container() {
		return dynamic_cast<GridAttributeCellContainerT<V, DA>* >(RawDataMapperT<V>::container());
	}
};

#endif // RAWDATACELLMAPPERT_H
