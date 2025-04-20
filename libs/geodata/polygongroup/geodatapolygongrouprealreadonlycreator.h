#ifndef GEODATAPOLYGONGROUPREALREADONLYCREATOR_H
#define GEODATAPOLYGONGROUPREALREADONLYCREATOR_H

#include "geodatapolygongrouprealcreator.h"

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupRealReadOnlyCreator : public GeoDataPolygonGroupRealCreator
{
public:
	GeoDataPolygonGroupRealReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATAPOLYGONGROUPREALREADONLYCREATOR_H
