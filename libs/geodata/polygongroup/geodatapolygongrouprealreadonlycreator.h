#ifndef GEODATAPOLYGONGROUPREALREADONLYCREATOR_H
#define GEODATAPOLYGONGROUPREALREADONLYCREATOR_H

#include "geodatapolygongroupreadonlycreatort.h"

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupRealReadOnlyCreator : public GeoDataPolygonGroupReadOnlyCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolygonGroupRealReadOnlyCreator();
};

#endif // GEODATAPOLYGONGROUPREALREADONLYCREATOR_H
