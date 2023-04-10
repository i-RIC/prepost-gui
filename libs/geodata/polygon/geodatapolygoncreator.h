#ifndef GEODATAPOLYGONCREATOR_H
#define GEODATAPOLYGONCREATOR_H

#include "gd_polygon_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_POLYGON_EXPORT GeoDataPolygonCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPolygonCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;
};

#endif // GEODATAPOLYGONCREATOR_H
