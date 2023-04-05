#ifndef GEODATAPOINTCREATORT_DETAIL_H
#define GEODATAPOINTCREATORT_DETAIL_H

#include "../geodatapointcreatort.h"
#include "../geodatapointcellmappert.h"
#include "../geodatapointnodemappert.h"

template <class V, class DA>
GeoDataPointCreatorT<V, DA>::GeoDataPointCreatorT(const QString& typeName) :
	GeoDataPointCreator {typeName}
{}

template <class V, class DA>
GeoDataPointCreatorT<V, DA>::~GeoDataPointCreatorT()
{}

template <class V, class DA>
GeoData* GeoDataPointCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto point = new GeoDataPoint(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		point->setMapper(new GeoDataPointNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		point->setMapper(new GeoDataPointCellMapperT<V, DA>(this));
	}
	return point;
}

template <class V, class DA>
bool GeoDataPointCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	return condition->isReferenceInformation();
}

#endif // GEODATAPOINTCREATORT_DETAIL_H
