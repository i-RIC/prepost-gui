#ifndef GEODATANETCDFCREATORT_DETAIL_H
#define GEODATANETCDFCREATORT_DETAIL_H

#include "../geodatanetcdfcellmappert.h"
#include "../geodatanetcdfcreatort.h"
#include "../geodatanetcdfnodemappert.h"

template <class V, class DA>
GeoDataNetcdfCreatorT<V, DA>::GeoDataNetcdfCreatorT(const QString& typeName) :
	GeoDataNetcdfCreator(typeName)
{
	nodeMappers().push_back(new GeoDataNetcdfNodeMapperT<V, DA>(this));
	cellMappers().push_back(new GeoDataNetcdfCellMapperT<V, DA>(this));
}

template <class V, class DA>
bool GeoDataNetcdfCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (cond->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATANETCDFCREATORT_DETAIL_H
