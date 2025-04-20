#ifndef GEODATAPOINTMAPREALREADONLYCREATOR_H
#define GEODATAPOINTMAPREALREADONLYCREATOR_H

#include "gd_pointmap_global.h"
#include "geodatapointmaprealcreator.h"

class GD_POINTMAP_EXPORT GeoDataPointmapRealReadOnlyCreator : public GeoDataPointmapRealCreator
{
	Q_OBJECT

public:
	GeoDataPointmapRealReadOnlyCreator();

	bool isReadOnly() const override;
};

#endif // GEODATAPOINTMAPREALREADONLYCREATOR_H
