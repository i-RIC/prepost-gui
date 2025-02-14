#ifndef GEODATAGDALCREATOR_H
#define GEODATAGDALCREATOR_H

#include "gd_gdal_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_GDAL_EXPORT GeoDataGdalCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataGdalCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool requestCoordinateSystem() const override;
	bool isReadOnly() const override;
};

#endif // GEODATAGDALCREATOR_H
