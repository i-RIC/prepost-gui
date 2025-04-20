#ifndef GEODATAPOINTGROUPREALCREATOR_H
#define GEODATAPOINTGROUPREALCREATOR_H

#include "geodatapointgroupcreatort.h"

#include <vtkDoubleArray.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupRealCreator : public GeoDataPointGroupCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPointGroupRealCreator();

protected:
	GeoDataPointGroupRealCreator(const QString& typeName);
};

#endif // GEODATAPOINTGROUPREALCREATOR_H
