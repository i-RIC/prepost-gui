#ifndef GEODATAPOLYLINEGROUPINTEGERREADONLYCREATOR_H
#define GEODATAPOLYLINEGROUPINTEGERREADONLYCREATOR_H

#include "geodatapolylinegroupintegercreator.h"

#include <vtkDoubleArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupIntegerReadOnlyCreator : public GeoDataPolyLineGroupIntegerCreator
{
public:
	GeoDataPolyLineGroupIntegerReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};


#endif // GEODATAPOLYLINEGROUPINTEGERREADONLYCREATOR_H
