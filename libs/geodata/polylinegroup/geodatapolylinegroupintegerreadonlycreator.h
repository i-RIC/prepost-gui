#ifndef GEODATAPOLYLINEGROUPINTEGERREADONLYCREATOR_H
#define GEODATAPOLYLINEGROUPINTEGERREADONLYCREATOR_H

#include "geodatapolylinegroupreadonlycreatort.h"

#include <vtkDoubleArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupIntegerReadOnlyCreator : public GeoDataPolyLineGroupReadOnlyCreatorT<int, vtkIntArray>
{
public:
	GeoDataPolyLineGroupIntegerReadOnlyCreator();
};


#endif // GEODATAPOLYLINEGROUPINTEGERREADONLYCREATOR_H
