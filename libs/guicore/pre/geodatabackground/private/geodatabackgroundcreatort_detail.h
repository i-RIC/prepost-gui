#ifndef GEODATABACKGROUNDCREATORT_DETAIL_H
#define GEODATABACKGROUNDCREATORT_DETAIL_H

#include "../geodatabackgroundcreatort.h"
#include "../geodatabackground.h"
#include "../geodatabackgroundcellmappert.h"
#include "../geodatabackgroundifacemappert.h"
#include "../geodatabackgroundjfacemappert.h"
#include "../geodatabackgroundnodemappert.h"

template <class V, class DA>
GeoDataBackgroundCreatorT<V, DA>::GeoDataBackgroundCreatorT(const QString& typeName) :
	GeoDataCreator(typeName, "Background")
{}

template <class V, class DA>
GeoData* GeoDataBackgroundCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto geodata = new GeoDataBackground(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		geodata->setMapper(new GeoDataBackgroundNodeMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		geodata->setMapper(new GeoDataBackgroundCellMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {
		geodata->setMapper(new GeoDataBackgroundIFaceMapperT<V, DA>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {
		geodata->setMapper(new GeoDataBackgroundJFaceMapperT<V, DA>(this));
	}
	return geodata;
}

template <class V, class DA>
bool GeoDataBackgroundCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute*) const
{
	return true;
}

template <class V, class DA>
QString GeoDataBackgroundCreatorT<V, DA>::defaultCaption(unsigned int) const
{
	return "";
}

#endif // GEODATABACKGROUNDCREATORT_DETAIL_H
