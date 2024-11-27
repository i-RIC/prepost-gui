#ifndef GEODATANETCDFTIMESERIESCREATORT_H
#define GEODATANETCDFTIMESERIESCREATORT_H

#include "geodatanetcdftimeseriescreator.h"

class SolverDefinitionGridAttribute;

template <class V, class DA>
class GeoDataNetcdfTimeSeriesCreatorT : public GeoDataNetcdfTimeSeriesCreator
{
public:
	GeoDataNetcdfTimeSeriesCreatorT(const QString& typeName);

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatanetcdftimeseriescreatort_detail.h"

#endif // GEODATANETCDFTIMESERIESCREATORT_H
