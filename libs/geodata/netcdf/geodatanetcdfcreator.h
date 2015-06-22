#ifndef GEODATANETCDFCREATOR_H
#define GEODATANETCDFCREATOR_H

#include "gd_netcdf_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_NETCDF_EXPORT GeoDataNetcdfCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	/// Constructor
	GeoDataNetcdfCreator();
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
};

#endif // GEODATANETCDFCREATOR_H
