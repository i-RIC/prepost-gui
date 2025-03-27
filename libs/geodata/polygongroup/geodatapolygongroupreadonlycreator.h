#ifndef GEODATAPOLYGONGROUPREADONLYCREATOR_H
#define GEODATAPOLYGONGROUPREADONLYCREATOR_H

#include "gd_polygongroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupReadOnlyCreator : public GeoDataPolyDataGroupCreator
{
	Q_OBJECT

public:
	GeoDataPolygonGroupReadOnlyCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	int shapeType() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	bool isReadOnly() const override;
};


#endif // GEODATAPOLYGONGROUPREADONLYCREATOR_H
