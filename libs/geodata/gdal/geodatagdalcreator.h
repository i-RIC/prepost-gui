#ifndef GEODATANETCDFCREATOR_H
#define GEODATANETCDFCREATOR_H

#include "gd_netcdf_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_NETCDF_EXPORT GeoDataNetcdfCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataNetcdfCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool requestCoordinateSystem() const override;
};

#endif // GEODATANETCDFCREATOR_H
