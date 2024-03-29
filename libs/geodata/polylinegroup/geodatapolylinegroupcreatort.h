#ifndef GEODATAPOLYLINEGROUPCREATORT_H
#define GEODATAPOLYLINEGROUPCREATORT_H

#include "geodatapolylinegroupcreator.h"

template <typename V, typename DA>
class GeoDataPolyLineGroupCreatorT : public GeoDataPolyLineGroupCreator
{
public:
	GeoDataPolyLineGroupCreatorT(const QString& typeName);
	virtual ~GeoDataPolyLineGroupCreatorT();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute *condition) const override;
};

#include "private/geodatapolylinegroupcreatort_detail.h"

#endif // GEODATAPOLYLINEGROUPCREATORT_H
