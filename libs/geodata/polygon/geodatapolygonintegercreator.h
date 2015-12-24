#ifndef GEODATAPOLYGONINTEGERCREATOR_H
#define GEODATAPOLYGONINTEGERCREATOR_H

#include "geodatapolygoncreatort.h"
#include <vtkIntArray.h>

class GD_POLYGON_EXPORT GeoDataPolygonIntegerCreator : public GeoDataPolygonCreatorT<int, vtkIntArray>
{
public:
	GeoDataPolygonIntegerCreator();
};

#endif // GEODATAPOLYGONINTEGERCREATOR_H
