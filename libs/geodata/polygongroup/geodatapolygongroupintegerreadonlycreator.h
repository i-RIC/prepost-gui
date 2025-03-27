#ifndef GEODATAPOLYGONGROUPINTEGERREADONLYCREATOR_H
#define GEODATAPOLYGONGROUPINTEGERREADONLYCREATOR_H

#include "geodatapolygongroupreadonlycreatort.h"

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupIntegerReadOnlyCreator : public GeoDataPolygonGroupReadOnlyCreatorT<int, vtkIntArray>
{
public:
	GeoDataPolygonGroupIntegerReadOnlyCreator();
};

#endif // GEODATAPOLYGONGROUPINTEGERREADONLYCREATOR_H
