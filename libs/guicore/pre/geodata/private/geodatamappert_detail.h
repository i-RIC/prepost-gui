#ifndef GEODATAMAPPERT_DETAIL_H
#define GEODATAMAPPERT_DETAIL_H

#include "../geodatamappert.h"

template <class V>
GeoDataMapperT<V>::GeoDataMapperT(const QString& caption, QObject* parent) :
	GeoDataMapper {caption, parent}
{}

template <class V>
GridAttributeContainerT<V>* GeoDataMapperT<V>::container() const
{
	return dynamic_cast<GridAttributeContainerT<V>* >(GeoDataMapper::container());
}

template <class V>
V GeoDataMapperT<V>::fromVariant(const QVariant& val) const
{
	SolverDefinitionGridAttribute* c = geoData()->gridAttribute();
	SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(c);
	return cond->fromVariant(val);
}

#endif // GEODATAMAPPERT_DETAIL_H
