#ifndef GEODATAPOINTGROUPREADONLYCREATORT_H
#define GEODATAPOINTGROUPREADONLYCREATORT_H

#include "geodatapointgroupreadonlycreator.h"

template <typename V, typename DA>
class GeoDataPointGroupReadOnlyCreatorT : public GeoDataPointGroupReadOnlyCreator
{
public:
	GeoDataPointGroupReadOnlyCreatorT(const QString& typeName);
	virtual ~GeoDataPointGroupReadOnlyCreatorT();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute *condition) const override;
};

#include "private/geodatapointgroupreadonlycreatort_detail.h"

#endif // GEODATAPOINTGROUPREADONLYCREATORT_H
