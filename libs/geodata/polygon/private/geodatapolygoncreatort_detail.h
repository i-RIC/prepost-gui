#ifndef GEODATAPOLYGONCREATORT_DETAIL_H
#define GEODATAPOLYGONCREATORT_DETAIL_H

#include "../geodatapolygoncreatort.h"
#include "../geodatapolygoncellmappert.h"
#include "../geodatapolygonnodemappert.h"

template <class V, class DA>
GeoDataPolygonCreatorT<V, DA>::GeoDataPolygonCreatorT(const QString& typeName) :
	GeoDataPolygonCreator {typeName}
{}

template <class V, class DA>
GeoDataPolygonCreatorT<V, DA>::~GeoDataPolygonCreatorT()
{}

template <class V, class DA>
GeoData* GeoDataPolygonCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto polygon = new GeoDataPolygon(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		polygon->setMapper(new GeoDataPolygonNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		polygon->setMapper(new GeoDataPolygonCellMapperT<V, DA>(this));
	}
	return polygon;
}

template <class V, class DA>
bool GeoDataPolygonCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATAPOLYGONCREATORT_DETAIL_H
