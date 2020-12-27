#ifndef GEODATAPOLYLINEGROUPCREATORT_DETAIL_H
#define GEODATAPOLYLINEGROUPCREATORT_DETAIL_H

#include "../geodatapolylinegroupcellmappert.h"
#include "../geodatapolylinegroupcreatort.h"
#include "../geodatapolylinegroupnodemappert.h"

template <typename V, typename DA>
GeoDataPolyLineGroupCreatorT<V, DA>::GeoDataPolyLineGroupCreatorT(const QString& typeName) :
	GeoDataPolyLineGroupCreator {typeName}
{
	nodeMappers().push_back(new GeoDataPolyLineGroupNodeMapperT<V, DA>(this));
	cellMappers().push_back(new GeoDataPolyLineGroupCellMapperT<V, DA>(this));
}

template <typename V, typename DA>
GeoDataPolyLineGroupCreatorT<V, DA>::~GeoDataPolyLineGroupCreatorT()
{}

template <typename V, typename DA>
bool GeoDataPolyLineGroupCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATAPOLYLINEGROUPCREATORT_DETAIL_H
