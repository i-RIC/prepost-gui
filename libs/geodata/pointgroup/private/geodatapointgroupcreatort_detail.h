#ifndef GEODATAPOINTGROUPCREATORT_DETAIL_H
#define GEODATAPOINTGROUPCREATORT_DETAIL_H

#include "../geodatapointgroupcellmappert.h"
#include "../geodatapointgroupcreatort.h"
#include "../geodatapointgroupnodemappert.h"

template <typename V, typename DA>
GeoDataPointGroupCreatorT<V, DA>::GeoDataPointGroupCreatorT(const QString& typeName) :
	GeoDataPointGroupCreator {typeName}
{
	nodeMappers().push_back(new GeoDataPointGroupNodeMapperT<V, DA>(this));
	cellMappers().push_back(new GeoDataPointGroupCellMapperT<V, DA>(this));
}

template <typename V, typename DA>
GeoDataPointGroupCreatorT<V, DA>::~GeoDataPointGroupCreatorT()
{}

template <typename V, typename DA>
bool GeoDataPointGroupCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return condition->isReferenceInformation();
}

#endif // GEODATAPOINTGROUPCREATORT_DETAIL_H
