#ifndef GEODATAPOINTGROUPINTEGERREADONLYCREATOR_H
#define GEODATAPOINTGROUPINTEGERREADONLYCREATOR_H

#include "geodatapointgroupintegercreator.h"

class GD_POINTGROUP_EXPORT GeoDataPointGroupIntegerReadOnlyCreator : public GeoDataPointGroupIntegerCreator
{
public:
	GeoDataPointGroupIntegerReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATAPOINTGROUPINTEGERREADONLYCREATOR_H
