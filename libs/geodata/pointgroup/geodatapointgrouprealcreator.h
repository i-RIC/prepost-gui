#ifndef GEODATAPOINTGROUPREALCREATOR_H
#define GEODATAPOINTGROUPREALCREATOR_H

#include "geodatapointgroupcreatort.h"

#include <vtkDoubleArray.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupRealCreator : public GeoDataPointGroupCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPointGroupRealCreator();
};

#endif // GEODATAPOINTGROUPREALCREATOR_H
