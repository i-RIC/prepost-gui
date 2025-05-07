#ifndef GEODATAGDALTIMESERIESCREATORT_DETAIL_H
#define GEODATAGDALTIMESERIESCREATORT_DETAIL_H

#include "../geodatagdalcellmappert.h"
#include "../geodatagdaltimeseriescreatort.h"
#include "../geodatagdalnodemappert.h"

template <class V, class DA>
GeoDataGdalTimeSeriesCreatorT<V, DA>::GeoDataGdalTimeSeriesCreatorT(const QString& typeName) :
        GeoDataGdalTimeSeriesCreator(typeName)
{}

template <class V, class DA>
bool GeoDataGdalTimeSeriesCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (cond->dimensions().size() == 0) {return false;}

	return true;
}

#endif // GEODATAGDALTIMESERIESCREATORT_DETAIL_H
