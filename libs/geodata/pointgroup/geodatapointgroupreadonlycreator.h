#ifndef GEODATAPOINTGROUPREADONLYCREATOR_H
#define GEODATAPOINTGROUPREADONLYCREATOR_H

#include "gd_pointgroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupReadOnlyCreator : public GeoDataPolyDataGroupCreator
{
	Q_OBJECT

public:
	GeoDataPointGroupReadOnlyCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	int shapeType() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	bool isReadOnly() const override;
};

#endif // GEODATAPOINTGROUPREADONLYCREATOR_H
