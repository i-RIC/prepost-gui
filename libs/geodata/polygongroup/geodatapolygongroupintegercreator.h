#ifndef GEODATAPOLYGONGROUPINTEGERCREATOR_H
#define GEODATAPOLYGONGROUPINTEGERCREATOR_H

#include "geodatapolygongroupcreatort.h"

#include <vtkIntArray.h>

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupIntegerCreator : public GeoDataPolygonGroupCreatorT<int, vtkIntArray>
{
public:
	GeoDataPolygonGroupIntegerCreator();
};

#endif // GEODATAPOLYGONGROUPINTEGERCREATOR_H
