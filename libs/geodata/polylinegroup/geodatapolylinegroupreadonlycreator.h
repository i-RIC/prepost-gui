#ifndef GEODATAPOLYLINEGROUPREADONLYCREATOR_H
#define GEODATAPOLYLINEGROUPREADONLYCREATOR_H

#include "gd_polylinegroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupReadOnlyCreator : public GeoDataPolyDataGroupCreator
{
	Q_OBJECT

public:
	GeoDataPolyLineGroupReadOnlyCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	int shapeType() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;

	bool isReadOnly() const override;
};

#endif // GEODATAPOLYLINEGROUPREADONLYCREATOR_H
