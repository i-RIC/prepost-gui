#ifndef GEODATAMAPPERT_DETAIL_H
#define GEODATAMAPPERT_DETAIL_H

#include "../geodatamappert.h"

template <class V, class DA>
GeoDataMapperT<V, DA>::GeoDataMapperT(const QString& caption, QObject* parent) :
	GeoDataMapper {caption, parent}
{}

template <class V, class DA>
GridAttributeContainerT<V, DA>* GeoDataMapperT<V, DA>::container() const
{
	return dynamic_cast<GridAttributeContainerT<V, DA>* >(GeoDataMapper::container());
}

template <class V, class DA>
V GeoDataMapperT<V, DA>::fromVariant(const QVariant& val) const
{
	SolverDefinitionGridAttribute* c = geoData()->gridAttribute();
	SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(c);
	return cond->fromVariant(val);
}

#endif // GEODATAMAPPERT_DETAIL_H
