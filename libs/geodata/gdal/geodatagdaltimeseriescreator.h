#ifndef GEODATAGDALTIMESERIESCREATOR_H
#define GEODATAGDALTIMESERIESCREATOR_H

#include "gd_gdal_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_GDAL_EXPORT GeoDataGdalTimeSeriesCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataGdalTimeSeriesCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool requestCoordinateSystem() const override;
};

#endif // GEODATAGDALTIMESERIESCREATOR_H
