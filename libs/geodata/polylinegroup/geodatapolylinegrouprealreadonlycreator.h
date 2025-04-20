#ifndef GEODATAPOLYLINEGROUPREALREADONLYCREATOR_H
#define GEODATAPOLYLINEGROUPREALREADONLYCREATOR_H

#include "geodatapolylinegrouprealcreator.h"

#include <vtkDoubleArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupRealReadOnlyCreator : public GeoDataPolyLineGroupRealCreator
{
public:
	GeoDataPolyLineGroupRealReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATAPOLYLINEGROUPREALREADONLYCREATOR_H
