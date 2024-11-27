#ifndef GEODATAGDALCREATORT_DETAIL_H
#define GEODATAGDALCREATORT_DETAIL_H

#include "../geodatagdalcellmappert.h"
#include "../geodatagdalcreatort.h"
#include "../geodatagdalnodemappert.h"

template <class V, class DA>
GeoDataGdalCreatorT<V, DA>::GeoDataGdalCreatorT(const QString& typeName) :
	GeoDataGdalCreator(typeName)
{}

template <class V, class DA>
bool GeoDataGdalCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (cond->dimensions().size() > 0) {return false;}
	if (cond->isReferenceInformation()) {return false;}

	return true;
}

#endif // GEODATAGDALCREATORT_DETAIL_H
