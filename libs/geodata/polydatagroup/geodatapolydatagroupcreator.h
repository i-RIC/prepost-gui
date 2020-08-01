#ifndef GEODATAPOLYDATAGROUPCREATOR_H
#define GEODATAPOLYDATAGROUPCREATOR_H

#include "gd_polydatagroup_global.h"

#include <guicore/pre/geodata/geodatacreator.h>

class GD_POLYDATAGROUP_EXPORT GeoDataPolyDataGroupCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPolyDataGroupCreator(const QString& typeName, const QString& caption);

	virtual int shapeType() const = 0;
	virtual QString shapeName() const = 0;
	virtual QString shapeNameCamelCase() const = 0;
};

#endif // GEODATAPOLYDATAGROUPCREATOR_H
