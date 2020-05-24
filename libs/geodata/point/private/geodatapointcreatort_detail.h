#ifndef GEODATAPOINTCREATORT_DETAIL_H
#define GEODATAPOINTCREATORT_DETAIL_H

#include "../geodatapointcreatort.h"
#include "../geodatapointcellmappert.h"
#include "../geodatapointnodemappert.h"

template <class V, class DA>
GeoDataPointCreatorT<V, DA>::GeoDataPointCreatorT(const QString& typeName) :
	GeoDataPointCreator {typeName}
{
	nodeMappers().push_back(new GeoDataPointNodeMapperT<V, DA>(this));
	cellMappers().push_back(new GeoDataPointCellMapperT<V, DA>(this));
}

template <class V, class DA>
GeoDataPointCreatorT<V, DA>::~GeoDataPointCreatorT()
{}

template <class V, class DA>
bool GeoDataPointCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	return condition->isReferenceInformation();
}

#endif // GEODATAPOINTCREATORT_DETAIL_H
