#ifndef GEODATABACKGROUNDCREATORT_DETAIL_H
#define GEODATABACKGROUNDCREATORT_DETAIL_H

#include "../geodatabackgroundcreatort.h"
#include "../geodatabackground.h"
#include "../geodatabackgroundnodemappert.h"
#include "../geodatabackgroundcellmappert.h"

template <class V, class DA>
GeoDataBackgroundCreatorT<V, DA>::GeoDataBackgroundCreatorT(const QString& typeName) :
	GeoDataCreator(typeName, "Background")
{
	nodeMappers().append(new GeoDataBackgroundNodeMapperT<V, DA>(this));
	cellMappers().append(new GeoDataBackgroundCellMapperT<V, DA>(this));
}

template <class V, class DA>
GeoData* GeoDataBackgroundCreatorT<V, DA>::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto tmpcond = dynamic_cast<SolverDefinitionGridAttributeT<V> * >(condition);
	GeoData* geodata = new GeoDataBackground(parent, this, tmpcond);

	geodata->setPosition(tmpcond->position());
	geodata->setDefaultMapper();

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
