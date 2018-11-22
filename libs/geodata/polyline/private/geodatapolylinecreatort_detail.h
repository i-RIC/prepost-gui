#ifndef GEODATAPOLYLINECREATORT_DETAIL_H
#define GEODATAPOLYLINECREATORT_DETAIL_H

#include "../geodatapolylinecreatort.h"
#include "../geodatapolylinecellmappert.h"
#include "../geodatapolylinenodemappert.h"

template <class V, class DA>
GeoDataPolyLineCreatorT<V, DA>::GeoDataPolyLineCreatorT(const QString& typeName) :
	GeoDataPolyLineCreator {typeName}
{
	cellMappers().push_back(new GeoDataPolyLineCellMapperT<V, DA>(this));
	nodeMappers().push_back(new GeoDataPolyLineNodeMapperT<V, DA>(this));
}

template <class V, class DA>
GeoDataPolyLineCreatorT<V, DA>::~GeoDataPolyLineCreatorT()
{}

template <class V, class DA>
bool GeoDataPolyLineCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return condition->isReferenceInformation();
}

#endif // GEODATAPOLYLINECREATORT_DETAIL_H
