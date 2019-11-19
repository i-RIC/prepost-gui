#ifndef GEODATAPOLYGONGROUPCREATOR_H
#define GEODATAPOLYGONGROUPCREATOR_H

#include "gd_polygongroup_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPolygonGroupCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAPOLYGONGROUPCREATOR_H
