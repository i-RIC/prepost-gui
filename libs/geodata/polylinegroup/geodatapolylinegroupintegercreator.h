#ifndef GEODATAPOLYLINEGROUPINTEGERCREATOR_H
#define GEODATAPOLYLINEGROUPINTEGERCREATOR_H

#include "geodatapolylinegroupcreatort.h"

#include <vtkIntArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupIntegerCreator : public GeoDataPolyLineGroupCreatorT<int, vtkIntArray>
{
public:
	GeoDataPolyLineGroupIntegerCreator();
};

#endif // GEODATAPOLYLINEGROUPINTEGERCREATOR_H
