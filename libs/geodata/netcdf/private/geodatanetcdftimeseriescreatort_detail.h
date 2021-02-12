#ifndef GEODATANETCDFTIMESERIESCREATORT_DETAIL_H
#define GEODATANETCDFTIMESERIESCREATORT_DETAIL_H

#include "../geodatanetcdfcellmappert.h"
#include "../geodatanetcdftimeseriescreatort.h"
#include "../geodatanetcdfnodemappert.h"

template <class V, class DA>
GeoDataNetcdfTimeSeriesCreatorT<V, DA>::GeoDataNetcdfTimeSeriesCreatorT(const QString& typeName) :
        GeoDataNetcdfTimeSeriesCreator(typeName)
{
	nodeMappers().push_back(new GeoDataNetcdfNodeMapperT<V, DA>(this));
	cellMappers().push_back(new GeoDataNetcdfCellMapperT<V, DA>(this));
}

template <class V, class DA>
bool GeoDataNetcdfTimeSeriesCreatorT<V, DA>::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	auto cond = dynamic_cast<SolverDefinitionGridAttributeT<V>* >(condition);
	if (cond == nullptr) {return false;}
	if (cond->dimensions().size() == 0) {return false;}

	return true;
}

#endif // GEODATANETCDFTIMESERIESCREATORT_DETAIL_H
