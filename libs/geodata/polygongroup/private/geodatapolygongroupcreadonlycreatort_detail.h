#ifndef GEODATAPOLYGONGROUPREADONLYCREATORT_DETAIL_H
#define GEODATAPOLYGONGROUPREADONLYCREATORT_DETAIL_H

#include "../geodatapolygongroupcellmappert.h"
#include "../geodatapolygongroupreadonlycreatort.h"
#include "../geodatapolygongroupnodemappert.h"
#include "../geodatapolygongroupifacemappert.h"
#include "../geodatapolygongroupjfacemappert.h"

template <typename V, typename DA>
GeoDataPolygonGroupReadOnlyCreatorT<V, DA>::GeoDataPolygonGroupReadOnlyCreatorT(const QString& typeName) :
	GeoDataPolygonGroupReadOnlyCreator {typeName}
{}

template <typename V, typename DA>
GeoDataPolygonGroupReadOnlyCreatorT<V, DA>::~GeoDataPolygonGroupReadOnlyCreatorT()
{}


template <typename V, typename DA>
GeoData* GeoDataPolygonGroupReadOnlyCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto g = new GeoDataPolygonGroup(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		g->setMapper(new GeoDataPolygonGroupNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		g->setMapper(new GeoDataPolygonGroupCellMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {
		g->setMapper(new GeoDataPolygonGroupIFaceMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {
		g->setMapper(new GeoDataPolygonGroupJFaceMapperT<V, DA>(this));
	}
	return g;
}

template <typename V, typename DA>
bool GeoDataPolygonGroupReadOnlyCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute *condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}

	return true;
}

#endif // GEODATAPOLYGONGROUPREADONLYCREATORT_DETAIL_H
