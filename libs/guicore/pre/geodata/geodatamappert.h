#ifndef GEODATAMAPPERT_H
#define GEODATAMAPPERT_H

#include "geodatamapper.h"
#include "geodata.h"
#include "../gridcond/base/gridattributecontainert.h"

template <class V>
class GeoDataMapperT : public GeoDataMapper
{

public:
	GeoDataMapperT(GeoDataCreator* parent)
		: GeoDataMapper(parent)
	{}

protected:
	GridAttributeContainerT<V>* container() const {
		return dynamic_cast<GridAttributeContainerT<V>* >(m_container);
	}
	V fromVariant(const QVariant& val) {
		SolverDefinitionGridAttribute* c = m_geodata->gridRelatedCondition();
		SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(c);
		return cond->fromVariant(val);
	}
};

#endif // GEODATAMAPPERT_H
