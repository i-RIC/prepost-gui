#ifndef GEODATAPOLYLINEGROUPREALREADONLYCREATOR_H
#define GEODATAPOLYLINEGROUPREALREADONLYCREATOR_H

#include "geodatapolylinegroupreadonlycreatort.h"

#include <vtkDoubleArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupRealReadOnlyCreator : public GeoDataPolyLineGroupReadOnlyCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolyLineGroupRealReadOnlyCreator();
};

#endif // GEODATAPOLYLINEGROUPREALREADONLYCREATOR_H
