#ifndef RAWDATAMAPPERT_H
#define RAWDATAMAPPERT_H

#include "rawdatamapper.h"
#include "rawdata.h"
#include "../gridcond/base/gridattributecontainert.h"

template <class V>
class RawDataMapperT : public RawDataMapper
{
public:
	RawDataMapperT(RawDataCreator* parent)
		: RawDataMapper(parent)
	{}
protected:
	GridAttributeContainerT<V>* container() const {
		return dynamic_cast<GridAttributeContainerT<V>* >(m_container);
	}
	V fromVariant(const QVariant& val) {
		SolverDefinitionGridAttribute* c = m_rawdata->gridRelatedCondition();
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(c);
		return cond->fromVariant(val);
	}
};

#endif // RAWDATAMAPPERT_H
