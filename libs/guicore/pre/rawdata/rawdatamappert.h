#ifndef RAWDATAMAPPERT_H
#define RAWDATAMAPPERT_H

#include "rawdatamapper.h"
#include "rawdata.h"
#include "../gridcond/base/gridrelatedconditioncontainert.h"

template <class V>
class RawDataMapperT : public RawDataMapper
{
public:
	RawDataMapperT(RawDataCreator* parent)
		: RawDataMapper(parent)
	{}
protected:
	GridRelatedConditionContainerT<V>* container() {
		return dynamic_cast<GridRelatedConditionContainerT<V>* >(m_container);
	}
	V fromVariant(const QVariant& val) {
		SolverDefinitionGridRelatedCondition* c = m_rawdata->gridRelatedCondition();
		SolverDefinitionGridRelatedConditionT<V>* cond = dynamic_cast<SolverDefinitionGridRelatedConditionT<V>* >(c);
		return cond->fromVariant(val);
	}
};

#endif // RAWDATAMAPPERT_H
