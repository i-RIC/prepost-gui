#ifndef GEODATAPOLYGONGROUPCREATORT_DETAIL_H
#define GEODATAPOLYGONGROUPCREATORT_DETAIL_H

#include "../geodatapolygongroupcellmappert.h"
#include "../geodatapolygongroupcreatort.h"
#include "../geodatapolygongroupnodemappert.h"

template <typename V, typename DA>
GeoDataPolygonGroupCreatorT<V, DA>::GeoDataPolygonGroupCreatorT(const QString& typeName) :
	GeoDataPolygonGroupCreator {typeName}
{
	nodeMappers().push_back(new GeoDataPolygonGroupNodeMapperT<V, DA>(this));
	cellMappers().push_back(new GeoDataPolygonGroupCellMapperT<V, DA>(this));
}

template <typename V, typename DA>
GeoDataPolygonGroupCreatorT<V, DA>::~GeoDataPolygonGroupCreatorT()
{}

template <typename V, typename DA>
bool GeoDataPolygonGroupCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATAPOLYGONGROUPCREATORT_DETAIL_H
