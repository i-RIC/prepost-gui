#ifndef RAWDATANODEMAPPERT_H
#define RAWDATANODEMAPPERT_H

#include "rawdatamappert.h"
#include "../gridcond/base/gridrelatedconditionnodecontainert.h"

template <class V, class DA>
class RawDataNodeMapperT : public RawDataMapperT<V>
{
public:
	RawDataNodeMapperT(RawDataCreator* parent)
		: RawDataMapperT<V>(parent)
	{}
protected:
	GridRelatedConditionNodeContainerT<V, DA> * container(){
		return dynamic_cast<GridRelatedConditionNodeContainerT<V, DA>* >(RawDataMapperT<V>::container());
	}
};

#endif // RAWDATANODEMAPPERT_H
