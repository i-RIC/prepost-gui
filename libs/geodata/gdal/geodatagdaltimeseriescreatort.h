#ifndef GEODATAGDALTIMESERIESCREATORT_H
#define GEODATAGDALTIMESERIESCREATORT_H

#include "geodatagdaltimeseriescreator.h"

class SolverDefinitionGridAttribute;

template <class V, class DA>
class GeoDataGdalTimeSeriesCreatorT : public GeoDataGdalTimeSeriesCreator
{
public:
	GeoDataGdalTimeSeriesCreatorT(const QString& typeName);

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatagdaltimeseriescreatort_detail.h"

#endif // GEODATAGDALTIMESERIESCREATORT_H
