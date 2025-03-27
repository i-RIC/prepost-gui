#ifndef GEODATAPOINTGROUPINTEGERREADONLYCREATOR_H
#define GEODATAPOINTGROUPINTEGERREADONLYCREATOR_H

#include "geodatapointgroupreadonlycreatort.h"

#include <vtkDoubleArray.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupIntegerReadOnlyCreator : public GeoDataPointGroupReadOnlyCreatorT<int, vtkIntArray>
{
public:
	GeoDataPointGroupIntegerReadOnlyCreator();
};


#endif // GEODATAPOINTGROUPINTEGERREADONLYCREATOR_H
