#ifndef GEODATAPOLYLINEGROUPREADONLYCREATORT_H
#define GEODATAPOLYLINEGROUPREADONLYCREATORT_H

#include "geodatapolylinegroupreadonlycreator.h"

template <typename V, typename DA>
class GeoDataPolyLineGroupReadOnlyCreatorT : public GeoDataPolyLineGroupReadOnlyCreator
{
public:
	GeoDataPolyLineGroupReadOnlyCreatorT(const QString& typeName);
	virtual ~GeoDataPolyLineGroupReadOnlyCreatorT();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute *condition) const override;
};

#include "private/geodatapolylinegroupreadonlycreatort_detail.h"


#endif // GEODATAPOLYLINEGROUPREADONLYCREATORT_H
