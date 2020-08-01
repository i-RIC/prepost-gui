#ifndef GEODATAPOINTGROUPINTEGERCREATOR_H
#define GEODATAPOINTGROUPINTEGERCREATOR_H

#include "geodatapointgroupcreatort.h"

#include <vtkIntArray.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupIntegerCreator : public GeoDataPointGroupCreatorT<int, vtkIntArray>
{
public:
	GeoDataPointGroupIntegerCreator();
};

#endif // GEODATAPOINTGROUPINTEGERCREATOR_H
