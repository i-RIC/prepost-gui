#ifndef GEODATAPOINTGROUPCREATORT_DETAIL_H
#define GEODATAPOINTGROUPCREATORT_DETAIL_H

#include "../geodatapointgroupcellmappert.h"
#include "../geodatapointgroupcreatort.h"
#include "../geodatapointgroupnodemappert.h"

template <typename V, typename DA>
GeoDataPointGroupCreatorT<V, DA>::GeoDataPointGroupCreatorT(const QString& typeName) :
	GeoDataPointGroupCreator {typeName}
{}

template <typename V, typename DA>
GeoDataPointGroupCreatorT<V, DA>::~GeoDataPointGroupCreatorT()
{}

template <typename V, typename DA>
GeoData* GeoDataPointGroupCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto g = new GeoDataPointGroup(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		g->setMapper(new GeoDataPointGroupNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		g->setMapper(new GeoDataPointGroupCellMapperT<V, DA>(this));
	}
	return g;
}

template <typename V, typename DA>
bool GeoDataPointGroupCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return condition->isReferenceInformation();
}

#endif // GEODATAPOINTGROUPCREATORT_DETAIL_H
