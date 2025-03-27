#ifndef GEODATAPOINTGROUPREADONLYCREATORT_DETAIL_H
#define GEODATAPOINTGROUPREADONLYCREATORT_DETAIL_H

#include "../geodatapointgroupcellmappert.h"
#include "../geodatapointgroupnodemappert.h"
#include "../geodatapointgroupreadonlycreatort.h"

template <typename V, typename DA>
GeoDataPointGroupReadOnlyCreatorT<V, DA>::GeoDataPointGroupReadOnlyCreatorT(const QString& typeName) :
	GeoDataPointGroupReadOnlyCreator {typeName}
{}

template <typename V, typename DA>
GeoDataPointGroupReadOnlyCreatorT<V, DA>::~GeoDataPointGroupReadOnlyCreatorT()
{}

template <typename V, typename DA>
GeoData* GeoDataPointGroupReadOnlyCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
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
bool GeoDataPointGroupReadOnlyCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return condition->isReferenceInformation();
}

#endif // GEODATAPOINTGROUPREADONLYCREATORT_DETAIL_H
