#ifndef GEODATAPOINTGROUPCREATORT_H
#define GEODATAPOINTGROUPCREATORT_H

#include "geodatapointgroupcreator.h"

template <typename V, typename DA>
class GeoDataPointGroupCreatorT : public GeoDataPointGroupCreator
{
public:
	GeoDataPointGroupCreatorT(const QString& typeName);
	virtual ~GeoDataPointGroupCreatorT();

	bool isCompatibleWith(SolverDefinitionGridAttribute *condition) const override;
};

#include "private/geodatapointgroupcreatort_detail.h"

#endif // GEODATAPOINTGROUPCREATORT_H
