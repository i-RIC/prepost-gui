#ifndef RAWDATACELLMAPPERT_H
#define RAWDATACELLMAPPERT_H

#include "rawdatamappert.h"
#include "../gridcond/base/gridrelatedconditioncellcontainert.h"

template <class V, class DA>
class RawDataCellMapperT : public RawDataMapperT<V>
{
public:
	RawDataCellMapperT(RawDataCreator* parent)
		: RawDataMapperT<V>(parent)
	{}
protected:
	GridRelatedConditionCellContainerT<V, DA> * container(){
		return dynamic_cast<GridRelatedConditionCellContainerT<V, DA>* >(RawDataMapperT<V>::container());
	}
};

#endif // RAWDATACELLMAPPERT_H
