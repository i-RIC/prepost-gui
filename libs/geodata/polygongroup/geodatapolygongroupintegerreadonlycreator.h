#ifndef GEODATAPOLYGONGROUPINTEGERREADONLYCREATOR_H
#define GEODATAPOLYGONGROUPINTEGERREADONLYCREATOR_H

#include "geodatapolygongroupintegercreator.h"

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupIntegerReadOnlyCreator : public GeoDataPolygonGroupIntegerCreator
{
public:
	GeoDataPolygonGroupIntegerReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATAPOLYGONGROUPINTEGERREADONLYCREATOR_H
