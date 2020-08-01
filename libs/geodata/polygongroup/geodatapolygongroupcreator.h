#ifndef GEODATAPOLYGONGROUPCREATOR_H
#define GEODATAPOLYGONGROUPCREATOR_H

#include "gd_polygongroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupCreator : public GeoDataPolyDataGroupCreator
{
	Q_OBJECT

public:
	GeoDataPolygonGroupCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;

	int shapeType() const override;
	QString shapeName() const override;
	QString shapeNameCamelCase() const override;
};

#endif // GEODATAPOLYGONGROUPCREATOR_H
