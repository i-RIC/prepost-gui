#ifndef GEODATAPOLYLINECREATORT_H
#define GEODATAPOLYLINECREATORT_H

#include "geodatapolylinecreator.h"

template <class V, class DA>
class GeoDataPolyLineCreatorT : public GeoDataPolyLineCreator
{
public:
	GeoDataPolyLineCreatorT(const QString& typeName);

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatapolylinecreatort_detail.h"

#endif // GEODATAPOLYLINECREATORT_H
