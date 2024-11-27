#ifndef GEODATANETCDFTIMESERIESCREATOR_H
#define GEODATANETCDFTIMESERIESCREATOR_H

#include "gd_netcdf_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_NETCDF_EXPORT GeoDataNetcdfTimeSeriesCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataNetcdfTimeSeriesCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool requestCoordinateSystem() const override;
};

#endif // GEODATANETCDFTIMESERIESCREATOR_H
