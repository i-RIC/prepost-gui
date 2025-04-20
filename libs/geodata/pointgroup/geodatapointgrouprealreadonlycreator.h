#ifndef GEODATAPOINTGROUPREALREADONLYCREATOR_H
#define GEODATAPOINTGROUPREALREADONLYCREATOR_H

#include "geodatapointgrouprealcreator.h"

class GD_POINTGROUP_EXPORT GeoDataPointGroupRealReadOnlyCreator : public GeoDataPointGroupRealCreator
{
public:
	GeoDataPointGroupRealReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATAPOINTGROUPREALREADONLYCREATOR_H
