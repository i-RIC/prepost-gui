#ifndef GEODATAPOINTGROUPREALREADONLYCREATOR_H
#define GEODATAPOINTGROUPREALREADONLYCREATOR_H

#include "geodatapointgroupreadonlycreatort.h"

#include <vtkDoubleArray.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupRealReadOnlyCreator : public GeoDataPointGroupReadOnlyCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPointGroupRealReadOnlyCreator();
};

#endif // GEODATAPOINTGROUPREALREADONLYCREATOR_H
