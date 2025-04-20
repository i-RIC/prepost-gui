#ifndef GEODATAPOLYLINEGROUPREALCREATOR_H
#define GEODATAPOLYLINEGROUPREALCREATOR_H

#include "geodatapolylinegroupcreatort.h"

#include <vtkDoubleArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupRealCreator : public GeoDataPolyLineGroupCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolyLineGroupRealCreator();

protected:
	GeoDataPolyLineGroupRealCreator(const QString& typeName);
};

#endif // GEODATAPOLYLINEGROUPREALCREATOR_H
