#ifndef GEODATAPOINTGROUPCREATOR_H
#define GEODATAPOINTGROUPCREATOR_H

#include "gd_pointgroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupCreator : public GeoDataPolyDataGroupCreator
{
	Q_OBJECT

public:
	GeoDataPointGroupCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;

	int shapeType() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;
};

#endif // GEODATAPOINTGROUPCREATOR_H
