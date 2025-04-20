#ifndef GEODATAPOINTMAPTINREADONLYCREATOR_H
#define GEODATAPOINTMAPTINREADONLYCREATOR_H

#include "gd_pointmap_global.h"
#include "geodatapointmaptincreator.h"

class GD_POINTMAP_EXPORT GeoDataPointmapTINReadOnlyCreator : public GeoDataPointmapTINCreator
{
	Q_OBJECT

public:
	GeoDataPointmapTINReadOnlyCreator();

	bool isReadOnly() const override;
};

#endif // GEODATAPOINTMAPTINREADONLYCREATOR_H
